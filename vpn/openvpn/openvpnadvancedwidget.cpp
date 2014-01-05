/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include <KLocalizedString>
#include <KProcess>
#include <KStandardDirs>
#include <KAcceleratorManager>

class OpenVpnAdvancedWidget::Private {
public:
    NetworkManager::VpnSetting::Ptr setting;
    KProcess * openvpnProcess;
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

OpenVpnAdvancedWidget::OpenVpnAdvancedWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::OpenVpnAdvancedWidget),
    d(new Private)
{
    m_ui->setupUi(this);
    m_ui->kurlTlsAuthKey->setMode(KFile::LocalOnly);

    setWindowTitle(i18n("Advanced OpenVPN properties"));

    d->openvpnProcess = 0;
    d->gotOpenVpnCiphers = false;
    d->readConfig = false;
    d->setting = setting;

    connect(m_ui->proxyPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(proxyPasswordStorageChanged(int)));
    connect(m_ui->chkProxyShowPassword, SIGNAL(toggled(bool)), this, SLOT(proxyPasswordToggled(bool)));
    connect(m_ui->cmbProxyType, SIGNAL(currentIndexChanged(int)), this, SLOT(proxyTypeChanged(int)));

    // start openVPN process and get its cipher list
    const QString openVpnBinary = KStandardDirs::findExe("openvpn", "/sbin:/usr/sbin");
    const QStringList args(QLatin1String("--show-ciphers"));
    d->openvpnProcess = new KProcess(this);
    d->openvpnProcess->setOutputChannelMode(KProcess::OnlyStdoutChannel);
    d->openvpnProcess->setReadChannel(QProcess::StandardOutput);
    connect(d->openvpnProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(openVpnError(QProcess::ProcessError)));
    connect(d->openvpnProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(gotOpenVpnOutput()));
    connect(d->openvpnProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(openVpnFinished(int,QProcess::ExitStatus)));

    d->openvpnProcess->setProgram(openVpnBinary, args);

    connect(m_ui->buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), SLOT(reject()));

    KAcceleratorManager::manage(this);

    loadConfig();
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
        bool foundFirstSpace = false;;
        foreach (const QByteArray &cipher, rawOutputLines) {
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
    if (dataMap.contains(NM_OPENVPN_KEY_PORT)) {
        m_ui->sbCustomPort->setValue(dataMap[NM_OPENVPN_KEY_PORT].toUInt());
    } else {
        m_ui->sbCustomPort->setValue(0);
    }
    if (dataMap.contains(NM_OPENVPN_KEY_TUNNEL_MTU)) {
        m_ui->sbMtu->setValue(dataMap[NM_OPENVPN_KEY_TUNNEL_MTU].toUInt());
    } else {
        m_ui->sbMtu->setValue(0);
    }
    if (dataMap.contains(NM_OPENVPN_KEY_FRAGMENT_SIZE)) {
        m_ui->sbUdpFragmentSize->setValue(dataMap[NM_OPENVPN_KEY_FRAGMENT_SIZE].toUInt());
    } else {
        m_ui->sbUdpFragmentSize->setValue(0);
    }
    if (dataMap.contains(NM_OPENVPN_KEY_RENEG_SECONDS)) {
        m_ui->chkUseCustomReneg->setChecked(true);
        m_ui->sbCustomReneg->setValue(dataMap[NM_OPENVPN_KEY_RENEG_SECONDS].toUInt());
    } else {
        m_ui->chkUseCustomReneg->setChecked(false);
        m_ui->sbCustomReneg->setValue(0);
    }
    m_ui->chkUseLZO->setChecked( dataMap[NM_OPENVPN_KEY_COMP_LZO] == "yes" );
    m_ui->chkUseTCP->setChecked( dataMap[NM_OPENVPN_KEY_PROTO_TCP] == "yes" );
    m_ui->chkUseTAP->setChecked( dataMap[NM_OPENVPN_KEY_TAP_DEV] == "yes" );
    m_ui->chkMssRestrict->setChecked(dataMap[NM_OPENVPN_KEY_MSSFIX] == "yes");
    // Optional Security Settings
    QString hmacKeyAuth = dataMap[NM_OPENVPN_KEY_AUTH];
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
    // ciphers populated above?
    if (d->gotOpenVpnCiphers && dataMap.contains(NM_OPENVPN_KEY_CIPHER)) {
        m_ui->cboCipher->setCurrentIndex(m_ui->cboCipher->findText(dataMap[NM_OPENVPN_KEY_CIPHER]));
    }

    // Optional TLS
    if (dataMap.contains(NM_OPENVPN_KEY_TLS_REMOTE)) {
        m_ui->subjectMatch->setText(dataMap[NM_OPENVPN_KEY_TLS_REMOTE]);
    }
    m_ui->useExtraTlsAuth->setChecked(!dataMap[NM_OPENVPN_KEY_TA].isEmpty());
    m_ui->kurlTlsAuthKey->setUrl(KUrl(dataMap[NM_OPENVPN_KEY_TA]) );
    if (dataMap.contains(NM_OPENVPN_KEY_TA_DIR)) {
        const uint tlsAuthDirection = dataMap[NM_OPENVPN_KEY_TA_DIR].toUInt();
        m_ui->cboDirection->setCurrentIndex(tlsAuthDirection + 1);
    }
    // Proxies
    if (dataMap[NM_OPENVPN_KEY_PROXY_TYPE] == "http") {
        m_ui->cmbProxyType->setCurrentIndex(Private::EnumProxyType::HTTP);
    } else if (dataMap[NM_OPENVPN_KEY_PROXY_TYPE] == "socks") {
        m_ui->cmbProxyType->setCurrentIndex(Private::EnumProxyType::SOCKS);
    } else {
        m_ui->cmbProxyType->setCurrentIndex(Private::EnumProxyType::NotRequired);
    }
    proxyTypeChanged(m_ui->cmbProxyType->currentIndex());
    m_ui->proxyServerAddress->setText(dataMap[NM_OPENVPN_KEY_PROXY_SERVER]);
    if (dataMap.contains(NM_OPENVPN_KEY_PROXY_PORT)) {
        m_ui->sbProxyPort->setValue(dataMap[NM_OPENVPN_KEY_PROXY_PORT].toUInt());
    } else {
        m_ui->sbProxyPort->setValue(0);
    }
    m_ui->chkProxyRetry->setChecked(dataMap[NM_OPENVPN_KEY_PROXY_RETRY] == "yes");
    m_ui->proxyUsername->setText(dataMap[NM_OPENVPN_KEY_HTTP_PROXY_USERNAME]);
    d->readConfig = true;

    NetworkManager::Setting::SecretFlags type;
    type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"].toInt();
    if (!(type & NetworkManager::Setting::NotSaved || type & NetworkManager::Setting::NotRequired)) {
        m_ui->proxyPassword->setText(secrets.value(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD));
    }
    fillOnePasswordCombo(m_ui->proxyPasswordStorage, type);
}

