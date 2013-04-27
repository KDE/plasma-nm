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

#include <NetworkManagerQt/settings.h>
#include <NetworkManagerQt/wireddevice.h>
#include <NetworkManagerQt/settings/802-11-wireless.h>

#include "model/modelwirelessitem.h"
#include "model/modelwireditem.h"
#include "model/modelvpnitem.h"
#include "model/modelmodemitem.h"
#include "model/modelbtitem.h"

#include "debug.h"

Model::Model(QObject* parent):
    QAbstractListModel(parent),
    m_monitor(new Monitor(this)),
    m_connections(QList<ModelItem*>())
{
    QHash<int, QByteArray> roles = roleNames();
    roles[NameRole] = "itemName";
    roles[UuidRole] = "itemUuid";
    roles[TypeRole] = "itemType";
    roles[ConnectedRole] = "itemConnected";
    roles[ConnectingRole] = "itemConnecting";
    roles[SsidRole] = "itemSsid";
    roles[SignalRole] = "itemSignal";
    roles[SecureRole] = "itemSecure";
    roles[DeviceRole] = "itemDevice";
    roles[ConnectionPathRole] = "itemConnectionPath";
    roles[DevicePathRole] = "itemDevicePaths";
    roles[ActiveDevicePathRole] = "itemActiveDevicePath";
    roles[SpecificParameterRole] = "itemSpecificParameter";
    roles[ConnectionIconRole] = "itemConnectionIcon";
    roles[ConnectionDetailInformationsRole] = "itemDetailInformations";
    roles[SectionRole] = "itemSection";
    setRoleNames(roles);

    connect(m_monitor, SIGNAL(addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr, NetworkManager::Device::Ptr)),
            SLOT(addWirelessNetwork(NetworkManager::WirelessNetwork::Ptr, NetworkManager::Device::Ptr)));
    connect(m_monitor, SIGNAL(addActiveConnection(NetworkManager::ActiveConnection::Ptr)),
            SLOT(addActiveConnection(NetworkManager::ActiveConnection::Ptr)));
    connect(m_monitor, SIGNAL(addConnection(NetworkManager::Settings::Connection::Ptr,NetworkManager::Device::Ptr)),
            SLOT(addConnection(NetworkManager::Settings::Connection::Ptr,NetworkManager::Device::Ptr)));
    connect(m_monitor, SIGNAL(removeWirelessNetwork(QString, NetworkManager::Device::Ptr)),
            SLOT(removeWirelessNetwork(QString, NetworkManager::Device::Ptr)));
    connect(m_monitor, SIGNAL(removeWirelessNetworks()),
            SLOT(removeWirelessNetworks()));
    connect(m_monitor, SIGNAL(removeConnection(QString)),
            SLOT(removeConnection(QString)));
    connect(m_monitor, SIGNAL(removeConnectionsByDevice(QString)),
            SLOT(removeConnectionsByDevice(QString)));
    connect(m_monitor, SIGNAL(addVpnConnection(NetworkManager::Settings::Connection::Ptr)),
            SLOT(addVpnConnection(NetworkManager::Settings::Connection::Ptr)));
    connect(m_monitor, SIGNAL(removeVpnConnections()),
            SLOT(removeVpnConnections()));

    m_monitor->init();
}

Model::~Model()
{
    qDeleteAll(m_connections);
}

int Model::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_connections.count();
}

QVariant Model::data(const QModelIndex& index, int role) const
{
    int row = index.row();

    if (row >= 0 && row < m_connections.count()) {
        ModelItem * item = m_connections[row];

        switch (role) {
            case NameRole:
                return item->name();
                break;
            case UuidRole:
                return item->uuid();
                break;
            case TypeRole:
                return item->type();
                break;
            case ConnectedRole:
                return item->connected();
                break;
            case ConnectingRole:
                return item->connecting();
                break;
            case SsidRole:
                return item->ssid();
                break;
            case SignalRole:
                return item->signal();
                break;
            case SecureRole:
                return item->secure();
                break;
            case ConnectionPathRole:
                return item->connectionPath();
                break;
            case DevicePathRole:
                return item->devicePaths();
                break;
            case ActiveDevicePathRole:
                return item->activeDevicePath();
                break;
            case SpecificParameterRole:
                return item->specificParameter();
                break;
            case ConnectionIconRole:
                return item->icon();
                break;
            case ConnectionDetailInformationsRole:
                return item->detailInformations();
                break;
            case SectionRole:
                return item->sectionType();
                break;
            default:
                break;
        }
    }

    return QVariant();
}

