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

#include "model.h"
#include "modelitem.h"

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/settings/Wireless>

#include "debug.h"

Model::Model(QObject* parent):
    QAbstractListModel(parent),
    m_monitor(new Monitor(this))
{
    QHash<int, QByteArray> roles = roleNames();
    roles[ConnectingRole] = "itemConnecting";
    roles[ConnectedRole] = "itemConnected";
    roles[ConnectionPathRole] = "itemConnectionPath";
    roles[ConnectionIconRole] = "itemConnectionIcon";
    roles[ConnectionDetailsRole] = "itemDetails";
    roles[DeviceNameRole] = "itemDeviceName";
    roles[DevicePathRole] = "itemDevicePath";
    roles[NameRole] = "itemName";
    roles[SecureRole] = "itemSecure";
    roles[SectionRole] = "itemSection";
    roles[SignalRole] = "itemSignal";
    roles[SsidRole] = "itemSsid";
    roles[SpecificPathRole] = "itemSpecificPath";
    roles[UuidRole] = "itemUuid";
    roles[TypeRole] = "itemType";
    setRoleNames(roles);

    connect(m_monitor, SIGNAL(addActiveConnection(QString)),
            SLOT(addActiveConnection(QString)));
    connect(m_monitor, SIGNAL(activeConnectionStateChanged(QString,NetworkManager::ActiveConnection::State)),
            SLOT(activeConnectionStateChanged(QString, NetworkManager::ActiveConnection::State)));
    connect(m_monitor, SIGNAL(addConnection(QString,QString)),
            SLOT(addConnection(QString,QString)));
    connect(m_monitor, SIGNAL(addVpnConnection(QString)),
            SLOT(addVpnConnection(QString)));
    connect(m_monitor, SIGNAL(connectionUpdated(QString)),
            SLOT(connectionUpdated(QString)));
    connect(m_monitor, SIGNAL(addWirelessNetwork(QString,QString)),
            SLOT(addWirelessNetwork(QString,QString)));
    connect(m_monitor, SIGNAL(removeActiveConnection(QString)),
            SLOT(removeActiveConnection(QString)));
    connect(m_monitor, SIGNAL(removeConnection(QString)),
            SLOT(removeConnection(QString)));
    connect(m_monitor, SIGNAL(removeConnectionsByDevice(QString)),
            SLOT(removeConnectionsByDevice(QString)));
    connect(m_monitor, SIGNAL(removeVpnConnections()),
            SLOT(removeVpnConnections()));
    connect(m_monitor, SIGNAL(removeWirelessNetwork(QString,QString)),
            SLOT(removeWirelessNetwork(QString,QString)));
    connect(m_monitor, SIGNAL(removeWirelessNetworks()),
            SLOT(removeWirelessNetworks()));

    m_monitor->init();
}

Model::~Model()
{
}

int Model::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_items.count();
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    if (row >= 0 && row < m_items.count()) {
        ModelItem * item = m_items.itemAt(row);

        switch (role) {
            case ConnectingRole:
                return item->connecting();
                break;
            case ConnectedRole:
                return item->connected();
                break;
            case ConnectionPathRole:
                return item->connectionPath();
                break;
            case ConnectionIconRole:
                return item->icon();
                break;
            case ConnectionDetailsRole:
                return item->details();
                break;
            case DeviceNameRole:
                return item->deviceName();
                break;
            case DevicePathRole:
                return item->devicePath();
                break;
            case NameRole:
                return item->name();
                break;
            case SecureRole:
                return item->secure();
                break;
            case SectionRole:
                return item->sectionType();
                break;
            case SignalRole:
                return item->signal();
                break;
            case SsidRole:
                return item->ssid();
                break;
            case SpecificPathRole:
                return item->specificPath();
                break;
            case UuidRole:
                return item->uuid();
                break;
            case TypeRole:
                return item->type();
                break;
            default:
                break;
        }
    }

    return QVariant();
}

void Model::setDetailKeys(const QStringList& keys)
{
    m_keys = keys;

    foreach (ModelItem * item, m_items.items()) {
        item->setDetailKeys(m_keys);
    }
}

void Model::addActiveConnection(const QString& active)
{
    bool found = false;

    NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(active);

    if (!activeConnection) {
        return;
    }

    if (!m_items.itemsByUuid(activeConnection->uuid()).isEmpty()) {
        found = true;
    }

    if (!found) {
        if (activeConnection->devices().isEmpty()) {
            return;
        }

        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(activeConnection->devices().first());
        NetworkManager::Settings::Connection::Ptr connection = activeConnection->connection();

        if (!device || !connection) {
            return;
        }

        addConnection(connection->path(), device->uni());
    }

    foreach (ModelItem * item, m_items.itemsByUuid(activeConnection->connection()->uuid())) {
        item->setActiveConnection(active);

        if (updateItem(item)) {
            NMModelDebug() << "Connection " << item->name() << " has been updated by active connection";
        }
    }
}

