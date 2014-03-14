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

#include "editorproxymodel.h"
#include "networkmodelitem.h"
#include "networkmodel.h"

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

bool EditorProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    // slaves are always filtered-out
    const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();
    const bool isDuplicate = sourceModel()->data(index, NetworkModel::DuplicateRole).toBool();

    if (isSlave || isDuplicate) {
        return false;
    }

    NetworkModelItem::ItemType itemType = (NetworkModelItem::ItemType)sourceModel()->data(index, NetworkModel::ItemTypeRole).toUInt();
    if (itemType == NetworkModelItem::AvailableAccessPoint) {
        return false;
    }

    const QString pattern = filterRegExp().pattern();
    if (!pattern.isEmpty()) {  // filtering on data (connection name), wildcard-only
        const QString data = sourceModel()->data(index, Qt::DisplayRole).toString();
        //qDebug() << "Filtering " << data << "with pattern" << pattern;
        return data.contains(pattern, Qt::CaseInsensitive);
    }

    return true;
}

bool EditorProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (sourceModel()) { // special sorting case, only for editor
        if (sortColumn() == 1) {
            const QDateTime leftDate = sourceModel()->data(left, NetworkModel::TimeStampRole).toDateTime();
            const QDateTime rightDate = sourceModel()->data(right, NetworkModel::TimeStampRole).toDateTime();
            return leftDate < rightDate;
        }
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
