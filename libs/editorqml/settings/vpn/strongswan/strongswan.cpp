/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "strongswan.h"

#include "nm-strongswan-service.h"

#include <QUrl>

namespace
{
const QLatin1String YesString("yes");
const QLatin1String NoString("no");

QString pathToUrl(const QString &path)
{
    return path.isEmpty() ? QString() : QUrl::fromLocalFile(path).toString();
}

QString urlToPath(const QString &url)
{
    return url.isEmpty() ? QString() : QUrl(url).toLocalFile();
}
}

StrongswanSetting::StrongswanSetting(QObject *parent)
    : VpnSettingBase(parent)
{
}

StrongswanSetting::~StrongswanSetting() = default;

void StrongswanSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setUserPassword(QString());

    const NMStringMap data = setting->data();

    // Gateway
    setGateway(data.value(QLatin1String(NM_STRONGSWAN_GATEWAY)));
    setGatewayCertificate(pathToUrl(data.value(QLatin1String(NM_STRONGSWAN_CERTIFICATE))));
    setRemoteIdentity(data.value(QLatin1String(NM_STRONGSWAN_RIDENTITY)));

    const QString method = data.value(QLatin1String(NM_STRONGSWAN_METHOD));
    // Newer versions of the GNOME editor write method "cert" and store the
    // kind of private key in a separate "cert-source" data item
    const QString certSource = data.value(QLatin1String(NM_STRONGSWAN_CERT_SOURCE));
    const bool certMethod = method == QLatin1String(NM_STRONGSWAN_AUTH_CERT);
    if (method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT) || (certMethod && certSource == QLatin1String(NM_STRONGSWAN_AUTH_AGENT))) {
        setAuthMethod(SshAgent);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD) || (certMethod && certSource == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD))) {
        setAuthMethod(Smartcard);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP)) {
        setAuthMethod(Eap);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP_TTLS)) {
        setAuthMethod(EapTtls);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_PSK)) {
        setAuthMethod(Psk);
    } else if (method.isEmpty() || method == QLatin1String(NM_STRONGSWAN_AUTH_KEY) || certMethod) {
        setAuthMethod(PrivateKey);
    } else {
        // Unknown authentication method (e.g. "eap-tls" written by another
        // editor): keep it together with its authentication data so that
        // saving doesn't break the connection
        m_unknownMethod = method;
        Q_EMIT unknownMethodChanged();
        setAuthMethod(Unknown);
        const QList<QString> authKeys = {
            QStringLiteral(NM_STRONGSWAN_USER),
            QStringLiteral(NM_STRONGSWAN_USERCERT),
            QStringLiteral(NM_STRONGSWAN_USERKEY),
            QStringLiteral(NM_STRONGSWAN_CERT_SOURCE),
            QStringLiteral(NM_STRONGSWAN_LIDENTITY),
            QStringLiteral(NM_STRONGSWAN_POPTION),
        };
        for (const QString &key : authKeys) {
            const QString value = data.value(key);
            if (!value.isEmpty()) {
                m_preservedAuth.insert(key, value);
            }
        }
    }

    setUserCertificate(pathToUrl(data.value(QLatin1String(NM_STRONGSWAN_USERCERT))));
    setUserKey(pathToUrl(data.value(QLatin1String(NM_STRONGSWAN_USERKEY))));

    setUsername(data.value(QLatin1String(NM_STRONGSWAN_USER)));

    // The editor asks NetworkManager for secrets only when a *-flags key
    // exists; connections written by other tools may lack it, so inject a
    // default to trigger the secret fetch
    if ((m_authMethod == Eap || m_authMethod == EapTtls || m_authMethod == Psk) && !data.contains(QLatin1String(NM_STRONGSWAN_POPTION))) {
        NMStringMap writableData = setting->data();
        writableData.insert(QLatin1String(NM_STRONGSWAN_POPTION), QString::number(NetworkManager::Setting::None));
        setting->setData(writableData);
    }

    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_STRONGSWAN_POPTION)).toInt());
    if (flags == NetworkManager::Setting::None) {
        setUserPasswordOption(StoreForAllUsers);
    } else if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        setUserPasswordOption(StoreForUser);
    } else if (flags.testFlag(NetworkManager::Setting::NotSaved)) {
        setUserPasswordOption(AlwaysAsk);
    } else {
        setUserPasswordOption(NotRequired);
    }

    // Options
    setRequestInnerIp(data.value(QLatin1String(NM_STRONGSWAN_INNERIP)) == YesString);
    setEnforceUdpEncapsulation(data.value(QLatin1String(NM_STRONGSWAN_ENCAP)) == YesString);
    setUseIpCompression(data.value(QLatin1String(NM_STRONGSWAN_IPCOMP)) == YesString);

    // Custom cipher proposals
    setUseCustomProposals(data.value(QLatin1String(NM_STRONGSWAN_PROPOSAL)) == YesString);
    setIke(data.value(QLatin1String(NM_STRONGSWAN_IKE)));
    setEsp(data.value(QLatin1String(NM_STRONGSWAN_ESP)));

    loadSecrets(setting);
}

void StrongswanSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    m_loadedSecrets = setting->secrets();

    if (m_authMethod == Unknown) {
        m_preservedSecrets = m_loadedSecrets;
        return;
    }

    const QString password = m_loadedSecrets.value(QLatin1String(NM_STRONGSWAN_SECRET));
    if (!password.isEmpty()) {
        setUserPassword(password);
    }
}

QString StrongswanSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_STRONGSWAN);
}

QVariantMap StrongswanSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_STRONGSWAN));

    NMStringMap data;
    NMStringMap secrets;

    // Gateway
    if (!m_gateway.isEmpty()) {
        data.insert(QLatin1String(NM_STRONGSWAN_GATEWAY), m_gateway);
    }

    if (!m_gatewayCertificate.isEmpty()) {
        data.insert(QLatin1String(NM_STRONGSWAN_CERTIFICATE), urlToPath(m_gatewayCertificate));
    }

    if (!m_remoteIdentity.isEmpty()) {
        data.insert(QLatin1String(NM_STRONGSWAN_RIDENTITY), m_remoteIdentity);
    }

    switch (m_authMethod) {
    case PrivateKey:
        data.insert(QLatin1String(NM_STRONGSWAN_METHOD), QLatin1String(NM_STRONGSWAN_AUTH_KEY));
        if (!m_userCertificate.isEmpty()) {
            data.insert(QLatin1String(NM_STRONGSWAN_USERCERT), urlToPath(m_userCertificate));
        }
        if (!m_userKey.isEmpty()) {
            data.insert(QLatin1String(NM_STRONGSWAN_USERKEY), urlToPath(m_userKey));
        }
        break;

    case SshAgent:
        data.insert(QLatin1String(NM_STRONGSWAN_METHOD), QLatin1String(NM_STRONGSWAN_AUTH_AGENT));
        if (!m_userCertificate.isEmpty()) {
            data.insert(QLatin1String(NM_STRONGSWAN_USERCERT), urlToPath(m_userCertificate));
        }
        break;

    case Smartcard:
        data.insert(QLatin1String(NM_STRONGSWAN_METHOD), QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD));
        break;

    case Eap:
    case EapTtls:
    case Psk: {
        if (m_authMethod == EapTtls) {
            data.insert(QLatin1String(NM_STRONGSWAN_METHOD), QLatin1String(NM_STRONGSWAN_AUTH_EAP_TTLS));
        } else if (m_authMethod == Psk) {
            data.insert(QLatin1String(NM_STRONGSWAN_METHOD), QLatin1String(NM_STRONGSWAN_AUTH_PSK));
        } else {
            data.insert(QLatin1String(NM_STRONGSWAN_METHOD), QLatin1String(NM_STRONGSWAN_AUTH_EAP));
        }

        if (!m_username.isEmpty()) {
            data.insert(QLatin1String(NM_STRONGSWAN_USER), m_username);
        }

        NetworkManager::Setting::SecretFlagType passwordFlag = NetworkManager::Setting::NotRequired;
        switch (m_userPasswordOption) {
        case StoreForAllUsers:
            passwordFlag = NetworkManager::Setting::None;
            break;
        case StoreForUser:
            passwordFlag = NetworkManager::Setting::AgentOwned;
            break;
        case AlwaysAsk:
            passwordFlag = NetworkManager::Setting::NotSaved;
            break;
        case NotRequired:
            passwordFlag = NetworkManager::Setting::NotRequired;
            break;
        }
        data.insert(QLatin1String(NM_STRONGSWAN_POPTION), QString::number(passwordFlag));

        if (m_userPasswordOption == StoreForAllUsers || m_userPasswordOption == StoreForUser) {
            // Keep an already stored secret when the field was left empty
            secrets = m_loadedSecrets;
            if (!m_userPassword.isEmpty()) {
                secrets.insert(QLatin1String(NM_STRONGSWAN_SECRET), m_userPassword);
            }
        }
        break;
    }

    case Unknown:
        // An unknown method was loaded and the user didn't pick a known one:
        // keep it together with its authentication data and secrets
        data.insert(QLatin1String(NM_STRONGSWAN_METHOD), m_unknownMethod);
        for (auto it = m_preservedAuth.cbegin(); it != m_preservedAuth.cend(); ++it) {
            data.insert(it.key(), it.value());
        }
        secrets = m_preservedSecrets;
        break;
    }

    data.insert(QLatin1String(NM_STRONGSWAN_INNERIP), m_requestInnerIp ? YesString : NoString);
    data.insert(QLatin1String(NM_STRONGSWAN_ENCAP), m_enforceUdpEncapsulation ? YesString : NoString);
    data.insert(QLatin1String(NM_STRONGSWAN_IPCOMP), m_useIpCompression ? YesString : NoString);

    // Custom cipher proposals
    if (m_useCustomProposals) {
        data.insert(QLatin1String(NM_STRONGSWAN_PROPOSAL), YesString);
        data.insert(QLatin1String(NM_STRONGSWAN_IKE), m_ike);
        data.insert(QLatin1String(NM_STRONGSWAN_ESP), m_esp);
    } else {
        data.insert(QLatin1String(NM_STRONGSWAN_PROPOSAL), NoString);
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool StrongswanSetting::isValid() const
{
    if (m_gateway.isEmpty()) {
        return false;
    }
    // charon-nm refuses pre-shared keys shorter than 20 characters
    if (m_authMethod == Psk && (m_userPasswordOption == StoreForAllUsers || m_userPasswordOption == StoreForUser) && m_userPassword.length() < 20) {
        return false;
    }
    return true;
}

QString StrongswanSetting::gateway() const
{
    return m_gateway;
}

void StrongswanSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::gatewayCertificate() const
{
    return m_gatewayCertificate;
}

void StrongswanSetting::setGatewayCertificate(const QString &certificate)
{
    if (m_gatewayCertificate == certificate) {
        return;
    }
    m_gatewayCertificate = certificate;
    Q_EMIT gatewayCertificateChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::remoteIdentity() const
{
    return m_remoteIdentity;
}

void StrongswanSetting::setRemoteIdentity(const QString &identity)
{
    if (m_remoteIdentity == identity) {
        return;
    }
    m_remoteIdentity = identity;
    Q_EMIT remoteIdentityChanged();
    Q_EMIT validChanged();
}

StrongswanSetting::AuthMethod StrongswanSetting::authMethod() const
{
    return m_authMethod;
}

void StrongswanSetting::setAuthMethod(AuthMethod method)
{
    if (m_authMethod == method) {
        return;
    }
    m_authMethod = method;
    Q_EMIT authMethodChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::unknownMethod() const
{
    return m_unknownMethod;
}

QString StrongswanSetting::userCertificate() const
{
    return m_userCertificate;
}

void StrongswanSetting::setUserCertificate(const QString &certificate)
{
    if (m_userCertificate == certificate) {
        return;
    }
    m_userCertificate = certificate;
    Q_EMIT userCertificateChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::userKey() const
{
    return m_userKey;
}

void StrongswanSetting::setUserKey(const QString &key)
{
    if (m_userKey == key) {
        return;
    }
    m_userKey = key;
    Q_EMIT userKeyChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::username() const
{
    return m_username;
}

void StrongswanSetting::setUsername(const QString &username)
{
    if (m_username == username) {
        return;
    }
    m_username = username;
    Q_EMIT usernameChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::userPassword() const
{
    return m_userPassword;
}

void StrongswanSetting::setUserPassword(const QString &password)
{
    if (m_userPassword == password) {
        return;
    }
    m_userPassword = password;
    Q_EMIT userPasswordChanged();
    Q_EMIT validChanged();
}

StrongswanSetting::PasswordOption StrongswanSetting::userPasswordOption() const
{
    return m_userPasswordOption;
}

void StrongswanSetting::setUserPasswordOption(PasswordOption option)
{
    if (m_userPasswordOption == option) {
        return;
    }
    m_userPasswordOption = option;
    Q_EMIT userPasswordOptionChanged();
    Q_EMIT validChanged();
}

bool StrongswanSetting::requestInnerIp() const
{
    return m_requestInnerIp;
}

void StrongswanSetting::setRequestInnerIp(bool request)
{
    if (m_requestInnerIp == request) {
        return;
    }
    m_requestInnerIp = request;
    Q_EMIT requestInnerIpChanged();
    Q_EMIT validChanged();
}

bool StrongswanSetting::enforceUdpEncapsulation() const
{
    return m_enforceUdpEncapsulation;
}

void StrongswanSetting::setEnforceUdpEncapsulation(bool enforce)
{
    if (m_enforceUdpEncapsulation == enforce) {
        return;
    }
    m_enforceUdpEncapsulation = enforce;
    Q_EMIT enforceUdpEncapsulationChanged();
    Q_EMIT validChanged();
}

bool StrongswanSetting::useIpCompression() const
{
    return m_useIpCompression;
}

void StrongswanSetting::setUseIpCompression(bool use)
{
    if (m_useIpCompression == use) {
        return;
    }
    m_useIpCompression = use;
    Q_EMIT useIpCompressionChanged();
    Q_EMIT validChanged();
}

bool StrongswanSetting::useCustomProposals() const
{
    return m_useCustomProposals;
}

void StrongswanSetting::setUseCustomProposals(bool use)
{
    if (m_useCustomProposals == use) {
        return;
    }
    m_useCustomProposals = use;
    Q_EMIT useCustomProposalsChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::ike() const
{
    return m_ike;
}

void StrongswanSetting::setIke(const QString &ike)
{
    if (m_ike == ike) {
        return;
    }
    m_ike = ike;
    Q_EMIT ikeChanged();
    Q_EMIT validChanged();
}

QString StrongswanSetting::esp() const
{
    return m_esp;
}

void StrongswanSetting::setEsp(const QString &esp)
{
    if (m_esp == esp) {
        return;
    }
    m_esp = esp;
    Q_EMIT espChanged();
    Q_EMIT validChanged();
}

#include "moc_strongswan.cpp"
