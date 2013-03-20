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

#include "monitor.h"

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/wireddevice.h>
#include <QtNetworkManager/settings/802-11-wireless.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/settings/connection.h>

#include "debug.h"

Monitor::Monitor(QObject* parent):
    QObject(parent),
    m_wirelessInterfaces(QList<NetworkManager::WirelessNetworkInterfaceEnvironment*>()),
    m_devices(NetworkManager::networkInterfaces())
{
}

Monitor::~Monitor()
{
    qDeleteAll(m_wirelessInterfaces);
}

void Monitor::init()
{
    foreach (NetworkManager::Device * dev, m_devices) {
        addDevice(dev);
    }

    statusChanged(NetworkManager::status());

    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(changeActiveConnections()));
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

    foreach (NetworkManager::ActiveConnection * active, NetworkManager::activeConnections()) {
        NMMonitorSignalDebug() << "Emit signal addActiveConnection(" << active->connection()->id() << ")";
        Q_EMIT addActiveConnection(active);
    }
}

void Monitor::addAvailableConnectionsForDevice(NetworkManager::Device* device)
{
    foreach (NetworkManager::Settings::Connection * con, device->availableConnections()) {
        NMMonitorDebug() << "Available connection " << con->id() << " for device " << device->interfaceName();
        NMMonitorSignalDebug() << "Emit signal addConnection(" << con->id() << ")";
        Q_EMIT addConnection(con, device);
    }
}

void Monitor::addDevice(NetworkManager::Device* device)
{
    addAvailableConnectionsForDevice(device);

    if (device->type() == NetworkManager::Device::Ethernet) {
        NetworkManager::WiredDevice * wiredDev = static_cast<NetworkManager::WiredDevice*>(device);
        connect(wiredDev, SIGNAL(carrierChanged(bool)),
                SLOT(cablePlugged(bool)));

    } else if (device->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice * wifiDev = static_cast<NetworkManager::WirelessDevice*>(device);
        NetworkManager::WirelessNetworkInterfaceEnvironment * wifiEnv = new NetworkManager::WirelessNetworkInterfaceEnvironment(wifiDev);
        m_wirelessInterfaces << wifiEnv;

        foreach (const QString & network, wifiEnv->networks()) {
            NetworkManager::WirelessNetwork * wifiNetwork = wifiEnv->findNetwork(network);

            NMMonitorSignalDebug() << "Emit signal addWirelessNetwork(" << wifiNetwork->ssid() << ")";
            Q_EMIT addWirelessNetwork(wifiNetwork, wifiEnv->interface());
        }

        connect(wifiEnv, SIGNAL(networkAppeared(QString)),
                SLOT(wirelessNetworkAppeared(QString)));
        connect(wifiEnv, SIGNAL(networkDisappeared(QString)),
                SLOT(wirelessNetworkDisappeared(QString)));
    }
}

void Monitor::cablePlugged(bool plugged)
{
    NetworkManager::Device * device = qobject_cast<NetworkManager::Device*>(sender());

    if (plugged) {
        NMMonitorDebug() << "Cable plugged to " << device->interfaceName() ;
        addAvailableConnectionsForDevice(device);
    } else {
        NMMonitorDebug() << "Cable unplugged from " << device->interfaceName() ;
        NMMonitorSignalDebug() << "Emit signal removeConnectionsByDevice(" << device->interfaceName()  << ")";
        Q_EMIT removeConnectionsByDevice(device->udi());
    }
}

void Monitor::connectionAdded(const QString& connection)
{
    NetworkManager::Settings::Connection * newConnection = new NetworkManager::Settings::Connection(connection);

    foreach (NetworkManager::Device * dev, m_devices) {
        foreach (NetworkManager::Settings::Connection * con, dev->availableConnections()) {
            if (con->uuid() == newConnection->uuid()) {
                NMMonitorDebug() << "Connection " << con->id() << " added";
                NMMonitorSignalDebug() << "Emit signal addConnection(" << con->id() << ")";
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

void Monitor::changeActiveConnections()
{
    NMMonitorDebug() << "Active connections have been changed";
    foreach (NetworkManager::ActiveConnection * active, NetworkManager::activeConnections()) {
        NMMonitorSignalDebug() << "Emit signal addActiveConnection(" << active->connection()->id() << ")";
        Q_EMIT addActiveConnection(active);
    }
}

void Monitor::deviceAdded(const QString& device)
{
    NetworkManager::Device * dev = new NetworkManager::Device(device);
    m_devices << dev;
    addDevice(dev);

    NMMonitorDebug() << "Device " << dev->interfaceName()  << " added";
}

void Monitor::deviceRemoved(const QString& device)
{
    foreach (NetworkManager::Device * dev, m_devices) {
        if (dev->uni() == device) {
            NMMonitorDebug() << "Device " << dev->interfaceName() << " removed";
            NMMonitorSignalDebug() << "Emit signal removeConnectionsByDevice(" << dev->interfaceName()  << ")";
            Q_EMIT removeConnectionsByDevice(dev->udi());
            m_devices.removeOne(dev);
            delete dev;
            break;
        }
    }
}

void Monitor::statusChanged(NetworkManager::Status status)
{
    NMMonitorDebug() << "NetworkManager status changed to " << status;

    if (status == NetworkManager::Connected ||
        status == NetworkManager::ConnectedLinkLocal ||
        status == NetworkManager::ConnectedSiteOnly) {

        foreach (NetworkManager::Settings::Connection * con, NetworkManager::Settings::listConnections()) {
            NetworkManager::Settings::ConnectionSettings settings;
            settings.fromMap(con->settings());

            if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Vpn) {
                NMMonitorSignalDebug() << "Emit signal addVpnConnection(" << con->id() << ")";
                Q_EMIT addVpnConnection(con);
            }
        }

    } else {
        NMMonitorSignalDebug() << "Emit signal removeVpnConnections()";
        Q_EMIT removeVpnConnections();
    }
}

void Monitor::wirelessNetworkAppeared(const QString& ssid)
{
    NetworkManager::WirelessNetwork * network = 0;
    NetworkManager::WirelessNetworkInterfaceEnvironment * wirelessInterface = 0;

    foreach (NetworkManager::WirelessNetworkInterfaceEnvironment * env, m_wirelessInterfaces) {
        network = env->findNetwork(ssid);

        if (network) {
            wirelessInterface = env;
            break;
        }
    }

    NMMonitorDebug() << "Wireless network " << ssid << " appeared";
    NMMonitorSignalDebug() << "Emit signal addWirelessNetwork(" << ssid << ")";
    Q_EMIT addWirelessNetwork(network, wirelessInterface->interface());

    addAvailableConnectionsForDevice(wirelessInterface->interface());
}

void Monitor::wirelessNetworkDisappeared(const QString& ssid)
{
    NMMonitorDebug() << "Wireless network " << ssid << " disappeared";
    NMMonitorSignalDebug() << "Emit signal removeWirelessNetwork(" << ssid << ")";
    Q_EMIT removeWirelessNetwork(ssid);
}
