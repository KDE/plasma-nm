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
    : QObject(parent)
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
    if (method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT)) {
        setAuthMethod(SshAgent);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD)) {
        setAuthMethod(Smartcard);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP)) {
        setAuthMethod(Eap);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP_TTLS)) {
        setAuthMethod(EapTtls);
    } else {
        setAuthMethod(PrivateKey);
    }

    setUserCertificate(pathToUrl(data.value(QLatin1String(NM_STRONGSWAN_USERCERT))));
    setUserKey(pathToUrl(data.value(QLatin1String(NM_STRONGSWAN_USERKEY))));

    setUsername(data.value(QLatin1String(NM_STRONGSWAN_USER)));

    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_STRONGSWAN_POPTION)).toInt());
    if (flags.testFlag(NetworkManager::Setting::None)) {
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

    const QString password = setting->secrets().value(QLatin1String(NM_STRONGSWAN_SECRET));
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
    case EapTtls: {
        data.insert(QLatin1String(NM_STRONGSWAN_METHOD),
                    m_authMethod == EapTtls ? QLatin1String(NM_STRONGSWAN_AUTH_EAP_TTLS) : QLatin1String(NM_STRONGSWAN_AUTH_EAP));

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

        if (m_userPasswordOption != NotRequired && !m_userPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_STRONGSWAN_SECRET), m_userPassword);
        }
        break;
    }
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
    return !m_gateway.isEmpty();
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
