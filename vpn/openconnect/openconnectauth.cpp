/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openconnectauth.h"

#include "passwordfield.h"
#include "plasma_nm_openconnect.h"

#include <QComboBox>
#include <QCryptographicHash>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDomDocument>
#include <QEventLoop>
#include <QFile>
#include <QFormLayout>
#include <QIcon>
#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QTimer>
#include <QWaitCondition>

#include <KLocalizedString>

#include "nm-openconnect-service.h"

#include <cstdarg>

extern "C" {
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
}

#if !OPENCONNECT_CHECK_VER(2, 1)
#define __openconnect_set_token_mode(...) -EOPNOTSUPP
#elif !OPENCONNECT_CHECK_VER(2, 2)
#define __openconnect_set_token_mode(vpninfo, mode, secret) openconnect_set_stoken_mode(vpninfo, 1, secret)
#else
#define __openconnect_set_token_mode openconnect_set_token_mode
#endif

#if OPENCONNECT_CHECK_VER(3, 4)
static int updateToken(void *, const char *);
#endif

class OpenconnectAuthWidgetPrivate
{
public:
};

OpenconnectAuthWidget::OpenconnectAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    m_setting = setting;
    ui.setupUi(this);
    userQuit = false;
    m_formGroupChanged = false;

    if (pipe2(cancelPipes, O_NONBLOCK | O_CLOEXEC)) {
        // Should never happen. Just don't do real cancellation if it does
        cancelPipes[0] = -1;
        cancelPipes[1] = -1;
    }

    connect(ui.cmbLogLevel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OpenconnectAuthWidget::logLevelChanged);
    connect(ui.viewServerLog, &QCheckBox::toggled, this, &OpenconnectAuthWidget::viewServerLogToggled);
    connect(ui.btnConnect, &QPushButton::clicked, this, &OpenconnectAuthWidget::connectHost);

    ui.cmbLogLevel->setCurrentIndex(OpenconnectAuthWidget::Debug);
    ui.btnConnect->setIcon(QIcon::fromTheme("network-connect"));
    ui.viewServerLog->setChecked(false);

    worker = new OpenconnectAuthWorkerThread(&mutex, &workerWaiting, &userQuit, &m_formGroupChanged, cancelPipes[0]);

    // gets the pointer to struct openconnect_info (defined in openconnect.h), which contains data that OpenConnect needs,
    // and which needs to be populated with settings we get from NM, like host, certificate or private key
    vpninfo = worker->getOpenconnectInfo();

    connect(worker,
            QOverload<const QString &, const QString &, const QString &, bool *>::of(&OpenconnectAuthWorkerThread::validatePeerCert),
            this,
            &OpenconnectAuthWidget::validatePeerCert);
    connect(worker, &OpenconnectAuthWorkerThread::processAuthForm, this, &OpenconnectAuthWidget::processAuthForm);
    connect(worker, &OpenconnectAuthWorkerThread::updateLog, this, &OpenconnectAuthWidget::updateLog);
    connect(worker, QOverload<const QString &>::of(&OpenconnectAuthWorkerThread::writeNewConfig), this, &OpenconnectAuthWidget::writeNewConfig);
    connect(worker, &OpenconnectAuthWorkerThread::cookieObtained, this, &OpenconnectAuthWidget::workerFinished);
    connect(worker, &OpenconnectAuthWorkerThread::initTokens, this, &OpenconnectAuthWidget::initTokens);

    readConfig();
    readSecrets();

#if OPENCONNECT_CHECK_VER(3, 4)
    openconnect_set_token_callbacks(vpninfo, &secrets, NULL, &updateToken);
#endif

    // This might be set by readSecrets() so don't connect it until now
    connect(ui.cmbHosts, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OpenconnectAuthWidget::connectHost);

    KAcceleratorManager::manage(this);
}

OpenconnectAuthWidget::~OpenconnectAuthWidget()
{
    userQuit = true;
    if (write(cancelPipes[1], "x", 1)) {
        // not a lot we can do
    }
    workerWaiting.wakeAll();
    worker->wait();
    ::close(cancelPipes[0]);
    ::close(cancelPipes[1]);
    deleteAllFromLayout(ui.loginBoxLayout);
    delete worker;
}

