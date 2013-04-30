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

#ifndef PLASMA_NM_MODEL_H
#define PLASMA_NM_MODEL_H

#include <QtCore/QAbstractListModel>

#include <NetworkManagerQt/manager.h>
#include <NetworkManagerQt/activeconnection.h>
#include <NetworkManagerQt/accesspoint.h>
#include <NetworkManagerQt/device.h>

class ModelItem;

#include "monitor.h"

class Model : public QAbstractListModel
{
Q_OBJECT
public:
    Q_ENUMS(Detail)
    enum ItemRole {ConnectingRole = Qt::UserRole + 1, ConnectedRole, ConnectionPathRole, ConnectionIconRole, ConnectionDetailsRole,
                   DeviceNameRole, DevicePathRole, NameRole, SecureRole, SectionRole, SignalRole, SsidRole, SpecificPathRole, UuidRole, TypeRole};

    enum Detail { None = 0,
                  ConnectionType = 1, ConnectionState = 2,
                  DeviceSystemName = 4, DeviceIpv4Address = 8, DeviceIpv6Address = 16, DeviceDriver = 32, DeviceMac = 64, DeviceSpeed = 128,
                  WirelessDeviceSignal = 256, WirelessDeviceSsid = 512, WirelessDeviceBssid = 1024, WirelessDeviceFrequency = 2048,
                  ModemOperator = 4096, ModemSignalQuality = 8192, ModemAccessTechnology = 16384, ModemAllowedMode = 32768,
                  BluetoothName = 65536, VpnBanner = 131072, VpnPlugin = 262144 };
    Q_DECLARE_FLAGS(Details, Detail)

    Model(QObject* parent = 0);
    virtual ~Model();

    int rowCount(const QModelIndex & parent) const;
    QVariant data(const QModelIndex & index, int role) const;

public Q_SLOTS:
    void setDetailFlags(int flags);

private Q_SLOTS:
    void onChanged();

    void addActiveConnection(const NetworkManager::ActiveConnection::Ptr & active);
    void addConnection(const NetworkManager::Settings::Connection::Ptr & connection, const NetworkManager::Device::Ptr &device);
    void addVpnConnection(const NetworkManager::Settings::Connection::Ptr & connection);
    void addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network, const NetworkManager::Device::Ptr &device);
    void removeConnection(const QString & connection);
    void removeConnectionsByDevice(const QString & device);
    void removeVpnConnections();
    void removeWirelessNetwork(const QString & ssid, const NetworkManager::Device::Ptr &device);
    void removeWirelessNetworks();
    bool updateItem(ModelItem * item, int index, bool removeDirectly = false);
private:
    Monitor * m_monitor;
    QList<ModelItem*> m_connections;
    Details m_flags;

    void insertItem(ModelItem * item);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Model::Details)

#endif // PLASMA_NM_MODEL_H
