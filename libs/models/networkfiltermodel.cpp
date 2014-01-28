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

#include "networkfiltermodel.h"
#include "networkmodel.h"

NetworkFilterModel::NetworkFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_filterType(NetworkFilterModel::All)
{
    setDynamicSortFilter(true);
    setFilterRole(NetworkModel::ItemTypeRole);
    sort(0, Qt::DescendingOrder);
}

NetworkFilterModel::~NetworkFilterModel()
{
}

void NetworkFilterModel::setFilterType(int type)
{
    switch (type) {
        case 0:
            setFilterType(NetworkFilterModel::All);
            break;
        case 1:
            setFilterType(NetworkFilterModel::EditableConnections);
            break;
        case 2:
            setFilterType(NetworkFilterModel::AvailableConnections);
            break;
        default:
            setFilterType(NetworkFilterModel::All);
    }
}

NetworkFilterModel::FilterType NetworkFilterModel::filterType() const
{
    return m_filterType;
}

void NetworkFilterModel::setFilterType(NetworkFilterModel::FilterType type)
{
    m_filterType = type;
    filterChanged();
    sort(0, Qt::DescendingOrder);
}

void NetworkFilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

QAbstractItemModel* NetworkFilterModel::sourceModel() const
{
    return QSortFilterProxyModel::sourceModel();
}

bool NetworkFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (m_filterType == NetworkFilterModel::All) {
        return true;
    }

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    NetworkModelItem::ItemType itemType = (NetworkModelItem::ItemType)sourceModel()->data(index, NetworkModel::ItemTypeRole).toUInt();

    if (m_filterType == NetworkFilterModel::AvailableConnections) {
        if (itemType == NetworkModelItem::AvailableConnection ||
            itemType == NetworkModelItem::AvailableAccessPoint) {
            return true;
        }
        return false;
    } else if (m_filterType == NetworkFilterModel::EditableConnections) {
        if (itemType == NetworkModelItem::UnavailableConnection ||
            itemType == NetworkModelItem::AvailableConnection) {
            return true;
        }
        return false;
    }

    return false;
}