void OpenVpnAdvancedWidget::setPasswordType(KLineEdit *edit, int type)
{
    edit->setEnabled(type == SettingWidget::EnumPasswordStorageType::Store);
}

void OpenVpnAdvancedWidget::fillOnePasswordCombo(KComboBox * combo, NetworkManager::Setting::SecretFlags type)
{
    if (type.testFlag(NetworkManager::Setting::AgentOwned) || type.testFlag(NetworkManager::Setting::None)) {
        combo->setCurrentIndex(SettingWidget::EnumPasswordStorageType::Store);
    } else if (type.testFlag(NetworkManager::Setting::NotRequired)) {
        combo->setCurrentIndex(SettingWidget::EnumPasswordStorageType::NotRequired);
    } else if (type.testFlag(NetworkManager::Setting::NotSaved)) {
        combo->setCurrentIndex(SettingWidget::EnumPasswordStorageType::AlwaysAsk);
    }
}

NetworkManager::VpnSetting::Ptr OpenVpnAdvancedWidget::setting() const
{
    NMStringMap data;
    NMStringMap secretData;

    // optional settings
    if ( m_ui->sbCustomPort->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_PORT), QString::number(m_ui->sbCustomPort->value()));
    }
    if ( m_ui->sbMtu->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU), QString::number(m_ui->sbMtu->value()));
    }
    if ( m_ui->sbUdpFragmentSize->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE), QString::number(m_ui->sbUdpFragmentSize->value()));
    }
    if (m_ui->chkUseCustomReneg->isChecked()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS), QString::number(m_ui->sbCustomReneg->value()));
    }
    data.insert( QLatin1String(NM_OPENVPN_KEY_PROTO_TCP), m_ui->chkUseTCP->isChecked() ? "yes" : "no" );
    data.insert( QLatin1String(NM_OPENVPN_KEY_COMP_LZO), m_ui->chkUseLZO->isChecked() ? "yes" : "no" );
    data.insert( QLatin1String(NM_OPENVPN_KEY_TAP_DEV), m_ui->chkUseTAP->isChecked() ? "yes" : "no" );
    data.insert( QLatin1String(NM_OPENVPN_KEY_MSSFIX), m_ui->chkMssRestrict->isChecked() ? "yes" : "no" );

    // Optional Security
    switch ( m_ui->cboHmac->currentIndex()) {
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
    if (m_ui->cboCipher->currentIndex() != 0) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CIPHER), m_ui->cboCipher->currentText());
    }

    // optional tls authentication
    if (!m_ui->subjectMatch->text().isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE), m_ui->subjectMatch->text());
    }
    if (m_ui->useExtraTlsAuth->isChecked()) {
        KUrl tlsAuthKeyUrl = m_ui->kurlTlsAuthKey->url();
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
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), "http");
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), m_ui->proxyServerAddress->text());
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(m_ui->sbProxyPort->value()));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), m_ui->chkProxyRetry->isChecked() ? "yes" : "no");
        if (!m_ui->proxyUsername->text().isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME), m_ui->proxyUsername->text());
            secretData.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD), m_ui->proxyPassword->text());
            handleOnePasswordType(m_ui->proxyPasswordStorage, QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"), data);
        }
        break;
    case Private::EnumProxyType::SOCKS:
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), "socks");
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), m_ui->proxyServerAddress->text());
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(m_ui->sbProxyPort->value()));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), m_ui->chkProxyRetry->isChecked() ? "yes" : "no");
        break;
    }

    d->setting->setData(data);
    d->setting->setSecrets(secretData);

    return d->setting;
}

