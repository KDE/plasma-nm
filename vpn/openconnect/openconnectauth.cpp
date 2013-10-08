/*
Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) version 3, or any
later version accepted by the membership of KDE e.V. (or its
successor approved by the membership of KDE e.V.), which shall
act as a proxy defined in Section 6 of version 3 of the license.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "openconnectauth.h"
#include "openconnectauthworkerthread.h"
#include "ui_openconnectauth.h"

#include <KDialog>
#include <QPushButton>
#include <QString>
#include <QLabel>
#include <QEventLoop>
#include <QFormLayout>
#include <KLineEdit>
#include <KDialogButtonBox>
#include <KPushButton>
#include <KComboBox>
#include <QDebug>
#include <QDomDocument>
#include <QMutex>
#include <QWaitCondition>
#include <QCryptographicHash>
#include <QFile>

#include "nm-openconnect-service.h"

#include <cstdarg>

extern "C"
{
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
}

// name/address: IP/domain name of the host (OpenConnect accepts both, so no difference here)
// group: user group on the server
typedef struct {
    QString name;
    QString group;
    QString address;
} VPNHost;

class OpenconnectAuthWidgetPrivate
{
public:
    Ui_OpenconnectAuth ui;
    NetworkManager::VpnSetting::Ptr setting;
    struct openconnect_info *vpninfo;
    QStringList certificateFingerprints;
    NMStringMap secrets;
    QMutex mutex;
    QWaitCondition workerWaiting;
    OpenconnectAuthWorkerThread *worker;
    QList<VPNHost> hosts;
    bool userQuit;
    int cancelPipes[2];
    QList<QPair<QString, int> > serverLog;

    enum LogLevels {Error = 0, Info, Debug, Trace};
};


OpenconnectAuthWidget::OpenconnectAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent), d_ptr(new OpenconnectAuthWidgetPrivate)
{
    Q_D(OpenconnectAuthWidget);
    d->setting = setting;
    d->ui.setupUi(this);
    d->userQuit = false;
    if (pipe2(d->cancelPipes, O_NONBLOCK|O_CLOEXEC)) {
        // Should never happen. Just don't do real cancellation if it does
        d->cancelPipes[0] = -1;
        d->cancelPipes[1] = -1;
    }

    connect(d->ui.cmbLogLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(logLevelChanged(int)));
    connect(d->ui.viewServerLog, SIGNAL(toggled(bool)), this, SLOT(viewServerLogToggled(bool)));
    connect(d->ui.btnConnect, SIGNAL(clicked()), this, SLOT(connectHost()));
    connect(d->ui.cmbHosts, SIGNAL(currentIndexChanged(int)), this, SLOT(connectHost()));

    d->ui.cmbLogLevel->setCurrentIndex(OpenconnectAuthWidgetPrivate::Debug);
    d->ui.btnConnect->setIcon(KIcon("network-connect"));
    d->ui.viewServerLog->setChecked(false);

    d->worker = new OpenconnectAuthWorkerThread(&d->mutex, &d->workerWaiting, &d->userQuit, d->cancelPipes[0]);

    // gets the pointer to struct openconnect_info (defined in openconnect.h), which contains data that OpenConnect needs,
    // and which needs to be populated with settings we get from NM, like host, certificate or private key
    d->vpninfo = d->worker->getOpenconnectInfo();

    connect(d->worker, SIGNAL(validatePeerCert(QString,QString,QString,bool*)), this, SLOT(validatePeerCert(QString,QString,QString,bool*)));
    connect(d->worker, SIGNAL(processAuthForm(oc_auth_form*)), this, SLOT(processAuthForm(oc_auth_form*)));
    connect(d->worker, SIGNAL(updateLog(QString,int)), this, SLOT(updateLog(QString,int)));
    connect(d->worker, SIGNAL(writeNewConfig(QString)), this, SLOT(writeNewConfig(QString)));
    connect(d->worker, SIGNAL(cookieObtained(int)), this, SLOT(workerFinished(int)));

    readConfig();
    readSecrets();

    KAcceleratorManager::manage(this);
}

OpenconnectAuthWidget::~OpenconnectAuthWidget()
{
    Q_D(OpenconnectAuthWidget);
    d->userQuit = true;
    if (write(d->cancelPipes[1], "x", 1)) {
        // not a lot we can do
    }
    d->workerWaiting.wakeAll();
    d->worker->wait();
    ::close(d->cancelPipes[0]);
    ::close(d->cancelPipes[1]);
    deleteAllFromLayout(d->ui.loginBoxLayout);
    delete d->worker;
    delete d;
}

void OpenconnectAuthWidget::readConfig()
{
    Q_D(OpenconnectAuthWidget);

    const NMStringMap dataMap = d->setting->data();

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
        d->hosts.append(host);
    }
    if (!dataMap[NM_OPENCONNECT_KEY_CACERT].isEmpty()) {
        const QByteArray crt = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_CACERT]);
        openconnect_set_cafile(d->vpninfo, strdup(crt.data()));
    }
    if (dataMap[NM_OPENCONNECT_KEY_CSD_ENABLE] == "yes") {
        char *wrapper;
        wrapper = 0;
        if (!dataMap[NM_OPENCONNECT_KEY_CSD_WRAPPER].isEmpty()) {
            const QByteArray wrapperScript = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_CSD_WRAPPER]);
            wrapper = strdup(wrapperScript.data());
        }
        openconnect_setup_csd(d->vpninfo, getuid(), 1, wrapper);
    }
    if (!dataMap[NM_OPENCONNECT_KEY_PROXY].isEmpty()) {
        const QByteArray proxy = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_PROXY]);
        openconnect_set_http_proxy(d->vpninfo, strdup(proxy.data()));
    }
    if (!dataMap[NM_OPENCONNECT_KEY_USERCERT].isEmpty()) {
        const QByteArray crt = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_USERCERT]);
        const QByteArray key = QFile::encodeName(dataMap[NM_OPENCONNECT_KEY_PRIVKEY]);
        openconnect_set_client_cert (d->vpninfo, strdup(crt.data()), strdup(key.data()));

        if (!crt.isEmpty() && dataMap[NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID] == "yes") {
            openconnect_passphrase_from_fsid(d->vpninfo);
        }
    }
}

void OpenconnectAuthWidget::readSecrets()
{
    Q_D(OpenconnectAuthWidget);

    d->secrets = d->setting->secrets();

    if (!d->secrets[NM_OPENCONNECT_KEY_GWCERT].isEmpty()) {
        d->certificateFingerprints.append(d->secrets[NM_OPENCONNECT_KEY_GWCERT]);
    }

    if (!d->secrets["xmlconfig"].isEmpty()) {

        QByteArray config = QByteArray::fromBase64(d->secrets["xmlconfig"].toAscii());

        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(config.data(), config.size());
        const char *sha1_text = hash.result().toHex();
        openconnect_set_xmlsha1 (d->vpninfo, (char *)sha1_text, strlen(sha1_text)+1);

        QDomDocument xmlconfig;
        xmlconfig.setContent(config);
        const QDomNode anyConnectProfile = xmlconfig.elementsByTagName(QLatin1String("AnyConnectProfile")).at(0);
        bool matchedGw = false;
        const QDomNode serverList = anyConnectProfile.firstChildElement(QLatin1String("ServerList"));
        for (QDomElement entry = serverList.firstChildElement(QLatin1String("HostEntry")); !entry.isNull(); entry = entry.nextSiblingElement(QLatin1String("HostEntry"))) {
            VPNHost host;
            host.name = entry.firstChildElement(QLatin1String("HostName")).text();
            host.group = entry.firstChildElement(QLatin1String("UserGroup")).text();
            host.address = entry.firstChildElement(QLatin1String("HostAddress")).text();
            // We added the originally configured host in readConfig(). But if
            // it matches one of the ones in the XML config (as presumably it
            // should), remove the original and use the one with the pretty name.
            if (!matchedGw && host.address == d->hosts.at(0).address) {
                d->hosts.removeFirst();
                matchedGw = true;
            }
            d->hosts.append(host);
        }
    }

    for (int i = 0; i < d->hosts.size(); i++) {
        d->ui.cmbHosts->addItem(d->hosts.at(i).name, i);
        if (d->secrets["lasthost"] == d->hosts.at(i).name || d->secrets["lasthost"] == d->hosts.at(i).address)
            d->ui.cmbHosts->setCurrentIndex(i);
    }

    if (d->secrets["autoconnect"] == "yes") {
        d->ui.chkAutoconnect->setChecked(true);
        connectHost();
    }
    if (!d->secrets["certsigs"].isEmpty()) {
        d->certificateFingerprints.append(d->secrets["certsigs"].split('\t'));
    }
    d->certificateFingerprints.removeDuplicates();
}

void OpenconnectAuthWidget::acceptDialog()
{
    KDialog *dialog = qobject_cast<KDialog*>(parentWidget());
    if (dialog) {
        dialog->accept();
    }
}

// This starts the worker thread, which connects to the selected AnyConnect host
// and retrieves the login form
void OpenconnectAuthWidget::connectHost()
{
    Q_D(OpenconnectAuthWidget);
    d->userQuit = true;
    if (write(d->cancelPipes[1], "x", 1)) {
        // not a lot we can do
    }
    d->workerWaiting.wakeAll();
    d->worker->wait();
    d->userQuit = false;

    /* Suck out the cancel byte(s) */
    char buf;
    while (read(d->cancelPipes[0], &buf, 1) == 1)
        ;
    deleteAllFromLayout(d->ui.loginBoxLayout);
    int i = d->ui.cmbHosts->currentIndex();
    if (i == -1)
        return;
    i = d->ui.cmbHosts->itemData(i).toInt();
    const VPNHost &host = d->hosts.at(i);
    if (openconnect_parse_url(d->vpninfo, host.address.toAscii().data())) {
        qWarning() << "Failed to parse server URL" << host.address;
        openconnect_set_hostname(d->vpninfo, strdup(host.address.toAscii().data()));
    }
    if (!openconnect_get_urlpath(d->vpninfo) && !host.group.isEmpty())
        openconnect_set_urlpath(d->vpninfo, strdup(host.group.toAscii().data()));
    d->secrets["lasthost"] = host.name;
    addFormInfo(QLatin1String("dialog-information"), i18n("Contacting host, please wait..."));
    d->worker->start();
}

