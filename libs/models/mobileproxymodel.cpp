/*
 * Mobile proxy model - model for displaying netwoks in mobile kcm
 * Copyright 2017  Martin Kacej <m.kacej@atlas.sk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

#include "mobileproxymodel.h"
#include "networkmodel.h"
#include "networkmodelitem.h"
#include "uiutils.h"

MobileProxyModel::MobileProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0, Qt::DescendingOrder);
}

MobileProxyModel::~MobileProxyModel()
{
}

void MobileProxyModel::setShowSavedMode(bool mode){
    _showSavedMode = mode;
}

bool MobileProxyModel::showSavedMode() const{
    return _showSavedMode;
}

bool MobileProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    // slaves are always filtered-out
    const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();

    if (isSlave) {
        return false;
    }

    const NetworkManager::ConnectionSettings::ConnectionType type = (NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(index, NetworkModel::TypeRole).toUInt();
    if (type == NetworkManager::ConnectionSettings::Wireless) {
        NetworkModelItem::ItemType itemType = (NetworkModelItem::ItemType)sourceModel()->data(index, NetworkModel::ItemTypeRole).toUInt();
        if (showSavedMode()) {
            return itemType == NetworkModelItem::UnavailableConnection;
        } else {
            return itemType >= NetworkModelItem::AvailableConnection;
        }
    }
    return false;
}

bool MobileProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    const bool leftAvailable = (NetworkModelItem::ItemType)sourceModel()->data(left, NetworkModel::ItemTypeRole).toUInt() != NetworkModelItem::UnavailableConnection;
    const bool leftConnected = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const int leftConnectionState = sourceModel()->data(left, NetworkModel::ConnectionStateRole).toUInt();
    const QString leftName = sourceModel()->data(left, NetworkModel::NameRole).toString();
    const QString leftUuid = sourceModel()->data(left, NetworkModel::UuidRole).toString();
    const int leftSignal = sourceModel()->data(left, NetworkModel::SignalRole).toInt();
    const QDateTime leftDate = sourceModel()->data(left, NetworkModel::TimeStampRole).toDateTime();

    const bool rightAvailable = (NetworkModelItem::ItemType)sourceModel()->data(right, NetworkModel::ItemTypeRole).toUInt() != NetworkModelItem::UnavailableConnection;
    const bool rightConnected = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const int rightConnectionState = sourceModel()->data(right, NetworkModel::ConnectionStateRole).toUInt();
    const QString rightName = sourceModel()->data(right, NetworkModel::NameRole).toString();
    const QString rightUuid = sourceModel()->data(right, NetworkModel::UuidRole).toString();
    const int rightSignal = sourceModel()->data(right, NetworkModel::SignalRole).toInt();
    const QDateTime rightDate = sourceModel()->data(right, NetworkModel::TimeStampRole).toDateTime();

    if (leftAvailable < rightAvailable) {
        return true;
    } else if (leftAvailable > rightAvailable) {
        return false;
    }

    if (leftConnected < rightConnected) {
        return true;
    } else if (leftConnected > rightConnected) {
        return false;
    }

    if (leftConnectionState > rightConnectionState) {
        return true;
    } else if (leftConnectionState < rightConnectionState) {
        return false;
    }

    if (leftUuid.isEmpty() && !rightUuid.isEmpty()) {
        return true;
    } else if (!leftUuid.isEmpty() && rightUuid.isEmpty()) {
        return false;
    }

    if (leftDate > rightDate) {
        return false;
    } else if (leftDate < rightDate) {
        return true;
    }

    if (leftSignal < rightSignal) {
        return true;
    } else if (leftSignal > rightSignal) {
        return false;
    }

    if (QString::localeAwareCompare(leftName, rightName) > 0) {
        return true;
    } else {
        return false;
    }
}
