/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "wireguard.h"

#include "wireguardkeyvalidator.h"

#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>

#include <QFile>
#include <QFileInfo>
#include <QHostAddress>
#include <QTextStream>

namespace
{
const QLatin1String SectionInterface("[Interface]");
const QLatin1String SectionPeer("[Peer]");

const QLatin1String TagAddress("Address");
const QLatin1String TagListenPort("ListenPort");
const QLatin1String TagPrivateKey("PrivateKey");
const QLatin1String TagDns("DNS");
const QLatin1String TagMtu("MTU");
const QLatin1String TagFwmark("FwMark");
const QLatin1String TagTable("Table");
const QLatin1String TagPreUp("PreUp");
const QLatin1String TagPostUp("PostUp");
const QLatin1String TagPreDown("PreDown");
const QLatin1String TagPostDown("PostDown");

const QLatin1String TagPublicKey("PublicKey");
const QLatin1String TagAllowedIps("AllowedIPs");
const QLatin1String TagEndpoint("Endpoint");
const QLatin1String TagPresharedKey("PresharedKey");

const QLatin1String PeerKeyPublicKey("public-key");
const QLatin1String PeerKeyAllowedIps("allowed-ips");
const QLatin1String PeerKeyEndpoint("endpoint");
const QLatin1String PeerKeyPresharedKey("preshared-key");

const QLatin1String DefaultInterfaceName("wg0");

QString sanitizeInterfaceName(const QString &interfaceName)
{
    QString name = interfaceName;

    name.removeIf([](QChar c) {
        return c == QLatin1Char('\r') || c == QLatin1Char('\n') || c == QLatin1Char('\t') || c == QLatin1Char('\f') || c == QLatin1Char(' ')
            || c == QLatin1Char(':') || c == QLatin1Char('/');
    });

    if (name.length() >= 16) {
        name.truncate(15);
    }

    if (name.isEmpty() || name == QLatin1String(".") || name == QLatin1String("..")) {
        return DefaultInterfaceName;
    }

    return name;
}

QString valueOf(const QStringList &fields)
{
    return QStringList(fields.mid(1)).join(QLatin1Char('=')).trimmed();
}

bool isAllowedIpsList(const QString &value, QStringList &addresses)
{
    const QStringList entries = value.split(QLatin1Char(','), Qt::SkipEmptyParts);

    if (entries.isEmpty()) {
        return false;
    }

    addresses.clear();
    for (const QString &entry : entries) {
        const QString trimmed = entry.trimmed();
        const QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(trimmed);

        if (subnet.first.isNull()) {
            return false;
        }

        addresses.append(trimmed);
    }

    return true;
}
}

QString WireguardSetting::supportedFileExtensions()
{
    return QStringLiteral("*.conf");
}

