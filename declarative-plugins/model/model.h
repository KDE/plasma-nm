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

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Device>

class ModelItem;

#include "monitor.h"
#include "modelitems.h"

class Model : public QAbstractListModel
{
Q_OBJECT
public:
    enum ItemRole {ConnectingRole = Qt::UserRole + 1, ConnectedRole, ConnectionPathRole, ConnectionIconRole, ConnectionDetailsRole,
                   DeviceNameRole, DevicePathRole, NameRole, SecureRole, SecurityTypeRole, SecurityTypeStringRole, SectionRole, SignalRole, SsidRole, SpecificPathRole, UuidRole, TypeRole};

    explicit Model(QObject* parent = 0);
    virtual ~Model();

    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

public Q_SLOTS:
    void updateItems();

private Q_SLOTS:
    void addActiveConnection(const QString& active);
    void activeConnectionStateChanged(const QString& active, NetworkManager::ActiveConnection::State state);
    void addConnection(const QString& connection, const QString& device);
    void addVpnConnection(const QString& connection);
    void addWimaxNsp(const QString& nsp, const QString& device);
    void addWirelessNetwork(const QString& ssid, const QString& device);
    void connectionUpdated(const QString& connection);
    void modemPropertiesChanged(const QString& modem);
    void removeActiveConnection(const QString& active);
    void removeConnection(const QString& connection);
    void removeConnectionsByDevice(const QString& device);
    void removeVpnConnections();
    void removeWimaxNsp(const QString& nsp, const QString& device);
    void removeWimaxNsps();
    void removeWirelessNetwork(const QString& ssid,const QString& device);
    void removeWirelessNetworks();
    void wimaxNspSignalChanged(const QString& nsp, int strength);
    void wirelessNetworkSignalChanged(const QString& ssid, int strength);
    void wirelessNetworkApChanged(const QString& ssid, const QString& ap);
private:
    Monitor * m_monitor;
    ModelItems m_items;
    QStringList m_keys;

    void insertItem(ModelItem * item);
    bool removeItem(ModelItem * item);
    bool updateItem(ModelItem * item);
};

#endif // PLASMA_NM_MODEL_H
