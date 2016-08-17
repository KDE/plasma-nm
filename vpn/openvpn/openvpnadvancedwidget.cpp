/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

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

#include "openvpnadvancedwidget.h"
#include "ui_openvpnadvanced.h"
#include "nm-openvpn-service.h"
#include "settingwidget.h"

#include <QStandardPaths>
#include <QUrl>
#include <QComboBox>

#include <KLocalizedString>
#include <KProcess>
#include <KAcceleratorManager>

class OpenVpnAdvancedWidget::Private {
public:
    NetworkManager::VpnSetting::Ptr setting;
    KProcess *openvpnProcess;
    QByteArray openVpnCiphers;
    bool gotOpenVpnCiphers;
    bool readConfig;

    class EnumProxyType
    {
    public:
        enum ProxyType {NotRequired = 0, HTTP = 1, SOCKS = 2};
    };
    class EnumHashingAlgorithms
    {
    public:
        enum HashingAlgorithms {Default = 0, None, Md4, Md5, Sha1, Sha224, Sha256, Sha384, Sha512, Ripemd160};
    };
};

OpenVpnAdvancedWidget::OpenVpnAdvancedWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::OpenVpnAdvancedWidget)
    , d(new Private)
{
    m_ui->setupUi(this);

    setWindowTitle(i18nc("@title: window advanced openvpn properties", "Advanced OpenVPN properties"));

    d->openvpnProcess = 0;
    d->gotOpenVpnCiphers = false;
    d->readConfig = false;
    d->setting = setting;

    m_ui->proxyPassword->setPasswordOptionsEnabled(true);
    m_ui->proxyPassword->setPasswordOptionEnabled(PasswordField::NotRequired, true);

    connect(m_ui->cmbProxyType, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &OpenVpnAdvancedWidget::proxyTypeChanged);

    // start openVPN process and get its cipher list
    const QString openVpnBinary = QStandardPaths::findExecutable("openvpn", QStringList() << "/sbin" << "/usr/sbin");
    const QStringList args(QLatin1String("--show-ciphers"));

    d->openvpnProcess = new KProcess(this);
    d->openvpnProcess->setOutputChannelMode(KProcess::OnlyStdoutChannel);
    d->openvpnProcess->setReadChannel(QProcess::StandardOutput);
    connect(d->openvpnProcess, static_cast<void (KProcess::*)(QProcess::ProcessError)>(&KProcess::error), this, &OpenVpnAdvancedWidget::openVpnError);
    connect(d->openvpnProcess, &KProcess::readyReadStandardOutput, this, &OpenVpnAdvancedWidget::gotOpenVpnOutput);
    connect(d->openvpnProcess, static_cast<void (KProcess::*)(int, QProcess::ExitStatus)>(&KProcess::finished), this, &OpenVpnAdvancedWidget::openVpnFinished);
    d->openvpnProcess->setProgram(openVpnBinary, args);

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &OpenVpnAdvancedWidget::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected, this, &OpenVpnAdvancedWidget::reject);

    KAcceleratorManager::manage(this);

    if (d->setting && !d->setting->isNull()) {
        loadConfig();
    }
}

OpenVpnAdvancedWidget::~OpenVpnAdvancedWidget()
{
    delete d;
}

void OpenVpnAdvancedWidget::init()
{
    d->openvpnProcess->start();
}

void OpenVpnAdvancedWidget::gotOpenVpnOutput()
{
    d->openVpnCiphers.append(d->openvpnProcess->readAll());
}

void OpenVpnAdvancedWidget::openVpnError(QProcess::ProcessError)
{
    m_ui->cboCipher->removeItem(0);
    m_ui->cboCipher->addItem(i18nc("@item:inlistbox Item added when OpenVPN cipher lookup failed", "OpenVPN cipher lookup failed"));
}