NMVariantMapMap WireguardSetting::importConnectionSettings(const QString &fileName)
{
    NMVariantMapMap result;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return result;
    }

    const QString connectionName = QFileInfo(fileName).completeBaseName();

    WireGuardKeyValidator keyValidator;
    NetworkManager::WireGuardSetting wireguardSetting;
    NetworkManager::Ipv4Setting ipv4Setting;
    NetworkManager::Ipv6Setting ipv6Setting;

    QList<NetworkManager::IpAddress> ipv4Addresses;
    QList<NetworkManager::IpAddress> ipv6Addresses;
    NMVariantMapList peers;
    QVariantMap *currentPeer = nullptr;

    bool havePrivateKey = false;
    bool haveIpv4Setting = false;
    bool haveIpv6Setting = false;
    bool havePublicKey = true;
    bool haveAllowedIps = true;

    ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Disabled);
    ipv6Setting.setMethod(NetworkManager::Ipv6Setting::Ignored);

    enum {
        Idle,
        Interface,
        Peer,
    } section = Idle;

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();

        const int comment = line.indexOf(QLatin1Char('#'));
        if (comment >= 0) {
            line.truncate(comment);
        }

        line = line.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line == SectionInterface) {
            section = Interface;
            continue;
        }

        if (line == SectionPeer) {
            if (!havePublicKey || !haveAllowedIps) {
                return NMVariantMapMap();
            }

            havePublicKey = false;
            haveAllowedIps = false;
            section = Peer;

            peers.append(QVariantMap());
            currentPeer = &peers.last();
            continue;
        }

        const QStringList fields = line.split(QLatin1Char('='));
        if (fields.size() < 2) {
            continue;
        }

        const QString key = fields.first().trimmed();
        const QString value = valueOf(fields);
        int position = 0;

        if (section == Interface) {
            if (key == TagAddress) {
                const QStringList entries = value.split(QLatin1Char(','), Qt::SkipEmptyParts);
                if (entries.isEmpty()) {
                    return NMVariantMapMap();
                }

                for (const QString &entry : entries) {
                    const QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(entry.trimmed());

                    NetworkManager::IpAddress address;
                    address.setIp(subnet.first);
                    address.setPrefixLength(subnet.second);

                    if (subnet.first.protocol() == QAbstractSocket::IPv4Protocol) {
                        ipv4Addresses.append(address);
                    } else if (subnet.first.protocol() == QAbstractSocket::IPv6Protocol) {
                        ipv6Addresses.append(address);
                    } else {
                        return NMVariantMapMap();
                    }
                }

                if (!ipv4Addresses.isEmpty()) {
                    ipv4Setting.setAddresses(ipv4Addresses);
                    ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Manual);
                    haveIpv4Setting = true;
                }
                if (!ipv6Addresses.isEmpty()) {
                    ipv6Setting.setAddresses(ipv6Addresses);
                    ipv6Setting.setMethod(NetworkManager::Ipv6Setting::Manual);
                    haveIpv6Setting = true;
                }
            } else if (key == TagListenPort) {
                const uint port = value.toUInt();
                if (port <= 65535) {
                    wireguardSetting.setListenPort(port);
                }
            } else if (key == TagPrivateKey) {
                QString privateKey = value;
                if (keyValidator.validate(privateKey, position) == QValidator::Acceptable) {
                    wireguardSetting.setPrivateKey(privateKey);
                    havePrivateKey = true;
                }
            } else if (key == TagDns) {
                QList<QHostAddress> ipv4Dns;
                QList<QHostAddress> ipv6Dns;

                const QStringList entries = value.split(QLatin1Char(','), Qt::SkipEmptyParts);
                for (const QString &entry : entries) {
                    const QPair<QHostAddress, int> subnet = QHostAddress::parseSubnet(entry.trimmed());

                    if (subnet.first.protocol() == QAbstractSocket::IPv4Protocol) {
                        ipv4Dns.append(subnet.first);
                    } else if (subnet.first.protocol() == QAbstractSocket::IPv6Protocol) {
                        ipv6Dns.append(subnet.first);
                    } else {
                        return NMVariantMapMap();
                    }
                }

                if (!ipv4Dns.isEmpty()) {
                    if (ipv4Setting.method() == NetworkManager::Ipv4Setting::Disabled) {
                        ipv4Setting.setMethod(NetworkManager::Ipv4Setting::Automatic);
                    }
                    ipv4Setting.setIgnoreAutoDns(true);
                    ipv4Setting.setDns(ipv4Dns);
                    haveIpv4Setting = true;
                }
                if (!ipv6Dns.isEmpty()) {
                    ipv6Setting.setMethod(NetworkManager::Ipv6Setting::Automatic);
                    ipv6Setting.setIgnoreAutoDns(true);
                    ipv6Setting.setDns(ipv6Dns);
                    haveIpv6Setting = true;
                }
            } else if (key == TagMtu) {
                const uint mtu = value.toUInt();
                if (mtu > 0) {
                    wireguardSetting.setMtu(mtu);
                }
            } else if (key == TagFwmark) {
                wireguardSetting.setFwmark(value.toLower() == QLatin1String("off") ? 0 : value.toUInt());
            } else if (key == TagTable || key == TagPreUp || key == TagPostUp || key == TagPreDown || key == TagPostDown) {
                // plasma-nm does not handle these items
            } else {
                return NMVariantMapMap();
            }
        } else if (section == Peer) {
            if (key == TagPublicKey) {
                QString publicKey = value;
                if (keyValidator.validate(publicKey, position) == QValidator::Acceptable) {
                    currentPeer->insert(PeerKeyPublicKey, publicKey);
                    havePublicKey = true;
                }
            } else if (key == TagAllowedIps) {
                QStringList addresses;
                if (isAllowedIpsList(value, addresses)) {
                    currentPeer->insert(PeerKeyAllowedIps, addresses);
                    haveAllowedIps = true;
                }
            } else if (key == TagEndpoint) {
                if (!value.isEmpty()) {
                    currentPeer->insert(PeerKeyEndpoint, value);
                }
            } else if (key == TagPresharedKey) {
                QString presharedKey = value;
                if (keyValidator.validate(presharedKey, position) == QValidator::Acceptable) {
                    currentPeer->insert(PeerKeyPresharedKey, presharedKey);
                }
            }
        } else {
            return NMVariantMapMap();
        }
    }

    if (!havePrivateKey || !havePublicKey || !haveAllowedIps) {
        return NMVariantMapMap();
    }

    wireguardSetting.setPeers(peers);

    QVariantMap connection;
    connection.insert(QLatin1String("id"), connectionName);
    connection.insert(QLatin1String("interface-name"), sanitizeInterfaceName(connectionName));
    connection.insert(QLatin1String("type"), QLatin1String("wireguard"));
    connection.insert(QLatin1String("autoconnect"), false);

    result.insert(QLatin1String("connection"), connection);
    result.insert(QLatin1String("wireguard"), wireguardSetting.toMap());

    if (haveIpv4Setting) {
        result.insert(QLatin1String("ipv4"), ipv4Setting.toMap());
    }
    if (haveIpv6Setting) {
        result.insert(QLatin1String("ipv6"), ipv6Setting.toMap());
    }

    return result;
}

