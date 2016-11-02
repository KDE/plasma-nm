/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

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

#include "editorproxymodel.h"

EditorProxyModel::SortedConnectionType EditorProxyModel::connectionTypeToSortedType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    switch (type) {
        case NetworkManager::ConnectionSettings::Unknown:
            return EditorProxyModel::EditorProxyModel::Unknown;
            break;
        case NetworkManager::ConnectionSettings::Adsl:
            return EditorProxyModel::EditorProxyModel::Adsl;
            break;
        case NetworkManager::ConnectionSettings::Bluetooth:
            return EditorProxyModel::Bluetooth;
            break;
        case NetworkManager::ConnectionSettings::Bond:
            return EditorProxyModel::Bond;
            break;
        case NetworkManager::ConnectionSettings::Bridge:
            return EditorProxyModel::Bridge;
            break;
        case NetworkManager::ConnectionSettings::Cdma:
            return EditorProxyModel::Cdma;
            break;
        case NetworkManager::ConnectionSettings::Gsm:
            return EditorProxyModel::Gsm;
            break;
        case NetworkManager::ConnectionSettings::Infiniband:
            return EditorProxyModel::Infiniband;
            break;
        case NetworkManager::ConnectionSettings::OLPCMesh:
            return EditorProxyModel::OLPCMesh;
            break;
        case NetworkManager::ConnectionSettings::Pppoe:
            return EditorProxyModel::Pppoe;
            break;
#if NM_CHECK_VERSION(0, 9, 10)
        case NetworkManager::ConnectionSettings::Team:
            return EditorProxyModel::Team;
            break;
#endif
        case NetworkManager::ConnectionSettings::Vlan:
            return EditorProxyModel::Vlan;
            break;
        case NetworkManager::ConnectionSettings::Vpn:
            return EditorProxyModel::Vpn;
            break;
        case NetworkManager::ConnectionSettings::Wimax:
            return EditorProxyModel::Wimax;
            break;
        case NetworkManager::ConnectionSettings::Wired:
            return EditorProxyModel::Wired;
            break;
        case NetworkManager::ConnectionSettings::Wireless:
            return EditorProxyModel::Wireless;
            break;
        default:
            return EditorProxyModel::Unknown;
            break;
    }
}

EditorProxyModel::EditorProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setSortLocaleAware(true);
    sort(0, Qt::DescendingOrder);
}

EditorProxyModel::~EditorProxyModel()
{
}
#include "debug.h"
bool EditorProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    // slaves are always filtered-out
    const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();
    const bool isDuplicate = sourceModel()->data(index, NetworkModel::DuplicateRole).toBool();

    if (isSlave || isDuplicate) {
        return false;
    }

#if NM_CHECK_VERSION(0, 9, 10)
    const NetworkManager::ConnectionSettings::ConnectionType type = (NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(index, NetworkModel::TypeRole).toUInt();
    if (type == NetworkManager::ConnectionSettings::Generic) {
        return false;
    }
#if NM_CHECK_VERSION(1, 1, 92)
    if (type == NetworkManager::ConnectionSettings::Tun) {
        return false;
    }
#endif
#endif

    NetworkModelItem::ItemType itemType = (NetworkModelItem::ItemType)sourceModel()->data(index, NetworkModel::ItemTypeRole).toUInt();
    if (itemType == NetworkModelItem::AvailableAccessPoint || itemType == NetworkModelItem::AvailableNsp) {
        return false;
    }

    const QString pattern = filterRegExp().pattern();
    if (!pattern.isEmpty()) {  // filtering on data (connection name), wildcard-only
        QString data = sourceModel()->data(index, Qt::DisplayRole).toString();
        if (data.isEmpty()) {
            data = sourceModel()->data(index, NetworkModel::NameRole).toString();
        }
        qCDebug(PLASMA_NM) << "Filtering " << data << "with pattern" << pattern;
        return data.contains(pattern, Qt::CaseInsensitive);
    }

    return true;
}

bool EditorProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{ 
    const bool leftConnected = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const QString leftName = sourceModel()->data(left, NetworkModel::NameRole).toString();
    const SortedConnectionType leftType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(left, NetworkModel::TypeRole).toUInt());
    const QDateTime leftDate = sourceModel()->data(left, NetworkModel::TimeStampRole).toDateTime();

    const bool rightConnected = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const QString rightName = sourceModel()->data(right, NetworkModel::NameRole).toString();
    const SortedConnectionType rightType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(right, NetworkModel::TypeRole).toUInt());
    const QDateTime rightDate = sourceModel()->data(right, NetworkModel::TimeStampRole).toDateTime();

    if (leftType < rightType) {
        return false;
    } else if (leftType > rightType) {
        return true;
    }
    
    if (leftConnected < rightConnected) {
        return true;
    } else if (leftConnected > rightConnected) {
        return false;
    }

    if (leftDate > rightDate) {
        return false;
    } else if (leftDate < rightDate) {
        return true;
    }

    if (QString::localeAwareCompare(leftName, rightName) > 0) {
        return true;
    } else {
        return false;
    }
}
