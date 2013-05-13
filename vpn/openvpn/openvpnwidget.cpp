/*
Copyright 2008 Will Stephenson <wstephenson@kde.org>
Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "openvpnwidget.h"
#include "nm-openvpn-service.h"

#include <QDBusMetaType>

#include <KDebug>
#include <KProcess>
#include <KStandardDirs>
#include <KUrlRequester>
#include <KLineEdit>

class OpenVpnSettingWidget::Private
{
public:
    Ui_OpenVPNProp ui;
    NetworkManager::VpnSetting::Ptr setting;
    KProcess * openvpnProcess;
    QByteArray openVpnCiphers;
    bool gotOpenVpnCiphers;
    bool readConfig;
    class EnumPasswordStorageType
    {
    public:
        enum PasswordStorageType {AlwaysAsk = 0, Store, NotRequired};
    };
    class EnumConnectionType
    {
    public:
        enum ConnectionType {Certificates = 0, Psk, Password, CertsPassword};
    };
    class EnumProxyType
    {
    public:
        enum ProxyType {NotRequired = 0, HTTP = 1, SOCKS = 2};
    };
    class EnumHashingAlgorithms
    {
    public:
        enum HashingAlgorithms {Default = 0, None, Md5, Sha1, Sha224, Sha256, Sha384, Sha512, Ripemd160};
    };
    class EnumKeyDirection
    {
    public:
        enum KeyDirection {None = 0, D0, D1};
    };
};


OpenVpnSettingWidget::OpenVpnSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent), d(new Private)
{
    qDBusRegisterMetaType<NMStringMap>();

    d->ui.setupUi(this);
    d->ui.x509CaFile->setMode(KFile::LocalOnly);
    d->ui.x509Cert->setMode(KFile::LocalOnly);
    d->ui.x509Key->setMode(KFile::LocalOnly);
    d->setting = setting;
    d->openvpnProcess = 0;
    d->gotOpenVpnCiphers = false;
    d->readConfig = false;

    // start openVPN process and get its cipher list
    QString openVpnBinary = KStandardDirs::findExe("openvpn", "/sbin:/usr/sbin");
    QStringList args(QLatin1String("--show-ciphers"));
    d->openvpnProcess = new KProcess(this);
    d->openvpnProcess->setOutputChannelMode(KProcess::OnlyStdoutChannel);
    d->openvpnProcess->setReadChannel(QProcess::StandardOutput);
    connect(d->openvpnProcess, SIGNAL(error(QProcess::ProcessError)), this, SLOT(openVpnError(QProcess::ProcessError)));
    connect(d->openvpnProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(gotOpenVpnOutput()));
    connect(d->openvpnProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(openVpnFinished(int,QProcess::ExitStatus)));

    d->openvpnProcess->setProgram(openVpnBinary, args);

    // use requesters' urlSelected signals to set other requester's startDirs to save clicking
    // around the filesystem
    QList<const KUrlRequester *> requesters;
    requesters << d->ui.x509CaFile << d->ui.x509Cert << d->ui.x509Key << d->ui.pskSharedKey << d->ui.passCaFile
               << d->ui.x509PassCaFile << d->ui.x509PassCert << d->ui.x509PassKey << d->ui.kurlTlsAuthKey;
    foreach (const KUrlRequester * requester, requesters) {
        connect(requester, SIGNAL(urlSelected(KUrl)), this, SLOT(updateStartDir(KUrl)));
    }

    connect(d->ui.x509KeyPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(x509KeyPasswordStorageChanged(int)));
    connect(d->ui.passPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(passPasswordStorageChanged(int)));
    connect(d->ui.x509PassKeyPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(x509PassKeyPasswordStorageChanged(int)));
    connect(d->ui.x509PassPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(x509PassPasswordStorageChanged(int)));
    connect(d->ui.proxyPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(proxyPasswordStorageChanged(int)));
    connect(d->ui.chkShowPasswords, SIGNAL(toggled(bool)), this, SLOT(showPasswordsToggled(bool)));
    connect(d->ui.chkProxyShowPassword, SIGNAL(toggled(bool)), this, SLOT(proxyPasswordToggled(bool)));
    connect(d->ui.cmbProxyType, SIGNAL(currentIndexChanged(int)), this, SLOT(proxyTypeChanged(int)));

    if (d->setting)
        loadConfig(d->setting);
}

OpenVpnSettingWidget::~OpenVpnSettingWidget()
{
    delete d;
}

void OpenVpnSettingWidget::init()
{
    d->openvpnProcess->start();
}

void OpenVpnSettingWidget::gotOpenVpnOutput()
{
    d->openVpnCiphers.append(d->openvpnProcess->readAll());
}

void OpenVpnSettingWidget::openVpnError(QProcess::ProcessError)
{
    d->ui.cboCipher->removeItem(0);
    d->ui.cboCipher->addItem(i18nc("@item:inlistbox Item added when OpenVPN cipher lookup failed", "OpenVPN cipher lookup failed"));
}

void OpenVpnSettingWidget::openVpnFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    d->ui.cboCipher->removeItem(0);
    if (!exitCode && exitStatus == QProcess::NormalExit) {
        d->ui.cboCipher->addItem(i18nc("@item::inlist Default openvpn cipher item", "Default"));
        QList<QByteArray> rawOutputLines = d->openVpnCiphers.split('\n');
        bool foundFirstSpace = false;;
        foreach (const QByteArray &cipher, rawOutputLines) {
            if (cipher.length() == 0) {
                foundFirstSpace = true;
            } else if (foundFirstSpace) {
                d->ui.cboCipher->addItem(QString::fromLocal8Bit(cipher.left(cipher.indexOf(' '))));
            }
        }

        if (d->ui.cboCipher->count()) {
            d->ui.cboCipher->setEnabled(true);
        } else {
            d->ui.cboCipher->addItem(i18nc("@item:inlistbox Item added when OpenVPN cipher lookup failed", "No OpenVPN ciphers found"));
        }
    } else {
        d->ui.cboCipher->addItem(i18nc("@item:inlistbox Item added when OpenVPN cipher lookup failed", "OpenVPN cipher lookup failed"));
    }
    delete d->openvpnProcess;
    d->openvpnProcess = 0;
    d->openVpnCiphers = QByteArray();
    d->gotOpenVpnCiphers = true;

    if (d->readConfig) {
        NMStringMap dataMap = d->setting->data();
        if (dataMap.contains(NM_OPENVPN_KEY_CIPHER)) {
            d->ui.cboCipher->setCurrentIndex(d->ui.cboCipher->findText(dataMap.value(NM_OPENVPN_KEY_CIPHER)));
        }
    }
}

void OpenVpnSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting)

    // General settings
    const NMStringMap dataMap = d->setting->data();
    const QString cType = dataMap.value(NM_OPENVPN_KEY_CONNECTION_TYPE);
    const NMStringMap secrets = d->setting->secrets();

    if ( cType == QLatin1String( NM_OPENVPN_CONTYPE_PASSWORD_TLS ) ) {
        d->ui.cmbConnectionType->setCurrentIndex( Private::EnumConnectionType::CertsPassword);
        d->ui.x509PassUsername->setText( dataMap[NM_OPENVPN_KEY_USERNAME]);
        d->ui.x509PassCaFile->setUrl( KUrl(dataMap[NM_OPENVPN_KEY_CA]) );
        d->ui.x509PassCert->setUrl(KUrl( dataMap[NM_OPENVPN_KEY_CERT] ));
        d->ui.x509PassKey->setUrl(KUrl( dataMap[NM_OPENVPN_KEY_KEY] ));
    } else if ( cType == QLatin1String( NM_OPENVPN_CONTYPE_STATIC_KEY ) ) {
        d->ui.cmbConnectionType->setCurrentIndex( Private::EnumConnectionType::Psk );
        d->ui.pskSharedKey->setText( dataMap[NM_OPENVPN_KEY_STATIC_KEY]);
        if (dataMap.contains(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION)) {
            switch (dataMap[NM_OPENVPN_KEY_STATIC_KEY_DIRECTION].toUInt())
            {
            case 0:
                d->ui.cmbKeyDirection->setCurrentIndex(Private::EnumKeyDirection::D0);
                break;
            case 1:
                d->ui.cmbKeyDirection->setCurrentIndex(Private::EnumKeyDirection::D1);
                break;
            }
        } else {
            d->ui.cmbKeyDirection->setCurrentIndex(Private::EnumKeyDirection::None);
        }
        d->ui.pskRemoteIp->setText( dataMap[NM_OPENVPN_KEY_REMOTE_IP]);
        d->ui.pskLocalIp->setText( dataMap[NM_OPENVPN_KEY_LOCAL_IP]);
    } else if ( cType == QLatin1String( NM_OPENVPN_CONTYPE_PASSWORD ) ) {
        d->ui.passUserName->setText( dataMap[NM_OPENVPN_KEY_USERNAME] );
        d->ui.passCaFile->setUrl(KUrl( dataMap[NM_OPENVPN_KEY_CA] ));
        d->ui.cmbConnectionType->setCurrentIndex( Private::EnumConnectionType::Password );
    } else if ( cType == QLatin1String( NM_OPENVPN_CONTYPE_TLS ) ) {
        d->ui.cmbConnectionType->setCurrentIndex( Private::EnumConnectionType::Certificates );
        d->ui.x509CaFile->setUrl(KUrl( dataMap[NM_OPENVPN_KEY_CA] ));
        d->ui.x509Cert->setUrl(KUrl( dataMap[NM_OPENVPN_KEY_CERT] ));
        d->ui.x509Key->setUrl(KUrl( dataMap[NM_OPENVPN_KEY_KEY] ));
    }

    d->ui.gateway->setText( dataMap[NM_OPENVPN_KEY_REMOTE] );

    // Optional Settings
    if (dataMap.contains(NM_OPENVPN_KEY_PORT)) {
        d->ui.sbCustomPort->setValue(dataMap[NM_OPENVPN_KEY_PORT].toUInt());
    } else {
        d->ui.sbCustomPort->setValue(0);
    }
    if (dataMap.contains(NM_OPENVPN_KEY_TUNNEL_MTU)) {
        d->ui.sbMtu->setValue(dataMap[NM_OPENVPN_KEY_TUNNEL_MTU].toUInt());
    } else {
        d->ui.sbMtu->setValue(0);
    }
    if (dataMap.contains(NM_OPENVPN_KEY_FRAGMENT_SIZE)) {
        d->ui.sbUdpFragmentSize->setValue(dataMap[NM_OPENVPN_KEY_FRAGMENT_SIZE].toUInt());
    } else {
        d->ui.sbUdpFragmentSize->setValue(0);
    }
    if (dataMap.contains(NM_OPENVPN_KEY_RENEG_SECONDS)) {
        d->ui.chkUseCustomReneg->setChecked(true);
        d->ui.sbCustomReneg->setValue(dataMap[NM_OPENVPN_KEY_RENEG_SECONDS].toUInt());
    } else {
        d->ui.chkUseCustomReneg->setChecked(false);
        d->ui.sbCustomReneg->setValue(0);
    }
    d->ui.chkUseLZO->setChecked( dataMap[NM_OPENVPN_KEY_COMP_LZO] == "yes" );
    d->ui.chkUseTCP->setChecked( dataMap[NM_OPENVPN_KEY_PROTO_TCP] == "yes" );
    d->ui.chkUseTAP->setChecked( dataMap[NM_OPENVPN_KEY_TAP_DEV] == "yes" );
    d->ui.chkMssRestrict->setChecked(dataMap[NM_OPENVPN_KEY_MSSFIX] == "yes");
    // Optional Security Settings
    QString hmacKeyAuth = dataMap[NM_OPENVPN_KEY_AUTH];
    if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_NONE)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::None);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_MD5)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Md5);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA1)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha1);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA224)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha224);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA256)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha256);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA384)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha384);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_SHA512)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Sha512);
    } else if (hmacKeyAuth == QLatin1String(NM_OPENVPN_AUTH_RIPEMD160)) {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Ripemd160);
    } else {
        d->ui.cboHmac->setCurrentIndex(Private::EnumHashingAlgorithms::Default);
    }
    // ciphers populated above?
    if (d->gotOpenVpnCiphers && dataMap.contains(NM_OPENVPN_KEY_CIPHER)) {
        d->ui.cboCipher->setCurrentIndex(d->ui.cboCipher->findText(dataMap[NM_OPENVPN_KEY_CIPHER]));
    }

    // Optional TLS
    if (dataMap.contains(NM_OPENVPN_KEY_TLS_REMOTE)) {
        d->ui.subjectMatch->setText(dataMap[NM_OPENVPN_KEY_TLS_REMOTE]);
    }
    d->ui.useExtraTlsAuth->setChecked(!dataMap[NM_OPENVPN_KEY_TA].isEmpty());
    d->ui.kurlTlsAuthKey->setUrl(KUrl(dataMap[NM_OPENVPN_KEY_TA]) );
    if (dataMap.contains(NM_OPENVPN_KEY_TA_DIR)) {
        uint tlsAuthDirection = dataMap[NM_OPENVPN_KEY_TA_DIR].toUInt();
        d->ui.cboDirection->setCurrentIndex(tlsAuthDirection + 1);
    }
    // Proxies
    if (dataMap[NM_OPENVPN_KEY_PROXY_TYPE] == "http") {
        d->ui.cmbProxyType->setCurrentIndex(Private::EnumProxyType::HTTP);
    } else if (dataMap[NM_OPENVPN_KEY_PROXY_TYPE] == "socks") {
        d->ui.cmbProxyType->setCurrentIndex(Private::EnumProxyType::SOCKS);
    } else {
        d->ui.cmbProxyType->setCurrentIndex(Private::EnumProxyType::NotRequired);
    }
    proxyTypeChanged(d->ui.cmbProxyType->currentIndex());
    d->ui.proxyServerAddress->setText(dataMap[NM_OPENVPN_KEY_PROXY_SERVER]);
    if (dataMap.contains(NM_OPENVPN_KEY_PROXY_PORT)) {
        d->ui.sbProxyPort->setValue(dataMap[NM_OPENVPN_KEY_PROXY_PORT].toUInt());
    } else {
        d->ui.sbProxyPort->setValue(0);
    }
    d->ui.chkProxyRetry->setChecked(dataMap[NM_OPENVPN_KEY_PROXY_RETRY] == "yes");
    d->ui.proxyUsername->setText(dataMap[NM_OPENVPN_KEY_HTTP_PROXY_USERNAME]);
    d->readConfig = true;

    NetworkManager::Setting::SecretFlags type;

    if (cType == QLatin1String(NM_OPENVPN_CONTYPE_TLS)) {
        type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_CERTPASS"-flags"].toInt();
        if (type & NetworkManager::Setting::AgentOwned || type & NetworkManager::Setting::None) {
            d->ui.x509KeyPassword->setText(secrets.value(NM_OPENVPN_KEY_CERTPASS));
        }
        fillOnePasswordCombo(d->ui.x509KeyPasswordStorage, type);
    } else if (cType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD)) {
        type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_PASSWORD"-flags"].toInt();
        if (type & NetworkManager::Setting::AgentOwned || type & NetworkManager::Setting::None) {
            d->ui.passPassword->setText(secrets.value(NM_OPENVPN_KEY_PASSWORD));
        }
        fillOnePasswordCombo(d->ui.passPasswordStorage, type);
    } else if (cType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS)) {
        type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_PASSWORD"-flags"].toInt();
        if (type & NetworkManager::Setting::AgentOwned || type & NetworkManager::Setting::None) {
            d->ui.x509PassPassword->setText(secrets.value(NM_OPENVPN_KEY_PASSWORD));
        }
        fillOnePasswordCombo(d->ui.x509PassPasswordStorage, type);

        type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_CERTPASS"-flags"].toInt();
        if (type & NetworkManager::Setting::AgentOwned || type & NetworkManager::Setting::None) {
            d->ui.x509PassKeyPassword->setText(secrets.value(NM_OPENVPN_KEY_CERTPASS));
        }
        fillOnePasswordCombo(d->ui.x509PassKeyPasswordStorage, type);
    }

    type = (NetworkManager::Setting::SecretFlags)dataMap[NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"].toInt();
    if (type & NetworkManager::Setting::AgentOwned || type & NetworkManager::Setting::None) {
        d->ui.proxyPassword->setText(secrets.value(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD));
    }
    fillOnePasswordCombo(d->ui.proxyPasswordStorage, type);
}

QVariantMap OpenVpnSettingWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned)

    NMStringMap data;
    NMStringMap secretData;

    // required settings
    data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE), d->ui.gateway->text());

    QString contype;

    switch ( d->ui.cmbConnectionType->currentIndex())
    {
    case Private::EnumConnectionType::Certificates:
        contype = QLatin1String(NM_OPENVPN_CONTYPE_TLS);
        kDebug() << "saving VPN TLS settings as urls:" << d->ui.x509CaFile->url().path() << d->ui.x509Cert->url().path() << d->ui.x509Key->url().path();
        data.insert( QLatin1String(NM_OPENVPN_KEY_CA), d->ui.x509CaFile->url().path());
        data.insert( QLatin1String(NM_OPENVPN_KEY_CERT), d->ui.x509Cert->url().path());
        data.insert( QLatin1String(NM_OPENVPN_KEY_KEY), d->ui.x509Key->url().path());
        // key password
        if (!d->ui.x509KeyPassword->text().isEmpty()) {
            secretData.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS), d->ui.x509KeyPassword->text());
        }
        handleOnePasswordType(d->ui.x509KeyPasswordStorage, QLatin1String(NM_OPENVPN_KEY_CERTPASS"-flags"), data);
        break;
    case Private::EnumConnectionType::Psk:
        contype = QLatin1String(NM_OPENVPN_CONTYPE_STATIC_KEY);
        data.insert( QLatin1String(NM_OPENVPN_KEY_STATIC_KEY), d->ui.pskSharedKey->url().path());
        switch (d->ui.cmbKeyDirection->currentIndex())
        {
        case Private::EnumKeyDirection::None:
            break;
        case Private::EnumKeyDirection::D0:
            data.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION), QString::number(0));
            break;
        case Private::EnumKeyDirection::D1:
            data.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION), QString::number(1));
            break;
        }
        // ip addresses
        data.insert( QLatin1String(NM_OPENVPN_KEY_REMOTE_IP), d->ui.pskRemoteIp->text());
        data.insert( QLatin1String(NM_OPENVPN_KEY_LOCAL_IP), d->ui.pskLocalIp->text());
        break;
    case Private::EnumConnectionType::Password:
        contype = QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD);
        // username
        if (!d->ui.passUserName->text().isEmpty()) {
            data.insert( QLatin1String(NM_OPENVPN_KEY_USERNAME), d->ui.passUserName->text());
        }
        // password
        if (!d->ui.passPassword->text().isEmpty()) {
            secretData.insert(QLatin1String( NM_OPENVPN_KEY_PASSWORD ), d->ui.passPassword->text());
        }
        handleOnePasswordType(d->ui.passPasswordStorage, QLatin1String(NM_OPENVPN_KEY_PASSWORD"-flags"), data);
        // ca
        data.insert(QLatin1String(NM_OPENVPN_KEY_CA), d->ui.passCaFile->url().path());
        break;
    case Private::EnumConnectionType::CertsPassword:
        contype = QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS);
        // username
        if (!d->ui.x509PassUsername->text().isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_USERNAME), d->ui.x509PassUsername->text());
        }
        // ca
        data.insert(QLatin1String(NM_OPENVPN_KEY_CA), d->ui.x509PassCaFile->url().path());
        // cert
        data.insert(QLatin1String(NM_OPENVPN_KEY_CERT), d->ui.x509PassCert->url().path());
        // key file
        data.insert(QLatin1String(NM_OPENVPN_KEY_KEY), d->ui.x509PassKey->url().path());
        // key password
        if (!d->ui.x509PassKeyPassword->text().isEmpty()) {
            secretData.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS), d->ui.x509PassKeyPassword->text());
        }
        handleOnePasswordType(d->ui.x509PassKeyPasswordStorage, QLatin1String(NM_OPENVPN_KEY_CERTPASS"-flags"), data);
        // password
        if (!d->ui.x509PassPassword->text().isEmpty()) {
            secretData.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD), d->ui.x509PassPassword->text());
        }
        handleOnePasswordType(d->ui.x509PassPasswordStorage, QLatin1String(NM_OPENVPN_KEY_PASSWORD"-flags"), data);
        break;
    }
    data.insert( QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE), contype);

    // optional settings
    if ( d->ui.sbCustomPort->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_PORT), QString::number(d->ui.sbCustomPort->value()));
    }
    if ( d->ui.sbMtu->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU), QString::number(d->ui.sbMtu->value()));
    }
    if ( d->ui.sbUdpFragmentSize->value() > 0 ) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE), QString::number(d->ui.sbUdpFragmentSize->value()));
    }
    if (d->ui.chkUseCustomReneg->isChecked()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS), QString::number(d->ui.sbCustomReneg->value()));
    }
    data.insert( QLatin1String(NM_OPENVPN_KEY_PROTO_TCP), d->ui.chkUseTCP->isChecked() ? "yes" : "no" );
    data.insert( QLatin1String(NM_OPENVPN_KEY_COMP_LZO), d->ui.chkUseLZO->isChecked() ? "yes" : "no" );
    data.insert( QLatin1String(NM_OPENVPN_KEY_TAP_DEV), d->ui.chkUseTAP->isChecked() ? "yes" : "no" );
    data.insert( QLatin1String(NM_OPENVPN_KEY_MSSFIX), d->ui.chkMssRestrict->isChecked() ? "yes" : "no" );

    // Optional Security
    switch ( d->ui.cboHmac->currentIndex()) {
    case Private::EnumHashingAlgorithms::Default:
        break;
    case Private::EnumHashingAlgorithms::None:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_NONE));
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
    if (d->ui.cboCipher->currentIndex() != 0) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CIPHER), d->ui.cboCipher->currentText());
    }

    // optional tls authentication
    if (!d->ui.subjectMatch->text().isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE), d->ui.subjectMatch->text());
    }
    if (d->ui.useExtraTlsAuth->isChecked()) {
        KUrl tlsAuthKeyUrl = d->ui.kurlTlsAuthKey->url();
        if (!tlsAuthKeyUrl.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TA), tlsAuthKeyUrl.path());
        }
        if (d->ui.cboDirection->currentIndex() > 0) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TA_DIR), QString::number(d->ui.cboDirection->currentIndex() - 1));
        }
    }

    // Proxies
    switch (d->ui.cmbProxyType->currentIndex()) {
    case Private::EnumProxyType::NotRequired:
        break;
    case Private::EnumProxyType::HTTP:
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), "http");
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), d->ui.proxyServerAddress->text());
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(d->ui.sbProxyPort->value()));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), d->ui.chkProxyRetry->isChecked() ? "yes" : "no");
        if (!d->ui.proxyUsername->text().isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME), d->ui.proxyUsername->text());
            secretData.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD), d->ui.proxyPassword->text());
            handleOnePasswordType(d->ui.proxyPasswordStorage, QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"), data);
        }
        break;
    case Private::EnumProxyType::SOCKS:
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), "socks");
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), d->ui.proxyServerAddress->text());
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(d->ui.sbProxyPort->value()));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), d->ui.chkProxyRetry->isChecked() ? "yes" : "no");
        break;
    }

    d->setting->setData(data);
    d->setting->setSecrets(secretData);

    return d->setting->toMap();
}

void OpenVpnSettingWidget::updateStartDir(const KUrl & url)
{
    QList<KUrlRequester *> requesters;
    requesters << d->ui.x509CaFile << d->ui.x509Cert << d->ui.x509Key << d->ui.pskSharedKey << d->ui.passCaFile << d->ui.x509PassCaFile
               << d->ui.x509PassCert << d->ui.x509PassKey << d->ui.kurlTlsAuthKey;
    foreach (KUrlRequester * requester, requesters) {
        requester->setStartDir(KUrl(url.directory()));
    }
}

void OpenVpnSettingWidget::x509KeyPasswordStorageChanged(int index)
{
    setPasswordType(d->ui.x509KeyPassword, index);
}

void OpenVpnSettingWidget::passPasswordStorageChanged(int index)
{
    setPasswordType(d->ui.passPassword, index);
}

void OpenVpnSettingWidget::x509PassKeyPasswordStorageChanged(int index)
{
    setPasswordType(d->ui.x509PassKeyPassword, index);
}

void OpenVpnSettingWidget::x509PassPasswordStorageChanged(int index)
{
    setPasswordType(d->ui.x509PassPassword, index);
}

void OpenVpnSettingWidget::proxyPasswordStorageChanged(int index)
{
    setPasswordType(d->ui.proxyPassword, index);
}

void OpenVpnSettingWidget::setPasswordType(KLineEdit *edit, int type)
{
    switch (type)
    {
    case Private::EnumPasswordStorageType::AlwaysAsk:
    case Private::EnumPasswordStorageType::NotRequired:
        edit->setEnabled(false);
        break;
    case Private::EnumPasswordStorageType::Store:
        edit->setEnabled(true);
        break;
    }
}

void OpenVpnSettingWidget::fillOnePasswordCombo(KComboBox * combo, NetworkManager::Setting::SecretFlags type)
{
    if (type.testFlag(NetworkManager::Setting::AgentOwned) || type.testFlag(NetworkManager::Setting::None)) {
        combo->setCurrentIndex(Private::EnumPasswordStorageType::Store);
    } else if (type.testFlag(NetworkManager::Setting::NotRequired)) {
        combo->setCurrentIndex(Private::EnumPasswordStorageType::NotRequired);
    } else if (type.testFlag(NetworkManager::Setting::NotSaved)) {
        combo->setCurrentIndex(Private::EnumPasswordStorageType::AlwaysAsk);
    }
}

uint OpenVpnSettingWidget::handleOnePasswordType(const KComboBox * combo, const QString & key, NMStringMap & data) const
{
    uint type = combo->currentIndex();
    switch (type) {
    case Private::EnumPasswordStorageType::AlwaysAsk:
        data.insert(key, QString::number(NetworkManager::Setting::NotSaved));
        break;
    case Private::EnumPasswordStorageType::Store:
        data.insert(key, QString::number(NetworkManager::Setting::AgentOwned));
        break;
    case Private::EnumPasswordStorageType::NotRequired:
        data.insert(key, QString::number(NetworkManager::Setting::NotRequired));
        break;
    }
    return type;
}

void OpenVpnSettingWidget::showPasswordsToggled(bool toggled)
{
    d->ui.x509KeyPassword->setPasswordMode(!toggled);
    d->ui.passPassword->setPasswordMode(!toggled);
    d->ui.x509PassKeyPassword->setPasswordMode(!toggled);
    d->ui.x509PassPassword->setPasswordMode(!toggled);
}

void OpenVpnSettingWidget::proxyPasswordToggled(bool toggled)
{
    d->ui.proxyPassword->setPasswordMode(!toggled);
}

void OpenVpnSettingWidget::proxyTypeChanged(int type)
{
    switch (type)
    {
    case Private::EnumProxyType::NotRequired:
        d->ui.proxyServerAddress->setEnabled(false);
        d->ui.sbProxyPort->setEnabled(false);
        d->ui.chkProxyRetry->setEnabled(false);
        d->ui.proxyUsername->setEnabled(false);
        d->ui.proxyPassword->setEnabled(false);
        d->ui.proxyPasswordStorage->setEnabled(false);
        d->ui.chkProxyShowPassword->setEnabled(false);
        break;
    case Private::EnumProxyType::HTTP:
        d->ui.proxyServerAddress->setEnabled(true);
        d->ui.sbProxyPort->setEnabled(true);
        d->ui.chkProxyRetry->setEnabled(true);
        d->ui.proxyUsername->setEnabled(true);
        d->ui.proxyPasswordStorage->setEnabled(true);
        setPasswordType(d->ui.proxyPassword, d->ui.proxyPasswordStorage->currentIndex());
        d->ui.chkProxyShowPassword->setEnabled(true);
        break;
    case Private::EnumProxyType::SOCKS:
        d->ui.proxyServerAddress->setEnabled(true);
        d->ui.sbProxyPort->setEnabled(true);
        d->ui.chkProxyRetry->setEnabled(true);
        d->ui.proxyUsername->setEnabled(false);
        d->ui.proxyPassword->setEnabled(false);
        d->ui.proxyPasswordStorage->setEnabled(false);
        d->ui.chkProxyShowPassword->setEnabled(false);
        break;
    }
}
