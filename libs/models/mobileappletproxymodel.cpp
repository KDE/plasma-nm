/*
 * <one line to give the library's name and an idea of what it does.>
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

#include "mobileappletproxymodel.h"
#include "uiutils.h"

MobileAppletProxymodel::MobileAppletProxymodel(QObject* parent):AppletProxyModel(parent)
{
    setDynamicSortFilter(true);
    sort(0, Qt::DescendingOrder);
}

MobileAppletProxymodel::~MobileAppletProxymodel()
{
}


bool MobileAppletProxymodel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);

    // slaves are always filtered-out
    const bool isSlave = sourceModel()->data(index, NetworkModel::SlaveRole).toBool();
    if (isSlave) {
        return false;
    }
    const NetworkManager::ConnectionSettings::ConnectionType type = (NetworkManager::ConnectionSettings::ConnectionType) sourceModel()->data(index, NetworkModel::TypeRole).toUInt();
    
    if(type == NetworkManager::ConnectionSettings::Wireless){
        if (!UiUtils::isConnectionTypeSupported(type)) {
            return false;
        }
        NetworkModelItem::ItemType itemType = (NetworkModelItem::ItemType)sourceModel()->data(index, NetworkModel::ItemTypeRole).toUInt();
        
        if (itemType == NetworkModelItem::AvailableConnection ||
            itemType == NetworkModelItem::AvailableAccessPoint ||
            itemType == NetworkModelItem::AvailableNsp) {
            return true;
        }
    }
    return false;
}