void OpenconnectAuthWidget::readConfig()
{
    const NMStringMap dataMap = m_setting->data();

    if (!dataMap[NM_OPENCONNECT_KEY_GATEWAY].isEmpty()) {
        const QString gw = dataMap[NM_OPENCONNECT_KEY_GATEWAY];
        VPNHost host;
        const int index = gw.indexOf(QLatin1Char('/'));
        if (index > -1) {
            host.name = host.address = gw.left(index);
            host.group = gw.right(gw.length() - index - 1);
        } else {
            host.name = host.address = gw;
        }
        hosts.append(host);
    }
    if (!dataMap[NM_OPENCONNECT_KEY_CACERT].isEmpty()) {
        const QByteArray crt = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_CACERT]);
        openconnect_set_cafile(vpninfo, OC3DUP(crt.data()));
    }
    if (dataMap[NM_OPENCONNECT_KEY_CSD_ENABLE] == "yes") {
        char *wrapper;
        wrapper = nullptr;
        if (!dataMap[NM_OPENCONNECT_KEY_CSD_WRAPPER].isEmpty()) {
            const QByteArray wrapperScript = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_CSD_WRAPPER]);
            wrapper = strdup(wrapperScript.data());
        }
        openconnect_setup_csd(vpninfo, getuid(), 1, wrapper);
    }
    if (!dataMap[NM_OPENCONNECT_KEY_PROXY].isEmpty()) {
        const QByteArray proxy = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_PROXY]);
        openconnect_set_http_proxy(vpninfo, OC3DUP(proxy.data()));
    }
#if OPENCONNECT_CHECK_VER(5, 8)
    if (!dataMap[NM_OPENCONNECT_KEY_USERAGENT].isEmpty()) {
        const QByteArray useragent = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_USERAGENT]);
        openconnect_set_useragent(vpninfo, OC3DUP(useragent.data()));
    }
#endif
    if (!dataMap[NM_OPENCONNECT_KEY_USERCERT].isEmpty()) {
        const QByteArray crt = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_USERCERT]);
        const QByteArray key = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_PRIVKEY]);
        openconnect_set_client_cert(vpninfo, OC3DUP(crt.data()), OC3DUP(key.isEmpty() ? nullptr : key.data()));

        if (!crt.isEmpty() && dataMap[NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID] == "yes") {
            openconnect_passphrase_from_fsid(vpninfo);
        }
    }
    if (!dataMap[NM_OPENCONNECT_KEY_PROTOCOL].isEmpty()) {
        const QString protocol = dataMap[NM_OPENCONNECT_KEY_PROTOCOL];
        openconnect_set_protocol(vpninfo, OC3DUP(protocol == "juniper" ? "nc" : protocol.toUtf8().data()));
    }
    if (!dataMap[NM_OPENCONNECT_KEY_REPORTED_OS].isEmpty()) {
        const QString reportedOs = dataMap[NM_OPENCONNECT_KEY_REPORTED_OS];
        openconnect_set_reported_os(vpninfo, reportedOs.toUtf8().data());
    }

    tokenMode = dataMap[NM_OPENCONNECT_KEY_TOKEN_MODE].toUtf8();
}

