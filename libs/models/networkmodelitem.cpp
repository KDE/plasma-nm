/*
    SPDX-FileCopyrightText: 2013-2018 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "networkmodelitem.h"
#include "connectiondetails.h"
#include "networkmodel.h"
#include "uiutils.h"

#include <KLocalizedString>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>

NetworkModelItem::NetworkModelItem(QObject *parent)
    : QObject(parent)
    , m_activeConnectionPath()
    , m_connectionPath()
    , m_connectionState(NetworkManager::ActiveConnection::Deactivated)
    , m_devicePath()
    , m_deviceName()
    , m_deviceState(NetworkManager::Device::UnknownState)
    , m_delayModelUpdates(false)
    , m_duplicate(false)
    , m_mode(NetworkManager::WirelessSetting::Infrastructure)
    , m_name()
    , m_connectionDetailsModel(new ConnectionDetailsModel(this))
    , m_securityType(NetworkManager::NoneSecurity)
    , m_signal(0)
    , m_slave(false)
    , m_specificPath()
    , m_ssid()
    , m_timestamp()
    , m_type(NetworkManager::ConnectionSettings::Unknown)
    , m_accessibleDescription()
    , m_uuid()
    , m_vpnType()
    , m_vpnState(NetworkManager::VpnConnection::Unknown)
    , m_rxBytes(0)
    , m_txBytes(0)
    , m_icon(computeIcon())
    , m_changedRoles()
{
}

NetworkModelItem::NetworkModelItem(const NetworkModelItem *item, QObject *parent)
    : QObject(parent)
    , m_activeConnectionPath()
    , m_connectionPath(item->connectionPath())
    , m_connectionState(NetworkManager::ActiveConnection::Deactivated)
    , m_devicePath()
    , m_deviceName()
    , m_deviceState(NetworkManager::Device::UnknownState)
    , m_delayModelUpdates(item->delayModelUpdates())
    , m_duplicate(true)
    , m_mode(item->mode())
    , m_name(item->name())
    , m_connectionDetailsModel(new ConnectionDetailsModel(this))
    , m_securityType(item->securityType())
    , m_signal(0)
    , m_slave(item->slave())
    , m_specificPath()
    , m_ssid(item->ssid())
    , m_timestamp(item->timestamp())
    , m_type(item->type())
    , m_accessibleDescription()
    , m_uuid(item->uuid())
    , m_vpnType()
    , m_vpnState(NetworkManager::VpnConnection::Unknown)
    , m_rxBytes(0)
    , m_txBytes(0)
    , m_icon(item->icon())
    , m_changedRoles()
{
}

ConnectionDetailsModel *NetworkModelItem::detailsModel() const
{
    return m_connectionDetailsModel;
}

NetworkModelItem::~NetworkModelItem() = default;

QString NetworkModelItem::activeConnectionPath() const
{
    return m_activeConnectionPath;
}

void NetworkModelItem::setActiveConnectionPath(const QString &path)
{
    m_activeConnectionPath = path;
}

QString NetworkModelItem::connectionPath() const
{
    return m_connectionPath;
}

void NetworkModelItem::setConnectionPath(const QString &path)
{
    if (m_connectionPath != path) {
        m_connectionPath = path;
        m_changedRoles << NetworkModel::ConnectionPathRole << NetworkModel::UniRole;
    }
}

NetworkManager::ActiveConnection::State NetworkModelItem::connectionState() const
{
    return m_connectionState;
}

void NetworkModelItem::setConnectionState(NetworkManager::ActiveConnection::State state)
{
    if (m_connectionState != state) {
        m_connectionState = state;
        m_changedRoles << NetworkModel::ConnectionStateRole << NetworkModel::SectionRole;
        refreshIcon();
    }
}


QList<ConnectionDetails::ConnectionDetailSection> NetworkModelItem::detailsList() const
{
    if (itemType() == NetworkModelItem::UnavailableConnection) {
        return {};
    }

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(m_uuid);

    // For Wi-Fi networks, pass the specific access point path for disconnected networks
    QString accessPointPath;
    if (m_type == NetworkManager::ConnectionSettings::Wireless && !m_specificPath.isEmpty()) {
        accessPointPath = m_specificPath;
    }

    return ConnectionDetails::getConnectionDetails(connection, device, accessPointPath);
}

QString NetworkModelItem::devicePath() const
{
    return m_devicePath;
}

QString NetworkModelItem::deviceName() const
{
    return m_deviceName;
}

void NetworkModelItem::setDeviceName(const QString &name)
{
    if (m_deviceName != name) {
        m_deviceName = name;
        m_changedRoles << NetworkModel::DeviceName;
    }
}

void NetworkModelItem::setDevicePath(const QString &path)
{
    if (m_devicePath != path) {
        m_devicePath = path;
        m_changedRoles << NetworkModel::DevicePathRole << NetworkModel::ItemTypeRole << NetworkModel::UniRole;
    }
}

QString NetworkModelItem::deviceState() const
{
    return UiUtils::connectionStateToString(m_deviceState);
}

void NetworkModelItem::setDeviceState(const NetworkManager::Device::State state)
{
    if (m_deviceState != state) {
        m_deviceState = state;
        m_changedRoles << NetworkModel::DeviceStateRole;
    }
}

bool NetworkModelItem::duplicate() const
{
    return m_duplicate;
}

void NetworkModelItem::setIcon(const QString &icon)
{
    if (icon != m_icon) {
        m_icon = icon;
        m_changedRoles << NetworkModel::ConnectionIconRole;
    }
}

void NetworkModelItem::refreshIcon()
{
    setIcon(computeIcon());
}

QString NetworkModelItem::computeIcon() const
{
    switch (m_type) {
    case NetworkManager::ConnectionSettings::Adsl:
        return QStringLiteral("network-mobile-100");
        break;
    case NetworkManager::ConnectionSettings::Bluetooth:
        return QStringLiteral("network-wireless-bluetooth-symbolic");
        break;
    case NetworkManager::ConnectionSettings::Bond:
        break;
    case NetworkManager::ConnectionSettings::Bridge:
        break;
    case NetworkManager::ConnectionSettings::Cdma:
    case NetworkManager::ConnectionSettings::Gsm:
        if (m_signal == 0) {
            return QStringLiteral("network-mobile-0");
        } else if (m_signal < 20) {
            return QStringLiteral("network-mobile-20");
        } else if (m_signal < 40) {
            return QStringLiteral("network-mobile-40");
        } else if (m_signal < 60) {
            return QStringLiteral("network-mobile-60");
        } else if (m_signal < 80) {
            return QStringLiteral("network-mobile-80");
        } else {
            return QStringLiteral("network-mobile-100");
        }
        break;
    case NetworkManager::ConnectionSettings::Infiniband:
        break;
    case NetworkManager::ConnectionSettings::OLPCMesh:
        break;
    case NetworkManager::ConnectionSettings::Pppoe:
        return QStringLiteral("network-mobile-100");
        break;
    case NetworkManager::ConnectionSettings::Vlan:
        break;
    case NetworkManager::ConnectionSettings::Vpn:
    case NetworkManager::ConnectionSettings::WireGuard:
        return QStringLiteral("network-vpn");
        break;
    case NetworkManager::ConnectionSettings::Wired:
        if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
            return QStringLiteral("network-wired-activated");
        } else {
            return QStringLiteral("network-wired");
        }
        break;
    case NetworkManager::ConnectionSettings::Wireless: {
        bool isOpen = m_securityType <= NetworkManager::NoneSecurity || m_securityType == NetworkManager::OWE;
        if (m_signal == 0) {
            if (m_mode == NetworkManager::WirelessSetting::Adhoc || m_mode == NetworkManager::WirelessSetting::Ap) {
                return isOpen ? QStringLiteral("network-wireless-100") : QStringLiteral("network-wireless-100-locked");
            }
            return isOpen ? QStringLiteral("network-wireless-0") : QStringLiteral("network-wireless-0-locked");
        } else if (m_signal < 20) {
            return isOpen ? QStringLiteral("network-wireless-20") : QStringLiteral("network-wireless-20-locked");
        } else if (m_signal < 40) {
            return isOpen ? QStringLiteral("network-wireless-40") : QStringLiteral("network-wireless-40-locked");
        } else if (m_signal < 60) {
            return isOpen ? QStringLiteral("network-wireless-60") : QStringLiteral("network-wireless-60-locked");
        } else if (m_signal < 80) {
            return isOpen ? QStringLiteral("network-wireless-80") : QStringLiteral("network-wireless-80-locked");
        } else {
            return isOpen ? QStringLiteral("network-wireless-100") : QStringLiteral("network-wireless-100-locked");
        }
        break;
    }
    default:
        break;
    }

    if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
        return QStringLiteral("network-wired-activated");
    } else {
        return QStringLiteral("network-wired");
    }
}

NetworkModelItem::ItemType NetworkModelItem::itemType() const
{
    if (!m_devicePath.isEmpty() //
        || m_type == NetworkManager::ConnectionSettings::Bond //
        || m_type == NetworkManager::ConnectionSettings::Bridge //
        || m_type == NetworkManager::ConnectionSettings::Vlan //
        || m_type == NetworkManager::ConnectionSettings::Team //
        || ((NetworkManager::status() == NetworkManager::Connected //
             || NetworkManager::status() == NetworkManager::ConnectedLinkLocal //
             || NetworkManager::status() == NetworkManager::ConnectedSiteOnly)
            && (m_type == NetworkManager::ConnectionSettings::Vpn || m_type == NetworkManager::ConnectionSettings::WireGuard))) {
        if (m_connectionPath.isEmpty() && m_type == NetworkManager::ConnectionSettings::Wireless) {
            return NetworkModelItem::AvailableAccessPoint;
        } else {
            return NetworkModelItem::AvailableConnection;
        }
    }
    return NetworkModelItem::UnavailableConnection;
}

NetworkManager::WirelessSetting::NetworkMode NetworkModelItem::mode() const
{
    return m_mode;
}

void NetworkModelItem::setMode(const NetworkManager::WirelessSetting::NetworkMode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        refreshIcon();
    }
}

QString NetworkModelItem::name() const
{
    return m_name;
}

void NetworkModelItem::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        m_changedRoles << NetworkModel::ItemUniqueNameRole << NetworkModel::NameRole;
    }
}

QString NetworkModelItem::originalName() const
{
    if (m_deviceName.isEmpty()) {
        return m_name;
    }
    return m_name % QLatin1String(" (") % m_deviceName % ')';
}

QString NetworkModelItem::sectionType() const
{
    if (m_connectionState == NetworkManager::ActiveConnection::Deactivated && itemType() != NetworkModelItem::UnavailableConnection) {
        return i18nc("@title:column header for list of available network connections", "Available");
        // clang-format off
    } else if (m_connectionState == NetworkManager::ActiveConnection::Activating
            || m_connectionState == NetworkManager::ActiveConnection::Activated
            || m_connectionState == NetworkManager::ActiveConnection::Deactivating) {
        // clang-format on
        return i18nc("@title:column header for list of connected network connections", "Connected");
    } else if (itemType() == NetworkModelItem::UnavailableConnection) {
        return i18nc("@title:column header for list of previously connected network connections", "Previously Used, Unavailable");
    } else {
        return {};
    }
}

NetworkManager::WirelessSecurityType NetworkModelItem::securityType() const
{
    return m_securityType;
}

void NetworkModelItem::setSecurityType(NetworkManager::WirelessSecurityType type)
{
    if (m_securityType != type) {
        m_securityType = type;
        m_changedRoles << NetworkModel::SecurityTypeStringRole << NetworkModel::SecurityTypeRole;
        refreshIcon();
    }
}

int NetworkModelItem::signal() const
{
    return m_signal;
}

void NetworkModelItem::setSignal(int signal)
{
    if (m_signal != signal) {
        m_signal = signal;
        m_changedRoles << NetworkModel::SignalRole;
        refreshIcon();
    }
}

bool NetworkModelItem::slave() const
{
    return m_slave;
}

void NetworkModelItem::setSlave(bool slave)
{
    if (m_slave != slave) {
        m_slave = slave;
        m_changedRoles << NetworkModel::SlaveRole;
    }
}

QString NetworkModelItem::specificPath() const
{
    return m_specificPath;
}

void NetworkModelItem::setSpecificPath(const QString &path)
{
    if (m_specificPath != path) {
        m_specificPath = path;
        m_changedRoles << NetworkModel::SpecificPathRole;
    }
}

QString NetworkModelItem::ssid() const
{
    return m_ssid;
}

void NetworkModelItem::setSsid(const QString &ssid)
{
    if (m_ssid != ssid) {
        m_ssid = ssid;
        m_changedRoles << NetworkModel::SsidRole << NetworkModel::UniRole;
    }
}

NetworkManager::ConnectionSettings::ConnectionType NetworkModelItem::type() const
{
    return m_type;
}

QDateTime NetworkModelItem::timestamp() const
{
    return m_timestamp;
}

void NetworkModelItem::setTimestamp(const QDateTime &date)
{
    if (m_timestamp != date) {
        m_timestamp = date;
        m_changedRoles << NetworkModel::TimeStampRole << NetworkModel::LastUsedRole;
    }
}

void NetworkModelItem::setType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    if (m_type == type) {
        return;
    }

    m_type = type;

    m_accessibleDescription = NetworkManager::ConnectionSettings::typeAsString(type);

    m_changedRoles << NetworkModel::TypeRole << NetworkModel::ItemTypeRole << NetworkModel::UniRole << Qt::AccessibleDescriptionRole;

    refreshIcon();
}

QString NetworkModelItem::accessibleDescription() const
{
    return m_accessibleDescription;
}

QString NetworkModelItem::uni() const
{
    if (m_type == NetworkManager::ConnectionSettings::Wireless && m_uuid.isEmpty()) {
        return m_ssid + '%' + m_devicePath;
    } else {
        return m_connectionPath + '%' + m_devicePath;
    }
}

QString NetworkModelItem::uuid() const
{
    return m_uuid;
}

void NetworkModelItem::setUuid(const QString &uuid)
{
    if (m_uuid != uuid) {
        m_uuid = uuid;
        m_changedRoles << NetworkModel::UuidRole;
    }
}

QString NetworkModelItem::vpnState() const
{
    return UiUtils::vpnConnectionStateToString(m_vpnState);
}

void NetworkModelItem::setVpnState(NetworkManager::VpnConnection::State state)
{
    if (m_vpnState != state) {
        m_vpnState = state;
        m_changedRoles << NetworkModel::VpnState;
    }
}

QString NetworkModelItem::vpnType() const
{
    return m_vpnType;
}

void NetworkModelItem::setVpnType(const QString &type)
{
    if (m_vpnType != type) {
        m_vpnType = type;
        m_changedRoles << NetworkModel::VpnType;
    }
}

qulonglong NetworkModelItem::rxBytes() const
{
    return m_rxBytes;
}

void NetworkModelItem::setRxBytes(qulonglong bytes)
{
    if (m_rxBytes != bytes) {
        m_rxBytes = bytes;
        m_changedRoles << NetworkModel::RxBytesRole;
    }
}

qulonglong NetworkModelItem::txBytes() const
{
    return m_txBytes;
}

void NetworkModelItem::setTxBytes(qulonglong bytes)
{
    if (m_txBytes != bytes) {
        m_txBytes = bytes;
        m_changedRoles << NetworkModel::TxBytesRole;
    }
}

bool NetworkModelItem::delayModelUpdates() const
{
    return m_delayModelUpdates;
}

void NetworkModelItem::setDelayModelUpdates(bool delay)
{
    // special case, does not need m_changedRoles
    m_delayModelUpdates = delay;
}

bool NetworkModelItem::operator==(const NetworkModelItem *item) const
{
    if (!item->uuid().isEmpty() && !uuid().isEmpty()) {
        if (item->devicePath() == devicePath() && item->uuid() == uuid()) {
            return true;
        }
    } else if (item->type() == NetworkManager::ConnectionSettings::Wireless && type() == NetworkManager::ConnectionSettings::Wireless) {
        if (item->ssid() == ssid() && item->devicePath() == devicePath()) {
            return true;
        }
    }
    return false;
}

void NetworkModelItem::updateConnectionDetailsModel()
{
    m_connectionDetailsModel->setDetailsList(detailsList());
}

#include "moc_networkmodelitem.cpp"
