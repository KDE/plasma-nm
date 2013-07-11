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

#include "networksettingsmodel.h"

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>

NetworkSettingsModel::NetworkSettingsModel(QObject *parent):
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[TypeRole] = "itemType";
    roles[NameRole] = "itemName";
    roles[IconRole] = "itemIcon";
    roles[PathRole] = "itemPath";

    setRoleNames(roles);

    bool nonVirtualDevice = false;
    NetworkSettingModelItem *item = 0;

    item = new NetworkSettingModelItem(NetworkSettingModelItem::General, QString());
    int index = m_networkItems.count();
    beginInsertRows(QModelIndex(), index, index);
    m_networkItems.push_back(item);
    endInsertRows();

    foreach (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Ethernet) {
            item = new NetworkSettingModelItem(NetworkSettingModelItem::Ethernet, device->uni());
            nonVirtualDevice = true;
        } else if (device->type() == NetworkManager::Device::Modem) {
            item = new NetworkSettingModelItem(NetworkSettingModelItem::Modem, device->uni());
            nonVirtualDevice = true;
        } else if (device->type() == NetworkManager::Device::Wifi) {
            item = new NetworkSettingModelItem(NetworkSettingModelItem::Wifi, device->uni());
            nonVirtualDevice = true;
        }

        if (nonVirtualDevice) {
            index = m_networkItems.count();
            beginInsertRows(QModelIndex(), index, index);
            m_networkItems.push_back(item);
            endInsertRows();

            nonVirtualDevice = false;
            item = 0;
        }
    }

    // Insert VPN setting at the end
    item = new NetworkSettingModelItem(NetworkSettingModelItem::Vpn, QString());
    index = m_networkItems.count();
    beginInsertRows(QModelIndex(), index, index);
    m_networkItems.push_back(item);
    endInsertRows();
}

NetworkSettingsModel::~NetworkSettingsModel()
{
}

int NetworkSettingsModel::count() const
{
    return m_networkItems.count();
}

int NetworkSettingsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_networkItems.count();
}

QVariant NetworkSettingsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_networkItems.count()) {
        NetworkSettingModelItem *item = m_networkItems.at(row);

        switch (role) {
            case TypeRole:
                return item->type();
                break;
            case NameRole:
                return item->name();
                break;
            case IconRole:
                return item->icon();
                break;
            case PathRole:
                return item->path();
                break;
            default:
                break;
        }
    }

    return QVariant();
}

