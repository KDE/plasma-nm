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

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/activeconnection.h>
#include <QtNetworkManager/accesspoint.h>
#include <QtNetworkManager/device.h>

#include "modelitem.h"
#include "monitor.h"

class Model : public QAbstractListModel
{
Q_OBJECT
public:
    enum ConnectionRole {NameRole = Qt::UserRole + 1, UuidRole, TypeRole, ConnectedRole, ConnectingRole, SsidRole, SignalRole, SecureRole, DeviceRole,
                         ConnectionPathRole, DevicePathTypeRole, SpecificPathRole, ConnectionIconRole, ConnectionDetailInformationsRole, SectionRole/*, SectionStringRole*/};

    Model(QObject* parent = 0);
    virtual ~Model();

    int rowCount(const QModelIndex & parent) const;
    QVariant data(const QModelIndex & index, int role) const;

private Q_SLOTS:
    void onChanged();

    void addActiveConnection(const NetworkManager::ActiveConnection::Ptr & active);
    void addConnection(const NetworkManager::Settings::Connection::Ptr & connection, const NetworkManager::Device::Ptr &device);
    void addVpnConnection(const NetworkManager::Settings::Connection::Ptr & connection);
    void addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network, const NetworkManager::Device::Ptr &device);
    void removeConnection(const QString & connection);
    void removeConnectionsByDevice(const QString & udi);
    void removeVpnConnections();
    void removeWirelessNetwork(const QString & ssid);

private:
    Monitor * m_monitor;
    QList<ModelItem*> m_connections;

    void insertItem(ModelItem * item);
};


#endif // PLASMA_NM_MODEL_H
