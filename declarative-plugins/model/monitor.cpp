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

#include "monitor.h"

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/settings/Wireless>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/settings/Connection>
#include <NetworkManagerQt/WirelessDevice>

#include "debug.h"

Monitor::Monitor(QObject* parent):
    QObject(parent)
{
}

Monitor::~Monitor()
{
}

void Monitor::init()
{
    foreach (const NetworkManager::Device::Ptr& dev, NetworkManager::networkInterfaces()) {
        addDevice(dev);
    }

    statusChanged(NetworkManager::status());

    connect(NetworkManager::notifier(), SIGNAL(activeConnectionAdded(QString)),
            SLOT(activeConnectionAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionRemoved(QString)),
            SLOT(activeConnectionRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            SLOT(deviceRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)));
    connect(NetworkManager::Settings::notifier(), SIGNAL(connectionAdded(QString)),
            SLOT(connectionAdded(QString)));
    connect(NetworkManager::Settings::notifier(), SIGNAL(connectionRemoved(QString)),
            SLOT(connectionRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
            SLOT(wirelessEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
            SLOT(wirelessEnabled(bool)));

    foreach (const NetworkManager::ActiveConnection::Ptr& active, NetworkManager::activeConnections()) {
        connect(active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
        NMMonitorDebug() << "Available active connection (" << active->connection()->name() << ")";

        Q_EMIT addActiveConnection(active->path());
    }
}

void Monitor::addAvailableConnectionsForDevice(const NetworkManager::Device::Ptr& device)
{
    foreach (const NetworkManager::Settings::Connection::Ptr& con, device->availableConnections()) {
        connect(con.data(), SIGNAL(updated()), SLOT(connectionUpdated()), Qt::UniqueConnection);
        NMMonitorDebug() << "Available connection " << con->name() << " for device " << device->interfaceName();
        Q_EMIT addConnection(con->path(), device->uni());
    }
}

void Monitor::addDevice(const NetworkManager::Device::Ptr& device)
{
    // TODO add more devices with "carrier" - infiniband, bond, bridge, vlan, adsl
    if (device->type() == NetworkManager::Device::Ethernet) {
        NMMonitorDebug() << "Available wired device " << device->interfaceName();
        NetworkManager::WiredDevice::Ptr wiredDev = device.objectCast<NetworkManager::WiredDevice>();
        connect(wiredDev.data(), SIGNAL(carrierChanged(bool)),
                SLOT(cablePlugged(bool)));

    } else if (device->type() == NetworkManager::Device::Wifi) {
        NMMonitorDebug() << "Available wireless device " << device->interfaceName();
        NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();

        foreach (const NetworkManager::WirelessNetwork::Ptr& wifiNetwork, wifiDev->networks()) {
            NMMonitorDebug() << "Available wireless network " << wifiNetwork->ssid() << " for device " << device->interfaceName();
            Q_EMIT addWirelessNetwork(wifiNetwork->ssid(), wifiDev->uni());
        }

        connect(wifiDev.data(), SIGNAL(networkAppeared(QString)),
                SLOT(wirelessNetworkAppeared(QString)));
        connect(wifiDev.data(), SIGNAL(networkDisappeared(QString)),
                SLOT(wirelessNetworkDisappeared(QString)));
    }

    connect(device.data(), SIGNAL(availableConnectionAppeared(QString)),
            SLOT(availableConnectionAppeared(QString)));
    connect(device.data(), SIGNAL(availableConnectionDisappeared(QString)),
            SLOT(availableConnectionDisappeared(QString)));

   addAvailableConnectionsForDevice(device);
}

void Monitor::availableConnectionAppeared(const QString& connection)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    if (!device) {
        return;
    }

    NetworkManager::Settings::Connection::Ptr con = NetworkManager::Settings::findConnection(connection);
    if (!con) {
        return;
    }

    if (con->settings()->isSlave()) {
        return;
    }

    connect(con.data(), SIGNAL(updated()), SLOT(connectionUpdated()), Qt::UniqueConnection);

    NMMonitorDebug() << "New available connection " << con->name() << " for " << device->udi();
    Q_EMIT addConnection(con->path(), device->uni());
}

void Monitor::availableConnectionDisappeared(const QString& connection)
{
    NMMonitorDebug() << "Remove previously available connection " << connection;
    Q_EMIT removeConnection(connection);
}

void Monitor::activeConnectionAdded(const QString& active)
{
    NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(active);

    connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
            SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
    NMMonitorDebug() << "Active connection " << activeConnection->connection()->name() << " added";
    Q_EMIT addActiveConnection(active);
}

void Monitor::activeConnectionRemoved(const QString& active)
{
    NMMonitorDebug() << "Active connection " << active << "removed";
    Q_EMIT removeActiveConnection(active);
}

void Monitor::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    NetworkManager::ActiveConnection *activePtr = qobject_cast<NetworkManager::ActiveConnection*>(sender());
    NetworkManager::ActiveConnection::Ptr active = NetworkManager::findActiveConnection(activePtr->path());

    if (active) {
        Q_EMIT activeConnectionStateChanged(active->path(), state);
    }
}

void Monitor::cablePlugged(bool plugged)
{
    NetworkManager::Device *devicePtr = qobject_cast<NetworkManager::Device*>(sender());
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(devicePtr->uni());

    if (device) {
        if (plugged) {
            NMMonitorDebug() << "Cable plugged to " << device->interfaceName() ;
            addAvailableConnectionsForDevice(device);
        } else {
            NMMonitorDebug() << "Cable unplugged from " << device->interfaceName() ;
            Q_EMIT removeConnectionsByDevice(device->uni());
        }
    }
}

void Monitor::connectionAdded(const QString& connection)
{
    NetworkManager::Settings::Connection::Ptr newConnection = NetworkManager::Settings::findConnection(connection);

    if (!newConnection) {
        NMMonitorDebug() << "The new connection has been added, but it was not found";
        return;
    }

    connect(newConnection.data(), SIGNAL(updated()), SLOT(connectionUpdated()), Qt::UniqueConnection);

    if (newConnection->settings()->connectionType() == NetworkManager::Settings::ConnectionSettings::Vpn) {
        if (NetworkManager::status() == NetworkManager::Connected ||
            NetworkManager::status() == NetworkManager::ConnectedLinkLocal ||
            NetworkManager::status() == NetworkManager::ConnectedSiteOnly) {
            NMMonitorDebug() << "VPN connection " << newConnection->name() << " added";
            Q_EMIT addVpnConnection(newConnection->path());
            return;
        }
    }

    foreach (const NetworkManager::Device::Ptr& dev, NetworkManager::networkInterfaces()) {
        foreach (const NetworkManager::Settings::Connection::Ptr& con, dev->availableConnections()) {
            if (con->uuid() == newConnection->uuid()) {
                NMMonitorDebug() << "Connection " << con->name() << " added";
                Q_EMIT addConnection(con->path(), dev->uni());
            }
        }
    }
}

void Monitor::connectionRemoved(const QString& connection)
{
    NMMonitorDebug() << "Connection " << connection << " removed";
    Q_EMIT removeConnection(connection);
}

void Monitor::connectionUpdated()
{
    NetworkManager::Settings::Connection * connectionPtr = qobject_cast<NetworkManager::Settings::Connection*>(sender());
    NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnection(connectionPtr->path());

    NMMonitorDebug() << "Connection " << connection->name() << " updated";

    Q_EMIT connectionUpdated(connection->path());
}

void Monitor::deviceAdded(const QString& device)
{
    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);

    if (dev) {
        NMMonitorDebug() << "Device " << dev->interfaceName()  <<  dev->uni() << " added";
        addDevice(dev);
    }
}

