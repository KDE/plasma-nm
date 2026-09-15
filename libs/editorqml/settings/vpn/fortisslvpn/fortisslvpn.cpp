/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fortisslvpn.h"

#include "nm-fortisslvpn-service.h"

#include <QUrl>

FortisslvpnSetting::PasswordOption optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return FortisslvpnSetting::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return FortisslvpnSetting::StoreForUser;
    }
    if (flags.testFlag(NetworkManager::Setting::NotSaved)) {
        return FortisslvpnSetting::AlwaysAsk;
    }
    return FortisslvpnSetting::NotRequired;
}

QString flagsFromOption(FortisslvpnSetting::PasswordOption option)
{
    switch (option) {
    case FortisslvpnSetting::StoreForAllUsers:
        return QString::number(NetworkManager::Setting::None);
    case FortisslvpnSetting::StoreForUser:
        return QString::number(NetworkManager::Setting::AgentOwned);
    case FortisslvpnSetting::AlwaysAsk:
        return QString::number(NetworkManager::Setting::NotSaved);
    case FortisslvpnSetting::NotRequired:
        break;
    }
    return QString::number(NetworkManager::Setting::NotRequired);
}

QString pathToUrl(const QString &path)
{
    return path.isEmpty() ? QString() : QUrl::fromLocalFile(path).toString();
}

QString urlToPath(const QString &url)
{
    return url.isEmpty() ? QString() : QUrl(url).toLocalFile();
}

FortisslvpnSetting::FortisslvpnSetting(QObject *parent)
    : QObject(parent)
{
}

FortisslvpnSetting::~FortisslvpnSetting() = default;

void FortisslvpnSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setPassword(QString());

    const NMStringMap data = setting->data();

    // General
    setGateway(data.value(QLatin1String(NM_FORTISSLVPN_KEY_GATEWAY)));

    // Authentication
    setUser(data.value(QLatin1String(NM_FORTISSLVPN_KEY_USER)));
    setPasswordOption(optionFromFlags(data.value(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD "-flags"))));

    setCaCert(pathToUrl(data.value(QLatin1String(NM_FORTISSLVPN_KEY_CA))));
    setUserCert(pathToUrl(data.value(QLatin1String(NM_FORTISSLVPN_KEY_CERT))));
    setUserKey(pathToUrl(data.value(QLatin1String(NM_FORTISSLVPN_KEY_KEY))));

    // Advanced
    setTrustedCert(data.value(QLatin1String(NM_FORTISSLVPN_KEY_TRUSTED_CERT)));
    setRealm(data.value(QLatin1String(NM_FORTISSLVPN_KEY_REALM)));

    // Neither of these is a value of its own: the plugin asks for a one-time
    // password by flagging the secret as never-stored, and for 2FA by flagging
    // it agent-owned.
    const QString rawOtpFlags = data.value(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"));
    if (!rawOtpFlags.isEmpty()) {
        const auto otpFlags = static_cast<NetworkManager::Setting::SecretFlags>(rawOtpFlags.toInt());
        setUseOtp(otpFlags.testFlag(NetworkManager::Setting::NotSaved));
    }

    const QString rawTfaFlags = data.value(QLatin1String(NM_FORTISSLVPN_KEY_2FA "-flags"));
    if (!rawTfaFlags.isEmpty()) {
        const auto tfaFlags = static_cast<NetworkManager::Setting::SecretFlags>(rawTfaFlags.toInt());
        setUseTwoFactorAuth(tfaFlags.testFlag(NetworkManager::Setting::AgentOwned));
    }

    loadSecrets(setting);
}

void FortisslvpnSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const QString password = setting->secrets().value(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setPassword(password);
    }
}

QString FortisslvpnSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_FORTISSLVPN);
}

