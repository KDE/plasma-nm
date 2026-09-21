/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "openvpn.h"

#include "nm-openvpn-service.h"
#include "openvpnhelpers_p.h"

OpenvpnSetting::OpenvpnSetting(QObject *parent)
    : QObject(parent)
    , m_advanced(new OpenvpnAdvancedSetting(this))
{
    connect(m_advanced, &OpenvpnAdvancedSetting::changed, this, &OpenvpnSetting::validChanged);
}

OpenvpnSetting::~OpenvpnSetting() = default;

OpenvpnAdvancedSetting *OpenvpnSetting::advanced() const
{
    return m_advanced;
}

void OpenvpnSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setPassword(QString());
    setPrivateKeyPassword(QString());

    const NMStringMap data = setting->data();

    setGateway(data.value(QLatin1String(NM_OPENVPN_KEY_REMOTE)));

    const QString connectionType = data.value(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE));
    if (connectionType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS)) {
        setConnectionType(CertsPassword);
    } else if (connectionType == QLatin1String(NM_OPENVPN_CONTYPE_STATIC_KEY)) {
        setConnectionType(StaticKeyType);
    } else if (connectionType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD)) {
        setConnectionType(Password);
    } else {
        setConnectionType(Certificates);
    }

    setCaCert(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_CA))));
    setUserCert(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_CERT))));
    setPrivateKey(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_KEY))));
    setPrivateKeyPasswordOption(static_cast<PasswordOption>(OpenvpnHelpers::optionFromFlags(data.value(QLatin1String(NM_OPENVPN_KEY_CERTPASS_FLAGS)))));

    setUsername(data.value(QLatin1String(NM_OPENVPN_KEY_USERNAME)));
    setPasswordOption(static_cast<PasswordOption>(OpenvpnHelpers::optionFromFlags(data.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD_FLAGS)))));

    setStaticKey(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY))));
    if (data.contains(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION))) {
        setKeyDirection(data.value(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION)).toUInt() == 1 ? Direction1 : Direction0);
    } else {
        setKeyDirection(NoDirection);
    }
    setLocalIp(data.value(QLatin1String(NM_OPENVPN_KEY_LOCAL_IP)));
    setRemoteIp(data.value(QLatin1String(NM_OPENVPN_KEY_REMOTE_IP)));

    m_advanced->loadConfig(setting);

    loadSecrets(setting);
}

void OpenvpnSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap secrets = setting->secrets();

    const QString password = secrets.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setPassword(password);
    }

    const QString certPass = secrets.value(QLatin1String(NM_OPENVPN_KEY_CERTPASS));
    if (!certPass.isEmpty()) {
        setPrivateKeyPassword(certPass);
    }

    m_advanced->loadSecrets(setting);
}

QString OpenvpnSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_OPENVPN);
}

QVariantMap OpenvpnSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_OPENVPN));

    NMStringMap data = m_advanced->data();
    NMStringMap secrets = m_advanced->secrets();

    data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE), m_gateway);

    switch (m_connectionType) {
    case Certificates:
        data.insert(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE), QLatin1String(NM_OPENVPN_CONTYPE_TLS));
        data.insert(QLatin1String(NM_OPENVPN_KEY_CA), OpenvpnHelpers::urlToPath(m_caCert));
        data.insert(QLatin1String(NM_OPENVPN_KEY_CERT), OpenvpnHelpers::urlToPath(m_userCert));
        data.insert(QLatin1String(NM_OPENVPN_KEY_KEY), OpenvpnHelpers::urlToPath(m_privateKey));
        if (!m_privateKeyPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS), m_privateKeyPassword);
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS_FLAGS), OpenvpnHelpers::flagsFromOption(m_privateKeyPasswordOption));
        break;

    case StaticKeyType:
        data.insert(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE), QLatin1String(NM_OPENVPN_CONTYPE_STATIC_KEY));
        data.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY), OpenvpnHelpers::urlToPath(m_staticKey));
        if (m_keyDirection != NoDirection) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION), QString::number(m_keyDirection == Direction1 ? 1 : 0));
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_IP), m_remoteIp);
        data.insert(QLatin1String(NM_OPENVPN_KEY_LOCAL_IP), m_localIp);
        break;

    case Password:
        data.insert(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE), QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD));
        if (!m_username.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_USERNAME), m_username);
        }
        if (!m_password.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD), m_password);
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD_FLAGS), OpenvpnHelpers::flagsFromOption(m_passwordOption));
        data.insert(QLatin1String(NM_OPENVPN_KEY_CA), OpenvpnHelpers::urlToPath(m_caCert));
        break;

    case CertsPassword:
        data.insert(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE), QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS));
        if (!m_username.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_USERNAME), m_username);
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_CA), OpenvpnHelpers::urlToPath(m_caCert));
        data.insert(QLatin1String(NM_OPENVPN_KEY_CERT), OpenvpnHelpers::urlToPath(m_userCert));
        data.insert(QLatin1String(NM_OPENVPN_KEY_KEY), OpenvpnHelpers::urlToPath(m_privateKey));
        if (!m_privateKeyPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS), m_privateKeyPassword);
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS_FLAGS), OpenvpnHelpers::flagsFromOption(m_privateKeyPasswordOption));
        if (!m_password.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD), m_password);
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD_FLAGS), OpenvpnHelpers::flagsFromOption(m_passwordOption));
        break;
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool OpenvpnSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