void Model::setDetailFlags(int flags)
{
    m_flags = Details(flags);

    foreach (ModelItem * item, m_connections) {
        item->setDetailFlags(m_flags);
    }
}

void Model::addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network, const NetworkManager::Device::Ptr &device)
{
    ModelWirelessItem * item = new ModelWirelessItem(device);
    item->addWirelessNetwork(network, device);

    insertItem(item);
}

void Model::addActiveConnection(const NetworkManager::ActiveConnection::Ptr & active)
{
    bool found = false;

    foreach (ModelItem * item, m_connections) {
        if (active->connection()->uuid() == item->uuid()) {
            found = true;
            break;
        }
    }

    if (!found) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
        NetworkManager::Settings::Connection::Ptr connection = active->connection();

        if (!device || !connection) {
            return;
        }

        addConnection(connection, device);
    }

    foreach (ModelItem * item, m_connections) {
        if (active->connection()->uuid() == item->uuid()) {
            item->setActiveConnection(active);
            int row = m_connections.indexOf(item);
            if (row >= 0) {
                QModelIndex modelIndex = createIndex(row, 0);
                dataChanged(modelIndex, modelIndex);
            }
            NMModelDebug() << "Connection " << item->name() << " has been updated by active connection";
            found = true;
            break;
        }
    }
}

void Model::addConnection(const NetworkManager::Settings::Connection::Ptr & connection, const NetworkManager::Device::Ptr &device)
{
    NetworkManager::Settings::ConnectionSettings::Ptr settings = connection->settings();

    if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
        ModelWirelessItem * item = new ModelWirelessItem(device);
        item->setConnection(connection);
        insertItem(item);
    } else if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Wired) {
        ModelWiredItem * item = new ModelWiredItem(device);
        item->setConnection(connection);
        insertItem(item);
    } else if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Gsm ||
               settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Cdma) {
        ModelModemItem * item = new ModelModemItem(device);
        item->setConnection(connection);
        insertItem(item);
    } else if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Bluetooth) {
        ModelBtItem * item = new ModelBtItem(device);
        item->setConnection(connection);
        insertItem(item);
    } else {
        ModelItem * item = new ModelItem(device);
        item->setConnection(connection);
        insertItem(item);
    }
}

void Model::addVpnConnection(const NetworkManager::Settings::Connection::Ptr & connection)
{
    ModelVpnItem * item = new ModelVpnItem();
    item->setConnection(connection);

    NMModelDebug() << "VPN Connection " << connection->name() << " has been added";
    insertItem(item);
}

void Model::removeWirelessNetwork(const QString& ssid, const NetworkManager::Device::Ptr & device)
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->ssid() == ssid) {
            row  = m_connections.indexOf(item);

            if (row >= 0) {
                if (item->type() == NetworkManager::Settings::ConnectionSettings::Wireless) {
                    ModelWirelessItem * wifiItem = qobject_cast<ModelWirelessItem*>(item);
                    if (wifiItem) {
                        wifiItem->removeWirelessNetwork(device);
                        bool removed = updateItem(item, row);

                        if (removed) {
                            NMModelDebug() << "Wireless network " << ssid << " has been completely removed";
                        } else {
                            NMModelDebug() << "Removed network from " << wifiItem->name() << " connection";
                        }
                    } else {
                        return;
                    }
                }
            }
        }
    }
}

void Model::removeWirelessNetworks()
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->type() == NetworkManager::Settings::ConnectionSettings::Wireless) {
            row  = m_connections.indexOf(item);

            if (row >= 0) {
                updateItem(item, row, true);
                NMModelDebug() << "Wireless network " << item->ssid() << " has been completely removed";
                row = -1;
            }
        }

    }
}