QVariantMap OpenconnectAuthWidget::setting(bool agentOwned) const
{
    Q_D(const OpenconnectAuthWidget);
    Q_UNUSED(agentOwned)

    NMStringMap secrets;
    QVariantMap secretData;

    secrets.unite(d->secrets);
    QString host(openconnect_get_hostname(d->vpninfo));
    QString port = QString::number(openconnect_get_port(d->vpninfo));
    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY), host + ':' + port);

    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_COOKIE), QLatin1String(openconnect_get_cookie(d->vpninfo)));
    openconnect_clear_cookie(d->vpninfo);

    OPENCONNECT_X509 *cert = openconnect_get_peer_cert(d->vpninfo);
    char fingerprint[41];
    openconnect_get_cert_sha1(d->vpninfo, cert, fingerprint);
    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_GWCERT), QLatin1String(fingerprint));
    secrets.insert(QLatin1String("certsigs"), d->certificateFingerprints.join("\t"));
    secrets.insert(QLatin1String("autoconnect"), d->ui.chkAutoconnect->isChecked() ? "yes" : "no");

    NMStringMap::iterator i = secrets.begin();
    while (i != secrets.end()) {
        if (i.value().isEmpty())
            i = secrets.erase(i);
        else
            i++;
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}

void OpenconnectAuthWidget::writeNewConfig(const QString & buf)
{
    Q_D(OpenconnectAuthWidget);
    d->secrets["xmlconfig"] = buf;
}

