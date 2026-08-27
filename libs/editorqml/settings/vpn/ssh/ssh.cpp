/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "ssh.h"

#include "nm-ssh-service.h"

#include <KPluginFactory>

#include <QUrl>

SshSetting::SshSetting(QObject *parent)
    : QObject(parent)
    , m_port(NM_SSH_DEFAULT_PORT)
    , m_mtu(NM_SSH_DEFAULT_MTU)
    , m_extraOptions(QLatin1String(NM_SSH_DEFAULT_EXTRA_OPTS))
    , m_remoteDevice(NM_SSH_DEFAULT_REMOTE_DEV)
    , m_remoteUsername(QLatin1String(NM_SSH_DEFAULT_REMOTE_USERNAME))
{
}

SshSetting::~SshSetting() = default;

void SshSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap data = setting->data();

    // General
    setGateway(data[QLatin1String(NM_SSH_KEY_REMOTE)]);

    // Network settings
    setRemoteIp(data[QLatin1String(NM_SSH_KEY_REMOTE_IP)]);
    setLocalIp(data[QLatin1String(NM_SSH_KEY_LOCAL_IP)]);
    setNetmask(data[QLatin1String(NM_SSH_KEY_NETMASK)]);

    // IPv6 network settings
    setUseIpv6(data[QLatin1String(NM_SSH_KEY_IP_6)] == QLatin1String("yes"));
    if (useIpv6()) {
        setRemoteIpv6(data[QLatin1String(NM_SSH_KEY_REMOTE_IP_6)]);
        setLocalIpv6(data[QLatin1String(NM_SSH_KEY_LOCAL_IP_6)]);
        setNetmaskIpv6(data[QLatin1String(NM_SSH_KEY_NETMASK_6)]);
    }

    // Authentication
    const QString authType = data[QLatin1String(NM_SSH_KEY_AUTH_TYPE)];
    if (authType == QLatin1String(NM_SSH_AUTH_TYPE_PASSWORD)) {
        setAuthType(Password);

        const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(data[NM_SSH_KEY_PASSWORD "-flags"].toInt());
        if (flags.testFlag(NetworkManager::Setting::None)) {
            setSshPasswordOption(StoreForAllUsers);
        } else if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
            setSshPasswordOption(StoreForUser);
        } else {
            setSshPasswordOption(AlwaysAsk);
        }
    } else if (authType == QLatin1String(NM_SSH_AUTH_TYPE_KEY)) {
        setAuthType(Key);
        setKeyFile(QUrl::fromLocalFile(data[QLatin1String(NM_SSH_KEY_KEY_FILE)]).toString());
    } else {
        setAuthType(SshAgent);
    }

    // Advanced
    const QString port = data[QLatin1String(NM_SSH_KEY_PORT)];
    setUseCustomPort(!port.isEmpty());
    if (useCustomPort()) {
        setPort(port.toInt());
    }

    const QString mtu = data[QLatin1String(NM_SSH_KEY_TUNNEL_MTU)];
    setUseCustomMtu(!mtu.isEmpty());
    if (useCustomMtu()) {
        setMtu(mtu.toInt());
    }

    const QString extraOptions = data[QLatin1String(NM_SSH_KEY_EXTRA_OPTS)];
    setUseExtraOptions(!extraOptions.isEmpty());
    if (useExtraOptions()) {
        setExtraOptions(extraOptions);
    }

    const QString remoteDevice = data[QLatin1String(NM_SSH_KEY_REMOTE_DEV)];
    setUseRemoteDevice(!remoteDevice.isEmpty());
    if (useRemoteDevice()) {
        setRemoteDevice(remoteDevice.toInt());
    }

    setUseTapDevice(data[QLatin1String(NM_SSH_KEY_TAP_DEV)] == QLatin1String("yes"));

    const QString remoteUsername = data[QLatin1String(NM_SSH_KEY_REMOTE_USERNAME)];
    setUseRemoteUsername(!remoteUsername.isEmpty());
    if (useRemoteUsername()) {
        setRemoteUsername(remoteUsername);
    }

    setDoNotReplaceDefaultRoute(data[QLatin1String(NM_SSH_KEY_NO_DEFAULT_ROUTE)] == QLatin1String("yes"));

    loadSecrets(setting);
}

void SshSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const QString password = setting->secrets().value(QLatin1String(NM_SSH_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setSshPassword(password);
    }
}

QString SshSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_SSH);
}

QVariantMap SshSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_SSH));

    NMStringMap data;
    NMStringMap secrets;

    data.insert(QLatin1String(NM_SSH_KEY_REMOTE), m_gateway);

    if (!m_remoteIp.isEmpty()) {
        data.insert(QLatin1String(NM_SSH_KEY_REMOTE_IP), m_remoteIp);
    }

    if (!m_localIp.isEmpty()) {
        data.insert(QLatin1String(NM_SSH_KEY_LOCAL_IP), m_localIp);
    }

    if (!m_netmask.isEmpty()) {
        data.insert(QLatin1String(NM_SSH_KEY_NETMASK), m_netmask);
    }

    if (m_useIpv6) {
        data.insert(QLatin1String(NM_SSH_KEY_IP_6), QLatin1String("yes"));

        if (!m_remoteIpv6.isEmpty()) {
            data.insert(QLatin1String(NM_SSH_KEY_REMOTE_IP_6), m_remoteIpv6);
        }

        if (!m_localIpv6.isEmpty()) {
            data.insert(QLatin1String(NM_SSH_KEY_LOCAL_IP_6), m_localIpv6);
        }

        if (!m_netmaskIpv6.isEmpty()) {
            data.insert(QLatin1String(NM_SSH_KEY_NETMASK_6), m_netmaskIpv6);
        }
    }

    switch (m_authType) {
    case SshAgent:
        data.insert(QLatin1String(NM_SSH_KEY_AUTH_TYPE), QLatin1String(NM_SSH_AUTH_TYPE_SSH_AGENT));
        break;
    case Password: {
        data.insert(QLatin1String(NM_SSH_KEY_AUTH_TYPE), QLatin1String(NM_SSH_AUTH_TYPE_PASSWORD));

        if (!m_sshPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_SSH_KEY_PASSWORD), m_sshPassword);
        }

        NetworkManager::Setting::SecretFlagType flag = NetworkManager::Setting::AgentOwned;
        switch (m_sshPasswordOption) {
        case StoreForAllUsers:
            flag = NetworkManager::Setting::None;
            break;
        case StoreForUser:
            flag = NetworkManager::Setting::AgentOwned;
            break;
        case AlwaysAsk:
            flag = NetworkManager::Setting::NotSaved;
            break;
        }
        data.insert(QLatin1String(NM_SSH_KEY_PASSWORD "-flags"), QString::number(flag));
        break;
    }
    case Key:
        data.insert(QLatin1String(NM_SSH_KEY_AUTH_TYPE), QLatin1String(NM_SSH_AUTH_TYPE_KEY));

        if (!m_keyFile.isEmpty()) {
            data.insert(QLatin1String(NM_SSH_KEY_KEY_FILE), QUrl(m_keyFile).toLocalFile());
        }
        break;
    }

    if (m_useCustomPort) {
        data.insert(QLatin1String(NM_SSH_KEY_PORT), QString::number(m_port));
    }

    if (m_useCustomMtu) {
        data.insert(QLatin1String(NM_SSH_KEY_TUNNEL_MTU), QString::number(m_mtu));
    }

    if (m_useExtraOptions) {
        data.insert(QLatin1String(NM_SSH_KEY_EXTRA_OPTS), m_extraOptions);
    }

    if (m_useRemoteDevice) {
        data.insert(QLatin1String(NM_SSH_KEY_REMOTE_DEV), QString::number(m_remoteDevice));
    }

    if (m_useTapDevice) {
        data.insert(QLatin1String(NM_SSH_KEY_TAP_DEV), QLatin1String("yes"));
    }

    if (m_useRemoteUsername) {
        data.insert(QLatin1String(NM_SSH_KEY_REMOTE_USERNAME), m_remoteUsername);
    }

    if (m_doNotReplaceDefaultRoute) {
        data.insert(QLatin1String(NM_SSH_KEY_NO_DEFAULT_ROUTE), QLatin1String("yes"));
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool SshSetting::isValid() const
{
    return !m_gateway.isEmpty() && !m_localIp.isEmpty() && !m_remoteIp.isEmpty() && !m_netmask.isEmpty();
}

QString SshSetting::gateway() const
{
    return m_gateway;
}

void SshSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString SshSetting::remoteIp() const
{
    return m_remoteIp;
}

void SshSetting::setRemoteIp(const QString &remoteIp)
{
    if (m_remoteIp == remoteIp) {
        return;
    }
    m_remoteIp = remoteIp;
    Q_EMIT remoteIpChanged();
    Q_EMIT validChanged();
}

QString SshSetting::localIp() const
{
    return m_localIp;
}

void SshSetting::setLocalIp(const QString &localIp)
{
    if (m_localIp == localIp) {
        return;
    }
    m_localIp = localIp;
    Q_EMIT localIpChanged();
    Q_EMIT validChanged();
}

QString SshSetting::netmask() const
{
    return m_netmask;
}

void SshSetting::setNetmask(const QString &netmask)
{
    if (m_netmask == netmask) {
        return;
    }
    m_netmask = netmask;
    Q_EMIT netmaskChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useIpv6() const
{
    return m_useIpv6;
}

void SshSetting::setUseIpv6(bool useIpv6)
{
    if (m_useIpv6 == useIpv6) {
        return;
    }
    m_useIpv6 = useIpv6;
    Q_EMIT useIpv6Changed();
    Q_EMIT validChanged();
}

QString SshSetting::remoteIpv6() const
{
    return m_remoteIpv6;
}

void SshSetting::setRemoteIpv6(const QString &remoteIpv6)
{
    if (m_remoteIpv6 == remoteIpv6) {
        return;
    }
    m_remoteIpv6 = remoteIpv6;
    Q_EMIT remoteIpv6Changed();
    Q_EMIT validChanged();
}

QString SshSetting::localIpv6() const
{
    return m_localIpv6;
}

void SshSetting::setLocalIpv6(const QString &localIpv6)
{
    if (m_localIpv6 == localIpv6) {
        return;
    }
    m_localIpv6 = localIpv6;
    Q_EMIT localIpv6Changed();
    Q_EMIT validChanged();
}

QString SshSetting::netmaskIpv6() const
{
    return m_netmaskIpv6;
}

void SshSetting::setNetmaskIpv6(const QString &netmaskIpv6)
{
    if (m_netmaskIpv6 == netmaskIpv6) {
        return;
    }
    m_netmaskIpv6 = netmaskIpv6;
    Q_EMIT netmaskIpv6Changed();
    Q_EMIT validChanged();
}

SshSetting::AuthType SshSetting::authType() const
{
    return m_authType;
}

void SshSetting::setAuthType(AuthType authType)
{
    if (m_authType == authType) {
        return;
    }
    m_authType = authType;
    Q_EMIT authTypeChanged();
    Q_EMIT validChanged();
}

QString SshSetting::sshPassword() const
{
    return m_sshPassword;
}

void SshSetting::setSshPassword(const QString &password)
{
    if (m_sshPassword == password) {
        return;
    }
    m_sshPassword = password;
    Q_EMIT sshPasswordChanged();
    Q_EMIT validChanged();
}

SshSetting::PasswordOption SshSetting::sshPasswordOption() const
{
    return m_sshPasswordOption;
}

void SshSetting::setSshPasswordOption(PasswordOption option)
{
    if (m_sshPasswordOption == option) {
        return;
    }
    m_sshPasswordOption = option;
    Q_EMIT sshPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString SshSetting::keyFile() const
{
    return m_keyFile;
}

void SshSetting::setKeyFile(const QString &keyFile)
{
    if (m_keyFile == keyFile) {
        return;
    }
    m_keyFile = keyFile;
    Q_EMIT keyFileChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useCustomPort() const
{
    return m_useCustomPort;
}

void SshSetting::setUseCustomPort(bool useCustomPort)
{
    if (m_useCustomPort == useCustomPort) {
        return;
    }
    m_useCustomPort = useCustomPort;
    Q_EMIT useCustomPortChanged();
    Q_EMIT validChanged();
}

int SshSetting::port() const
{
    return m_port;
}

void SshSetting::setPort(int port)
{
    if (m_port == port) {
        return;
    }
    m_port = port;
    Q_EMIT portChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useCustomMtu() const
{
    return m_useCustomMtu;
}

void SshSetting::setUseCustomMtu(bool useCustomMtu)
{
    if (m_useCustomMtu == useCustomMtu) {
        return;
    }
    m_useCustomMtu = useCustomMtu;
    Q_EMIT useCustomMtuChanged();
    Q_EMIT validChanged();
}

int SshSetting::mtu() const
{
    return m_mtu;
}

void SshSetting::setMtu(int mtu)
{
    if (m_mtu == mtu) {
        return;
    }
    m_mtu = mtu;
    Q_EMIT mtuChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useExtraOptions() const
{
    return m_useExtraOptions;
}

void SshSetting::setUseExtraOptions(bool useExtraOptions)
{
    if (m_useExtraOptions == useExtraOptions) {
        return;
    }
    m_useExtraOptions = useExtraOptions;
    Q_EMIT useExtraOptionsChanged();
    Q_EMIT validChanged();
}

QString SshSetting::extraOptions() const
{
    return m_extraOptions;
}

void SshSetting::setExtraOptions(const QString &extraOptions)
{
    if (m_extraOptions == extraOptions) {
        return;
    }
    m_extraOptions = extraOptions;
    Q_EMIT extraOptionsChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useRemoteDevice() const
{
    return m_useRemoteDevice;
}

void SshSetting::setUseRemoteDevice(bool useRemoteDevice)
{
    if (m_useRemoteDevice == useRemoteDevice) {
        return;
    }
    m_useRemoteDevice = useRemoteDevice;
    Q_EMIT useRemoteDeviceChanged();
    Q_EMIT validChanged();
}

int SshSetting::remoteDevice() const
{
    return m_remoteDevice;
}

void SshSetting::setRemoteDevice(int remoteDevice)
{
    if (m_remoteDevice == remoteDevice) {
        return;
    }
    m_remoteDevice = remoteDevice;
    Q_EMIT remoteDeviceChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useTapDevice() const
{
    return m_useTapDevice;
}

void SshSetting::setUseTapDevice(bool useTapDevice)
{
    if (m_useTapDevice == useTapDevice) {
        return;
    }
    m_useTapDevice = useTapDevice;
    Q_EMIT useTapDeviceChanged();
    Q_EMIT validChanged();
}

bool SshSetting::useRemoteUsername() const
{
    return m_useRemoteUsername;
}

void SshSetting::setUseRemoteUsername(bool useRemoteUsername)
{
    if (m_useRemoteUsername == useRemoteUsername) {
        return;
    }
    m_useRemoteUsername = useRemoteUsername;
    Q_EMIT useRemoteUsernameChanged();
    Q_EMIT validChanged();
}

QString SshSetting::remoteUsername() const
{
    return m_remoteUsername;
}

void SshSetting::setRemoteUsername(const QString &remoteUsername)
{
    if (m_remoteUsername == remoteUsername) {
        return;
    }
    m_remoteUsername = remoteUsername;
    Q_EMIT remoteUsernameChanged();
    Q_EMIT validChanged();
}

bool SshSetting::doNotReplaceDefaultRoute() const
{
    return m_doNotReplaceDefaultRoute;
}

void SshSetting::setDoNotReplaceDefaultRoute(bool doNotReplaceDefaultRoute)
{
    if (m_doNotReplaceDefaultRoute == doNotReplaceDefaultRoute) {
        return;
    }
    m_doNotReplaceDefaultRoute = doNotReplaceDefaultRoute;
    Q_EMIT doNotReplaceDefaultRouteChanged();
    Q_EMIT validChanged();
}

#include "moc_ssh.cpp"
