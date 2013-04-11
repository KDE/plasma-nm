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

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/wireddevice.h>
#include <QtNetworkManager/settings/802-11-wireless.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/wirelessdevice.h>

#include "debug.h"

Monitor::Monitor(QObject* parent):
    QObject(parent),
    m_devices(NetworkManager::networkInterfaces())
{
}

Monitor::~Monitor()
{
}

void Monitor::init()
{
    foreach (const NetworkManager::Device::Ptr &dev, m_devices) {
        addDevice(dev);
    }

    statusChanged(NetworkManager::status());

    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));
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

    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        NMMonitorDebug() << "Available active connection (" << active.data()->connection()->name() << ")";
        NMMonitorSignalDebug() << "Emit signal addActiveConnection(" << active.data()->connection()->name() << ")";
        Q_EMIT addActiveConnection(active);
    }
}

void Monitor::addAvailableConnectionsForDevice(const NetworkManager::Device::Ptr &device)
{
    foreach (const NetworkManager::Settings::Connection::Ptr &con, device->availableConnections()) {
        NMMonitorDebug() << "Available connection " << con->name() << " for device " << device->interfaceName();
        NMMonitorSignalDebug() << "Emit signal addConnection(" << con->name() << ")";
        Q_EMIT addConnection(con, device);
    }
}

void Monitor::addDevice(const NetworkManager::Device::Ptr &device)
{
    if (device->type() == NetworkManager::Device::Ethernet) {
        NMMonitorDebug() << "Available wired device " << device->interfaceName();
        NetworkManager::WiredDevice::Ptr wiredDev = device.objectCast<NetworkManager::WiredDevice>();
        connect(wiredDev.data(), SIGNAL(carrierChanged(bool)),
                SLOT(cablePlugged(bool)));

    } else if (device->type() == NetworkManager::Device::Wifi) {
        NMMonitorDebug() << "Available wireless device " << device->interfaceName();
        NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
        m_wirelessDevices << wifiDev;

        foreach (const NetworkManager::WirelessNetwork::Ptr &wifiNetwork, wifiDev->networks()) {
            NMMonitorDebug() << "Available wireless network " << wifiNetwork << " for device " << device->interfaceName();
            NMMonitorSignalDebug() << "Emit signal addWirelessNetwork(" << wifiNetwork->ssid() << ")";
            Q_EMIT addWirelessNetwork(wifiNetwork, wifiDev);
        }

        connect(wifiDev.data(), SIGNAL(networkAppeared(QString)),
                SLOT(wirelessNetworkAppeared(QString)));
        connect(wifiDev.data(), SIGNAL(networkDisappeared(QString)),
                SLOT(wirelessNetworkDisappeared(QString)));
    }

    connect(device.data(), SIGNAL(availableConnectionChanged()),
            SLOT(availableConnectionsChanged()));

   addAvailableConnectionsForDevice(device);
}

void Monitor::availableConnectionsChanged()
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    if (!device) {
        return;
    }

    foreach (const NetworkManager::Settings::Connection::Ptr & connection, device->availableConnections()) {
       NMMonitorDebug() << "Connection " << connection->name() << " added";
       NMMonitorSignalDebug() << "Emit signal addConnection(" << connection->name() << ")";
       Q_EMIT addConnection(connection, device);
    }
}

void Monitor::activeConnectionsChanged()
{
    NMMonitorDebug() << "Active connections have been changed";
    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        NMMonitorSignalDebug() << "Emit signal addActiveConnection(" << active.data()->connection()->name() << ")";
        Q_EMIT addActiveConnection(active);
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
            NMMonitorSignalDebug() << "Emit signal removeConnectionsByDevice(" << device->interfaceName()  << ")";
            Q_EMIT removeConnectionsByDevice(device->udi());
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

    foreach (const NetworkManager::Device::Ptr &dev, m_devices) {
        foreach (const NetworkManager::Settings::Connection::Ptr &con, dev->availableConnections()) {
            qDebug() << con->name() << " == " << newConnection->name();
            if (con->uuid() == newConnection->uuid()) {
                NMMonitorDebug() << "Connection " << con->name() << " added";
                NMMonitorSignalDebug() << "Emit signal addConnection(" << con->name() << ")";
                Q_EMIT addConnection(con, dev);
            }
        }
    }
}