void OpenconnectAuthWidget::updateLog(const QString &message, const int &level)
{
    Q_D(OpenconnectAuthWidget);
    QPair<QString, int> pair;
    pair.first = message;
    if (pair.first.endsWith(QLatin1String("\n")))
        pair.first.chop(1);
    switch (level)
    {
    case PRG_ERR:
        pair.second = OpenconnectAuthWidgetPrivate::Error;
        break;
    case PRG_INFO:
        pair.second = OpenconnectAuthWidgetPrivate::Info;
        break;
    case PRG_DEBUG:
        pair.second = OpenconnectAuthWidgetPrivate::Debug;
        break;
    case PRG_TRACE:
        pair.second = OpenconnectAuthWidgetPrivate::Trace;
        break;
    }
    if (pair.second <= d->ui.cmbLogLevel->currentIndex()) {
        d->ui.serverLog->append(pair.first);
    }

    d->serverLog.append(pair);
    if (d->serverLog.size() > 100) {
        d->serverLog.removeFirst();
    }
}

void OpenconnectAuthWidget::logLevelChanged(int newLevel)
{
    Q_D(OpenconnectAuthWidget);
    d->ui.serverLog->clear();
    QList<QPair<QString, int> >::const_iterator i;

    for (i = d->serverLog.constBegin(); i != d->serverLog.constEnd(); ++i) {
        QPair<QString, int> pair = *i;
        if(pair.second <= newLevel) {
            d->ui.serverLog->append(pair.first);
        }
    }
}

