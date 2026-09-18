/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "l2tp.h"

#include "nm-l2tp-service.h"

#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

namespace
{
enum class IpsecDaemon {
    None,
    Libreswan,
    Strongswan,
    Unknown
};

IpsecDaemon probeIpsecDaemon()
{
    QString ipsecBinary = QStandardPaths::findExecutable(QStringLiteral("ipsec"), {QStringLiteral("/sbin"), QStringLiteral("/usr/sbin")});

    // On some Linux distributions, ipsec executable has been renamed strongswan
    if (ipsecBinary.isEmpty()) {
        ipsecBinary = QStandardPaths::findExecutable(QStringLiteral("strongswan"), {QStringLiteral("/sbin"), QStringLiteral("/usr/sbin")});
    }

    if (ipsecBinary.isEmpty()) {
        return IpsecDaemon::None;
    }

    QProcess ipsecVersionProcess;
    ipsecVersionProcess.setProgram(ipsecBinary);
    ipsecVersionProcess.setArguments({QStringLiteral("--version")});
    ipsecVersionProcess.start();
    ipsecVersionProcess.waitForFinished(-1);

    if (ipsecVersionProcess.exitStatus() != QProcess::NormalExit) {
        return IpsecDaemon::Unknown;
    }

    const QString version = QString::fromUtf8(ipsecVersionProcess.readAllStandardOutput());
    if (version.contains(QLatin1String("strongSwan"), Qt::CaseSensitive)) {
        return IpsecDaemon::Strongswan;
    }
    if (version.contains(QLatin1String("Libreswan"), Qt::CaseSensitive)) {
        return IpsecDaemon::Libreswan;
    }
    return IpsecDaemon::Unknown;
}

IpsecDaemon ipsecDaemon()
{
    static const IpsecDaemon daemon = probeIpsecDaemon();
    return daemon;
}

constexpr int OneHour = 3600;

int defaultIkeLifetime()
{
    return ipsecDaemon() == IpsecDaemon::Libreswan ? 1 * OneHour : 3 * OneHour;
}

int defaultSaLifetime()
{
    return ipsecDaemon() == IpsecDaemon::Libreswan ? 8 * OneHour : 1 * OneHour;
}

L2tpSetting::PasswordOption optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return L2tpSetting::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return L2tpSetting::StoreForUser;
    }
    if (flags.testFlag(NetworkManager::Setting::NotSaved)) {
        return L2tpSetting::AlwaysAsk;
    }
    return L2tpSetting::NotRequired;
}