#define OPENVPN_PROPERTY(Type, name, Name, member)                                                                                                             \
    Type OpenvpnSetting::name() const                                                                                                                          \
    {                                                                                                                                                          \
        return member;                                                                                                                                         \
    }                                                                                                                                                          \
    void OpenvpnSetting::set##Name(Type value)                                                                                                                 \
    {                                                                                                                                                          \
        if (member == value) {                                                                                                                                 \
            return;                                                                                                                                            \
        }                                                                                                                                                      \
        member = value;                                                                                                                                        \
        Q_EMIT name##Changed();                                                                                                                                \
        Q_EMIT validChanged();                                                                                                                                 \
    }

#define OPENVPN_STRING_PROPERTY(name, Name, member)                                                                                                            \
    QString OpenvpnSetting::name() const                                                                                                                       \
    {                                                                                                                                                          \
        return member;                                                                                                                                         \
    }                                                                                                                                                          \
    void OpenvpnSetting::set##Name(const QString &value)                                                                                                       \
    {                                                                                                                                                          \
        if (member == value) {                                                                                                                                 \
            return;                                                                                                                                            \
        }                                                                                                                                                      \
        member = value;                                                                                                                                        \
        Q_EMIT name##Changed();                                                                                                                                \
        Q_EMIT validChanged();                                                                                                                                 \
    }

OPENVPN_STRING_PROPERTY(gateway, Gateway, m_gateway)
OPENVPN_PROPERTY(OpenvpnSetting::ConnectionType, connectionType, ConnectionType, m_connectionType)

OPENVPN_STRING_PROPERTY(caCert, CaCert, m_caCert)
OPENVPN_STRING_PROPERTY(userCert, UserCert, m_userCert)
OPENVPN_STRING_PROPERTY(privateKey, PrivateKey, m_privateKey)
OPENVPN_STRING_PROPERTY(privateKeyPassword, PrivateKeyPassword, m_privateKeyPassword)
OPENVPN_PROPERTY(OpenvpnSetting::PasswordOption, privateKeyPasswordOption, PrivateKeyPasswordOption, m_privateKeyPasswordOption)

OPENVPN_STRING_PROPERTY(username, Username, m_username)
OPENVPN_STRING_PROPERTY(password, Password, m_password)
OPENVPN_PROPERTY(OpenvpnSetting::PasswordOption, passwordOption, PasswordOption, m_passwordOption)

OPENVPN_STRING_PROPERTY(staticKey, StaticKey, m_staticKey)
OPENVPN_PROPERTY(OpenvpnSetting::KeyDirection, keyDirection, KeyDirection, m_keyDirection)
OPENVPN_STRING_PROPERTY(localIp, LocalIp, m_localIp)
OPENVPN_STRING_PROPERTY(remoteIp, RemoteIp, m_remoteIp)

#undef OPENVPN_PROPERTY
#undef OPENVPN_STRING_PROPERTY

#include "moc_openvpn.cpp"