void OpenconnectAuthWidget::addFormInfo(const QString &iconName, const QString &message)
{
    Q_D(OpenconnectAuthWidget);
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *icon = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(icon->sizePolicy().hasHeightForWidth());
    icon->setSizePolicy(sizePolicy);
    icon->setMinimumSize(QSize(16, 16));
    icon->setMaximumSize(QSize(16, 16));
    layout->addWidget(icon);

    QLabel *text = new QLabel(this);
    text->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
    text->setWordWrap(false);
    layout->addWidget(text);

    icon->setPixmap(KIcon(iconName).pixmap(QSize(16,16)));
    text->setText(message);

    d->ui.loginBoxLayout->addLayout(layout);
}

void OpenconnectAuthWidget::processAuthForm(struct oc_auth_form *form)
{
    Q_D(OpenconnectAuthWidget);
    deleteAllFromLayout(d->ui.loginBoxLayout);
    if (form->banner) {
        addFormInfo(QLatin1String("dialog-information"), form->banner);
    }
    if (form->message) {
        addFormInfo(QLatin1String("dialog-information"), form->message);
    }
    if (form->error) {
        addFormInfo(QLatin1String("dialog-error"), form->error);
    }

    struct oc_form_opt *opt;
    QFormLayout *layout = new QFormLayout();
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QCheckBox *togglePasswordMode = new QCheckBox(this);
    togglePasswordMode->setText(i18n("&Show password"));
    togglePasswordMode->setChecked(false);
    connect(togglePasswordMode, SIGNAL(toggled(bool)), this, SLOT(passwordModeToggled(bool)));
    int passwordnumber = 0;
    bool focusSet = false;
    for (opt = form->opts; opt; opt = opt->next) {
        if (opt->type == OC_FORM_OPT_HIDDEN)
            continue;
        QLabel *text = new QLabel(this);
        text->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        text->setText(QString(opt->label));
        QWidget *widget = 0;
        const QString key = QString("form:%1:%2").arg(QLatin1String(form->auth_id)).arg(QLatin1String(opt->name));
        const QString value = d->secrets.value(key);
        if (opt->type == OC_FORM_OPT_PASSWORD || opt->type == OC_FORM_OPT_TEXT) {
            KLineEdit *le = new KLineEdit(this);
            if (opt->type == OC_FORM_OPT_PASSWORD) {
                le->setPasswordMode(true);
                passwordnumber++;
            }
            else {
                le->setText(value);
            }
            if (!focusSet && le->text().isEmpty()) {
                le->setFocus(Qt::OtherFocusReason);
                focusSet = true;
            }
            widget = qobject_cast<QWidget*>(le);
        } else if (opt->type == OC_FORM_OPT_SELECT) {
            KComboBox *cmb = new KComboBox(this);
            struct oc_form_opt_select *sopt = reinterpret_cast<oc_form_opt_select *>(opt);
            for (int i = 0; i < sopt->nr_choices; i++) {
                cmb->addItem(QString::fromUtf8(sopt->choices[i].label), QString::fromUtf8(sopt->choices[i].name));
                if (value == QString::fromUtf8(sopt->choices[i].name))
                    cmb->setCurrentIndex(i);
            }
            widget = qobject_cast<QWidget*>(cmb);
        }
        if (widget) {
            widget->setProperty("openconnect_opt", (quintptr)opt);
            widget->setSizePolicy(policy);
            layout->addRow(text, widget);
        }
    }
    d->ui.loginBoxLayout->addLayout(layout);
    d->ui.loginBoxLayout->addWidget(togglePasswordMode);
    if (passwordnumber == 0)
        togglePasswordMode->setVisible(false);
    KDialogButtonBox *box = new KDialogButtonBox(this);
    QPushButton *btn = box->addButton(QDialogButtonBox::Ok);
    btn->setText(i18n("Login"));
    btn->setDefault(true);
    d->ui.loginBoxLayout->addWidget(box);
    box->setProperty("openconnect_form", (quintptr)form);

    connect(box, SIGNAL(accepted()), this, SLOT(formLoginClicked()));
}

