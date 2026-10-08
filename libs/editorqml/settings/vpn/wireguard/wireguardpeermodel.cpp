/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "wireguardpeermodel.h"

#include "wireguardkeyvalidator.h"

namespace
{
const QLatin1String PublicKey("public-key");
const QLatin1String PresharedKey("preshared-key");
const QLatin1String PresharedKeyFlags("preshared-key-flags");
const QLatin1String AllowedIps("allowed-ips");
const QLatin1String Endpoint("endpoint");
const QLatin1String PersistentKeepalive("persistent-keepalive");

WireguardPeerModel::PresharedKeyOption optionFromFlags(uint rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags);

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return WireguardPeerModel::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::NotRequired)) {
        return WireguardPeerModel::NotRequired;
    }
    return WireguardPeerModel::StoreForUser;
}

QString endpointAddress(const QString &endpoint)
{
    const int separator = endpoint.lastIndexOf(QLatin1Char(':'));
    QString address = separator < 0 ? endpoint : endpoint.left(separator);

    if (address.startsWith(QLatin1Char('[')) && address.endsWith(QLatin1Char(']'))) {
        address = address.mid(1, address.size() - 2);
    }

    return address;
}

QString endpointPort(const QString &endpoint)
{
    const int separator = endpoint.lastIndexOf(QLatin1Char(':'));
    return separator < 0 ? QString() : endpoint.mid(separator + 1);
}

QString joinEndpoint(const QString &address, const QString &port)
{
    if (address.isEmpty() && port.isEmpty()) {
        return QString();
    }

    if (address.contains(QLatin1Char(':'))) {
        return QLatin1Char('[') + address + QLatin1String("]:") + port;
    }

    return address + QLatin1Char(':') + port;
}

uint flagsFromOption(int option)
{
    switch (option) {
    case WireguardPeerModel::StoreForAllUsers:
        return NetworkManager::Setting::None;
    case WireguardPeerModel::NotRequired:
        return NetworkManager::Setting::NotRequired;
    default:
        break;
    }
    return NetworkManager::Setting::AgentOwned;
}
}

WireguardPeerModel::WireguardPeerModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_keyValidator(new WireGuardKeyValidator(this))
{
    connect(this, &WireguardPeerModel::changed, this, &WireguardPeerModel::validChanged);
}

WireguardPeerModel::~WireguardPeerModel() = default;

int WireguardPeerModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_peers.size());
}

QVariant WireguardPeerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_peers.size()) {
        return QVariant();
    }

    const QVariantMap &peer = m_peers.at(index.row());

    switch (role) {
    case PublicKeyRole:
        return peer.value(PublicKey).toString();
    case PresharedKeyRole:
        return peer.value(PresharedKey).toString();
    case PresharedKeyOptionRole:
        return optionFromFlags(peer.value(PresharedKeyFlags).toUInt());
    case AllowedIpsRole:
        return peer.value(AllowedIps).toStringList().join(QLatin1String(", "));
    case EndpointAddressRole:
        return endpointAddress(peer.value(Endpoint).toString());
    case EndpointPortRole:
        return endpointPort(peer.value(Endpoint).toString());
    case PersistentKeepaliveRole:
        return peer.value(PersistentKeepalive).toUInt();
    default:
        return QVariant();
    }
}

bool WireguardPeerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_peers.size()) {
        return false;
    }

    QVariantMap &peer = m_peers[index.row()];

    switch (role) {
    case PublicKeyRole:
        if (peer.value(PublicKey).toString() == value.toString()) {
            return false;
        }
        peer.insert(PublicKey, value.toString());
        break;

    case PresharedKeyRole:
        if (peer.value(PresharedKey).toString() == value.toString()) {
            return false;
        }
        if (value.toString().isEmpty()) {
            peer.remove(PresharedKey);
        } else {
            peer.insert(PresharedKey, value.toString());
        }
        break;

    case PresharedKeyOptionRole: {
        const uint flags = flagsFromOption(value.toInt());
        if (peer.value(PresharedKeyFlags).toUInt() == flags) {
            return false;
        }
        peer.insert(PresharedKeyFlags, flags);

        if (value.toInt() == NotRequired && peer.remove(PresharedKey) > 0) {
            Q_EMIT dataChanged(index, index, {PresharedKeyRole});
        }
        break;
    }

    case AllowedIpsRole: {
        QStringList addresses = value.toString().split(QLatin1Char(','), Qt::SkipEmptyParts);
        for (QString &address : addresses) {
            address = address.trimmed();
        }
        if (peer.value(AllowedIps).toStringList() == addresses) {
            return false;
        }
        peer.insert(AllowedIps, addresses);
        break;
    }

    case EndpointAddressRole:
    case EndpointPortRole: {
        const QString stored = peer.value(Endpoint).toString();
        const QString edited = value.toString().trimmed();

        const bool isPort = role == EndpointPortRole;
        if ((isPort ? endpointPort(stored) : endpointAddress(stored)) == edited) {
            return false;
        }

        const QString endpoint = isPort ? joinEndpoint(endpointAddress(stored), edited) : joinEndpoint(edited, endpointPort(stored));
        if (endpoint.isEmpty()) {
            peer.remove(Endpoint);
        } else {
            peer.insert(Endpoint, endpoint);
        }
        break;
    }

    case PersistentKeepaliveRole: {
        const uint interval = value.toUInt();
        if (peer.value(PersistentKeepalive).toUInt() == interval) {
            return false;
        }
        // Zero means no keepalive, which is the absence of the key.
        if (interval == 0) {
            peer.remove(PersistentKeepalive);
        } else {
            peer.insert(PersistentKeepalive, interval);
        }
        break;
    }

    default:
        return false;
    }

    Q_EMIT dataChanged(index, index, {role});
    Q_EMIT changed();

    return true;
}