void OpenVpnAdvancedWidget::openVpnFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_ui->cboCipher->removeItem(0);
    if (!exitCode && exitStatus == QProcess::NormalExit) {
        m_ui->cboCipher->addItem(i18nc("@item::inlist Default openvpn cipher item", "Default"));
        const QList<QByteArray> rawOutputLines = d->openVpnCiphers.split('\n');
        bool foundFirstSpace = false;
        Q_FOREACH (const QByteArray &cipher, rawOutputLines) {
            if (cipher.length() == 0) {
                foundFirstSpace = true;
            } else if (foundFirstSpace) {
                m_ui->cboCipher->addItem(QString::fromLocal8Bit(cipher.left(cipher.indexOf(' '))));
            }
        }

        if (m_ui->cboCipher->count()) {
            m_ui->cboCipher->setEnabled(true);
        } else {
            m_ui->cboCipher->addItem(i18nc("@item:inlistbox Item added when OpenVPN cipher lookup failed", "No OpenVPN ciphers found"));
        }
    } else {
        m_ui->cboCipher->addItem(i18nc("@item:inlistbox Item added when OpenVPN cipher lookup failed", "OpenVPN cipher lookup failed"));
    }
    delete d->openvpnProcess;
    d->openvpnProcess = 0;
    d->openVpnCiphers = QByteArray();
    d->gotOpenVpnCiphers = true;

    if (d->readConfig) {
        const NMStringMap dataMap = d->setting->data();
        if (dataMap.contains(NM_OPENVPN_KEY_CIPHER)) {
            m_ui->cboCipher->setCurrentIndex(m_ui->cboCipher->findText(dataMap.value(NM_OPENVPN_KEY_CIPHER)));
        }
    }
}