void OpenconnectAuthWidget::readSecrets()
{
    secrets = m_setting->secrets();

    if (!secrets["xmlconfig"].isEmpty()) {
        const QByteArray config = QByteArray::fromBase64(secrets["xmlconfig"].toLatin1());

        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(config.data(), config.size());
        const char *sha1_text = hash.result().toHex();
        openconnect_set_xmlsha1(vpninfo, (char *)sha1_text, strlen(sha1_text) + 1);

        QDomDocument xmlconfig;
        xmlconfig.setContent(config);
        const QDomNode anyConnectProfile = xmlconfig.elementsByTagName(QLatin1String("AnyConnectProfile")).at(0);
        bool matchedGw = false;
        const QDomNode serverList = anyConnectProfile.firstChildElement(QLatin1String("ServerList"));
        for (QDomElement entry = serverList.firstChildElement(QLatin1String("HostEntry")); !entry.isNull();
             entry = entry.nextSiblingElement(QLatin1String("HostEntry"))) {
            VPNHost host;
            host.name = entry.firstChildElement(QLatin1String("HostName")).text();
            host.group = entry.firstChildElement(QLatin1String("UserGroup")).text();
            host.address = entry.firstChildElement(QLatin1String("HostAddress")).text();
            // We added the originally configured host in readConfig(). But if
            // it matches one of the ones in the XML config (as presumably it
            // should), remove the original and use the one with the pretty name.
            if (!matchedGw && host.address == hosts.at(0).address) {
                hosts.removeFirst();
                matchedGw = true;
            }
            hosts.append(host);
        }
    }

    for (int i = 0; i < hosts.size(); i++) {
        ui.cmbHosts->addItem(hosts.at(i).name, i);
        if (secrets["lasthost"] == hosts.at(i).name || secrets["lasthost"] == hosts.at(i).address) {
            ui.cmbHosts->setCurrentIndex(i);
        }
    }

    if (secrets["autoconnect"] == "yes") {
        ui.chkAutoconnect->setChecked(true);
        QTimer::singleShot(0, this, &OpenconnectAuthWidget::connectHost);
    }

    if (secrets["save_passwords"] == "yes") {
        ui.chkStorePasswords->setChecked(true);
    }

    token.tokenMode = OC_TOKEN_MODE_NONE;
    token.tokenSecret = nullptr;

    if (!tokenMode.isEmpty()) {
        int ret = 0;
        QByteArray tokenSecret = secrets[NM_OPENCONNECT_KEY_TOKEN_SECRET].toUtf8();

        if (tokenMode == QStringLiteral("manual") && !tokenSecret.isEmpty()) {
            ret = __openconnect_set_token_mode(vpninfo, OC_TOKEN_MODE_STOKEN, tokenSecret);
        } else if (tokenMode == QStringLiteral("stokenrc")) {
            ret = __openconnect_set_token_mode(vpninfo, OC_TOKEN_MODE_STOKEN, NULL);
        } else if (tokenMode == QStringLiteral("totp") && !tokenSecret.isEmpty()) {
            ret = __openconnect_set_token_mode(vpninfo, OC_TOKEN_MODE_TOTP, tokenSecret);
        }
#if OPENCONNECT_CHECK_VER(3, 4)
        else if (tokenMode == QStringLiteral("hotp") && !tokenSecret.isEmpty()) {
            ret = __openconnect_set_token_mode(vpninfo, OC_TOKEN_MODE_HOTP, tokenSecret);
        }
#endif
#if OPENCONNECT_CHECK_VER(5, 0)
        else if (tokenMode == "yubioath") {
            /* This needs to be done from a thread because it can call back to
                ask for the PIN */
            token.tokenMode = OC_TOKEN_MODE_YUBIOATH;
            if (!tokenSecret.isEmpty()) {
                token.tokenSecret = tokenSecret;
            }
        }
#endif
        if (ret) {
            addFormInfo(QLatin1String("dialog-error"), i18n("Failed to initialize software token: %1", ret));
        }
    }
}

void OpenconnectAuthWidget::acceptDialog()
{
    // Find top-level widget as this should be the QDialog itself
    QWidget *widget = parentWidget();
    while (widget->parentWidget() != nullptr) {
        widget = widget->parentWidget();
    }

    auto dialog = qobject_cast<QDialog *>(widget);
    if (dialog) {
        dialog->accept();
    }
}

