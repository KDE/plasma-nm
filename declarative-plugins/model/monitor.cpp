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
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WimaxDevice>

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
            SLOT(activeConnectionAdded(QString)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionRemoved(QString)),
            SLOT(activeConnectionRemoved(QString)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            SLOT(deviceAdded(QString)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            SLOT(deviceRemoved(QString)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)), Qt::UniqueConnection);
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionAdded(QString)),
            SLOT(connectionAdded(QString)), Qt::UniqueConnection);
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionRemoved(QString)),
            SLOT(connectionRemoved(QString)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(wimaxEnabledChanged(bool)),
            SLOT(wimaxEnabled(bool)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(wimaxHardwareEnabledChanged(bool)),
            SLOT(wimaxEnabled(bool)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
            SLOT(wirelessEnabled(bool)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
            SLOT(wirelessEnabled(bool)), Qt::UniqueConnection);
    connect(NetworkManager::notifier(), SIGNAL(serviceAppeared()),
            SLOT(init()), Qt::UniqueConnection);

    foreach (const NetworkManager::ActiveConnection::Ptr& active, NetworkManager::activeConnections()) {
        connect(active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
        NMMonitorDebug() << "Available active connection (" << active->connection()->name() << ")";

        Q_EMIT addActiveConnection(active->path());
    }
}

void Monitor::addAvailableConnectionsForDevice(const NetworkManager::Device::Ptr& device)
{
    foreach (const NetworkManager::Connection::Ptr& con, device->availableConnections()) {
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
                SLOT(cablePlugged(bool)), Qt::UniqueConnection);

    }  else if (device->type() == NetworkManager::Device::Wimax) {
        NMMonitorDebug() << "Available wimax device " << device->interfaceName();
        NetworkManager::WimaxDevice::Ptr wimaxDev = device.objectCast<NetworkManager::WimaxDevice>();

        foreach (const QString & nsp, wimaxDev->nsps()) {
            NetworkManager::WimaxNsp::Ptr wimaxNsp = wimaxDev->findNsp(nsp);

            if (wimaxNsp) {
                connect(wimaxNsp.data(), SIGNAL(signalQualityChanged(uint)),
                        SLOT(wimaxNspSignalChanged(uint)), Qt::UniqueConnection);
                NMMonitorDebug() << "Available wimax nsp " << wimaxNsp->name() << " for device " << device->interfaceName();
                Q_EMIT addNspNetwork(wimaxNsp->name(), wimaxDev->uni());
            }
        }

        connect(wimaxDev.data(), SIGNAL(nspAppeared(QString)),
                SLOT(wimaxNspAppeared(QString)), Qt::UniqueConnection);
        connect(wimaxDev.data(), SIGNAL(nspDisappeared(QString)),
                SLOT(wimaxNspDisappeared(QString)));
    } else if (device->type() == NetworkManager::Device::Wifi) {
        NMMonitorDebug() << "Available wireless device " << device->interfaceName();
        NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();

        foreach (const NetworkManager::WirelessNetwork::Ptr& wifiNetwork, wifiDev->networks()) {
            connect(wifiNetwork.data(), SIGNAL(signalStrengthChanged(int)),
                    SLOT(wirelessNetworkSignalChanged(int)), Qt::UniqueConnection);
            connect(wifiNetwork.data(), SIGNAL(referenceAccessPointChanged(QString)),
                    SLOT(wirelessNetworkReferenceApChanged(QString)), Qt::UniqueConnection);
            NMMonitorDebug() << "Available wireless network " << wifiNetwork->ssid() << " for device " << device->interfaceName();
            Q_EMIT addWirelessNetwork(wifiNetwork->ssid(), wifiDev->uni());
        }

        connect(wifiDev.data(), SIGNAL(networkAppeared(QString)),
                SLOT(wirelessNetworkAppeared(QString)), Qt::UniqueConnection);
        connect(wifiDev.data(), SIGNAL(networkDisappeared(QString)),
                SLOT(wirelessNetworkDisappeared(QString)), Qt::UniqueConnection);
    }else if (device->type() == NetworkManager::Device::Modem) {
        NMMonitorDebug() << "Available modem device " << device->interfaceName();
        NetworkManager::ModemDevice::Ptr modemDev = device.objectCast<NetworkManager::ModemDevice>();

        ModemManager::ModemGsmNetworkInterface::Ptr modemNetwork = modemDev->getModemNetworkIface().objectCast<ModemManager::ModemGsmNetworkInterface>();
        if (modemNetwork) {
            connect(modemNetwork.data(), SIGNAL(signalQualityChanged(uint)),
                    SIGNAL(gsmNetworkSignalQualityChanged(uint)), Qt::UniqueConnection);
            connect(modemNetwork.data(), SIGNAL(accessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)),
                    SLOT(gsmNetworkAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)), Qt::UniqueConnection);
            connect(modemNetwork.data(), SIGNAL(allowedModeChanged(ModemManager::ModemInterface::AllowedMode)),
                    SLOT(gsmNetworkAllowedModeChanged(ModemManager::ModemInterface::AllowedMode)), Qt::UniqueConnection);
        }
    }

    connect(device.data(), SIGNAL(availableConnectionAppeared(QString)),
            SLOT(availableConnectionAppeared(QString)), Qt::UniqueConnection);
    connect(device.data(), SIGNAL(availableConnectionDisappeared(QString)),
            SLOT(availableConnectionDisappeared(QString)), Qt::UniqueConnection);

    addAvailableConnectionsForDevice(device);
}