void OpenVpnAdvancedWidget::proxyPasswordStorageChanged(int index)
{
    setPasswordType(m_ui->proxyPassword, index);
}

void OpenVpnAdvancedWidget::proxyPasswordToggled(bool toggled)
{
    m_ui->proxyPassword->setPasswordMode(!toggled);
}

void OpenVpnAdvancedWidget::proxyTypeChanged(int type)
{
    switch (type)
    {
    case Private::EnumProxyType::NotRequired:
        m_ui->proxyServerAddress->setEnabled(false);
        m_ui->sbProxyPort->setEnabled(false);
        m_ui->chkProxyRetry->setEnabled(false);
        m_ui->proxyUsername->setEnabled(false);
        m_ui->proxyPassword->setEnabled(false);
        m_ui->proxyPasswordStorage->setEnabled(false);
        m_ui->chkProxyShowPassword->setEnabled(false);
        break;
    case Private::EnumProxyType::HTTP:
        m_ui->proxyServerAddress->setEnabled(true);
        m_ui->sbProxyPort->setEnabled(true);
        m_ui->chkProxyRetry->setEnabled(true);
        m_ui->proxyUsername->setEnabled(true);
        m_ui->proxyPasswordStorage->setEnabled(true);
        setPasswordType(m_ui->proxyPassword, m_ui->proxyPasswordStorage->currentIndex());
        m_ui->chkProxyShowPassword->setEnabled(true);
        break;
    case Private::EnumProxyType::SOCKS:
        m_ui->proxyServerAddress->setEnabled(true);
        m_ui->sbProxyPort->setEnabled(true);
        m_ui->chkProxyRetry->setEnabled(true);
        m_ui->proxyUsername->setEnabled(false);
        m_ui->proxyPassword->setEnabled(false);
        m_ui->proxyPasswordStorage->setEnabled(false);
        m_ui->chkProxyShowPassword->setEnabled(false);
        break;
    }
}

uint OpenVpnAdvancedWidget::handleOnePasswordType(const KComboBox * combo, const QString & key, NMStringMap & data) const
{
    uint type = combo->currentIndex();
    switch (type) {
    case SettingWidget::EnumPasswordStorageType::AlwaysAsk:
        data.insert(key, QString::number(NetworkManager::Setting::NotSaved));
        break;
    case SettingWidget::EnumPasswordStorageType::Store:
        data.insert(key, QString::number(NetworkManager::Setting::AgentOwned));
        break;
    case SettingWidget::EnumPasswordStorageType::NotRequired:
        data.insert(key, QString::number(NetworkManager::Setting::NotRequired));
        break;
    }
    return type;
}