// This starts the worker thread, which connects to the selected AnyConnect host
// and retrieves the login form
void OpenconnectAuthWidget::connectHost()
{
    userQuit = true;
    if (write(cancelPipes[1], "x", 1)) {
        // not a lot we can do
    }
    workerWaiting.wakeAll();
    worker->wait();
    userQuit = false;

    /* Suck out the cancel byte(s) */
    char buf;
    while (read(cancelPipes[0], &buf, 1) == 1) {
        ;
    }
    deleteAllFromLayout(ui.loginBoxLayout);
    int i = ui.cmbHosts->currentIndex();
    if (i == -1) {
        return;
    }
    i = ui.cmbHosts->itemData(i).toInt();
    const VPNHost &host = hosts.at(i);
    if (openconnect_parse_url(vpninfo, host.address.toLatin1().data())) {
        qCWarning(PLASMA_NM_OPENCONNECT_LOG) << "Failed to parse server URL" << host.address;
        openconnect_set_hostname(vpninfo, OC3DUP(host.address.toLatin1().data()));
    }
    if (!openconnect_get_urlpath(vpninfo) && !host.group.isEmpty()) {
        openconnect_set_urlpath(vpninfo, OC3DUP(host.group.toLatin1().data()));
    }
    secrets["lasthost"] = host.name;
    addFormInfo(QLatin1String("dialog-information"), i18n("Contacting host, please wait…"));
    worker->start();
}

void OpenconnectAuthWidget::initTokens()
{
    if (token.tokenMode != OC_TOKEN_MODE_NONE) {
        __openconnect_set_token_mode(vpninfo, token.tokenMode, token.tokenSecret);
    }
}

QVariantMap OpenconnectAuthWidget::setting() const
{
    NMStringMap secrets;
    QVariantMap secretData;

    secrets.insert(secrets);
    QString host(openconnect_get_hostname(vpninfo));
    const QString port = QString::number(openconnect_get_port(vpninfo));
    QString gateway = host + ':' + port;
    const char *urlpath = openconnect_get_urlpath(vpninfo);
    if (urlpath) {
        gateway += '/';
        gateway += urlpath;
    }
    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY), gateway);

    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_COOKIE), QLatin1String(openconnect_get_cookie(vpninfo)));
    openconnect_clear_cookie(vpninfo);

#if OPENCONNECT_CHECK_VER(5, 0)
    const char *fingerprint = openconnect_get_peer_cert_hash(vpninfo);
#else
    OPENCONNECT_X509 *cert = openconnect_get_peer_cert(vpninfo);
    char fingerprint[41];
    openconnect_get_cert_sha1(vpninfo, cert, fingerprint);
#endif
    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_GWCERT), QLatin1String(fingerprint));
    secrets.insert(QLatin1String("autoconnect"), ui.chkAutoconnect->isChecked() ? "yes" : "no");
    secrets.insert(QLatin1String("save_passwords"), ui.chkStorePasswords->isChecked() ? "yes" : "no");

    NMStringMap::iterator i = secrets.begin();
    while (i != secrets.end()) {
        if (i.value().isEmpty()) {
            i = secrets.erase(i);
        } else {
            ++i;
        }
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));

    // These secrets are not officially part of the secrets which would be returned back to NetworkManager. We just
    // need to somehow get them to our secret agent which will handle them separately and store them.
    if (!tmpSecrets.isEmpty()) {
        secretData.insert("tmp-secrets", QVariant::fromValue<NMStringMap>(tmpSecrets));
    }
    return secretData;
}

#if OPENCONNECT_CHECK_VER(3, 4)
static int updateToken(void *cbdata, const char *tok)
{
    auto secrets = static_cast<NMStringMap *>(cbdata);
    secrets->insert(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_SECRET), QLatin1String(tok));
    return 0;
}
#endif

void OpenconnectAuthWidget::writeNewConfig(const QString &buf)
{
    secrets["xmlconfig"] = buf;
}

void OpenconnectAuthWidget::updateLog(const QString &message, const int &level)
{
    QPair<QString, int> pair;
    pair.first = message;
    if (pair.first.endsWith(QLatin1String("\n"))) {
        pair.first.chop(1);
    }
    switch (level) {
    case PRG_ERR:
        pair.second = OpenconnectAuthWidget::Error;
        break;
    case PRG_INFO:
        pair.second = OpenconnectAuthWidget::Info;
        break;
    case PRG_DEBUG:
        pair.second = OpenconnectAuthWidget::Debug;
        break;
    case PRG_TRACE:
        pair.second = OpenconnectAuthWidget::Trace;
        break;
    }
    if (pair.second <= ui.cmbLogLevel->currentIndex()) {
        ui.serverLog->append(pair.first);
    }

    serverLog.append(pair);
    if (serverLog.size() > 100) {
        serverLog.removeFirst();
    }
}