QString flagsFromOption(L2tpSetting::PasswordOption option)
{
    switch (option) {
    case L2tpSetting::StoreForAllUsers:
        return QString::number(NetworkManager::Setting::None);
    case L2tpSetting::StoreForUser:
        return QString::number(NetworkManager::Setting::AgentOwned);
    case L2tpSetting::AlwaysAsk:
        return QString::number(NetworkManager::Setting::NotSaved);
    case L2tpSetting::NotRequired:
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

QString decodePresharedKey(const QString &key)
{
    if (key.length() > 2 && key.startsWith(QLatin1String("0s"))) {
        return QString::fromUtf8(QByteArray::fromBase64(key.mid(2).toUtf8()));
    }
    return key;
}
}

L2tpSetting::L2tpSetting(QObject *parent)
    : QObject(parent)
    , m_ikeLifetime(defaultIkeLifetime())
    , m_saLifetime(defaultSaLifetime())
{
}

L2tpSetting::~L2tpSetting() = default;

void L2tpSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setPassword(QString());
    setMachineKeyPassword(QString());
    setUserKeyPassword(QString());
    setPresharedKey(QString());

    const NMStringMap data = setting->data();
    const QLatin1String yesString("yes");
    const QLatin1String noString("no");

    setGateway(data.value(QLatin1String(NM_L2TP_KEY_GATEWAY)));

    // User authentication
    const bool usesTls = data.value(QLatin1String(NM_L2TP_KEY_USER_AUTH_TYPE)) == QLatin1String(NM_L2TP_AUTHTYPE_TLS);
    setAuthType(usesTls ? TlsAuth : PasswordAuth);

    setUsername(data.value(QLatin1String(NM_L2TP_KEY_USER)));
    setDomain(data.value(QLatin1String(NM_L2TP_KEY_DOMAIN)));

    const PasswordOption passwordOption = optionFromFlags(data.value(QLatin1String(NM_L2TP_KEY_PASSWORD "-flags")));
    setPasswordOption(passwordOption == NotRequired ? AlwaysAsk : passwordOption);

    setUserCa(pathToUrl(data.value(QLatin1String(NM_L2TP_KEY_USER_CA))));
    setUserCert(pathToUrl(data.value(QLatin1String(NM_L2TP_KEY_USER_CERT))));
    setUserKey(pathToUrl(data.value(QLatin1String(NM_L2TP_KEY_USER_KEY))));
    setUserKeyPasswordOption(optionFromFlags(data.value(QLatin1String(NM_L2TP_KEY_USER_CERTPASS "-flags"))));

    setEphemeralPort(data.value(QLatin1String(NM_L2TP_KEY_EPHEMERAL_PORT)) == yesString);

    // Machine authentication (IPsec)
    setIpsecEnabled(ipsecDaemonAvailable() && data.value(QLatin1String(NM_L2TP_KEY_IPSEC_ENABLE)) == yesString);

    const bool machineUsesTls = data.value(QLatin1String(NM_L2TP_KEY_MACHINE_AUTH_TYPE)) == QLatin1String(NM_L2TP_AUTHTYPE_TLS);
    setMachineAuthType(machineUsesTls ? MachineTlsAuth : PresharedKeyAuth);

    const QString legacyPresharedKey = data.value(QLatin1String(NM_L2TP_KEY_IPSEC_PSK));
    if (!legacyPresharedKey.isEmpty()) {
        setPresharedKey(decodePresharedKey(legacyPresharedKey));
    }
    setPresharedKeyOption(optionFromFlags(data.value(QLatin1String(NM_L2TP_KEY_IPSEC_PSK "-flags"))));

    setMachineCa(pathToUrl(data.value(QLatin1String(NM_L2TP_KEY_MACHINE_CA))));
    setMachineCert(pathToUrl(data.value(QLatin1String(NM_L2TP_KEY_MACHINE_CERT))));
    setMachineKey(pathToUrl(data.value(QLatin1String(NM_L2TP_KEY_MACHINE_KEY))));
    setMachineKeyPasswordOption(optionFromFlags(data.value(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS "-flags"))));

    const QString gatewayId = data.value(QLatin1String(NM_L2TP_KEY_IPSEC_GATEWAY_ID));
    setRemoteId(gatewayId.isEmpty() ? data.value(QLatin1String(NM_L2TP_KEY_IPSEC_REMOTE_ID)) : gatewayId);

    setIpsecIke(data.value(QLatin1String(NM_L2TP_KEY_IPSEC_IKE)));
    setIpsecEsp(data.value(QLatin1String(NM_L2TP_KEY_IPSEC_ESP)));

    const QString rawIkeLifetime = data.value(QLatin1String(NM_L2TP_KEY_IPSEC_IKELIFETIME));
    setUseIkeLifetime(!rawIkeLifetime.isEmpty());
    if (!rawIkeLifetime.isEmpty()) {
        setIkeLifetime(rawIkeLifetime.toInt());
    }

    const QString rawSaLifetime = data.value(QLatin1String(NM_L2TP_KEY_IPSEC_SALIFETIME));
    setUseSaLifetime(!rawSaLifetime.isEmpty());
    if (!rawSaLifetime.isEmpty()) {
        setSaLifetime(rawSaLifetime.toInt());
    }

    setEnforceUdpEncapsulation(data.value(QLatin1String(NM_L2TP_KEY_IPSEC_FORCEENCAPS)) == yesString);
    setUseIpCompression(data.value(QLatin1String(NM_L2TP_KEY_IPSEC_IPCOMP)) == yesString);
    setUseIkev2(data.value(QLatin1String(NM_L2TP_KEY_IPSEC_IKEV2)) == yesString);
    setDisablePfs(ipsecSupportsPfs() && data.value(QLatin1String(NM_L2TP_KEY_IPSEC_PFS)) == noString);

    // PPP
    setAllowPap(data.value(QLatin1String(NM_L2TP_KEY_REFUSE_PAP)) != yesString);
    setAllowChap(data.value(QLatin1String(NM_L2TP_KEY_REFUSE_CHAP)) != yesString);
    setAllowMschap(data.value(QLatin1String(NM_L2TP_KEY_REFUSE_MSCHAP)) != yesString);
    setAllowMschapv2(data.value(QLatin1String(NM_L2TP_KEY_REFUSE_MSCHAPV2)) != yesString);
    setAllowEap(data.value(QLatin1String(NM_L2TP_KEY_REFUSE_EAP)) != yesString);

    const bool mppe = data.value(QLatin1String(NM_L2TP_KEY_REQUIRE_MPPE)) == yesString;
    const bool mppe40 = data.value(QLatin1String(NM_L2TP_KEY_REQUIRE_MPPE_40)) == yesString;
    const bool mppe128 = data.value(QLatin1String(NM_L2TP_KEY_REQUIRE_MPPE_128)) == yesString;

    setUseMppe(mppe || mppe40 || mppe128);
    if (useMppe()) {
        if (mppe128) {
            setMppeCrypto(Mppe128);
        } else if (mppe40) {
            setMppeCrypto(Mppe40);
        } else {
            setMppeCrypto(MppeAny);
        }
        setStatefulEncryption(data.value(QLatin1String(NM_L2TP_KEY_MPPE_STATEFUL)) == yesString);
    }

    setAllowBsdCompression(data.value(QLatin1String(NM_L2TP_KEY_NOBSDCOMP)) != yesString);
    setAllowDeflateCompression(data.value(QLatin1String(NM_L2TP_KEY_NODEFLATE)) != yesString);
    setAllowTcpHeaderCompression(data.value(QLatin1String(NM_L2TP_KEY_NO_VJ_COMP)) != yesString);
    setUseProtocolFieldCompression(data.value(QLatin1String(NM_L2TP_KEY_NO_PCOMP)) != yesString);
    setUseAddressControlCompression(data.value(QLatin1String(NM_L2TP_KEY_NO_ACCOMP)) != yesString);

    setSendPppEchoPackets(data.value(QLatin1String(NM_L2TP_KEY_LCP_ECHO_INTERVAL)).toInt() > 0);

    const QString rawMrru = data.value(QLatin1String(NM_L2TP_KEY_MRRU));
    setUseMrru(!rawMrru.isEmpty());
    if (!rawMrru.isEmpty()) {
        setMrru(rawMrru.toInt());
    }

    setMru(data.value(QLatin1String(NM_L2TP_KEY_MRU)).toInt());
    setMtu(data.value(QLatin1String(NM_L2TP_KEY_MTU)).toInt());

    loadSecrets(setting);
}

