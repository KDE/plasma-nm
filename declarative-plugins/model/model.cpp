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
#include <NetworkManagerQt/WirelessSetting>

#include "debug.h"

Model::Model(QObject* parent):
    QAbstractListModel(parent),
    m_monitor(new Monitor(this))
{
    QHash<int, QByteArray> roles = roleNames();
    roles[ConnectionStateRole] = "itemConnectionState";
    roles[ConnectionPathRole] = "itemConnectionPath";
    roles[ConnectionIconRole] = "itemConnectionIcon";
    roles[ConnectionDetailsRole] = "itemDetails";
    roles[DeviceNameRole] = "itemDeviceName";
    roles[DevicePathRole] = "itemDevicePath";
    roles[NameRole] = "itemName";
    roles[SecurityTypeRole] = "itemSecurityType";
    roles[SectionRole] = "itemSection";
    roles[SignalRole] = "itemSignal";
    roles[SsidRole] = "itemSsid";
    roles[SpecificPathRole] = "itemSpecificPath";
    roles[UuidRole] = "itemUuid";
    roles[UniRole] = "itemUni";
    roles[TypeRole] = "itemType";
    setRoleNames(roles);

    connect(m_monitor, SIGNAL(addActiveConnection(QString)),
            SLOT(addActiveConnection(QString)));
    connect(m_monitor, SIGNAL(activeConnectionStateChanged(QString,NetworkManager::ActiveConnection::State)),
            SLOT(activeConnectionStateChanged(QString,NetworkManager::ActiveConnection::State)));
    connect(m_monitor, SIGNAL(addConnection(QString,QString)),
            SLOT(addConnection(QString,QString)));
    connect(m_monitor, SIGNAL(addVpnConnection(QString)),
            SLOT(addVpnConnection(QString)));
    connect(m_monitor, SIGNAL(connectionUpdated(QString)),
            SLOT(connectionUpdated(QString)));
    connect(m_monitor, SIGNAL(addWimaxNsp(QString,QString)),
            SLOT(addWimaxNsp(QString,QString)));
    connect(m_monitor, SIGNAL(addWirelessNetwork(QString,QString)),
            SLOT(addWirelessNetwork(QString,QString)));
#if WITH_MODEMMANAGER_SUPPORT
    connect(m_monitor, SIGNAL(modemAccessTechnologyChanged(QString)),
            SLOT(modemPropertiesChanged(QString)));
    connect(m_monitor, SIGNAL(modemAllowedModeChanged(QString)),
            SLOT(modemPropertiesChanged(QString)));
    connect(m_monitor, SIGNAL(modemSignalQualityChanged(uint, QString)),
            SLOT(modemSignalQualityChanged(uint, QString)));
#endif
    connect(m_monitor, SIGNAL(removeActiveConnection(QString)),
            SLOT(removeActiveConnection(QString)));
    connect(m_monitor, SIGNAL(removeAvailableConnection(QString,QString)),
            SLOT(removeAvailableConnection(QString,QString)));
    connect(m_monitor, SIGNAL(removeConnection(QString)),
            SLOT(removeConnection(QString)));
    connect(m_monitor, SIGNAL(removeConnectionsByDevice(QString)),
            SLOT(removeConnectionsByDevice(QString)));
    connect(m_monitor, SIGNAL(removeVpnConnections()),
            SLOT(removeVpnConnections()));
    connect(m_monitor, SIGNAL(removeWimaxNsp(QString,QString)),
            SLOT(removeWimaxNsp(QString,QString)));
    connect(m_monitor, SIGNAL(removeWimaxNsps()),
            SLOT(removeWimaxNsps()));
    connect(m_monitor, SIGNAL(removeWirelessNetwork(QString,QString)),
            SLOT(removeWirelessNetwork(QString,QString)));
    connect(m_monitor, SIGNAL(removeWirelessNetworks()),
            SLOT(removeWirelessNetworks()));
    connect(m_monitor, SIGNAL(wimaxNspSignalChanged(QString,int)),
            SLOT(wimaxNspSignalChanged(QString,int)));
    connect(m_monitor, SIGNAL(wirelessNetworkSignalChanged(QString,int)),
            SLOT(wirelessNetworkSignalChanged(QString,int)));
    connect(m_monitor, SIGNAL(wirelessNetworkAccessPointChanged(QString,QString)),
            SLOT(wirelessNetworkApChanged(QString,QString)));

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
    const int row = index.row();

    if (row >= 0 && row < m_items.count()) {
        ModelItem * item = m_items.itemAt(row);

        switch (role) {
            case ConnectionStateRole:
                return item->connectionState();
            case ConnectionPathRole:
                return item->connectionPath();
            case ConnectionIconRole:
                return item->icon();
            case ConnectionDetailsRole:
                return item->details();
            case DeviceNameRole:
                return item->deviceName();
            case DevicePathRole:
                return item->devicePath();
            case NameRole:
                if (m_items.itemsByName(item->name()).count() > 1) {
                    return item->originalName();
                } else {
                    return item->name();
                }
            case SecurityTypeRole:
                return item->securityType();
            case SectionRole:
                return item->sectionType();
            case SignalRole:
                return item->signal();
            case SsidRole:
                return item->ssid();
            case SpecificPathRole:
                return item->specificPath();
            case UuidRole:
                return item->uuid();
            case UniRole:
                return item->uni();
            case TypeRole:
                return item->type();
            default:
                break;
        }
    }

    return QVariant();
}

