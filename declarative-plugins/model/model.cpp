/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#include <QtNetworkManager/wireddevice.h>
#include <QtNetworkManager/settings/802-11-wireless.h>

#include "debug.h"

Model::Model(QObject* parent):
    QAbstractListModel(parent),
    m_monitor(new Monitor(this)),
    m_connections(QList<ModelItem*>())
{
    QHash<int, QByteArray> roles = roleNames();
    roles[NameRole] = "name";
    roles[UuidRole] = "uuid";
    roles[TypeRole] = "type";
    roles[ConnectedRole] = "connected";
    roles[ConnectingRole] = "connecting";
    roles[SsidRole] = "ssid";
    roles[SignalRole] = "signal";
    roles[SecureRole] = "secure";
    roles[DeviceRole] = "device";
    roles[ConnectionPathRole] = "connectionPath";
    roles[DevicePathTypeRole] = "devicePath";
    roles[SpecificPathRole] = "specificPath";
    roles[ConnectionIconRole] = "connectionIcon";
    roles[ConnectionDetailInformationsRole] = "detailInformations";
    setRoleNames(roles);

    connect(m_monitor, SIGNAL(addWirelessNetwork(NetworkManager::WirelessNetwork*, NetworkManager::Device*)),
            SLOT(addWirelessNetwork(NetworkManager::WirelessNetwork*, NetworkManager::Device*)));
    connect(m_monitor, SIGNAL(addActiveConnection(NetworkManager::ActiveConnection*)),
            SLOT(addActiveConnection(NetworkManager::ActiveConnection*)));
    connect(m_monitor, SIGNAL(addConnection(NetworkManager::Settings::Connection*,NetworkManager::Device*)),
            SLOT(addConnection(NetworkManager::Settings::Connection*,NetworkManager::Device*)));
    connect(m_monitor, SIGNAL(removeWirelessNetwork(QString)),
            SLOT(removeWirelessNetwork(QString)));
    connect(m_monitor, SIGNAL(removeConnection(QString)),
            SLOT(removeConnection(QString)));
    connect(m_monitor, SIGNAL(removeConnectionsByDevice(QString)),
            SLOT(removeConnectionsByDevice(QString)));
    connect(m_monitor, SIGNAL(addVpnConnection(NetworkManager::Settings::Connection*)),
            SLOT(addVpnConnection(NetworkManager::Settings::Connection*)));
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
            case DeviceRole:
                return item->device()->udi();
                break;
            case ConnectionPathRole:
                return item->connectionPath();
                break;
            case DevicePathTypeRole:
                return item->devicePath();
                break;
            case SpecificPathRole:
                return item->specificPath();
                break;
            case ConnectionIconRole:
                return connectionIcon(item->type());
                break;
            case ConnectionDetailInformationsRole:
                return item->detailInformations();
                break;
            default:
                break;
        }
    }

    return QVariant();
}

void Model::addWirelessNetwork(NetworkManager::WirelessNetwork * network, NetworkManager::Device * device)
{
    ModelItem * item = new ModelItem();
    item->setWirelessNetwork(network);
    item->setDevice(device);

    insertItem(item);
}

void Model::addActiveConnection(NetworkManager::ActiveConnection* active)
{
    foreach (ModelItem * item, m_connections) {
        if (active->connection()->uuid() == item->uuid()) {
            item->setActiveConnection(active);
            int row = m_connections.indexOf(item);
            if (row >= 0) {
                QModelIndex modelIndex = createIndex(row, 0);
                dataChanged(modelIndex, modelIndex);
            }
            NMModelDebug() << "Connection " << item->name() << " has been updated by active connection";
            break;
        }
    }
}

void Model::addConnection(NetworkManager::Settings::Connection* connection, NetworkManager::Device* device)
{
    NetworkManager::Settings::ConnectionSettings * settings = new NetworkManager::Settings::ConnectionSettings();
    settings->fromMap(connection->settings());

    ModelItem * item = new ModelItem();
    item->setConnection(connection);
    item->setDevice(device);

    insertItem(item);
}

void Model::addVpnConnection(NetworkManager::Settings::Connection * connection)
{
    NetworkManager::Settings::ConnectionSettings * settings = new NetworkManager::Settings::ConnectionSettings();
    settings->fromMap(connection->settings());

    ModelItem * item = new ModelItem();
    item->setConnection(connection);

    NMModelDebug() << "VPN Connection " << settings->id() << " has been added";
    insertItem(item);
}

void Model::removeWirelessNetwork(const QString& ssid)
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->ssid() == ssid) {
            row  = m_connections.indexOf(item);
        }

        if (row >= 0) {
            beginRemoveRows(QModelIndex(), row, row);
            m_connections.removeOne(item);
            delete item;
            endRemoveRows();
            NMModelDebug() << "Wireless network " << ssid << " has been removed";
            break;
        }
    }
}