void OpenconnectAuthWidget::logLevelChanged(int newLevel)
{
    ui.serverLog->clear();
    QList<QPair<QString, int>>::const_iterator i;

    for (i = serverLog.constBegin(); i != serverLog.constEnd(); ++i) {
        QPair<QString, int> pair = *i;
        if (pair.second <= newLevel) {
            ui.serverLog->append(pair.first);
        }
    }
}

void OpenconnectAuthWidget::addFormInfo(const QString &iconName, const QString &message)
{
    auto layout = new QHBoxLayout();
    auto icon = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(icon->sizePolicy().hasHeightForWidth());
    icon->setSizePolicy(sizePolicy);
    icon->setMinimumSize(QSize(16, 16));
    icon->setMaximumSize(QSize(16, 16));
    layout->addWidget(icon);

    auto text = new QLabel(this);
    text->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    text->setWordWrap(true);
    layout->addWidget(text);

    const int iconSize = icon->style()->pixelMetric(QStyle::PixelMetric::PM_SmallIconSize);
    icon->setPixmap(QIcon::fromTheme(iconName).pixmap(iconSize));
    text->setText(message);

    ui.loginBoxLayout->addLayout(layout);
}

void OpenconnectAuthWidget::processAuthForm(struct oc_auth_form *form)
{
    deleteAllFromLayout(ui.loginBoxLayout);

    struct oc_form_opt *opt;
    auto layout = new QFormLayout();
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    bool focusSet = false;
    for (opt = form->opts; opt; opt = opt->next) {
        if (opt->type == OC_FORM_OPT_HIDDEN || IGNORE_OPT(opt)) {
            continue;
        }
        auto text = new QLabel(this);
        text->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
        text->setText(QString(opt->label));
        QWidget *widget = nullptr;
        const QString key = QString("form:%1:%2").arg(QLatin1String(form->auth_id)).arg(QLatin1String(opt->name));
        const QString value = secrets.value(key);
        if (opt->type == OC_FORM_OPT_PASSWORD || opt->type == OC_FORM_OPT_TEXT) {
            auto le = new PasswordField(this);
            le->setText(value);
            if (opt->type == OC_FORM_OPT_PASSWORD) {
                le->setPasswordModeEnabled(true);
            }
            if (!focusSet && le->text().isEmpty()) {
                le->setFocus(Qt::OtherFocusReason);
                focusSet = true;
            }
            widget = qobject_cast<QWidget *>(le);
        } else if (opt->type == OC_FORM_OPT_SELECT) {
            auto cmb = new QComboBox(this);
            auto sopt = reinterpret_cast<oc_form_opt_select *>(opt);
#if !OPENCONNECT_CHECK_VER(8, 0)
            const QString protocol = m_setting->data()[NM_OPENCONNECT_KEY_PROTOCOL];
#endif
            for (int i = 0; i < sopt->nr_choices; i++) {
                cmb->addItem(QString::fromUtf8(FORMCHOICE(sopt, i)->label), QString::fromUtf8(FORMCHOICE(sopt, i)->name));
                if (value == QString::fromUtf8(FORMCHOICE(sopt, i)->name)) {
                    cmb->setCurrentIndex(i);
#if !OPENCONNECT_CHECK_VER(8, 0)
                    if (protocol != QLatin1String("nc") && sopt == AUTHGROUP_OPT(form) && i != AUTHGROUP_SELECTION(form)) {
#else
                    if (sopt == AUTHGROUP_OPT(form) && i != AUTHGROUP_SELECTION(form)) {
#endif
                        QTimer::singleShot(0, this, &OpenconnectAuthWidget::formGroupChanged);
                    }
                }
            }
#if !OPENCONNECT_CHECK_VER(8, 0)
            if (protocol != QLatin1String("nc") && sopt == AUTHGROUP_OPT(form)) {
#else
            if (sopt == AUTHGROUP_OPT(form)) {
#endif
                connect(cmb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &OpenconnectAuthWidget::formGroupChanged);
            }
            widget = qobject_cast<QWidget *>(cmb);
        }
        if (widget) {
            widget->setProperty("openconnect_opt", (quintptr)opt);
            widget->setSizePolicy(policy);
            layout->addRow(text, widget);
        }
    }
    if (!layout->rowCount()) {
        delete layout;
        workerWaiting.wakeAll();
        return;
    }

    if (form->banner) {
        addFormInfo(QLatin1String("dialog-information"), form->banner);
    }
    if (form->message) {
        addFormInfo(QLatin1String("dialog-information"), form->message);
    }
    if (form->error) {
        addFormInfo(QLatin1String("dialog-error"), form->error);
    }

    ui.loginBoxLayout->addLayout(layout);
    passwordFormIndex = ui.loginBoxLayout->count() - 1;

    auto box = new QDialogButtonBox(this);
    QPushButton *btn = box->addButton(QDialogButtonBox::Ok);
    btn->setText(i18nc("Verb, to proceed with login", "Login"));
    btn->setDefault(true);
    ui.loginBoxLayout->addWidget(box);
    box->setProperty("openconnect_form", (quintptr)form);

    connect(box, &QDialogButtonBox::accepted, this, &OpenconnectAuthWidget::formLoginClicked);
}

void OpenconnectAuthWidget::validatePeerCert(const QString &fingerprint, const QString &peerCert, const QString &reason, bool *accepted)
{
    const QString host = QLatin1String(openconnect_get_hostname(vpninfo));
    const QString port = QString::number(openconnect_get_port(vpninfo));
    const QString key = QString("certificate:%1:%2").arg(host, port);
    const QString value = secrets.value(key);

#if !OPENCONNECT_CHECK_VER(5, 0)
#define openconnect_check_peer_cert_hash(v, d) strcmp(d, fingerprint.toUtf8().data())
#endif

    if (openconnect_check_peer_cert_hash(vpninfo, value.toUtf8().data())) {
        auto widget = new QWidget();
        QVBoxLayout *verticalLayout;
        QHBoxLayout *horizontalLayout;
        QLabel *icon;
        QLabel *infoText;
        QTextBrowser *certificate;

        verticalLayout = new QVBoxLayout(widget);
        horizontalLayout = new QHBoxLayout(widget);
        icon = new QLabel(widget);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(icon->sizePolicy().hasHeightForWidth());
        icon->setSizePolicy(sizePolicy);
        icon->setMinimumSize(QSize(48, 48));
        icon->setMaximumSize(QSize(48, 48));

        horizontalLayout->addWidget(icon);

        infoText = new QLabel(widget);
        infoText->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

        horizontalLayout->addWidget(infoText);

        verticalLayout->addLayout(horizontalLayout);

        certificate = new QTextBrowser(widget);
        certificate->setTextInteractionFlags(Qt::TextSelectableByMouse);
        certificate->setOpenLinks(false);

        verticalLayout->addWidget(certificate);

        const int iconSize = icon->style()->pixelMetric(QStyle::PixelMetric::PM_LargeIconSize);
        icon->setPixmap(QIcon::fromTheme("dialog-information").pixmap(iconSize));
        infoText->setText(
            i18n("Check failed for certificate from VPN server \"%1\".\n"
                 "Reason: %2\nAccept it anyway?",
                 openconnect_get_hostname(vpninfo),
                 reason));
        infoText->setWordWrap(true);
        certificate->setText(peerCert);

        QPointer<QDialog> dialog = new QDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog.data()->setWindowModality(Qt::WindowModal);
        dialog->setLayout(new QVBoxLayout);
        auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog);
        connect(buttons, &QDialogButtonBox::accepted, dialog.data(), &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, dialog.data(), &QDialog::reject);
        dialog->layout()->addWidget(widget);
        dialog->layout()->addWidget(buttons);

        const NMStringMap dataMap = m_setting->data();
        buttons->button(QDialogButtonBox::Ok)->setEnabled(dataMap[NM_OPENCONNECT_KEY_PREVENT_INVALID_CERT] != "yes");

        if (dialog.data()->exec() == QDialog::Accepted) {
            *accepted = true;
        } else {
            *accepted = false;
        }
        widget->deleteLater();
    } else {
        *accepted = true;
    }
    if (*accepted) {
        secrets.insert(key, QString(fingerprint));
    }
    mutex.lock();
    workerWaiting.wakeAll();
    mutex.unlock();
}

void OpenconnectAuthWidget::formGroupChanged()
{
    m_formGroupChanged = true;
    formLoginClicked();
}

// Writes the user input from the form into the oc_auth_form structs we got from
// libopenconnect, and wakes the worker thread up to try to log in and obtain a
// cookie with this data
void OpenconnectAuthWidget::formLoginClicked()
{
    const int lastIndex = ui.loginBoxLayout->count() - 1;
    QLayout *layout = ui.loginBoxLayout->itemAt(passwordFormIndex)->layout();
    struct oc_auth_form *form = (struct oc_auth_form *)ui.loginBoxLayout->itemAt(lastIndex)->widget()->property("openconnect_form").value<quintptr>();

    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *item = layout->itemAt(i);
        QWidget *widget = item->widget();
        if (widget && widget->property("openconnect_opt").isValid()) {
            struct oc_form_opt *opt = (struct oc_form_opt *)widget->property("openconnect_opt").value<quintptr>();
            const QString key = QString("form:%1:%2").arg(QLatin1String(form->auth_id)).arg(QLatin1String(opt->name));
            if (opt->type == OC_FORM_OPT_PASSWORD || opt->type == OC_FORM_OPT_TEXT) {
                auto le = qobject_cast<PasswordField *>(widget);
                QByteArray text = le->text().toUtf8();
                openconnect_set_option_value(opt, text.data());
                if (opt->type == OC_FORM_OPT_TEXT) {
                    secrets.insert(key, le->text());
                } else {
                    tmpSecrets.insert(key, le->text());
                }
            } else if (opt->type == OC_FORM_OPT_SELECT) {
                auto cbo = qobject_cast<QComboBox *>(widget);
                QByteArray text = cbo->itemData(cbo->currentIndex()).toString().toLatin1();
                openconnect_set_option_value(opt, text.data());
                secrets.insert(key, cbo->itemData(cbo->currentIndex()).toString());
            }
        }
    }

    deleteAllFromLayout(ui.loginBoxLayout);
    workerWaiting.wakeAll();
}