void OpenVpnAdvancedWidget::loadConfig()
{
    const NMStringMap dataMap = d->setting->data();
    const NMStringMap secrets = d->setting->secrets();

    // Optional Settings
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_PORT))) {
        m_ui->sbCustomPort->setValue(dataMap[QLatin1String(NM_OPENVPN_KEY_PORT)].toUInt());
    } else {
        m_ui->sbCustomPort->setValue(0);
    }
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU))) {
        m_ui->sbMtu->setValue(dataMap[QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU)].toUInt());
    } else {
        m_ui->sbMtu->setValue(0);
    }
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE))) {
        m_ui->sbUdpFragmentSize->setValue(dataMap[QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE)].toUInt());
    } else {
        m_ui->sbUdpFragmentSize->setValue(0);
    }
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS))) {
        m_ui->chkUseCustomReneg->setChecked(true);
        m_ui->sbCustomReneg->setValue(dataMap[QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS)].toUInt());
    } else {
        m_ui->chkUseCustomReneg->setChecked(false);
        m_ui->sbCustomReneg->setValue(0);
    }
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_COMP_LZO))) {
        const QString compLzo = dataMap[QLatin1String(NM_OPENVPN_KEY_COMP_LZO)];
        if (compLzo == QLatin1String("no-by-default")) {
            m_ui->cmbUseLZO->setCurrentIndex(0);
        } else if (compLzo == QLatin1String("yes")) {
            m_ui->cmbUseLZO->setCurrentIndex(1);
        } else {
            m_ui->cmbUseLZO->setCurrentIndex(2);
        }
        m_ui->chkUseLZO->setChecked(true);
    }
    m_ui->chkUseTCP->setChecked(dataMap[QLatin1String(NM_OPENVPN_KEY_PROTO_TCP)] == QLatin1String("yes"));
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_DEV_TYPE))) {
        m_ui->chkUseVirtualDeviceType->setChecked(true);
        if (dataMap[QLatin1String(NM_OPENVPN_KEY_DEV_TYPE)] == QLatin1String("tap")) {
            m_ui->cmbDeviceType->setCurrentIndex(1);
        }
    }
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_DEV))) {
        m_ui->chkUseVirtualDeviceName->setChecked(true);
        m_ui->leVirtualDeviceName->setText(dataMap[QLatin1String(NM_OPENVPN_KEY_DEV)]);
    }
    m_ui->chkMssRestrict->setChecked(dataMap[QLatin1String(NM_OPENVPN_KEY_MSSFIX)] == QLatin1String("yes"));
    m_ui->chkRandRemHosts->setChecked(dataMap[QLatin1String(NM_OPENVPN_KEY_REMOTE_RANDOM)] == QLatin1String("yes"));

    // Optional Security Settings
    const QString hmacKeyAuth = dataMap[QLatin1String(NM_OPENVPN_KEY_AUTH)];
    if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_NONE)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::None);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_RSA_MD4)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Md4);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_MD5)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Md5);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA1)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha1);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA224)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha224);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA256)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha256);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA384)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha384);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA512)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha512);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_RIPEMD160)) {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Ripemd160);
    } else {
        m_ui->cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Default);
    }

    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_KEYSIZE))) {
        m_ui->chkUseCustomCipherKey->setChecked(true);
        m_ui->sbCustomCipherKey->setValue(dataMap[QLatin1String(NM_OPENVPN_KEY_KEYSIZE)].toUInt());
    }

    // ciphers populated above?
    if (d->gotOpenVpnCiphers && dataMap.contains(QLatin1String(NM_OPENVPN_KEY_CIPHER))) {
        m_ui->cboCipher->setCurrentIndex(m_ui->cboCipher->findText(dataMap[QLatin1String(NM_OPENVPN_KEY_CIPHER)]));
    }

    // Optional TLS
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE))) {
        m_ui->subjectMatch->setText(dataMap[QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE)]);
    }

    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_REMOTE_CERT_TLS))) {
        const QString remoteCertTls = dataMap[QLatin1String(NM_OPENVPN_KEY_REMOTE_CERT_TLS)];
        m_ui->chkRemoteCertTls->setChecked(true);
        m_ui->labelRemoteCertTls->setEnabled(true);
        m_ui->cmbRemoteCertTls->setEnabled(true);
        m_ui->cmbRemoteCertTls->setCurrentIndex(remoteCertTls == QLatin1String("server") ? 0 : 1);
    }

    m_ui->useExtraTlsAuth->setChecked(!dataMap[QLatin1String(NM_OPENVPN_KEY_TA)].isEmpty());
    m_ui->kurlTlsAuthKey->setUrl(QUrl::fromLocalFile(dataMap[QLatin1String(NM_OPENVPN_KEY_TA)]));
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_TA_DIR))) {
        const uint tlsAuthDirection = dataMap[QLatin1String(NM_OPENVPN_KEY_TA_DIR)].toUInt();
        m_ui->cboDirection->setCurrentIndex(tlsAuthDirection + 1);
    }

    // Proxies
    if (dataMap[QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE)] == QLatin1String("http")) {
        m_ui->cmbProxyType->setCurrentIndex(Private::EnumProxyType::HTTP);
    } else if (dataMap[QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE)] == QLatin1String("socks")) {
        m_ui->cmbProxyType->setCurrentIndex(Private::EnumProxyType::SOCKS);
    } else {
        m_ui->cmbProxyType->setCurrentIndex(Private::EnumProxyType::NotRequired);
    }
    proxyTypeChanged(m_ui->cmbProxyType->currentIndex());
    m_ui->proxyServerAddress->setText(dataMap[QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER)]);
    if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT))) {
        m_ui->sbProxyPort->setValue(dataMap[QLatin1String(NM_OPENVPN_KEY_PROXY_PORT)].toUInt());
    } else {
        m_ui->sbProxyPort->setValue(0);
    }
    m_ui->chkProxyRetry->setChecked(dataMap[QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY)] == QLatin1String("yes"));
    m_ui->proxyUsername->setText(dataMap[QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME)]);
    d->readConfig = true;

    NetworkManager::Setting::SecretFlags type;
    type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"].toInt();
    if (!(type & NetworkManager::Setting::NotSaved || type & NetworkManager::Setting::NotRequired)) {
        m_ui->proxyPassword->setText(secrets.value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD)));
    }
    fillOnePasswordCombo(m_ui->proxyPassword, type);
}