void Model::updateItems()
{
    foreach (ModelItem * item, m_items.items()) {
        item->updateDetails();
        int row = m_items.indexOf(item);
        if (row >= 0) {
            QModelIndex index = createIndex(row, 0);
            emit dataChanged(index, index);
        }
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
        NetworkManager::Connection::Ptr connection = activeConnection->connection();

        if (!device || !connection) {
            return;
        }

        addConnection(connection->path(), device->uni());
    }

    foreach (ModelItem * item, m_items.itemsByUuid(activeConnection->connection()->uuid())) {
        if ((!activeConnection->devices().isEmpty() && activeConnection->devices().first() == item->devicePath()) ||
            (item->type() == NetworkManager::ConnectionSettings::Vpn)) {
            item->setActiveConnection(active);

            if (updateItem(item)) {
                NMModelDebug() << "Connection " << item->name() << " has been changed (active connection added)";
            }
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
            NMModelDebug() << "Item " << item->name() << " has been changed (active connection updated)";
        }
    }
}

void Model::addConnection(const QString& connection, const QString& device)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (con->settings()->isSlave() || con->name().isEmpty() || con->uuid().isEmpty()) {
        return;
    }

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

void Model::addWimaxNsp(const QString& nsp, const QString& device)
{
    ModelItem * item = new ModelItem(device);
    item->setNsp(nsp);
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

        if (updateItem(item)) {
            NMModelDebug() << item->name() << ": Item has been changed (connection updated)";
        }
    }
}
#if WITH_MODEMMANAGER_SUPPORT
void Model::modemPropertiesChanged(const QString& modem)
{
    foreach (ModelItem * item, m_items.itemsByDevice(modem)) {
        item->updateDetails();

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed (modem properties updated)";
        }
    }
}

void Model::modemSignalQualityChanged(uint signal, const QString& modem)
{
    qDebug() << "Modem signal quality changed";
    foreach (ModelItem * item, m_items.itemsByDevice(modem)) {
        item->updateSignalStrenght(signal);

        if (updateItem(item)) {
            //NMModelDebug() << "Item " << item->name() << " has been changed (modem signal changed)";
        }
    }
}
#endif
void Model::removeActiveConnection(const QString& active)
{
    ModelItem * item = m_items.itemByActiveConnection(active);

    if (item) {
        item->setActiveConnection(QString());

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed (active connection removed)";
        }
    }
}

