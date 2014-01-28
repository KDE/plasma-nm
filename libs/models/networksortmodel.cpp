/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "networksortmodel.h"
#include "networkmodel.h"

NetworkSortModel::SortedConnectionType NetworkSortModel::connectionTypeToSortedType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    switch (type) {
        case NetworkManager::ConnectionSettings::Unknown:
            return NetworkSortModel::NetworkSortModel::Unknown;
            break;
        case NetworkManager::ConnectionSettings::Adsl:
            return NetworkSortModel::NetworkSortModel::Adsl;
            break;
        case NetworkManager::ConnectionSettings::Bluetooth:
            return NetworkSortModel::Bluetooth;
            break;
        case NetworkManager::ConnectionSettings::Bond:
            return NetworkSortModel::Bond;
            break;
        case NetworkManager::ConnectionSettings::Bridge:
            return NetworkSortModel::Bridge;
            break;
        case NetworkManager::ConnectionSettings::Cdma:
            return NetworkSortModel::Cdma;
            break;
        case NetworkManager::ConnectionSettings::Gsm:
            return NetworkSortModel::Gsm;
            break;
        case NetworkManager::ConnectionSettings::Infiniband:
            return NetworkSortModel::Infiniband;
            break;
        case NetworkManager::ConnectionSettings::OLPCMesh:
            return NetworkSortModel::OLPCMesh;
            break;
        case NetworkManager::ConnectionSettings::Pppoe:
            return NetworkSortModel::Pppoe;
            break;
        case NetworkManager::ConnectionSettings::Vlan:
            return NetworkSortModel::Vlan;
            break;
        case NetworkManager::ConnectionSettings::Vpn:
            return NetworkSortModel::Vpn;
            break;
        case NetworkManager::ConnectionSettings::Wimax:
            return NetworkSortModel::Wimax;
            break;
        case NetworkManager::ConnectionSettings::Wired:
            return NetworkSortModel::Wired;
            break;
        case NetworkManager::ConnectionSettings::Wireless:
            return NetworkSortModel::Wireless;
            break;
        default:
            return NetworkSortModel::Unknown;
            break;
    }
}

NetworkSortModel::NetworkSortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0, Qt::DescendingOrder);
}

NetworkSortModel::~NetworkSortModel()
{
}

bool NetworkSortModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const bool leftAvailable = (NetworkModelItem::ItemType)sourceModel()->data(left, NetworkModel::ItemTypeRole).toUInt() != NetworkModelItem::UnavailableConnection;
    const bool leftConnected = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const SortedConnectionType leftType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(left, NetworkModel::TypeRole).toUInt());
    const QString leftUuid = sourceModel()->data(left, NetworkModel::UuidRole).toString();
    const int leftSignal = sourceModel()->data(left, NetworkModel::SignalRole).toInt();

    const bool rightAvailable = (NetworkModelItem::ItemType)sourceModel()->data(right, NetworkModel::ItemTypeRole).toUInt() != NetworkModelItem::UnavailableConnection;
    const bool rightConnected = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const SortedConnectionType rightType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(right, NetworkModel::TypeRole).toUInt());
    const QString rightUuid = sourceModel()->data(right, NetworkModel::UuidRole).toString();
    const int rightSignal = sourceModel()->data(right, NetworkModel::SignalRole).toInt();

    if (leftAvailable < rightAvailable) {
        return true;
    } else if (leftAvailable > rightAvailable) {
        return false;
    }

    if (leftConnected < rightConnected) {
        return true;
    } else if (leftConnected > rightConnected) {
        return false;
    }

    if (leftUuid.isEmpty() && !rightUuid.isEmpty()) {
        return true;
    } else if (!leftUuid.isEmpty() && rightUuid.isEmpty()) {
        return false;
    }

    if (leftType < rightType) {
        return false;
    } else if (leftType > rightType) {
        return true;
    }

    if (leftSignal < rightSignal) {
        return true;
    } else {
        return false;
    }

    return false;
}
