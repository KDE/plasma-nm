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

#include "networkmodel.h"
#include "networkmodelitem.h"

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>

NetworkModel::NetworkModel(QObject* parent):
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[TypeRole] = "itemType";
    roles[NameRole] = "itemName";
    roles[SvgIconRole] = "itemSvgIcon";
    roles[IconRole] = "itemIcon";
    roles[RemovableItemRole] = "removableItem";
    roles[PathRole] = "itemPath";

    setRoleNames(roles);

    foreach (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        const int index = m_networkItems.count();
        beginInsertRows(QModelIndex(), index, index);
        NetworkModelItem * item;
        if (device->type() == NetworkManager::Device::Bond) {
            qDebug() << "Creating bond";
            item = new NetworkModelItem(NetworkModelItem::Bond, device->uni());
        } else if (device->type() == NetworkManager::Device::Bridge) {
            qDebug() << "Creating bridge";
            item = new NetworkModelItem(NetworkModelItem::Bridge, device->uni());
        } else if (device->type() == NetworkManager::Device::Ethernet) {
            qDebug() << "Creating ethernet";
            item = new NetworkModelItem(NetworkModelItem::Ethernet, device->uni());
        } else if (device->type() == NetworkManager::Device::Vlan) {
            qDebug() << "Creating vlan";
            item = new NetworkModelItem(NetworkModelItem::Vlan, device->uni());
        } else if (device->type() == NetworkManager::Device::Wifi) {
            qDebug() << "Creating wifi";
            item = new NetworkModelItem(NetworkModelItem::Wifi, device->uni());
        }
        m_networkItems.push_back(item);
        endInsertRows();
    }

    foreach (const NetworkManager::Connection::Ptr & connection, NetworkManager::listConnections()) {
        if (connection->settings()->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
            qDebug() << "Creating vpn";
            const int index = m_networkItems.count();
            beginInsertRows(QModelIndex(), index, index);
            NetworkModelItem * item = new NetworkModelItem(NetworkModelItem::Vpn, connection->path());
            m_networkItems.push_back(item);
            endInsertRows();
        }
    }
}

NetworkModel::~NetworkModel()
{
}

int NetworkModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_networkItems.count();
}

QVariant NetworkModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_networkItems.count()) {
        NetworkModelItem * item = m_networkItems.at(row);

        switch (role) {
            case TypeRole:
                return item->type();
                break;
            case NameRole:
                return item->name();
                break;
            case SvgIconRole:
                return item->svgIcon();
                break;
            case IconRole:
                return item->icon();
                break;
            case RemovableItemRole:
                return item->isRemovable();
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