void OpenconnectAuthWidget::validatePeerCert(const QString &fingerprint,
                                             const QString &peerCert, const QString &reason, bool *accepted)
{
    Q_D(OpenconnectAuthWidget);

    if (!d->certificateFingerprints.contains(fingerprint)) {
        QWidget *widget = new QWidget();
        QVBoxLayout *verticalLayout;
        QHBoxLayout *horizontalLayout;
        QLabel *icon;
        QLabel *infoText;
        KTextBrowser *certificate;

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
        infoText->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout->addWidget(infoText);

        verticalLayout->addLayout(horizontalLayout);

        certificate = new KTextBrowser(widget);
        certificate->setTextInteractionFlags(Qt::TextSelectableByMouse);
        certificate->setOpenLinks(false);

        verticalLayout->addWidget(certificate);

        icon->setPixmap(KIcon("dialog-information").pixmap(QSize(48,48)));
        infoText->setText(i18n("Check failed for certificate from VPN server \"%1\".\n"
                               "Reason: %2\nAccept it anyway?", openconnect_get_hostname(d->vpninfo),reason));
        infoText->setWordWrap(true);
        certificate->setText(peerCert);

        QWeakPointer<KDialog> dialog = new KDialog(this);
        dialog.data()->setWindowModality(Qt::WindowModal);
        dialog.data()->setButtons(KDialog::Yes | KDialog::No);
        dialog.data()->setMainWidget(widget);
        if(dialog.data()->exec() == KDialog::Yes) {
            d->certificateFingerprints.append(fingerprint);
            *accepted = true;
        } else {
            *accepted = false;
        }
        if (dialog) {
            dialog.data()->deleteLater();
        }
        widget->deleteLater();
    } else {
        *accepted = true;
    }
    d->mutex.lock();
    d->workerWaiting.wakeAll();
    d->mutex.unlock();
}