void OpenconnectAuthWidget::workerFinished(const int &ret)
{
    if (ret < 0) {
        QString message;
        QList<QPair<QString, int>>::const_iterator i;
        for (i = serverLog.constEnd() - 1; i >= serverLog.constBegin(); --i) {
            QPair<QString, int> pair = *i;
            if (pair.second <= OpenconnectAuthWidget::Error) {
                message = pair.first;
                break;
            }
        }
        if (message.isEmpty()) {
            message = i18n("Connection attempt was unsuccessful.");
        }
        deleteAllFromLayout(ui.loginBoxLayout);
        addFormInfo(QLatin1String("dialog-error"), message);
    } else {
        deleteAllFromLayout(ui.loginBoxLayout);
        acceptDialog();
    }
}

void OpenconnectAuthWidget::deleteAllFromLayout(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QLayout *itemLayout = item->layout()) {
            deleteAllFromLayout(itemLayout);
            itemLayout->deleteLater();
        } else {
            item->widget()->deleteLater();
        }
        delete item;
    }
    layout->invalidate();
}

void OpenconnectAuthWidget::viewServerLogToggled(bool toggled)
{
    ui.lblLogLevel->setVisible(toggled);
    ui.cmbLogLevel->setVisible(toggled);
    if (toggled) {
        delete ui.verticalLayout->takeAt(5);
        QSizePolicy policy = ui.serverLogBox->sizePolicy();
        policy.setVerticalPolicy(QSizePolicy::Expanding);
        ui.serverLogBox->setSizePolicy(policy);
        ui.serverLog->setVisible(true);
    } else {
        auto verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        ui.verticalLayout->addItem(verticalSpacer);
        ui.serverLog->setVisible(false);
        QSizePolicy policy = ui.serverLogBox->sizePolicy();
        policy.setVerticalPolicy(QSizePolicy::Fixed);
        ui.serverLogBox->setSizePolicy(policy);
    }
}