void Monitor::deviceRemoved(const QString& device)
{
    NMMonitorDebug() << "Device " << device << " removed";
    Q_EMIT removeConnectionsByDevice(device);
}

void Monitor::statusChanged(NetworkManager::Status status)
{
    NMMonitorDebug() << "NetworkManager status changed to " << status;

    // We can add vpn connections
    if (status == NetworkManager::Connected ||
        status == NetworkManager::ConnectedLinkLocal ||
        status == NetworkManager::ConnectedSiteOnly) {
        NMMonitorDebug() << "NetworkManager is connected";
        foreach (const NetworkManager::Settings::Connection::Ptr& con, NetworkManager::Settings::listConnections()) {
            NetworkManager::Settings::ConnectionSettings::Ptr settings = con->settings();

            if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Vpn) {
                Q_EMIT addVpnConnection(con->path());
            }
        }

    // We have to remove all vpn connections
    } else {
        NMMonitorDebug() << "NetworkManager is not connected";
        Q_EMIT removeVpnConnections();
    }
}

void Monitor::wirelessNetworkAppeared(const QString& ssid)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    if (!device) {
        return;
    }
    NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
    if (!wirelessDevice) {
        return;
    }
    NetworkManager::WirelessNetwork::Ptr network = wirelessDevice->findNetwork(ssid);
    if (!network) {
        return;
    }

    NMMonitorDebug() << "Wireless network " << ssid << " appeared";
    Q_EMIT addWirelessNetwork(ssid, wirelessDevice->uni());
}

void Monitor::wirelessNetworkDisappeared(const QString& ssid)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    if (device) {
        NMMonitorDebug() << "Wireless network " << ssid << " disappeared";
        Q_EMIT removeWirelessNetwork(ssid, device->uni());
    }
}

void Monitor::wirelessEnabled(bool enabled)
{
    if (!enabled) {
        NMMonitorDebug() << "Wireless disabled, removing all wireless networks";
        Q_EMIT removeWirelessNetworks();
    }
}