void Monitor::availableConnectionAppeared(const QString& connection)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    if (!device) {
        NMMonitorDebug() << "Device not found for connection " << connection;
        return;
    }

    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    if (!con) {
        NMMonitorDebug() << "Connection not found" << con->name();
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

    /* Just a prevention, the problem is when you get activeConenctionAdded() signal when NM starts (for example when you restart it)
     * you never get this active connection from libnm-qt, because it's not valid due to missing connection property. But it still does
     * not work properly, because the active connection is not added, and won't be added later */
    if (activeConnection) {
        connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
        NMMonitorDebug() << "Active connection " << activeConnection->connection()->name() << " added";
        Q_EMIT addActiveConnection(active);
    }
}

void Monitor::activeConnectionRemoved(const QString& active)
{
    NMMonitorDebug() << "Active connection " << active << "removed";
    Q_EMIT removeActiveConnection(active);
}

void Monitor::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    NetworkManager::ActiveConnection *activePtr = qobject_cast<NetworkManager::ActiveConnection*>(sender());
    NetworkManager::ActiveConnection::Ptr active;
    if (activePtr) {
        active = NetworkManager::findActiveConnection(activePtr->path());
    }

    if (active) {
        Q_EMIT activeConnectionStateChanged(active->path(), state);
    }
}

