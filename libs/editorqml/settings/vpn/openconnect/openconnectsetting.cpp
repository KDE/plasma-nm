/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openconnectsetting.h"

#include "nm-openconnect-service.h"

#include <QUrl>

namespace
{
const QLatin1String YES_STRING("yes");
const QLatin1String NO_STRING("no");

const QLatin1String PROTOCOL_NAMES[] = {
    QLatin1String("anyconnect"),
    QLatin1String("nc"),
    QLatin1String("gp"),
    QLatin1String("pulse"),
    QLatin1String("f5"),
    QLatin1String("fortinet"),
    QLatin1String("array"),
};

const QLatin1String REPORTED_OS_NAMES[] = {
    QLatin1String("linux"),
    QLatin1String("linux-64"),
    QLatin1String("win"),
    QLatin1String("mac-intel"),
    QLatin1String("android"),
    QLatin1String("apple-ios"),
};

const QLatin1String TOKEN_MODE_NAMES[] = {
    QLatin1String("disabled"),
    QLatin1String("stokenrc"),
    QLatin1String("manual"),
    QLatin1String("totp"),
    QLatin1String("hotp"),
    QLatin1String("yubioath"),
};

OpenconnectSetting::Protocol protocolFromName(const QString &name)
{
    for (int i = 0; i < static_cast<int>(std::size(PROTOCOL_NAMES)); ++i) {
        if (name == PROTOCOL_NAMES[i]) {
            return static_cast<OpenconnectSetting::Protocol>(i);
        }
    }

    return OpenconnectSetting::AnyConnect;
}

OpenconnectSetting::ReportedOs reportedOsFromName(const QString &name)
{
    for (int i = 0; i < static_cast<int>(std::size(REPORTED_OS_NAMES)); ++i) {
        if (name == REPORTED_OS_NAMES[i]) {
            return static_cast<OpenconnectSetting::ReportedOs>(i + 1);
        }
    }

    return OpenconnectSetting::OsDefault;
}

OpenconnectSetting::TokenMode tokenModeFromName(const QString &name)
{
    for (int i = 0; i < static_cast<int>(std::size(TOKEN_MODE_NAMES)); ++i) {
        if (name == TOKEN_MODE_NAMES[i]) {
            return static_cast<OpenconnectSetting::TokenMode>(i);
        }
    }

    return OpenconnectSetting::TokenDisabled;
}

OpenconnectSetting::PasswordOption optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return OpenconnectSetting::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return OpenconnectSetting::StoreForUser;
    }
    return OpenconnectSetting::AlwaysAsk;
}

QString flagsFromOption(OpenconnectSetting::PasswordOption option)
{
    switch (option) {
    case OpenconnectSetting::StoreForAllUsers:
        return QString::number(NetworkManager::Setting::None);
    case OpenconnectSetting::StoreForUser:
        return QString::number(NetworkManager::Setting::AgentOwned);
    case OpenconnectSetting::AlwaysAsk:
        break;
    }
    return QString::number(NetworkManager::Setting::NotSaved);
}

QString pathToUrl(const QString &path)
{
    return path.isEmpty() ? QString() : QUrl::fromLocalFile(path).toString();
}

QString urlToPath(const QString &url)
{
    return url.isEmpty() ? QString() : QUrl(url).toLocalFile();
}
}

OpenconnectSetting::OpenconnectSetting(QObject *parent)
    : QObject(parent)
{
}

OpenconnectSetting::~OpenconnectSetting() = default;

void OpenconnectSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    m_originalData = setting->data();
    setTokenSecret(QString());

    const NMStringMap data = setting->data();

    setGateway(data.value(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY)));
    setProtocol(protocolFromName(data.value(QLatin1String(NM_OPENCONNECT_KEY_PROTOCOL))));
    setCaCert(pathToUrl(data.value(QLatin1String(NM_OPENCONNECT_KEY_CACERT))));
    setProxy(data.value(QLatin1String(NM_OPENCONNECT_KEY_PROXY)));
    setUserAgent(data.value(QLatin1String(NM_OPENCONNECT_KEY_USERAGENT)));
    setReportedVersion(data.value(QLatin1String(NM_OPENCONNECT_KEY_VERSION_STRING)));
    setReportedOs(reportedOsFromName(data.value(QLatin1String(NM_OPENCONNECT_KEY_REPORTED_OS))));

    setAllowTrojan(data.value(QLatin1String(NM_OPENCONNECT_KEY_CSD_ENABLE)) == YES_STRING);
    setCsdWrapper(pathToUrl(data.value(QLatin1String(NM_OPENCONNECT_KEY_CSD_WRAPPER))));

    setMachineCert(pathToUrl(data.value(QLatin1String(NM_OPENCONNECT_KEY_MCACERT))));
    setMachineKey(pathToUrl(data.value(QLatin1String(NM_OPENCONNECT_KEY_MCAKEY))));
    setUserCert(pathToUrl(data.value(QLatin1String(NM_OPENCONNECT_KEY_USERCERT))));
    setUserKey(pathToUrl(data.value(QLatin1String(NM_OPENCONNECT_KEY_PRIVKEY))));

    setUseFsid(data.value(QLatin1String(NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID)) == YES_STRING);
    setPreventInvalidCert(data.value(QLatin1String(NM_OPENCONNECT_KEY_PREVENT_INVALID_CERT)) == YES_STRING);

    setTokenMode(tokenModeFromName(data.value(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_MODE))));
    setTokenSecretOption(optionFromFlags(data.value(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_SECRET "-flags"))));

    loadSecrets(setting);
}

void OpenconnectSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const QString secret = setting->secrets().value(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_SECRET));
    if (!secret.isEmpty()) {
        setTokenSecret(secret);
    }
}

QString OpenconnectSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_OPENCONNECT);
}

QVariantMap OpenconnectSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_OPENCONNECT));

    NMStringMap data;
    NMStringMap secrets;

    data.insert(QLatin1String(NM_OPENCONNECT_KEY_PROTOCOL), PROTOCOL_NAMES[m_protocol]);
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY), m_gateway);

    const QString caCertPath = urlToPath(m_caCert);
    if (!caCertPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_CACERT), caCertPath);
    }

    if (!m_proxy.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_PROXY), m_proxy);
    }

    if (!m_userAgent.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_USERAGENT), m_userAgent);
    }

    if (!m_reportedVersion.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_VERSION_STRING), m_reportedVersion);
    }

    // The default reported OS is the absence of the key.
    if (m_reportedOs != OsDefault) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_REPORTED_OS), REPORTED_OS_NAMES[m_reportedOs - 1]);
    }

    data.insert(QLatin1String(NM_OPENCONNECT_KEY_CSD_ENABLE), m_allowTrojan ? YES_STRING : NO_STRING);

    const QString csdWrapperPath = urlToPath(m_csdWrapper);
    if (!csdWrapperPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_CSD_WRAPPER), csdWrapperPath);
    }

    const QString machineCertPath = urlToPath(m_machineCert);
    if (!machineCertPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_MCACERT), machineCertPath);
    }

    const QString machineKeyPath = urlToPath(m_machineKey);
    if (!machineKeyPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_MCAKEY), machineKeyPath);
    }

    const QString userCertPath = urlToPath(m_userCert);
    if (!userCertPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_USERCERT), userCertPath);
    }

    const QString userKeyPath = urlToPath(m_userKey);
    if (!userKeyPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_PRIVKEY), userKeyPath);
    }

    data.insert(QLatin1String(NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID), m_useFsid ? YES_STRING : NO_STRING);
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_PREVENT_INVALID_CERT), m_preventInvalidCert ? YES_STRING : NO_STRING);

    data.insert(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_MODE), TOKEN_MODE_NAMES[m_tokenMode]);
    if (!m_tokenSecret.isEmpty()) {
        secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_SECRET), m_tokenSecret);
    }

    // Restore previous flags so previously stored secrets keep their storage mode.
    for (auto it = m_originalData.cbegin(); it != m_originalData.cend(); ++it) {
        if (it.key().endsWith(QLatin1String("-flags"))) {
            data.insert(it.key(), it.value());
        }
    }

    data.insert(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_SECRET "-flags"), flagsFromOption(m_tokenSecretOption));
    const QString notSaved = QString::number(NetworkManager::Setting::NotSaved);

    /* These are different for every login session, and should not be stored */
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_COOKIE "-flags"), notSaved);
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GWCERT "-flags"), notSaved);
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY "-flags"), notSaved);

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool OpenconnectSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString OpenconnectSetting::gateway() const
{
    return m_gateway;
}

void OpenconnectSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

OpenconnectSetting::Protocol OpenconnectSetting::protocol() const
{
    return m_protocol;
}

