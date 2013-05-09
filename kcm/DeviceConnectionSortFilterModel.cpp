/***************************************************************************
 *   Copyright (C) 2012 by Daniel Nicoletti                                *
 *   dantti12@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/
#include "DeviceConnectionSortFilterModel.h"

#include "DeviceConnectionModel.h"

#include <KDebug>

DeviceConnectionSortFilterModel::DeviceConnectionSortFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_showInactiveConnections(false)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sort(0);
}

bool DeviceConnectionSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_showInactiveConnections) {
        return true;
    }

    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    if (index.data(DeviceConnectionModel::RoleIsConnection).toBool()) {
        if (!index.data(DeviceConnectionModel::RoleIsVpnConnection).toBool()) {
            return !index.data(DeviceConnectionModel::RoleConnectionActivePath).isNull();
        }
    } else if (index.data(DeviceConnectionModel::RoleIsConnectionCategory).toBool()) {
        return index.data(DeviceConnectionModel::RoleIsConnectionCategoryActiveCount).toUInt();
    }

    // Return true if no filter was applied
    return true;
}

void DeviceConnectionSortFilterModel::setShowInactiveConnections(bool show)
{
    m_showInactiveConnections = show;
    invalidate();
}

bool DeviceConnectionSortFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool leftIsDeviceParent = left.data(DeviceConnectionModel::RoleIsDeviceParent).toBool();
    bool rightIsDeviceParent = right.data(DeviceConnectionModel::RoleIsDeviceParent).toBool();
    if (leftIsDeviceParent != rightIsDeviceParent) {
        // If the left item is a device parent the left should move right
        return leftIsDeviceParent;
    }

    bool leftIsDevice = left.data(DeviceConnectionModel::RoleIsDevice).toBool();
    bool rightIsDevice = right.data(DeviceConnectionModel::RoleIsDevice).toBool();
    if (leftIsDevice != rightIsDevice) {
        // If the left item is a device the left should move right
        return leftIsDevice;
    }

    bool leftIsConnection = left.data(DeviceConnectionModel::RoleIsConnection).toBool();
    if (leftIsConnection) {
        bool leftIsConnectionActive = !left.data(DeviceConnectionModel::RoleConnectionActivePath).isNull();
        bool rightIsConnectionActive = !right.data(DeviceConnectionModel::RoleConnectionActivePath).isNull();
        if (leftIsConnectionActive != rightIsConnectionActive) {
            // If the left item is a active connection the right should move left
            return leftIsConnectionActive;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