void OpenVpnAdvancedWidget::fillOnePasswordCombo(PasswordField *passwordField, NetworkManager::Setting::SecretFlags type)
{
    if (type.testFlag(NetworkManager::Setting::None)) {
        passwordField->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (type.testFlag(NetworkManager::Setting::AgentOwned)) {
        passwordField->setPasswordOption(PasswordField::StoreForUser);
    } else if (type.testFlag(NetworkManager::Setting::NotSaved)) {
        passwordField->setPasswordOption(PasswordField::AlwaysAsk);
    } else {
        passwordField->setPasswordOption(PasswordField::PasswordField::NotRequired);
    }
}

NetworkManager::VpnSetting::Ptr OpenVpnAdvancedWidget::setting() const
{
    NMStringMap data;
    NMStringMap secretData;

    // optional settings
    if (m_ui->sbCustomPort->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_PORT), QString::number(m_ui->sbCustomPort->value()));
    }
    if (m_ui->sbMtu->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU), QString::number(m_ui->sbMtu->value()));
    }
    if (m_ui->sbUdpFragmentSize->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE), QString::number(m_ui->sbUdpFragmentSize->value()));
    }
    if (m_ui->chkUseCustomReneg->isChecked()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS), QString::number(m_ui->sbCustomReneg->value()));
    }
    data.insert(QLatin1String(NM_OPENVPN_KEY_PROTO_TCP), m_ui->chkUseTCP->isChecked() ? QLatin1String("yes") : QLatin1String("no"));

    if (m_ui->chkUseLZO->isChecked()) {
        switch (m_ui->cmbUseLZO->currentIndex()) {
        case 0:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMP_LZO), QLatin1String("no-by-default"));
            break;
        case 1:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMP_LZO), QLatin1String("yes"));
            break;
        case 2:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMP_LZO), QLatin1String("adaptive"));
            break;
        }
    }

    if (m_ui->chkUseVirtualDeviceType->isChecked()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_DEV_TYPE), m_ui->cmbDeviceType->currentIndex() == 0 ? QLatin1String("tun") : QLatin1String("tap"));
    }
    if (m_ui->chkUseVirtualDeviceName->isChecked()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_DEV), m_ui->leVirtualDeviceName->text());
    }
    data.insert(QLatin1String(NM_OPENVPN_KEY_MSSFIX), m_ui->chkMssRestrict->isChecked() ? QLatin1String("yes") : QLatin1String("no"));
    data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_RANDOM), m_ui->chkRandRemHosts->isChecked() ? QLatin1String("yes") : QLatin1String("no"));

    // Optional Security
    switch (m_ui->cboHmac->currentIndex()) {
    case Private::EnumHashingAlgorithms::Default:
        break;
    case Private::EnumHashingAlgorithms::None:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_NONE));
        break;
    case Private::EnumHashingAlgorithms::Md4:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_RSA_MD4));
        break;
    case Private::EnumHashingAlgorithms::Md5:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_MD5));
        break;
    case Private::EnumHashingAlgorithms::Sha1:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA1));
        break;
    case Private::EnumHashingAlgorithms::Sha224:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA224));
        break;
    case Private::EnumHashingAlgorithms::Sha256:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA256));
        break;
    case Private::EnumHashingAlgorithms::Sha384:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA384));
        break;
    case Private::EnumHashingAlgorithms::Sha512:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA512));
        break;
    case Private::EnumHashingAlgorithms::Ripemd160:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_RIPEMD160));
        break;
    }

    if (m_ui->chkUseCustomCipherKey->isChecked()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_KEYSIZE), QString::number(m_ui->sbCustomCipherKey->value()));
    }

    if (m_ui->cboCipher->currentIndex() != 0) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CIPHER), m_ui->cboCipher->currentText());
    }

    // optional tls authentication
    if (!m_ui->subjectMatch->text().isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE), m_ui->subjectMatch->text());
    }

    if (m_ui->chkRemoteCertTls->isChecked()) {
        if (m_ui->cmbRemoteCertTls->currentIndex() == 0) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_CERT_TLS), QLatin1String("server"));
        } else {
            data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_CERT_TLS), QLatin1String("client"));
        }
    }

    if (m_ui->useExtraTlsAuth->isChecked()) {
        QUrl tlsAuthKeyUrl = m_ui->kurlTlsAuthKey->url();
        if (!tlsAuthKeyUrl.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TA), tlsAuthKeyUrl.path());
        }
        if (m_ui->cboDirection->currentIndex() > 0) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TA_DIR), QString::number(m_ui->cboDirection->currentIndex() - 1));
        }
    }

    // Proxies
    switch (m_ui->cmbProxyType->currentIndex()) {
    case Private::EnumProxyType::NotRequired:
        break;
    case Private::EnumProxyType::HTTP:
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), QLatin1String("http"));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), m_ui->proxyServerAddress->text());
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(m_ui->sbProxyPort->value()));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), m_ui->chkProxyRetry->isChecked() ? QLatin1String("yes") : QLatin1String("no"));
        if (!m_ui->proxyUsername->text().isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME), m_ui->proxyUsername->text());
            secretData.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD), m_ui->proxyPassword->text());
            handleOnePasswordType(m_ui->proxyPassword, QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"), data);
        }
        break;
    case Private::EnumProxyType::SOCKS:
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), QLatin1String("socks"));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), m_ui->proxyServerAddress->text());
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(m_ui->sbProxyPort->value()));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), m_ui->chkProxyRetry->isChecked() ? QLatin1String("yes") : QLatin1String("no"));
        break;
    }

    d->setting->setData(data);
    d->setting->setSecrets(secretData);

    return d->setting;
}

