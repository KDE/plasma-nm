/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>

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

#include "connectioneditorproxymodel.h"

#include "networkmodel.h"
#include "networkmodelitem.h"
#include "networkfiltermodel.h"

ConnectionEditorProxyModel::ConnectionEditorProxyModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
    NetworkModel * baseModel = new NetworkModel(this);
    NetworkFilterModel * filterModel = new NetworkFilterModel(this);
    filterModel->setFilterType(NetworkFilterModel::EditableConnections);
    filterModel->setSourceModel(baseModel);

    setSourceModel(baseModel);
}

Qt::ItemFlags ConnectionEditorProxyModel::flags(const QModelIndex& index) const
{
    const QModelIndex mappedProxyIndex = index.sibling(index.row(), 0);
    return QIdentityProxyModel::flags(mappedProxyIndex) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int ConnectionEditorProxyModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return 2;
}

QVariant ConnectionEditorProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Connection name");
            case 1:
                return tr("Last used");
        }
    }

    return QIdentityProxyModel::headerData(section, orientation, role);
}

QVariant ConnectionEditorProxyModel::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    const QModelIndex sourceIndex = sourceModel()->index(index.row(), 0);
    const QString connectionName = sourceModel()->data(sourceIndex, NetworkModel::NameRole).toString();
    const QString lastUsed = sourceModel()->data(sourceIndex, NetworkModel::LastUsedRole).toString();

    switch (index.column()) {
        case 0:
            return connectionName;
        case 1:
            return lastUsed;
    }

    return QVariant();
}

QModelIndex ConnectionEditorProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    return createIndex(row, column, 0);
}

QModelIndex ConnectionEditorProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (proxyIndex.column() > 0) {
        return QModelIndex();
    }

    return QIdentityProxyModel::mapToSource(proxyIndex);
}