void Monitor::cablePlugged(bool plugged)
{
    NetworkManager::Device *devicePtr = qobject_cast<NetworkManager::Device*>(sender());
    NetworkManager::Device::Ptr device;
    if (devicePtr) {
        device = NetworkManager::findNetworkInterface(devicePtr->uni());
    }

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
    NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnection(connection);

    if (!newConnection) {
        NMMonitorDebug() << "The new connection has been added, but it was not found";
        return;
    }

    connect(newConnection.data(), SIGNAL(updated()), SLOT(connectionUpdated()), Qt::UniqueConnection);

    if (newConnection->settings()->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
        if (NetworkManager::status() == NetworkManager::Connected ||
            NetworkManager::status() == NetworkManager::ConnectedLinkLocal ||
            NetworkManager::status() == NetworkManager::ConnectedSiteOnly) {
            NMMonitorDebug() << "VPN connection " << newConnection->name() << " added";
            Q_EMIT addVpnConnection(newConnection->path());
            return;
        }
    }

    foreach (const NetworkManager::Device::Ptr& dev, NetworkManager::networkInterfaces()) {
        foreach (const NetworkManager::Connection::Ptr& con, dev->availableConnections()) {
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
    NetworkManager::Connection * connectionPtr = qobject_cast<NetworkManager::Connection*>(sender());
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(connectionPtr->path());

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

void Monitor::gsmNetworkAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology technology)
{
    ModemManager::ModemGsmNetworkInterface * gsmNetwork = qobject_cast<ModemManager::ModemGsmNetworkInterface*>(sender());

    if (gsmNetwork) {
        NMMonitorDebug() << "Modem " << gsmNetwork->device() << " access technology changed to " << technology;
        Q_EMIT modemAccessTechnologyChanged(gsmNetwork->device());
    }
}

void Monitor::gsmNetworkAllowedModeChanged(ModemManager::ModemInterface::AllowedMode mode)
{
    ModemManager::ModemGsmNetworkInterface * gsmNetwork = qobject_cast<ModemManager::ModemGsmNetworkInterface*>(sender());

    if (gsmNetwork) {
        NMMonitorDebug() << "Modem " << gsmNetwork->device() << " allowed mode changed to " << mode;
        Q_EMIT modemAllowedModeChanged(gsmNetwork->device());
    }
}

void Monitor::gsmNetworkSignalQualityChanged(uint signal)
{
    ModemManager::ModemGsmNetworkInterface * gsmNetwork = qobject_cast<ModemManager::ModemGsmNetworkInterface*>(sender());

    if (gsmNetwork) {
        NMMonitorDebug() << "Modem " << gsmNetwork->device() << " signal changed to " << signal;
        Q_EMIT modemSignalQualityChanged(gsmNetwork->device());
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
        foreach (const NetworkManager::Connection::Ptr& con, NetworkManager::listConnections()) {
            NetworkManager::ConnectionSettings::Ptr settings = con->settings();

            if (settings->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
                Q_EMIT addVpnConnection(con->path());
            }
        }
    // We have to remove all vpn connections
    } else {
        NMMonitorDebug() << "NetworkManager is not connected";
        Q_EMIT removeVpnConnections();
    }
}

void Monitor::wimaxEnabled(bool enabled)
{
    if (!enabled) {
        NMMonitorDebug() << "Wimax disabled, removing all wimax networks";
        Q_EMIT removeWimaxNsps();
    }
}

void Monitor::wimaxNspAppeared(const QString& nsp)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    if (!device) {
        return;
    }
    NetworkManager::WimaxDevice::Ptr wimaxDevice = device.objectCast<NetworkManager::WimaxDevice>();
    if (!wimaxDevice) {
        return;
    }
    NetworkManager::WimaxNsp::Ptr wimaxNsp = wimaxDevice->findNsp(nsp);
    if (!wimaxNsp) {
        return;
    }

    connect(wimaxNsp.data(), SIGNAL(signalQualityChanged(uint)),
            SLOT(wimaxNspSignalChanged(uint)), Qt::UniqueConnection);

    NMMonitorDebug() << "Wimax nsp " << wimaxNsp->uni() << " appeared";
    Q_EMIT addWimaxNsp(wimaxNsp->uni(), wimaxDevice->uni());
}

void Monitor::wimaxNspDisappeared(const QString& nsp)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    if (device) {
        NMMonitorDebug() << "Wimax nsp " << nsp << " disappeared";
        Q_EMIT removeWimaxNsp(nsp, device->uni());
    }
}

void Monitor::wimaxNspSignalChanged(uint strength)
{
    NetworkManager::WimaxNsp * wimaxNsp = qobject_cast<NetworkManager::WimaxNsp*>(sender());

    if (wimaxNsp) {
        NMMonitorDebug() << "Wimax nsp " << wimaxNsp->name() << " signal strength changed to " << strength;
        Q_EMIT wimaxNspSignalChanged(wimaxNsp->name(), strength);
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

    connect(network.data(), SIGNAL(signalStrengthChanged(int)),
            SLOT(wirelessNetworkSignalChanged(int)), Qt::UniqueConnection);
    connect(network.data(), SIGNAL(referenceAccessPointChanged(QString)),
            SLOT(wirelessNetworkReferenceApChanged(QString)), Qt::UniqueConnection);

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

void Monitor::wirelessNetworkSignalChanged(int strength)
{
    NetworkManager::WirelessNetwork * networkPtr = qobject_cast<NetworkManager::WirelessNetwork*>(sender());

    if (networkPtr) {
        NMMonitorDebug() << "Wireless network " << networkPtr->ssid() << " signal strength changed to " << strength;
        Q_EMIT wirelessNetworkSignalChanged(networkPtr->ssid(), strength);
    }
}

void Monitor::wirelessNetworkReferenceApChanged(const QString& ap)
{
    NetworkManager::WirelessNetwork * networkPtr = qobject_cast<NetworkManager::WirelessNetwork*>(sender());

    if (networkPtr) {
        NMMonitorDebug() << "Wireless network " << networkPtr->ssid() << " ap changed to " << ap;
        Q_EMIT wirelessNetworkAccessPointChanged(networkPtr->ssid(), ap);
    }
}

void Monitor::wirelessEnabled(bool enabled)
{
    if (!enabled) {
        NMMonitorDebug() << "Wireless disabled, removing all wireless networks";
        Q_EMIT removeWirelessNetworks();
    }
}