void Model::removeAvailableConnection(const QString& connection, const QString& device)
{
    foreach (ModelItem * item, m_items.itemsByConnection(connection)) {
        if (item->devicePath() == device) {
            const QString name = item->name();
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
}

void Model::removeConnection(const QString& connection)
{
    foreach (ModelItem * item, m_items.itemsByConnection(connection)) {
        const QString name = item->name();
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
        const QString name = item->name();
        if (removeItem(item)) {
            NMModelDebug() << "Connection " << name << " has been removed (device removed)";
        }
    }
}

void Model::removeVpnConnections()
{
    foreach (ModelItem * item, m_items.itemsByType(NetworkManager::ConnectionSettings::Vpn)) {
        const QString name = item->name();
        if (removeItem(item)) {
            NMModelDebug() << "VPN Connection " << name << " has been removed";
        }
    }
}

void Model::removeWimaxNsp(const QString& nsp, const QString& device)
{
    foreach (ModelItem * item, m_items.itemsByNsp(nsp, device)) {
        if (removeItem(item)) {
            NMModelDebug() << "Wimax nsp " << nsp << " has been removed";
        }
    }
}

void Model::removeWimaxNsps()
{
    foreach (ModelItem * item, m_items.itemsByType(NetworkManager::ConnectionSettings::Wimax)) {
        if (removeItem(item)) {
            NMModelDebug() << "Wimax nsp " << item->ssid() << " has been completely removed";
        }
    }
}

void Model::removeWirelessNetwork(const QString& ssid, const QString& device)
{
    foreach (ModelItem * item, m_items.itemsBySsid(ssid, device)) {
        NetworkManager::AccessPoint::Ptr accessPoint;
        NetworkManager::WirelessDevice::Ptr wirelessDevice = NetworkManager::findNetworkInterface(item->devicePath()).objectCast<NetworkManager::WirelessDevice>();

        if (wirelessDevice && (wirelessDevice->mode() == NetworkManager::WirelessDevice::Adhoc || wirelessDevice->mode() == NetworkManager::WirelessDevice::ApMode) &&
            NetworkManager::isWirelessEnabled() && NetworkManager::isWirelessHardwareEnabled()) {
            item->setWirelessNetwork(QString());
            if (updateItem(item)) {
                NMModelDebug() << "Wireless network " << ssid << " has been removed";
            }
        } else {
            if (removeItem(item)) {
                NMModelDebug() << "Wireless network " << ssid << " has been completely removed";
            }
        }
    }
}

void Model::removeWirelessNetworks()
{
    foreach (ModelItem * item, m_items.itemsByType(NetworkManager::ConnectionSettings::Wireless)) {
        if (removeItem(item)) {
            NMModelDebug() << "Wireless network " << item->ssid() << " has been completely removed";
        }
    }
}

void Model::wimaxNspSignalChanged(const QString& nsp, int strength)
{
    foreach (ModelItem * item, m_items.itemsByNsp(nsp)) {
        item->updateSignalStrenght(strength);

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed (wimax signal changed)";
        }
    }
}

void Model::wirelessNetworkSignalChanged(const QString& ssid, int strength)
{
    foreach (ModelItem * item, m_items.itemsBySsid(ssid)) {
        item->updateSignalStrenght(strength);

        if (updateItem(item)) {
            //NMModelDebug() << "Item " << item->name() << " has been changed (wireless signal changed)";
        }
    }
}

void Model::wirelessNetworkApChanged(const QString& ssid, const QString& ap)
{
    foreach (ModelItem * item, m_items.itemsBySsid(ssid)) {
        item->updateAccessPoint(ap);

        if (updateItem(item)) {
            NMModelDebug() << "Item " << item->name() << " has been changed (wireless accesspoint changed)";
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
                if (item->type() == NetworkManager::ConnectionSettings::Wireless) {
                    it->setWirelessNetwork(item->ssid());
                } else if (item->type() == NetworkManager::ConnectionSettings::Wimax) {
                    it->setNsp(item->nspPath());
                }
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
                    emit dataChanged(index, index);
                }
            }

            break;
        }
    }
    // Item doesn't exist, let's add it
    if (!found) {
        const int index = m_items.count();
        beginInsertRows(QModelIndex(), index, index);
        m_items.insertItem(item);
        endInsertRows();
        NMModelDebug() << "Connection " << item->name() << " has been added";
    } else {
        delete item;
    }
}

bool Model::removeItem(ModelItem* item)
{
    const int row = m_items.indexOf(item);

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
    const int row = m_items.indexOf(item);

    if (row >= 0) {
        QModelIndex index = createIndex(row, 0);
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
