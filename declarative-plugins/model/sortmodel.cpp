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

#include "sortmodel.h"
#include "model.h"

SortModel::SortedConnectionType SortModel::connectionTypeToSortedType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    switch (type) {
        case NetworkManager::ConnectionSettings::Unknown:
            return SortModel::SortModel::Unknown;
            break;
        case NetworkManager::ConnectionSettings::Adsl:
            return SortModel::SortModel::Adsl;
            break;
        case NetworkManager::ConnectionSettings::Bluetooth:
            return SortModel::Bluetooth;
            break;
        case NetworkManager::ConnectionSettings::Bond:
            return SortModel::Bond;
            break;
        case NetworkManager::ConnectionSettings::Bridge:
            return SortModel::Bridge;
            break;
        case NetworkManager::ConnectionSettings::Cdma:
            return SortModel::Cdma;
            break;
        case NetworkManager::ConnectionSettings::Gsm:
            return SortModel::Gsm;
            break;
        case NetworkManager::ConnectionSettings::Infiniband:
            return SortModel::Infiniband;
            break;
        case NetworkManager::ConnectionSettings::OLPCMesh:
            return SortModel::OLPCMesh;
            break;
        case NetworkManager::ConnectionSettings::Pppoe:
            return SortModel::Pppoe;
            break;
        case NetworkManager::ConnectionSettings::Vlan:
            return SortModel::Vlan;
            break;
        case NetworkManager::ConnectionSettings::Vpn:
            return SortModel::Vpn;
            break;
        case NetworkManager::ConnectionSettings::Wimax:
            return SortModel::Wimax;
            break;
        case NetworkManager::ConnectionSettings::Wired:
            return SortModel::Wired;
            break;
        case NetworkManager::ConnectionSettings::Wireless:
            return SortModel::Wireless;
            break;
        default:
            return SortModel::Unknown;
            break;
    }
}

SortModel::SortModel(QObject* parent):
    QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0, Qt::DescendingOrder);
}

SortModel::~SortModel()
{
}

void SortModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

QAbstractItemModel* SortModel::sourceModel() const
{
    return QSortFilterProxyModel::sourceModel();
}

bool SortModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const bool leftConnected = sourceModel()->data(left, Model::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const SortedConnectionType leftType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(left, Model::TypeRole).toUInt());
    const QString leftUuid = sourceModel()->data(left, Model::UuidRole).toString();
    const int leftSignal = sourceModel()->data(left, Model::SignalRole).toInt();

    const bool rightConnected = sourceModel()->data(right, Model::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const SortedConnectionType rightType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(right, Model::TypeRole).toUInt());
    const QString rightUuid = sourceModel()->data(right, Model::UuidRole).toString();
    const int rightSignal = sourceModel()->data(right, Model::SignalRole).toInt();

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