void OpenconnectSetting::setProtocol(OpenconnectSetting::Protocol protocol)
{
    if (m_protocol == protocol) {
        return;
    }
    m_protocol = protocol;
    Q_EMIT protocolChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::caCert() const
{
    return m_caCert;
}

void OpenconnectSetting::setCaCert(const QString &caCert)
{
    if (m_caCert == caCert) {
        return;
    }
    m_caCert = caCert;
    Q_EMIT caCertChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::proxy() const
{
    return m_proxy;
}

void OpenconnectSetting::setProxy(const QString &proxy)
{
    if (m_proxy == proxy) {
        return;
    }
    m_proxy = proxy;
    Q_EMIT proxyChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::userAgent() const
{
    return m_userAgent;
}

void OpenconnectSetting::setUserAgent(const QString &userAgent)
{
    if (m_userAgent == userAgent) {
        return;
    }
    m_userAgent = userAgent;
    Q_EMIT userAgentChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::csdWrapper() const
{
    return m_csdWrapper;
}

void OpenconnectSetting::setCsdWrapper(const QString &csdWrapper)
{
    if (m_csdWrapper == csdWrapper) {
        return;
    }
    m_csdWrapper = csdWrapper;
    Q_EMIT csdWrapperChanged();
    Q_EMIT validChanged();
}

bool OpenconnectSetting::allowTrojan() const
{
    return m_allowTrojan;
}

void OpenconnectSetting::setAllowTrojan(bool allow)
{
    if (m_allowTrojan == allow) {
        return;
    }
    m_allowTrojan = allow;
    Q_EMIT allowTrojanChanged();
    Q_EMIT validChanged();
}

OpenconnectSetting::ReportedOs OpenconnectSetting::reportedOs() const
{
    return m_reportedOs;
}

void OpenconnectSetting::setReportedOs(OpenconnectSetting::ReportedOs reportedOs)
{
    if (m_reportedOs == reportedOs) {
        return;
    }
    m_reportedOs = reportedOs;
    Q_EMIT reportedOsChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::reportedVersion() const
{
    return m_reportedVersion;
}

void OpenconnectSetting::setReportedVersion(const QString &version)
{
    if (m_reportedVersion == version) {
        return;
    }
    m_reportedVersion = version;
    Q_EMIT reportedVersionChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::machineCert() const
{
    return m_machineCert;
}

void OpenconnectSetting::setMachineCert(const QString &machineCert)
{
    if (m_machineCert == machineCert) {
        return;
    }
    m_machineCert = machineCert;
    Q_EMIT machineCertChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::machineKey() const
{
    return m_machineKey;
}

void OpenconnectSetting::setMachineKey(const QString &machineKey)
{
    if (m_machineKey == machineKey) {
        return;
    }
    m_machineKey = machineKey;
    Q_EMIT machineKeyChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::userCert() const
{
    return m_userCert;
}

void OpenconnectSetting::setUserCert(const QString &userCert)
{
    if (m_userCert == userCert) {
        return;
    }
    m_userCert = userCert;
    Q_EMIT userCertChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::userKey() const
{
    return m_userKey;
}

void OpenconnectSetting::setUserKey(const QString &userKey)
{
    if (m_userKey == userKey) {
        return;
    }
    m_userKey = userKey;
    Q_EMIT userKeyChanged();
    Q_EMIT validChanged();
}

bool OpenconnectSetting::useFsid() const
{
    return m_useFsid;
}

void OpenconnectSetting::setUseFsid(bool use)
{
    if (m_useFsid == use) {
        return;
    }
    m_useFsid = use;
    Q_EMIT useFsidChanged();
    Q_EMIT validChanged();
}

bool OpenconnectSetting::preventInvalidCert() const
{
    return m_preventInvalidCert;
}

void OpenconnectSetting::setPreventInvalidCert(bool prevent)
{
    if (m_preventInvalidCert == prevent) {
        return;
    }
    m_preventInvalidCert = prevent;
    Q_EMIT preventInvalidCertChanged();
    Q_EMIT validChanged();
}

OpenconnectSetting::TokenMode OpenconnectSetting::tokenMode() const
{
    return m_tokenMode;
}

void OpenconnectSetting::setTokenMode(OpenconnectSetting::TokenMode mode)
{
    if (m_tokenMode == mode) {
        return;
    }
    m_tokenMode = mode;
    Q_EMIT tokenModeChanged();
    Q_EMIT validChanged();
}

QString OpenconnectSetting::tokenSecret() const
{
    return m_tokenSecret;
}

void OpenconnectSetting::setTokenSecret(const QString &secret)
{
    if (m_tokenSecret == secret) {
        return;
    }
    m_tokenSecret = secret;
    Q_EMIT tokenSecretChanged();
    Q_EMIT validChanged();
}

OpenconnectSetting::PasswordOption OpenconnectSetting::tokenSecretOption() const
{
    return m_tokenSecretOption;
}

void OpenconnectSetting::setTokenSecretOption(OpenconnectSetting::PasswordOption option)
{
    if (m_tokenSecretOption == option) {
        return;
    }
    m_tokenSecretOption = option;
    Q_EMIT tokenSecretOptionChanged();
    Q_EMIT validChanged();
}

#include "moc_openconnect.cpp"
