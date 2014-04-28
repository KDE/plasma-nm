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
#include "networkmodel.h"

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

void EditorProxyModel::setSortOrder(Qt::SortOrder order)
{
    sort(0, order);
}

void EditorProxyModel::setSortRole(const QByteArray& role)
{
    QSortFilterProxyModel::setSortRole(roleKey(role));
}

QByteArray EditorProxyModel::sortRole() const
{
    return roleNames().value(QSortFilterProxyModel::sortRole());
}

QString EditorProxyModel::filterString() const
{
    return filterRegExp().pattern();
}

void EditorProxyModel::setFilterString(const QString& filter)
{
    setFilterRegExp(QRegExp(filter, filterCaseSensitivity()));
}

QVariant EditorProxyModel::get(int row, const QString& role)
{
    QModelIndex index = EditorProxyModel::index(row, 0);

    if (role == QLatin1String("ConnectionPath")) {
        return data(index, NetworkModel::ConnectionPathRole);
    } else if (role == QLatin1String("ConnectionState")) {
        return data(index, NetworkModel::ConnectionStateRole);
    } else if (role == QLatin1String("Type")) {
        return data(index, NetworkModel::TypeRole);
    } else if (role == QLatin1String("Uuid")) {
        return data(index, NetworkModel::UuidRole);
    } else if (role == QLatin1String("Name")) {
        return data(index, NetworkModel::NameRole);
    }

    return QVariant();
}

int EditorProxyModel::roleKey(const QByteArray &role) const
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> it(roles);
    while (it.hasNext()) {
        it.next();
        if (it.value() == role)
            return it.key();
    }
    return -1;
}

QHash<int, QByteArray> EditorProxyModel::roleNames() const
{
    if (QAbstractItemModel *source = sourceModel())
        return source->roleNames();
    return QHash<int, QByteArray>();
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
        const QString data = sourceModel()->data(index, NetworkModel::NameRole).toString();
        //qDebug() << "Filtering " << data << "with pattern" << pattern;
        return data.contains(pattern, Qt::CaseInsensitive);
    }

    return true;
}

bool EditorProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const SortedConnectionType leftType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(left, NetworkModel::TypeRole).toUInt());
    const SortedConnectionType rightType = connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(right, NetworkModel::TypeRole).toUInt());

    if (sortOrder() == Qt::DescendingOrder && leftType != rightType) {
        return (leftType > rightType);
    } else if (leftType != rightType) {
        return (leftType < rightType);
    }

    return QSortFilterProxyModel::lessThan(left, right);
}