void Model::activeConnectionStateChanged(const QString& active, NetworkManager::ActiveConnection::State state)
{
    ModelItem * item = m_items.itemByActiveConnection(active);

    if (item) {
        item->updateActiveConnectionState(state);
        NMModelDebug() << "Active connection state changed in " << item->name();

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed";
        }
    }
}

void Model::addConnection(const QString& connection, const QString& device)
{
    ModelItem * item = new ModelItem(device);
    item->setConnection(connection);
    insertItem(item);
}

void Model::addVpnConnection(const QString& connection)
{
    ModelItem * item = new ModelItem();
    item->setConnection(connection);
    insertItem(item);
}

void Model::addWirelessNetwork(const QString& ssid, const QString& device)
{
    ModelItem * item = new ModelItem(device);
    item->setWirelessNetwork(ssid);
    insertItem(item);
}

void Model::connectionUpdated(const QString& connection)
{
    foreach (ModelItem * item, m_items.itemsByConnection(connection)) {
        item->setConnection(connection);
        NMModelDebug() << "Connection changed in " << item->name();

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed";
        }
    }
}

void Model::removeActiveConnection(const QString& active)
{
    ModelItem * item = m_items.itemByActiveConnection(active);

    if (item) {
        item->setActiveConnection(QString());
        NMModelDebug() << "Removed active connection from " << item->name();

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed";
        }
    }
}

void Model::removeConnection(const QString& connection)
{
    foreach (ModelItem * item, m_items.itemsByConnection(connection)) {
        QString name = item->name();
        item->setConnection(QString());

        /* We removed connection details, but this connection can be available
            as accesspoint, if not, we have to delete it */
        if (item->specificPath().isEmpty()) {
            if (removeItem(item)) {
                NMModelDebug() << "Connection " << name << " has been removed";
            }
        } else {
            if (updateItem(item)) {
                NMModelDebug() << "Connection " << name << " has been removed from known connections";
            }
        }
    }
}

void Model::removeConnectionsByDevice(const QString& device)
{
    foreach (ModelItem * item, m_items.itemsByDevice(device)) {
        QString name = item->name();
        if (removeItem(item)) {
            NMModelDebug() << "Connection " << name << " has been removed";
        }
    }
}

void Model::removeWirelessNetwork(const QString& ssid, const QString& device)
{
    foreach (ModelItem * item, m_items.itemsBySsid(ssid, device)) {
        if (removeItem(item)) {
            NMModelDebug() << "Wireless network " << ssid << " has been removed";
        }
    }
}

void Model::removeWirelessNetworks()
{
    foreach (ModelItem * item, m_items.itemsByType(NetworkManager::Settings::ConnectionSettings::Wireless)) {
        if (removeItem(item)) {
            NMModelDebug() << "Wireless network " << item->ssid() << " has been completely removed";
        }
    }
}

void Model::removeVpnConnections()
{
    foreach (ModelItem * item, m_items.itemsByType(NetworkManager::Settings::ConnectionSettings::Vpn)) {
        QString name = item->name();
        if (removeItem(item)) {
            NMModelDebug() << "VPN Connection " << name << " has been removed";
        }
    }
}

void Model::insertItem(ModelItem * item)
{
    bool found = false;
    bool updated = false;
    foreach (ModelItem * it, m_items.items()) {
        // Check for duplicity
        if (it->operator==(item)) {
            found = true;

            // Update info
            if (it->specificPath().isEmpty() && !item->specificPath().isEmpty()) {
                NMModelDebug() << "Connection " << it->name() << " has been updated by wireless network";
                it->setWirelessNetwork(item->ssid());
                updated = true;
            }

            if (it->connectionPath().isEmpty() && !item->connectionPath().isEmpty()) {
                NMModelDebug() << "Connection " << it->name() << " has been updated by connection";
                it->setConnection(item->connectionPath());
                updated = true;
            }

            if (updated) {
                int row  = m_items.indexOf(it);

                if (row >= 0) {
                    QModelIndex index = createIndex(row, 0);
                    dataChanged(index, index);
                }
            }

            break;
        }
    }
    // Item doesn't exist, let's add it
    if (!found) {
        int index = m_items.count();
        beginInsertRows(QModelIndex(), index, index);
        m_items.insertItem(item);
        item->setDetailKeys(m_keys);
        endInsertRows();
        NMModelDebug() << "Connection " << item->name() << " has been added";
    } else {
        delete item;
    }
}

bool Model::removeItem(ModelItem* item)
{
    int row  = m_items.indexOf(item);

    if (row >= 0) {
        beginRemoveRows(QModelIndex(), row, row);
        m_items.removeItem(item);
        item->deleteLater();
        endRemoveRows();
        return true;
    }

    return false;
}

bool Model::updateItem(ModelItem* item)
{
    int row = m_items.indexOf(item);

    if (row >= 0) {
        QModelIndex index = createIndex(row, 0);
        dataChanged(index, index);
        return true;
    }

    return false;
}