QVariantMap FortisslvpnSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_FORTISSLVPN));

    NMStringMap data;
    NMStringMap secrets;

    data.insert(QLatin1String(NM_FORTISSLVPN_KEY_GATEWAY), m_gateway);

    if (!m_user.isEmpty()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_USER), m_user);
    }

    if (!m_password.isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD), m_password);
    }
    data.insert(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD "-flags"), flagsFromOption(m_passwordOption));

    if (!m_caCert.isEmpty()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_CA), urlToPath(m_caCert));
    }

    if (!m_userCert.isEmpty()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_CERT), urlToPath(m_userCert));
    }

    if (!m_userKey.isEmpty()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_KEY), urlToPath(m_userKey));
    }

    // Advanced
    if (!m_trustedCert.isEmpty()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_TRUSTED_CERT), m_trustedCert);
    }

    data.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"), QString::number(m_useOtp ? NetworkManager::Setting::NotSaved : NetworkManager::Setting::None));

    if (m_useTwoFactorAuth) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_2FA "-flags"), QString::number(NetworkManager::Setting::AgentOwned));
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"), QString::number(NetworkManager::Setting::None));
    }

    if (!m_realm.isEmpty()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_REALM), m_realm);
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool FortisslvpnSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString FortisslvpnSetting::gateway() const
{
    return m_gateway;
}

void FortisslvpnSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::user() const
{
    return m_user;
}

void FortisslvpnSetting::setUser(const QString &user)
{
    if (m_user == user) {
        return;
    }
    m_user = user;
    Q_EMIT userChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::password() const
{
    return m_password;
}

void FortisslvpnSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
    Q_EMIT validChanged();
}

FortisslvpnSetting::PasswordOption FortisslvpnSetting::passwordOption() const
{
    return m_passwordOption;
}

void FortisslvpnSetting::setPasswordOption(PasswordOption option)
{
    if (m_passwordOption == option) {
        return;
    }
    m_passwordOption = option;
    Q_EMIT passwordOptionChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::caCert() const
{
    return m_caCert;
}

void FortisslvpnSetting::setCaCert(const QString &caCert)
{
    if (m_caCert == caCert) {
        return;
    }
    m_caCert = caCert;
    Q_EMIT caCertChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::userCert() const
{
    return m_userCert;
}

void FortisslvpnSetting::setUserCert(const QString &userCert)
{
    if (m_userCert == userCert) {
        return;
    }
    m_userCert = userCert;
    Q_EMIT userCertChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::userKey() const
{
    return m_userKey;
}

void FortisslvpnSetting::setUserKey(const QString &userKey)
{
    if (m_userKey == userKey) {
        return;
    }
    m_userKey = userKey;
    Q_EMIT userKeyChanged();
    Q_EMIT validChanged();
}

bool FortisslvpnSetting::useOtp() const
{
    return m_useOtp;
}

void FortisslvpnSetting::setUseOtp(bool use)
{
    if (m_useOtp == use) {
        return;
    }
    m_useOtp = use;
    Q_EMIT useOtpChanged();
    Q_EMIT validChanged();
}

bool FortisslvpnSetting::useTwoFactorAuth() const
{
    return m_useTwoFactorAuth;
}

void FortisslvpnSetting::setUseTwoFactorAuth(bool use)
{
    if (m_useTwoFactorAuth == use) {
        return;
    }
    m_useTwoFactorAuth = use;
    Q_EMIT useTwoFactorAuthChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::realm() const
{
    return m_realm;
}

void FortisslvpnSetting::setRealm(const QString &realm)
{
    if (m_realm == realm) {
        return;
    }
    m_realm = realm;
    Q_EMIT realmChanged();
    Q_EMIT validChanged();
}

QString FortisslvpnSetting::trustedCert() const
{
    return m_trustedCert;
}

void FortisslvpnSetting::setTrustedCert(const QString &trustedCert)
{
    if (m_trustedCert == trustedCert) {
        return;
    }
    m_trustedCert = trustedCert;
    Q_EMIT trustedCertChanged();
    Q_EMIT validChanged();
}

#include "moc_fortisslvpn.cpp"
