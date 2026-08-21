/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "ipv6routetablemodel.h"

#include <KLocalizedString>

Ipv6RouteTableModel::Ipv6RouteTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int Ipv6RouteTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_routes.size());
}

int Ipv6RouteTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant Ipv6RouteTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_routes.size())
        return {};

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    const Ipv6RouteEntry &entry = m_routes.at(index.row());

    switch (index.column()) {
    case AddressColumn:
        return entry.address;
    case PrefixColumn:
        return entry.prefix;
    case NextHopColumn:
        return entry.nextHop;
    case MetricColumn:
        return entry.metric;
    default:
        return {};
    }
}

bool Ipv6RouteTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_routes.size())
        return false;

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return false;

    Ipv6RouteEntry &entry = m_routes[index.row()];

    switch (index.column()) {
    case AddressColumn:
        if (entry.address == value.toString())
            return false;
        entry.address = value.toString();
        break;
    case PrefixColumn: {
        const uint prefix = qMin(value.toUInt(), MaxPrefixLength);
        if (entry.prefix == prefix)
            return false;
        entry.prefix = prefix;
        break;
    }
    case NextHopColumn:
        if (entry.nextHop == value.toString())
            return false;
        entry.nextHop = value.toString();
        break;
    case MetricColumn: {
        const uint metric = value.toUInt();
        if (entry.metric == metric)
            return false;
        entry.metric = metric;
        break;
    }
    default:
        return false;
    }

    Q_EMIT dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    Q_EMIT routesChanged();
    return true;
}

QVariant Ipv6RouteTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Vertical)
        return section + 1;

    switch (section) {
    case AddressColumn:
        return i18nc("@title:column", "Address");
    case PrefixColumn:
        return i18nc("@title:column IPv6 prefix length", "Prefix");
    case NextHopColumn:
        return i18nc("@title:column", "Gateway");
    case MetricColumn:
        return i18nc("@title:column", "Metric");
    default:
        return {};
    }
}

Qt::ItemFlags Ipv6RouteTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

bool Ipv6RouteTableModel::setRouteField(int row, int column, const QVariant &value)
{
    return setData(index(row, column), value, Qt::EditRole);
}

QList<Ipv6RouteEntry> Ipv6RouteTableModel::routes() const
{
    return m_routes;
}

void Ipv6RouteTableModel::setRoutes(const QList<Ipv6RouteEntry> &routes)
{
    beginResetModel();
    m_routes = routes;
    endResetModel();

    Q_EMIT routesChanged();
}

void Ipv6RouteTableModel::addRoute()
{
    const int row = static_cast<int>(m_routes.size());

    beginInsertRows(QModelIndex(), row, row);
    m_routes.append(Ipv6RouteEntry{});
    endInsertRows();

    Q_EMIT routesChanged();
}

void Ipv6RouteTableModel::removeRoute(int row)
{
    if (row < 0 || row >= m_routes.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_routes.removeAt(row);
    endRemoveRows();

    Q_EMIT routesChanged();
}

#include "moc_ipv6routetablemodel.cpp"
