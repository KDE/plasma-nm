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

#ifndef PLASMA_NM_MONITOR_H
#define PLASMA_NM_MONITOR_H

#include <QtNetworkManager/activeconnection.h>
#include <QtNetworkManager/accesspoint.h>
#include <QtNetworkManager/device.h>
#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/wirelessnetworkinterfaceenvironment.h>

class Monitor : public QObject
{
Q_OBJECT
public:
    Monitor(QObject* parent = 0);
    virtual ~Monitor();

public Q_SLOTS:
    void init();

private Q_SLOTS:
    void activeConnectionsChanged();
    void cablePlugged(bool plugged);
    void connectionAdded(const QString & connection);
    void connectionRemoved(const QString & connection);
    void deviceAdded(const QString & device);
    void deviceRemoved(const QString & device);
    void statusChanged(NetworkManager::Status status);
    void wirelessNetworkAppeared(const QString & ssid);
    void wirelessNetworkDisappeared(const QString & ssid);
Q_SIGNALS:
    void addActiveConnection(const NetworkManager::ActiveConnection::Ptr & active);
    void addConnection(NetworkManager::Settings::Connection * connection, const NetworkManager::Device::Ptr &device);
    void addVpnConnection(NetworkManager::Settings::Connection * connection);
    void addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network, const NetworkManager::Device::Ptr &device);
    void removeWirelessNetwork(const QString & ssid);
    void removeConnectionsByDevice(const QString & udi);
    void removeConnection(const QString & connection);
    void removeVpnConnections();

private:
    QList<NetworkManager::WirelessNetworkInterfaceEnvironment*> m_wirelessInterfaces;
    NetworkManager::Device::List m_devices;

    void addAvailableConnectionsForDevice(const NetworkManager::Device::Ptr &device);
    void addDevice(const NetworkManager::Device::Ptr &device);
};

#endif // PLASMA_NM_MONITOR_H
