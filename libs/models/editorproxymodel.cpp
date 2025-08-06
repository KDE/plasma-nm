/*
    SPDX-FileCopyrightText: 2013-2018 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "editorproxymodel.h"
#include "networkmodel.h"
#include "networkmodelitem.h"
#include "uiutils.h"

EditorProxyModel::EditorProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setFilterRole(NetworkModel::NameRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setSortLocaleAware(true);
    sort(0, Qt::DescendingOrder);
}

EditorProxyModel::~EditorProxyModel() = default;

bool EditorProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    // slaves are always filtered-out
    const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();
    const bool isDuplicate = sourceModel()->data(index, NetworkModel::DuplicateRole).toBool();

    if (isSlave || isDuplicate) {
        return false;
    }

    const NetworkManager::ConnectionSettings::ConnectionType type =
        (NetworkManager::ConnectionSettings::ConnectionType)sourceModel()->data(index, NetworkModel::TypeRole).toUInt();
    if (!UiUtils::isConnectionTypeSupported(type)) {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool EditorProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const bool leftDeactivated = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Deactivated;
    const auto leftAvailable = sourceModel()->data(left, NetworkModel::ItemTypeRole).toUInt() == NetworkModelItem::UnavailableConnection;
    const QString leftName = sourceModel()->data(left, NetworkModel::NameRole).toString();
    const UiUtils::SortedConnectionType leftType =
        UiUtils::connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType)sourceModel()->data(left, NetworkModel::TypeRole).toUInt());
    const QString leftVpnType = sourceModel()->data(left, NetworkModel::VpnType).toString();
    const QDateTime leftDate = sourceModel()->data(left, NetworkModel::TimeStampRole).toDateTime();

    const bool rightDeactivated = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Deactivated;
    const auto rightAvailable = sourceModel()->data(right, NetworkModel::ItemTypeRole).toUInt() == NetworkModelItem::UnavailableConnection;
    const QString rightName = sourceModel()->data(right, NetworkModel::NameRole).toString();
    const UiUtils::SortedConnectionType rightType =
        UiUtils::connectionTypeToSortedType((NetworkManager::ConnectionSettings::ConnectionType)sourceModel()->data(right, NetworkModel::TypeRole).toUInt());
    const QString rightVpnType = sourceModel()->data(right, NetworkModel::VpnType).toString();
    const QDateTime rightDate = sourceModel()->data(right, NetworkModel::TimeStampRole).toDateTime();

    if (leftAvailable < rightAvailable) {
        return false;
    } else if (leftAvailable > rightAvailable) {
        return true;
    }

    if (leftDeactivated < rightDeactivated) {
        return false;
    } else if (leftDeactivated > rightDeactivated) {
        return true;
    }

    if (leftType < rightType) {
        return false;
    } else if (leftType > rightType) {
        return true;
    }

    if (leftType == rightType && leftType == UiUtils::Vpn) {
        if (QString::localeAwareCompare(leftVpnType, rightVpnType) < 0) {
            return false;
        } else if (QString::localeAwareCompare(leftVpnType, rightVpnType) > 0) {
            return true;
        }
    }

    if (leftDate > rightDate) {
        return false;
    } else if (leftDate < rightDate) {
        return true;
    }

    if (QString::localeAwareCompare(leftName, rightName) > 0) {
        return true;
    } else {
        return false;
    }
}

#include "moc_editorproxymodel.cpp"
