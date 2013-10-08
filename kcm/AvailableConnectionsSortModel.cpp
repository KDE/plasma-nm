/***************************************************************************
 *   Copyright (C) 2013 by Daniel Nicoletti                                *
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
#include "AvailableConnectionsSortModel.h"

#include "AvailableConnectionsModel.h"

#include <QDebug>

AvailableConnectionsSortModel::AvailableConnectionsSortModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    sort(0);
}

bool AvailableConnectionsSortModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    uint leftKind = left.data(AvailableConnectionsModel::RoleKinds).toUInt();
    uint rightKind = right.data(AvailableConnectionsModel::RoleKinds).toUInt();

    if (leftKind != rightKind) {
        // If the right item does not have a connection the left should move right
        return !(rightKind & AvailableConnectionsModel::Connection);
    } else if (rightKind & AvailableConnectionsModel::NetworkWireless ||
               rightKind & AvailableConnectionsModel::NetworkNsp) {
        int leftSignalStrength = left.data(AvailableConnectionsModel::RoleSignalStrength).toInt();
        int rightSignalStrength = right.data(AvailableConnectionsModel::RoleSignalStrength).toInt();
        leftSignalStrength = normalizeSignalStrength(leftSignalStrength);
        rightSignalStrength = normalizeSignalStrength(rightSignalStrength);
        if (leftSignalStrength != rightSignalStrength) {
            // If the left item is more than the right one, left should move right
            return leftSignalStrength > rightSignalStrength;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

int AvailableConnectionsSortModel::normalizeSignalStrength(int signalStrength) const
{
    if (signalStrength < 13) {
        return 0;
    } else if (signalStrength < 38) {
        return 25;
    } else if (signalStrength < 63) {
        return 50;
    } else if (signalStrength < 88) {
        return 75;
    } else {
        return 100;
    }
}