WireguardSetting::WireguardSetting(QObject *parent)
    : QObject(parent)
    , m_peers(new WireguardPeerModel(this))
    , m_keyValidator(new WireGuardKeyValidator(this))
{
    connect(m_peers, &WireguardPeerModel::changed, this, &WireguardSetting::validChanged);
}

WireguardSetting::~WireguardSetting() = default;

WireguardPeerModel *WireguardSetting::peers() const
{
    return m_peers;
}

QValidator *WireguardSetting::keyValidator() const
{
    return m_keyValidator;
}

void WireguardSetting::loadConfig(const NetworkManager::ConnectionSettings::Ptr &connectionSettings)
{
    if (!connectionSettings) {
        return;
    }

    setInterfaceName(connectionSettings->interfaceName());

    const NetworkManager::WireGuardSetting::Ptr setting =
        connectionSettings->setting(NetworkManager::Setting::WireGuard).staticCast<NetworkManager::WireGuardSetting>();

    if (!setting) {
        return;
    }

    setPrivateKey(setting->privateKey());
    setListenPort(static_cast<int>(setting->listenPort()));
    setFwmark(static_cast<int>(setting->fwmark()));
    setMtu(static_cast<int>(setting->mtu()));
    setPeerRoutes(setting->peerRoutes());

    switch (setting->privateKeyFlags()) {
    case NetworkManager::Setting::None:
        setPrivateKeyOption(StoreForAllUsers);
        break;
    case NetworkManager::Setting::NotRequired:
        setPrivateKeyOption(NotRequired);
        break;
    default:
        setPrivateKeyOption(StoreForUser);
        break;
    }

    m_peers->setPeers(setting->peers());

    loadSecrets(setting);
}