QHash<int, QByteArray> WireguardPeerModel::roleNames() const
{
    return {
        {PublicKeyRole, QByteArrayLiteral("publicKey")},
        {PresharedKeyRole, QByteArrayLiteral("presharedKey")},
        {PresharedKeyOptionRole, QByteArrayLiteral("presharedKeyOption")},
        {AllowedIpsRole, QByteArrayLiteral("allowedIps")},
        {EndpointAddressRole, QByteArrayLiteral("endpointAddress")},
        {EndpointPortRole, QByteArrayLiteral("endpointPort")},
        {PersistentKeepaliveRole, QByteArrayLiteral("persistentKeepalive")},
    };
}

NMVariantMapList WireguardPeerModel::peers() const
{
    return m_peers;
}

void WireguardPeerModel::setPeers(const NMVariantMapList &peers)
{
    beginResetModel();
    m_peers = peers;
    endResetModel();

    Q_EMIT countChanged();
    Q_EMIT changed();
}

void WireguardPeerModel::loadSecrets(const NMVariantMapList &peers)
{
    for (const QVariantMap &source : peers) {
        const QString key = source.value(PresharedKey).toString();
        const QString publicKey = source.value(PublicKey).toString();

        if (key.isEmpty() || publicKey.isEmpty()) {
            continue;
        }

        for (int row = 0; row < m_peers.size(); ++row) {
            if (m_peers.at(row).value(PublicKey).toString() == publicKey) {
                m_peers[row].insert(PresharedKey, key);
                const QModelIndex changedIndex = index(row, 0);
                Q_EMIT dataChanged(changedIndex, changedIndex, {PresharedKeyRole});
                break;
            }
        }
    }

    Q_EMIT validChanged();
}

bool WireguardPeerModel::isKeyValid(const QString &key) const
{
    QString candidate = key;
    int position = 0;

    return m_keyValidator->validate(candidate, position) == QValidator::Acceptable;
}

bool WireguardPeerModel::isValid() const
{
    for (const QVariantMap &peer : m_peers) {
        if (!isKeyValid(peer.value(PublicKey).toString())) {
            return false;
        }

        const QString presharedKey = peer.value(PresharedKey).toString();
        if (!presharedKey.isEmpty() && !isKeyValid(presharedKey)) {
            return false;
        }

        if (peer.contains(PresharedKeyFlags)) {
            const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(peer.value(PresharedKeyFlags).toUInt());
            if (!flags.testFlag(NetworkManager::Setting::NotRequired) && presharedKey.isEmpty()) {
                return false;
            }
        }
    }

    return true;
}

void WireguardPeerModel::addPeer()
{
    const int row = static_cast<int>(m_peers.size());

    beginInsertRows(QModelIndex(), row, row);
    m_peers.append(QVariantMap());
    endInsertRows();

    Q_EMIT countChanged();
    Q_EMIT changed();
}

void WireguardPeerModel::beginEdit()
{
    m_snapshot = m_peers;
}

void WireguardPeerModel::revertEdit()
{
    if (m_snapshot == m_peers) {
        return;
    }

    setPeers(m_snapshot);
}

void WireguardPeerModel::removePeer(int row)
{
    if (row < 0 || row >= m_peers.size()) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    m_peers.removeAt(row);
    endRemoveRows();

    Q_EMIT countChanged();
    Q_EMIT changed();
}

#include "moc_wireguardpeermodel.cpp"