void Monitor::connectionRemoved(const QString& connection)
{
    NMMonitorDebug() << "Connection " << connection << " removed";
    NMMonitorSignalDebug() << "Emit signal removeConnection(" << connection << ")";
    Q_EMIT removeConnection(connection);
}

void Monitor::deviceAdded(const QString& device)
{
    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);

    if (dev) {
        m_devices << dev;

        NMMonitorDebug() << "Device " << dev->interfaceName()  << " added";
        addDevice(dev);
    }
}

void Monitor::deviceRemoved(const QString& device)
{
    foreach (const NetworkManager::Device::Ptr &dev, m_devices) {
        if (dev->uni() == device) {
            NMMonitorDebug() << "Device " << dev->interfaceName() << " removed";
            NMMonitorSignalDebug() << "Emit signal removeConnectionsByDevice(" << dev->interfaceName()  << ")";
            Q_EMIT removeConnectionsByDevice(dev->udi());
            m_devices.removeOne(dev);
            break;
        }
    }
}

void Monitor::statusChanged(NetworkManager::Status status)
{
    NMMonitorDebug() << "NetworkManager status changed to " << status;

    // We can add vpn connections
    if (status == NetworkManager::Connected ||
        status == NetworkManager::ConnectedLinkLocal ||
        status == NetworkManager::ConnectedSiteOnly) {
        NMMonitorDebug() << "NetworkManager is connected";
        foreach (const NetworkManager::Settings::Connection::Ptr &con, NetworkManager::Settings::listConnections()) {
            NetworkManager::Settings::ConnectionSettings::Ptr settings = con->settings();

            if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Vpn) {
                NMMonitorSignalDebug() << "Emit signal addVpnConnection(" << con->name() << ")";
                Q_EMIT addVpnConnection(con);
            }
        }

    // We have to remove all vpn connections
    } else {
        NMMonitorDebug() << "NetworkManager is not connected";
        NMMonitorSignalDebug() << "Emit signal removeVpnConnections()";
        Q_EMIT removeVpnConnections();
    }
}

void Monitor::wirelessNetworkAppeared(const QString& ssid)
{
    NetworkManager::WirelessNetwork::Ptr network;
    NetworkManager::WirelessDevice::Ptr wirelessDevice;

    foreach (const NetworkManager::WirelessDevice::Ptr &wifiDev, m_wirelessDevices) {
        network = wifiDev->findNetwork(ssid);
        if (network) {
            wirelessDevice = wifiDev;
            break;
        }
    }

    NMMonitorDebug() << "Wireless network " << ssid << " appeared";
    NMMonitorSignalDebug() << "Emit signal addWirelessNetwork(" << ssid << ")";
    Q_EMIT addWirelessNetwork(network, wirelessDevice);

    // Check if we have some known connection for this access point
    foreach (const NetworkManager::Device::Ptr &device, m_devices) {
        if (device->type() == NetworkManager::Device::Wifi) {
            foreach (const NetworkManager::Settings::Connection::Ptr &connection, device->availableConnections()) {
                NetworkManager::Settings::ConnectionSettings::Ptr settings = connection->settings();

                NetworkManager::Settings::WirelessSetting::Ptr wirelessSetting;
                wirelessSetting = settings->setting(NetworkManager::Settings::Setting::Wireless).dynamicCast<NetworkManager::Settings::WirelessSetting>();
                if (wirelessSetting->ssid() == ssid) {
                    NMMonitorDebug() << "Known connection for previusly added access point " << ssid;
                    NMMonitorSignalDebug() << "Emit signal addConnection(" << connection->name() << ")";
                    Q_EMIT addConnection(connection, wirelessDevice);
                }
            }
        }
    }
}

void Monitor::wirelessNetworkDisappeared(const QString& ssid)
{
    NMMonitorDebug() << "Wireless network " << ssid << " disappeared";
    NMMonitorSignalDebug() << "Emit signal removeWirelessNetwork(" << ssid << ")";
    Q_EMIT removeWirelessNetwork(ssid);
}