void WireguardSetting::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    const NetworkManager::WireGuardSetting::Ptr wireguardSetting = setting.staticCast<NetworkManager::WireGuardSetting>();

    if (!wireguardSetting) {
        return;
    }

    const QString privateKey = wireguardSetting->privateKey();
    if (!privateKey.isEmpty()) {
        setPrivateKey(privateKey);
    }

    m_peers->loadSecrets(wireguardSetting->peers());
}

QVariantMap WireguardSetting::setting() const
{
    NetworkManager::WireGuardSetting setting;

    if (!m_privateKey.isEmpty()) {
        setting.setPrivateKey(m_privateKey);
    }

    if (m_listenPort != 0) {
        setting.setListenPort(static_cast<quint32>(m_listenPort));
    }

    if (m_fwmark != 0) {
        setting.setFwmark(static_cast<quint32>(m_fwmark));
    }

    if (m_mtu != 0) {
        setting.setMtu(static_cast<quint32>(m_mtu));
    }

    setting.setPeerRoutes(m_peerRoutes);

    switch (m_privateKeyOption) {
    case StoreForAllUsers:
        setting.setPrivateKeyFlags(NetworkManager::Setting::None);
        break;
    case NotRequired:
        setting.setPrivateKeyFlags(NetworkManager::Setting::NotRequired);
        break;
    default:
        setting.setPrivateKeyFlags(NetworkManager::Setting::AgentOwned);
        break;
    }

    setting.setPeers(m_peers->peers());

    return setting.toMap();
}

bool WireguardSetting::isValid() const
{
    QString privateKey = m_privateKey;
    int position = 0;
    const bool privateKeyValid = m_keyValidator->validate(privateKey, position) == QValidator::Acceptable;

    return !m_interfaceName.isEmpty() && privateKeyValid && m_peers->isValid();
}

QString WireguardSetting::interfaceName() const
{
    return m_interfaceName;
}

void WireguardSetting::setInterfaceName(const QString &interfaceName)
{
    if (m_interfaceName == interfaceName) {
        return;
    }
    m_interfaceName = interfaceName;
    Q_EMIT interfaceNameChanged();
    Q_EMIT validChanged();
}

QString WireguardSetting::privateKey() const
{
    return m_privateKey;
}

void WireguardSetting::setPrivateKey(const QString &privateKey)
{
    if (m_privateKey == privateKey) {
        return;
    }
    m_privateKey = privateKey;
    Q_EMIT privateKeyChanged();
    Q_EMIT validChanged();
}

WireguardSetting::PasswordOption WireguardSetting::privateKeyOption() const
{
    return m_privateKeyOption;
}

void WireguardSetting::setPrivateKeyOption(PasswordOption option)
{
    if (m_privateKeyOption == option) {
        return;
    }
    m_privateKeyOption = option;
    Q_EMIT privateKeyOptionChanged();
    Q_EMIT validChanged();
}

int WireguardSetting::listenPort() const
{
    return m_listenPort;
}

void WireguardSetting::setListenPort(int port)
{
    if (m_listenPort == port) {
        return;
    }
    m_listenPort = port;
    Q_EMIT listenPortChanged();
    Q_EMIT validChanged();
}

int WireguardSetting::fwmark() const
{
    return m_fwmark;
}

void WireguardSetting::setFwmark(int fwmark)
{
    if (m_fwmark == fwmark) {
        return;
    }
    m_fwmark = fwmark;
    Q_EMIT fwmarkChanged();
    Q_EMIT validChanged();
}

int WireguardSetting::mtu() const
{
    return m_mtu;
}

void WireguardSetting::setMtu(int mtu)
{
    if (m_mtu == mtu) {
        return;
    }
    m_mtu = mtu;
    Q_EMIT mtuChanged();
    Q_EMIT validChanged();
}

bool WireguardSetting::peerRoutes() const
{
    return m_peerRoutes;
}

void WireguardSetting::setPeerRoutes(bool peerRoutes)
{
    if (m_peerRoutes == peerRoutes) {
        return;
    }
    m_peerRoutes = peerRoutes;
    Q_EMIT peerRoutesChanged();
    Q_EMIT validChanged();
}

#include "moc_wireguard.cpp"
