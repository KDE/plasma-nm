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
    bool leftConnected = sourceModel()->data(left, Model::ConnectedRole).toBool();
    QString leftName = sourceModel()->data(left, Model::NameRole).toString();
    NetworkManager::Settings::ConnectionSettings::ConnectionType leftType;
    leftType = (NetworkManager::Settings::ConnectionSettings::ConnectionType) sourceModel()->data(left, Model::TypeRole).toUInt();
    QString leftUuid = sourceModel()->data(left, Model::UuidRole).toString();
    int leftSignal = sourceModel()->data(left, Model::SignalRole).toInt();

    bool rightConnected = sourceModel()->data(right, Model::ConnectedRole).toBool();
    QString rightName = sourceModel()->data(right, Model::NameRole).toString();
    NetworkManager::Settings::ConnectionSettings::ConnectionType rightType;
    rightType = (NetworkManager::Settings::ConnectionSettings::ConnectionType) sourceModel()->data(right, Model::TypeRole).toUInt();
    QString rightUuid = sourceModel()->data(right, Model::UuidRole).toString();
    int rightSignal = sourceModel()->data(right, Model::SignalRole).toInt();

    Q_UNUSED(leftName);
    Q_UNUSED(rightName);

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
        return true;
    } else if (leftType > rightType) {
        return false;
    }

    if (leftSignal < rightSignal) {
        return true;
    } else {
        return false;
    }

    return false;
}
