/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "appletproxymodel.h"
#include "kcmproxymodel.h"
#include "networkmodelitem.h"
#include "networkmodel.h"

KCMProxyModel::KCMProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setSortLocaleAware(true);
    sort(0, Qt::DescendingOrder);
}

KCMProxyModel::~KCMProxyModel()
{
}

void KCMProxyModel::setFilteredDeviceName(const QString& deviceName)
{
    m_deviceName = deviceName;
    invalidateFilter();
}

void KCMProxyModel::setFilteredDeviceType(NetworkManager::Device::Type type)
{
    m_deviceType = type;
    invalidateFilter();
}

bool KCMProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

//     const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();
//     const bool isDuplicate = sourceModel()->data(index, NetworkModel::DuplicateRole).toBool();
    const QString deviceName = sourceModel()->data(index, NetworkModel::DeviceNameRole).toString();
    const NetworkManager::ConnectionSettings::ConnectionType connectionType = (NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(index, NetworkModel::TypeRole).toUInt();

    if (!deviceName.isEmpty()) {
        return m_deviceName == deviceName;
    } else {
        return (m_deviceType == NetworkManager::Device::Wifi && connectionType == NetworkManager::ConnectionSettings::Wireless) ||
               (m_deviceType == NetworkManager::Device::Ethernet && connectionType == NetworkManager::ConnectionSettings::Wired) ||
               (m_deviceType == NetworkManager::Device::Bluetooth && connectionType == NetworkManager::ConnectionSettings::Bluetooth) ||
               (m_deviceType == NetworkManager::Device::Modem && (connectionType == NetworkManager::ConnectionSettings::Gsm ||
                                                                 (connectionType == NetworkManager::ConnectionSettings::Cdma)));
    }

//     if (isSlave || isDuplicate) {
//         return false;
//     }
//
// #if NM_CHECK_VERSION(0, 9, 10)
//     const NetworkManager::ConnectionSettings::ConnectionType type = (NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(index, NetworkModel::TypeRole).toUInt();
//     if (type == NetworkManager::ConnectionSettings::Generic) {
//         return false;
//     }
//
//     // TODO advanced
//     if (type == NetworkManager::ConnectionSettings::Bond ||
//         type == NetworkManager::ConnectionSettings::Bridge ||
//         type == NetworkManager::ConnectionSettings::Infiniband ||
//         type == NetworkManager::ConnectionSettings::Team ||
//         type == NetworkManager::ConnectionSettings::Vlan) {
//         return false;
//     }
// #endif

//     NetworkModelItem::ItemType itemType = (NetworkModelItem::ItemType)sourceModel()->data(index, NetworkModel::ItemTypeRole).toUInt();
//     // TODO allow to hide available access points

//     const QString pattern = filterRegExp().pattern();
//     if (!pattern.isEmpty()) {
//         const QString data = sourceModel()->data(index, Qt::DisplayRole).toString();
//         return data.contains(pattern, Qt::CaseInsensitive);
//     }
//
//     return true;
}

bool KCMProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
//     const AppletProxyModel::SortedConnectionType leftType = AppletProxyModel::connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(left, NetworkModel::TypeRole).toUInt());
    const QString leftName = sourceModel()->data(left, NetworkModel::NameRole).toString();
    const bool leftAvailable = (NetworkModelItem::ItemType)sourceModel()->data(left, NetworkModel::ItemTypeRole).toUInt() != NetworkModelItem::UnavailableConnection;
    const bool leftConnected = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const int leftConnectionState = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt();
    const QString leftUuid = sourceModel()->data(left, NetworkModel::UuidRole).toString();
    const int leftSignal = sourceModel()->data(left, NetworkModel::SignalRole).toInt();
    const QDateTime leftDate = sourceModel()->data(left, NetworkModel::TimeStampRole).toDateTime();

//     const AppletProxyModel::SortedConnectionType rightType = AppletProxyModel::connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(right, NetworkModel::TypeRole).toUInt());
    const QString rightName = sourceModel()->data(right, NetworkModel::NameRole).toString();
    const bool rightAvailable = (NetworkModelItem::ItemType)sourceModel()->data(right, NetworkModel::ItemTypeRole).toUInt() != NetworkModelItem::UnavailableConnection;
    const bool rightConnected = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const int rightConnectionState = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt();
    const QString rightUuid = sourceModel()->data(right, NetworkModel::UuidRole).toString();
    const int rightSignal = sourceModel()->data(right, NetworkModel::SignalRole).toInt();
    const QDateTime rightDate = sourceModel()->data(right, NetworkModel::TimeStampRole).toDateTime();

    // Default sort
    if (leftUuid.isEmpty() && !rightUuid.isEmpty()) {
        return true;
    } else if (!leftUuid.isEmpty() && rightUuid.isEmpty()) {
        return false;
    } else if (leftUuid.isEmpty() && rightUuid.isEmpty()) {
        // Comparing available access points
        if (leftSignal < rightSignal) {
            return true;
        } else if (leftSignal > rightSignal) {
            return false;
        }

        if (QString::localeAwareCompare(leftName, rightName) > 0) {
            return true;
        } else {
            return false;
        }
    }

//     // Sort by connection type
//     if (leftType < rightType) {
//         return false;
//     } else if (leftType > rightType) {
//         return true;
//     }

    if (QString::localeAwareCompare(leftName, rightName) > 0) {
        return true;
    } else {
        return false;
    }
}
