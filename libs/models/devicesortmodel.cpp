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

#include "devicesortmodel.h"

DeviceSortModel::SortedDeviceType DeviceSortModel::deviceTypeToSortedType(NetworkManager::Device::Type type)
{
    switch (type) {
        case NetworkManager::Device::Ethernet:
            return DeviceSortModel::Ethernet;
            break;
      case NetworkManager::Device::Wifi:
            return DeviceSortModel::Wifi;
            break;
      case NetworkManager::Device::Modem:
            return DeviceSortModel::Modem;
            break;
      case NetworkManager::Device::Bluetooth:
            return DeviceSortModel::Bluetooth;
            break;
        default:
            return DeviceSortModel::Unknown;
            break;
    }
}

DeviceSortModel::DeviceSortModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0, Qt::DescendingOrder);
}

DeviceSortModel::~DeviceSortModel()
{
}

bool DeviceSortModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const SortedDeviceType leftType = deviceTypeToSortedType((NetworkManager::Device::Type) sourceModel()->data(left, DeviceModel::DeviceTypeRole).toUInt());
    const QString leftName = sourceModel()->data(left, DeviceModel::DeviceLabelRole).toString();

    const SortedDeviceType rightType = deviceTypeToSortedType((NetworkManager::Device::Type) sourceModel()->data(right, DeviceModel::DeviceTypeRole).toUInt());
    const QString rightName = sourceModel()->data(right, DeviceModel::DeviceLabelRole).toString();

    if (leftType < rightType) {
        return false;
    } else if (leftType > rightType) {
        return true;
    }

    return QString::localeAwareCompare(leftName, rightName) > 0;
}