// Writes the user input from the form into the oc_auth_form structs we got from
// libopenconnect, and wakes the worker thread up to try to log in and obtain a
// cookie with this data
void OpenconnectAuthWidget::formLoginClicked()
{
    Q_D(OpenconnectAuthWidget);
    int lastIndex = d->ui.loginBoxLayout->count() - 1;
    QLayout *layout = d->ui.loginBoxLayout->itemAt(lastIndex - 2)->layout();
    struct oc_auth_form *form = (struct oc_auth_form *) d->ui.loginBoxLayout->itemAt(lastIndex)->widget()->property("openconnect_form").value<quintptr>();

    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *item = layout->itemAt(i);
        QWidget *widget = item->widget();
        if (widget && widget->property("openconnect_opt").isValid()) {
            struct oc_form_opt *opt = (struct oc_form_opt *) widget->property("openconnect_opt").value<quintptr>();
            QString key = QString("form:%1:%2").arg(QLatin1String(form->auth_id)).arg(QLatin1String(opt->name));
            if (opt->type == OC_FORM_OPT_PASSWORD || opt->type == OC_FORM_OPT_TEXT) {
                KLineEdit *le = qobject_cast<KLineEdit*>(widget);
                QByteArray text = le->text().toUtf8();
                opt->value = strdup(text.data());
                if (opt->type == OC_FORM_OPT_TEXT) {
                    d->secrets.insert(key,le->text());
                }
            } else if (opt->type == OC_FORM_OPT_SELECT) {
                KComboBox *cbo = qobject_cast<KComboBox*>(widget);
                QByteArray text = cbo->itemData(cbo->currentIndex()).toString().toAscii();
                opt->value = strdup(text.data());
                d->secrets.insert(key,cbo->itemData(cbo->currentIndex()).toString());
            }
        }
    }
    deleteAllFromLayout(d->ui.loginBoxLayout);
    d->workerWaiting.wakeAll();
}

void OpenconnectAuthWidget::workerFinished(const int &ret)
{
    Q_D(OpenconnectAuthWidget);
    if (ret < 0) {
        QString message;
        QList<QPair<QString, int> >::const_iterator i;
        for (i = d->serverLog.constEnd()-1; i >= d->serverLog.constBegin(); --i) {
            QPair<QString, int> pair = *i;
            if(pair.second <= OpenconnectAuthWidgetPrivate::Error) {
                message = pair.first;
                break;
            }
        }
        if (message.isEmpty())
            message = i18n("Connection attempt was unsuccessful.");
        deleteAllFromLayout(d->ui.loginBoxLayout);
        addFormInfo(QLatin1String("dialog-error"), message);
    } else {
        deleteAllFromLayout(d->ui.loginBoxLayout);
        acceptDialog();
    }
}

void OpenconnectAuthWidget::deleteAllFromLayout(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QLayout *itemLayout = item->layout()) {
            deleteAllFromLayout(itemLayout);
            itemLayout->deleteLater();
        }
        else
            item->widget()->deleteLater();
        delete item;
    }
    layout->invalidate();
}

void OpenconnectAuthWidget::viewServerLogToggled(bool toggled)
{
    Q_D(OpenconnectAuthWidget);
    d->ui.lblLogLevel->setVisible(toggled);
    d->ui.cmbLogLevel->setVisible(toggled);
    if (toggled) {
        QLayoutItem *item = d->ui.verticalLayout->takeAt(4);
        if (item) {
            delete item;
        }
        QSizePolicy policy = d->ui.serverLogBox->sizePolicy();
        policy.setVerticalPolicy(QSizePolicy::Expanding);
        d->ui.serverLogBox->setSizePolicy(policy);
        d->ui.serverLog->setVisible(true);
    } else {
        QSpacerItem *verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        d->ui.verticalLayout->addItem(verticalSpacer);
        d->ui.serverLog->setVisible(false);
        QSizePolicy policy = d->ui.serverLogBox->sizePolicy();
        policy.setVerticalPolicy(QSizePolicy::Fixed);
        d->ui.serverLogBox->setSizePolicy(policy);
    }
}

void OpenconnectAuthWidget::passwordModeToggled(bool toggled)
{
    Q_D(OpenconnectAuthWidget);
    int lastIndex = d->ui.loginBoxLayout->count() - 1;
    QLayout *layout = d->ui.loginBoxLayout->itemAt(lastIndex - 2)->layout();
    for (int i = 0; i < layout->count(); i++) {
        QLayoutItem *item = layout->itemAt(i);
        QWidget *widget = item->widget();
        if (widget && widget->property("openconnect_opt").isValid()) {
            struct oc_form_opt *opt = (struct oc_form_opt *) widget->property("openconnect_opt").value<quintptr>();
            if (opt->type == OC_FORM_OPT_PASSWORD) {
                KLineEdit *le = qobject_cast<KLineEdit*>(widget);
                le->setPasswordMode(!toggled);
            }
        }
    }
}