void Model::removeConnection(const QString& connection)
{
    foreach (ModelItem * item, m_connections) {
        if (item->connectionPath() == connection) {
            QString name = item->name();
            item->setConnection(NetworkManager::Settings::Connection::Ptr());

            /* We removed connection details, but this connection can be available
               as accesspoint, if not, we have to delete it */
            int row  = m_connections.indexOf(item);

            if (row >= 0 ) {
                bool removed = updateItem(item, row);

                if (removed) {
                    NMModelDebug() << "Connection " << item->name() << " has been removed from known connections";
                } else {
                    NMModelDebug() << "Connection " << name << " has been removed";
                }

                break;
            }
        }
    }
}

void Model::removeConnectionsByDevice(const QString& device)
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->devicePaths().contains(device)) {
            item->removeDevice(device);
            row  = m_connections.indexOf(item);

            if (row >= 0) {
                QString name = item->name();
                bool removed = updateItem(item, row);
                if (removed) {
                    NMModelDebug() << "Connection " << name << " has been removed";
                } else {
                    NMModelDebug() << "Device was removed from " << name;
                }
            }
        }
    }
}

void Model::removeVpnConnections()
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->type() == NetworkManager::Settings::ConnectionSettings::Vpn) {
            row  = m_connections.indexOf(item);

            if (row >= 0) {
                updateItem(item, row, true);
                NMModelDebug() << "VPN Connection " << item->name() << " has been removed";
                row = -1;
            }
        }
    }
}

void Model::insertItem(ModelItem* item)
{
    bool found = false;

    foreach (ModelItem * it, m_connections) {
        // Check for duplicity
        if ((!it->uuid().isEmpty() && !item->uuid().isEmpty() && it->uuid() == item->uuid()) ||
            (!it->name().isEmpty() && !item->name().isEmpty() && it->name() == item->name()) ||
            (!it->ssid().isEmpty() && !item->ssid().isEmpty() && it->ssid() == item->ssid())) {
            // Update info
            if (it->type() == NetworkManager::Settings::ConnectionSettings::Wireless &&
                item->type() == NetworkManager::Settings::ConnectionSettings::Wireless) {
                ModelWirelessItem * wifiIt = qobject_cast<ModelWirelessItem*>(it);
                ModelWirelessItem * wifiItem = qobject_cast<ModelWirelessItem*>(item);
                if (wifiIt && wifiItem) {
                    foreach (const QString & key, wifiItem->wirelessNetworks().keys()) {
                        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(key);
                        if (device) {
                            NMModelDebug() << "Adding " << wifiItem->ssid() << " to " << wifiIt->name();
                            wifiIt->addWirelessNetwork(wifiItem->wirelessNetworks().value(key), device);
                        }
                    }
                }
            }

            if (!it->connection() && item->connection()) {
                NMModelDebug() << "Connection " << it->name() << " has been updated by connection";
                it->setConnection(item->connection());
            }

            foreach (const QString & device, item->devicePaths()) {
                NMModelDebug() << "Adding device " << device << " to " << it->name();
                it->addDevice(device);
            }

            found = true;
            break;
        }
    }
    // Item doesn't exist, let's add it
    if (!found) {
        int index = m_connections.count();
        beginInsertRows(QModelIndex(), index, index);
        m_connections << item;
        item->setDetailFlags(m_flags);
        endInsertRows();
        connect(item, SIGNAL(itemChanged()), SLOT(onChanged()));
        NMModelDebug() << "Connection " << item->name() << " has been added";
    } else {
        delete item;
    }
}

bool Model::updateItem(ModelItem* item, int index, bool removeDirectly)
{
    if (item->shouldBeRemoved() || removeDirectly) {
        beginRemoveRows(QModelIndex(), index, index);
        m_connections.removeOne(item);
        item->deleteLater();
        endRemoveRows();
        return true;
    } else {
        QModelIndex modelIndex = createIndex(index, 0);
        dataChanged(modelIndex, modelIndex);
        return false;
    }
}

void Model::onChanged()
{
    ModelItem * item = qobject_cast<ModelItem*>(sender());

    int row = m_connections.indexOf(item);
    if (row >= 0) {
        QModelIndex index = createIndex(row, 0);
        dataChanged(index, index);
        NMModelDebug() << "Item " << item->name() << " has been changed";
    }
}