void OpenVpnAdvancedWidget::proxyTypeChanged(int type)
{
    switch (type) {
    case Private::EnumProxyType::NotRequired:
        m_ui->proxyServerAddress->setEnabled(false);
        m_ui->sbProxyPort->setEnabled(false);
        m_ui->chkProxyRetry->setEnabled(false);
        m_ui->proxyUsername->setEnabled(false);
        m_ui->proxyPassword->setEnabled(false);
        break;
    case Private::EnumProxyType::HTTP:
        m_ui->proxyServerAddress->setEnabled(true);
        m_ui->sbProxyPort->setEnabled(true);
        m_ui->chkProxyRetry->setEnabled(true);
        m_ui->proxyUsername->setEnabled(true);
        m_ui->proxyPassword->setEnabled(true);
        break;
    case Private::EnumProxyType::SOCKS:
        m_ui->proxyServerAddress->setEnabled(true);
        m_ui->sbProxyPort->setEnabled(true);
        m_ui->chkProxyRetry->setEnabled(true);
        m_ui->proxyUsername->setEnabled(false);
        m_ui->proxyPassword->setEnabled(false);
        break;
    }
}

void OpenVpnAdvancedWidget::handleOnePasswordType(const PasswordField *passwordField, const QString & key, NMStringMap & data) const
{
    const PasswordField::PasswordOption option = passwordField->passwordOption();
    switch (option) {
    case PasswordField::StoreForAllUsers:
        data.insert(key, QString::number(NetworkManager::Setting::None));
        break;
    case PasswordField::StoreForUser:
        data.insert(key, QString::number(NetworkManager::Setting::AgentOwned));
        break;
    case PasswordField::AlwaysAsk:
        data.insert(key, QString::number(NetworkManager::Setting::NotSaved));
        break;
    case PasswordField::NotRequired:
        data.insert(key, QString::number(NetworkManager::Setting::NotRequired));
        break;
    }
}