void L2tpSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap secrets = setting->secrets();

    if (m_authType == TlsAuth) {
        const QString userKeyPassword = secrets.value(QLatin1String(NM_L2TP_KEY_USER_CERTPASS));
        if (!userKeyPassword.isEmpty()) {
            setUserKeyPassword(userKeyPassword);
        }
    } else {
        const QString password = secrets.value(QLatin1String(NM_L2TP_KEY_PASSWORD));
        if (!password.isEmpty()) {
            setPassword(password);
        }
    }

    const QString machineKeyPassword = secrets.value(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS));
    if (!machineKeyPassword.isEmpty()) {
        setMachineKeyPassword(machineKeyPassword);
    }

    const QString presharedKey = secrets.value(QLatin1String(NM_L2TP_KEY_IPSEC_PSK));
    if (!presharedKey.isEmpty()) {
        setPresharedKey(decodePresharedKey(presharedKey));
    }
}

QString L2tpSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_L2TP);
}

QVariantMap L2tpSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_L2TP));

    NMStringMap data;
    NMStringMap secrets;

    const QLatin1String yesString("yes");
    const QLatin1String noString("no");

    if (!m_gateway.isEmpty()) {
        data.insert(QLatin1String(NM_L2TP_KEY_GATEWAY), m_gateway);
    }

    if (m_authType == PasswordAuth) {
        if (!m_username.isEmpty()) {
            data.insert(QLatin1String(NM_L2TP_KEY_USER), m_username);
        }

        if (!m_password.isEmpty()) {
            secrets.insert(QLatin1String(NM_L2TP_KEY_PASSWORD), m_password);
        }
        data.insert(QLatin1String(NM_L2TP_KEY_PASSWORD "-flags"), flagsFromOption(m_passwordOption));

        if (!m_domain.isEmpty()) {
            data.insert(QLatin1String(NM_L2TP_KEY_DOMAIN), m_domain);
        }
    } else {
        data.insert(QLatin1String(NM_L2TP_KEY_USER_AUTH_TYPE), QLatin1String(NM_L2TP_AUTHTYPE_TLS));

        data.insert(QLatin1String(NM_L2TP_KEY_USER_CA), urlToPath(m_userCa));
        data.insert(QLatin1String(NM_L2TP_KEY_USER_CERT), urlToPath(m_userCert));
        data.insert(QLatin1String(NM_L2TP_KEY_USER_KEY), urlToPath(m_userKey));

        if (!m_userKeyPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_L2TP_KEY_USER_CERTPASS), m_userKeyPassword);
        }
        data.insert(QLatin1String(NM_L2TP_KEY_USER_CERTPASS "-flags"), flagsFromOption(m_userKeyPasswordOption));
    }

    data.insert(QLatin1String(NM_L2TP_KEY_EPHEMERAL_PORT), m_ephemeralPort ? yesString : noString);

    if (m_ipsecEnabled) {
        data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_ENABLE), yesString);

        if (m_machineAuthType == PresharedKeyAuth) {
            data.insert(QLatin1String(NM_L2TP_KEY_MACHINE_AUTH_TYPE), QLatin1String(NM_L2TP_AUTHTYPE_PSK));

            if (!m_presharedKey.isEmpty()) {
                secrets.insert(QLatin1String(NM_L2TP_KEY_IPSEC_PSK), m_presharedKey);
            }
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_PSK "-flags"), flagsFromOption(m_presharedKeyOption));
        } else {
            data.insert(QLatin1String(NM_L2TP_KEY_MACHINE_AUTH_TYPE), QLatin1String(NM_L2TP_AUTHTYPE_TLS));

            data.insert(QLatin1String(NM_L2TP_KEY_MACHINE_CA), urlToPath(m_machineCa));
            data.insert(QLatin1String(NM_L2TP_KEY_MACHINE_CERT), urlToPath(m_machineCert));
            data.insert(QLatin1String(NM_L2TP_KEY_MACHINE_KEY), urlToPath(m_machineKey));

            if (!m_machineKeyPassword.isEmpty()) {
                secrets.insert(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS), m_machineKeyPassword);
            }
            data.insert(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS "-flags"), flagsFromOption(m_machineKeyPasswordOption));
        }

        if (!m_remoteId.isEmpty()) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_GATEWAY_ID), m_remoteId);
        }

        if (!m_ipsecIke.isEmpty()) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_IKE), m_ipsecIke);
        }

        if (!m_ipsecEsp.isEmpty()) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_ESP), m_ipsecEsp);
        }

        if (m_useIkeLifetime) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_IKELIFETIME), QString::number(m_ikeLifetime));
        }

        if (m_useSaLifetime) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_SALIFETIME), QString::number(m_saLifetime));
        }

        if (m_enforceUdpEncapsulation) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_FORCEENCAPS), yesString);
        }

        if (m_useIpCompression) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_IPCOMP), yesString);
        }

        if (m_useIkev2) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_IKEV2), yesString);
        }

        if (ipsecSupportsPfs() && m_disablePfs) {
            data.insert(QLatin1String(NM_L2TP_KEY_IPSEC_PFS), noString);
        }
    }

    if (m_authType == PasswordAuth) {
        if (!m_allowPap) {
            data.insert(QLatin1String(NM_L2TP_KEY_REFUSE_PAP), yesString);
        }
        if (!m_allowChap) {
            data.insert(QLatin1String(NM_L2TP_KEY_REFUSE_CHAP), yesString);
        }
        if (!m_allowMschap) {
            data.insert(QLatin1String(NM_L2TP_KEY_REFUSE_MSCHAP), yesString);
        }
        if (!m_allowMschapv2) {
            data.insert(QLatin1String(NM_L2TP_KEY_REFUSE_MSCHAPV2), yesString);
        }
        if (!m_allowEap) {
            data.insert(QLatin1String(NM_L2TP_KEY_REFUSE_EAP), yesString);
        }
    }

    if (m_useMppe) {
        switch (m_mppeCrypto) {
        case MppeAny:
            data.insert(QLatin1String(NM_L2TP_KEY_REQUIRE_MPPE), yesString);
            break;
        case Mppe128:
            data.insert(QLatin1String(NM_L2TP_KEY_REQUIRE_MPPE_128), yesString);
            break;
        case Mppe40:
            data.insert(QLatin1String(NM_L2TP_KEY_REQUIRE_MPPE_40), yesString);
            break;
        }

        if (m_statefulEncryption) {
            data.insert(QLatin1String(NM_L2TP_KEY_MPPE_STATEFUL), yesString);
        }
    }

    if (!m_allowBsdCompression) {
        data.insert(QLatin1String(NM_L2TP_KEY_NOBSDCOMP), yesString);
    }

    if (!m_allowDeflateCompression) {
        data.insert(QLatin1String(NM_L2TP_KEY_NODEFLATE), yesString);
    }

    if (!m_allowTcpHeaderCompression) {
        data.insert(QLatin1String(NM_L2TP_KEY_NO_VJ_COMP), yesString);
    }

    if (!m_useProtocolFieldCompression) {
        data.insert(QLatin1String(NM_L2TP_KEY_NO_PCOMP), yesString);
    }

    if (!m_useAddressControlCompression) {
        data.insert(QLatin1String(NM_L2TP_KEY_NO_ACCOMP), yesString);
    }

    if (m_sendPppEchoPackets) {
        data.insert(QLatin1String(NM_L2TP_KEY_LCP_ECHO_FAILURE), QStringLiteral("5"));
        data.insert(QLatin1String(NM_L2TP_KEY_LCP_ECHO_INTERVAL), QStringLiteral("30"));
    }

    if (m_useMrru) {
        data.insert(QLatin1String(NM_L2TP_KEY_MRRU), QString::number(m_mrru));
    }

    if (m_mtu != 0) {
        data.insert(QLatin1String(NM_L2TP_KEY_MTU), QString::number(m_mtu));
    }

    if (m_mru != 0) {
        data.insert(QLatin1String(NM_L2TP_KEY_MRU), QString::number(m_mru));
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool L2tpSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString L2tpSetting::gateway() const
{
    return m_gateway;
}

void L2tpSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

L2tpSetting::AuthType L2tpSetting::authType() const
{
    return m_authType;
}

void L2tpSetting::setAuthType(AuthType type)
{
    if (m_authType == type) {
        return;
    }
    m_authType = type;
    Q_EMIT authTypeChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::username() const
{
    return m_username;
}

void L2tpSetting::setUsername(const QString &username)
{
    if (m_username == username) {
        return;
    }
    m_username = username;
    Q_EMIT usernameChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::password() const
{
    return m_password;
}

void L2tpSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
    Q_EMIT validChanged();
}

L2tpSetting::PasswordOption L2tpSetting::passwordOption() const
{
    return m_passwordOption;
}

void L2tpSetting::setPasswordOption(PasswordOption option)
{
    if (m_passwordOption == option) {
        return;
    }
    m_passwordOption = option;
    Q_EMIT passwordOptionChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::domain() const
{
    return m_domain;
}

void L2tpSetting::setDomain(const QString &domain)
{
    if (m_domain == domain) {
        return;
    }
    m_domain = domain;
    Q_EMIT domainChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::userCa() const
{
    return m_userCa;
}

void L2tpSetting::setUserCa(const QString &userCa)
{
    if (m_userCa == userCa) {
        return;
    }
    m_userCa = userCa;
    Q_EMIT userCaChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::userCert() const
{
    return m_userCert;
}

void L2tpSetting::setUserCert(const QString &userCert)
{
    if (m_userCert == userCert) {
        return;
    }
    m_userCert = userCert;
    Q_EMIT userCertChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::userKey() const
{
    return m_userKey;
}

void L2tpSetting::setUserKey(const QString &userKey)
{
    if (m_userKey == userKey) {
        return;
    }
    m_userKey = userKey;
    Q_EMIT userKeyChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::userKeyPassword() const
{
    return m_userKeyPassword;
}

void L2tpSetting::setUserKeyPassword(const QString &password)
{
    if (m_userKeyPassword == password) {
        return;
    }
    m_userKeyPassword = password;
    Q_EMIT userKeyPasswordChanged();
    Q_EMIT validChanged();
}

L2tpSetting::PasswordOption L2tpSetting::userKeyPasswordOption() const
{
    return m_userKeyPasswordOption;
}

void L2tpSetting::setUserKeyPasswordOption(PasswordOption option)
{
    if (m_userKeyPasswordOption == option) {
        return;
    }
    m_userKeyPasswordOption = option;
    Q_EMIT userKeyPasswordOptionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::ephemeralPort() const
{
    return m_ephemeralPort;
}

void L2tpSetting::setEphemeralPort(bool ephemeral)
{
    if (m_ephemeralPort == ephemeral) {
        return;
    }
    m_ephemeralPort = ephemeral;
    Q_EMIT ephemeralPortChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::ipsecDaemonAvailable() const
{
    // NetworkManager-l2tp currently only supports libreswan and strongswan.
    const IpsecDaemon daemon = ipsecDaemon();
    return daemon == IpsecDaemon::Libreswan || daemon == IpsecDaemon::Strongswan;
}

bool L2tpSetting::ipsecSupportsPfs() const
{
    return ipsecDaemon() == IpsecDaemon::Libreswan;
}

bool L2tpSetting::ipsecEnabled() const
{
    return m_ipsecEnabled;
}

void L2tpSetting::setIpsecEnabled(bool enabled)
{
    if (m_ipsecEnabled == enabled) {
        return;
    }
    m_ipsecEnabled = enabled;
    Q_EMIT ipsecEnabledChanged();
    Q_EMIT validChanged();
}

L2tpSetting::MachineAuthType L2tpSetting::machineAuthType() const
{
    return m_machineAuthType;
}

void L2tpSetting::setMachineAuthType(MachineAuthType type)
{
    if (m_machineAuthType == type) {
        return;
    }
    m_machineAuthType = type;
    Q_EMIT machineAuthTypeChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::presharedKey() const
{
    return m_presharedKey;
}

void L2tpSetting::setPresharedKey(const QString &key)
{
    if (m_presharedKey == key) {
        return;
    }
    m_presharedKey = key;
    Q_EMIT presharedKeyChanged();
    Q_EMIT validChanged();
}

L2tpSetting::PasswordOption L2tpSetting::presharedKeyOption() const
{
    return m_presharedKeyOption;
}

void L2tpSetting::setPresharedKeyOption(PasswordOption option)
{
    if (m_presharedKeyOption == option) {
        return;
    }
    m_presharedKeyOption = option;
    Q_EMIT presharedKeyOptionChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::machineCa() const
{
    return m_machineCa;
}

void L2tpSetting::setMachineCa(const QString &machineCa)
{
    if (m_machineCa == machineCa) {
        return;
    }
    m_machineCa = machineCa;
    Q_EMIT machineCaChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::machineCert() const
{
    return m_machineCert;
}

void L2tpSetting::setMachineCert(const QString &machineCert)
{
    if (m_machineCert == machineCert) {
        return;
    }
    m_machineCert = machineCert;
    Q_EMIT machineCertChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::machineKey() const
{
    return m_machineKey;
}

void L2tpSetting::setMachineKey(const QString &machineKey)
{
    if (m_machineKey == machineKey) {
        return;
    }
    m_machineKey = machineKey;
    Q_EMIT machineKeyChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::machineKeyPassword() const
{
    return m_machineKeyPassword;
}

void L2tpSetting::setMachineKeyPassword(const QString &password)
{
    if (m_machineKeyPassword == password) {
        return;
    }
    m_machineKeyPassword = password;
    Q_EMIT machineKeyPasswordChanged();
    Q_EMIT validChanged();
}

L2tpSetting::PasswordOption L2tpSetting::machineKeyPasswordOption() const
{
    return m_machineKeyPasswordOption;
}

void L2tpSetting::setMachineKeyPasswordOption(PasswordOption option)
{
    if (m_machineKeyPasswordOption == option) {
        return;
    }
    m_machineKeyPasswordOption = option;
    Q_EMIT machineKeyPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::remoteId() const
{
    return m_remoteId;
}

void L2tpSetting::setRemoteId(const QString &remoteId)
{
    if (m_remoteId == remoteId) {
        return;
    }
    m_remoteId = remoteId;
    Q_EMIT remoteIdChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::ipsecIke() const
{
    return m_ipsecIke;
}

void L2tpSetting::setIpsecIke(const QString &ike)
{
    if (m_ipsecIke == ike) {
        return;
    }
    m_ipsecIke = ike;
    Q_EMIT ipsecIkeChanged();
    Q_EMIT validChanged();
}

QString L2tpSetting::ipsecEsp() const
{
    return m_ipsecEsp;
}

void L2tpSetting::setIpsecEsp(const QString &esp)
{
    if (m_ipsecEsp == esp) {
        return;
    }
    m_ipsecEsp = esp;
    Q_EMIT ipsecEspChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useIkeLifetime() const
{
    return m_useIkeLifetime;
}

void L2tpSetting::setUseIkeLifetime(bool use)
{
    if (m_useIkeLifetime == use) {
        return;
    }
    m_useIkeLifetime = use;
    Q_EMIT useIkeLifetimeChanged();

    if (!use) {
        setIkeLifetime(defaultIkeLifetime());
    }

    Q_EMIT validChanged();
}

int L2tpSetting::ikeLifetime() const
{
    return m_ikeLifetime;
}

void L2tpSetting::setIkeLifetime(int seconds)
{
    if (m_ikeLifetime == seconds) {
        return;
    }
    m_ikeLifetime = seconds;
    Q_EMIT ikeLifetimeChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useSaLifetime() const
{
    return m_useSaLifetime;
}

void L2tpSetting::setUseSaLifetime(bool use)
{
    if (m_useSaLifetime == use) {
        return;
    }
    m_useSaLifetime = use;
    Q_EMIT useSaLifetimeChanged();

    if (!use) {
        setSaLifetime(defaultSaLifetime());
    }

    Q_EMIT validChanged();
}

int L2tpSetting::saLifetime() const
{
    return m_saLifetime;
}

void L2tpSetting::setSaLifetime(int seconds)
{
    if (m_saLifetime == seconds) {
        return;
    }
    m_saLifetime = seconds;
    Q_EMIT saLifetimeChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::enforceUdpEncapsulation() const
{
    return m_enforceUdpEncapsulation;
}

void L2tpSetting::setEnforceUdpEncapsulation(bool enforce)
{
    if (m_enforceUdpEncapsulation == enforce) {
        return;
    }
    m_enforceUdpEncapsulation = enforce;
    Q_EMIT enforceUdpEncapsulationChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useIpCompression() const
{
    return m_useIpCompression;
}

void L2tpSetting::setUseIpCompression(bool use)
{
    if (m_useIpCompression == use) {
        return;
    }
    m_useIpCompression = use;
    Q_EMIT useIpCompressionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useIkev2() const
{
    return m_useIkev2;
}

void L2tpSetting::setUseIkev2(bool use)
{
    if (m_useIkev2 == use) {
        return;
    }
    m_useIkev2 = use;
    Q_EMIT useIkev2Changed();
    Q_EMIT validChanged();
}

bool L2tpSetting::disablePfs() const
{
    return m_disablePfs;
}

void L2tpSetting::setDisablePfs(bool disable)
{
    if (m_disablePfs == disable) {
        return;
    }
    m_disablePfs = disable;
    Q_EMIT disablePfsChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowPap() const
{
    return m_allowPap;
}

void L2tpSetting::setAllowPap(bool allow)
{
    if (m_allowPap == allow) {
        return;
    }
    m_allowPap = allow;
    Q_EMIT allowPapChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowChap() const
{
    return m_allowChap;
}

void L2tpSetting::setAllowChap(bool allow)
{
    if (m_allowChap == allow) {
        return;
    }
    m_allowChap = allow;
    Q_EMIT allowChapChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowMschap() const
{
    return m_allowMschap;
}

void L2tpSetting::setAllowMschap(bool allow)
{
    if (m_allowMschap == allow) {
        return;
    }
    m_allowMschap = allow;
    Q_EMIT allowMschapChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowMschapv2() const
{
    return m_allowMschapv2;
}

void L2tpSetting::setAllowMschapv2(bool allow)
{
    if (m_allowMschapv2 == allow) {
        return;
    }
    m_allowMschapv2 = allow;
    Q_EMIT allowMschapv2Changed();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowEap() const
{
    return m_allowEap;
}

void L2tpSetting::setAllowEap(bool allow)
{
    if (m_allowEap == allow) {
        return;
    }
    m_allowEap = allow;
    Q_EMIT allowEapChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useMppe() const
{
    return m_useMppe;
}

void L2tpSetting::setUseMppe(bool use)
{
    if (m_useMppe == use) {
        return;
    }
    m_useMppe = use;
    Q_EMIT useMppeChanged();
    Q_EMIT validChanged();
}

L2tpSetting::MppeCrypto L2tpSetting::mppeCrypto() const
{
    return m_mppeCrypto;
}

void L2tpSetting::setMppeCrypto(MppeCrypto crypto)
{
    if (m_mppeCrypto == crypto) {
        return;
    }
    m_mppeCrypto = crypto;
    Q_EMIT mppeCryptoChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::statefulEncryption() const
{
    return m_statefulEncryption;
}

void L2tpSetting::setStatefulEncryption(bool stateful)
{
    if (m_statefulEncryption == stateful) {
        return;
    }
    m_statefulEncryption = stateful;
    Q_EMIT statefulEncryptionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowBsdCompression() const
{
    return m_allowBsdCompression;
}

void L2tpSetting::setAllowBsdCompression(bool allow)
{
    if (m_allowBsdCompression == allow) {
        return;
    }
    m_allowBsdCompression = allow;
    Q_EMIT allowBsdCompressionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowDeflateCompression() const
{
    return m_allowDeflateCompression;
}

void L2tpSetting::setAllowDeflateCompression(bool allow)
{
    if (m_allowDeflateCompression == allow) {
        return;
    }
    m_allowDeflateCompression = allow;
    Q_EMIT allowDeflateCompressionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::allowTcpHeaderCompression() const
{
    return m_allowTcpHeaderCompression;
}

void L2tpSetting::setAllowTcpHeaderCompression(bool allow)
{
    if (m_allowTcpHeaderCompression == allow) {
        return;
    }
    m_allowTcpHeaderCompression = allow;
    Q_EMIT allowTcpHeaderCompressionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useProtocolFieldCompression() const
{
    return m_useProtocolFieldCompression;
}

void L2tpSetting::setUseProtocolFieldCompression(bool use)
{
    if (m_useProtocolFieldCompression == use) {
        return;
    }
    m_useProtocolFieldCompression = use;
    Q_EMIT useProtocolFieldCompressionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useAddressControlCompression() const
{
    return m_useAddressControlCompression;
}

void L2tpSetting::setUseAddressControlCompression(bool use)
{
    if (m_useAddressControlCompression == use) {
        return;
    }
    m_useAddressControlCompression = use;
    Q_EMIT useAddressControlCompressionChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::sendPppEchoPackets() const
{
    return m_sendPppEchoPackets;
}

void L2tpSetting::setSendPppEchoPackets(bool send)
{
    if (m_sendPppEchoPackets == send) {
        return;
    }
    m_sendPppEchoPackets = send;
    Q_EMIT sendPppEchoPacketsChanged();
    Q_EMIT validChanged();
}

bool L2tpSetting::useMrru() const
{
    return m_useMrru;
}

void L2tpSetting::setUseMrru(bool use)
{
    if (m_useMrru == use) {
        return;
    }
    m_useMrru = use;
    Q_EMIT useMrruChanged();
    Q_EMIT validChanged();
}

int L2tpSetting::mrru() const
{
    return m_mrru;
}

void L2tpSetting::setMrru(int mrru)
{
    if (m_mrru == mrru) {
        return;
    }
    m_mrru = mrru;
    Q_EMIT mrruChanged();
    Q_EMIT validChanged();
}

int L2tpSetting::mru() const
{
    return m_mru;
}

void L2tpSetting::setMru(int mru)
{
    if (m_mru == mru) {
        return;
    }
    m_mru = mru;
    Q_EMIT mruChanged();
    Q_EMIT validChanged();
}

int L2tpSetting::mtu() const
{
    return m_mtu;
}

void L2tpSetting::setMtu(int mtu)
{
    if (m_mtu == mtu) {
        return;
    }
    m_mtu = mtu;
    Q_EMIT mtuChanged();
    Q_EMIT validChanged();
}

#include "moc_l2tp.cpp"