void Model::removeConnection(const QString& connection)
{
    foreach (ModelItem * item, m_connections) {
        if (item->connectionPath() == connection) {
            QString name = item->name();
            item->setConnection(0);

            /* We removed connection details, but this connection can be available
               as accesspoint, if not, we have to delete it */
            if (item->specificPath().isEmpty()) {
                int row  = m_connections.indexOf(item);

                if (row >= 0) {
                    beginRemoveRows(QModelIndex(), row, row);
                    m_connections.removeOne(item);
                    delete item;
                    endRemoveRows();
                    NMModelDebug() << "Connection " << name << " has been removed";
                }

            // Or it's still available so we have to update it
            } else {
                int row = m_connections.indexOf(item);
                if (row >= 0) {
                    QModelIndex modelIndex = createIndex(row, 0);
                    dataChanged(modelIndex, modelIndex);
                }
                NMModelDebug() << "Connection " << item->name() << " has been removed from known connections";
            }

            break;
        }
    }
}

void Model::removeConnectionsByDevice(const QString& udi)
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->deviceUdi() == udi) {
            row  = m_connections.indexOf(item);
        }

        if (row >= 0) {
            QString name = item->name();
            beginRemoveRows(QModelIndex(), row, row);
            m_connections.removeOne(item);
            delete item;
            endRemoveRows();
            NMModelDebug() << "Connection " << name << " has been removed";
            row = -1;
        }
    }
}

void Model::removeVpnConnections()
{
    int row = -1;

    foreach (ModelItem * item, m_connections) {
        if (item->type() == NetworkManager::Settings::ConnectionSettings::Vpn) {
            row  = m_connections.indexOf(item);
        }

        if (row >= 0) {
            beginRemoveRows(QModelIndex(), row, row);
            m_connections.removeOne(item);
            delete item;
            endRemoveRows();
            NMModelDebug() << "VPN Connection " << item->name() << " has been removed";
            row = -1;
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
            if (!it->wirelessNetwork() && item->wirelessNetwork()) {
                NMModelDebug() << "Connection " << it->name() << " has been updated by wireless network";
                it->setWirelessNetwork(item->wirelessNetwork());
            }
            if (!it->connection() && item->connection()) {
                NMModelDebug() << "Connection " << it->name() << " has been updated by connection";
                it->setConnection(item->connection());
            }

            int row = m_connections.indexOf(it);
            if (row >= 0) {
                QModelIndex modelIndex = createIndex(row, 0);
                dataChanged(modelIndex, modelIndex);
            }
            found = true;
            break;
        }
    }
    // Item doesn't exit, let's add it
    if (!found) {
        int index = m_connections.count();
        beginInsertRows(QModelIndex(), index, index);
        m_connections << item;
        endInsertRows();

        connect(item, SIGNAL(connectionChanged()), SLOT(onChanged()));
        connect(item, SIGNAL(signalChanged()), SLOT(onChanged()));
        connect(item, SIGNAL(stateChanged()), SLOT(onChanged()));
        connect(item, SIGNAL(accessPointChanged()), SLOT(onChanged()));

        NMModelDebug() << "Connection " << item->name() << " has been added";
    } else {
        delete item;
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

QString Model::connectionIcon(NetworkManager::Settings::ConnectionSettings::ConnectionType type) const
{
    switch (type) {
        case NetworkManager::Settings::ConnectionSettings::Adsl:
            break;
        case NetworkManager::Settings::ConnectionSettings::Bluetooth:
            break;
        case NetworkManager::Settings::ConnectionSettings::Bond:
            break;
        case NetworkManager::Settings::ConnectionSettings::Bridge:
            break;
        case NetworkManager::Settings::ConnectionSettings::Cdma:
            return QString("phone");
            break;
        case NetworkManager::Settings::ConnectionSettings::Gsm:
            return QString("phone");
            break;
        case NetworkManager::Settings::ConnectionSettings::Infiniband:
            break;
        case NetworkManager::Settings::ConnectionSettings::OLPCMesh:
            break;
        case NetworkManager::Settings::ConnectionSettings::Pppoe:
            break;
        case NetworkManager::Settings::ConnectionSettings::Vlan:
            break;
        case NetworkManager::Settings::ConnectionSettings::Vpn:
            return QString("secure-card");
            break;
        case NetworkManager::Settings::ConnectionSettings::Wimax:
            break;
        case NetworkManager::Settings::ConnectionSettings::Wired:
            return QString("network-wired");
            break;
        case NetworkManager::Settings::ConnectionSettings::Wireless:
            return QString("network-wireless");
            break;
        default:
            return QString();
            break;
    }

    return QString();
}
