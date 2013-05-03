/***************************************************************************
 *   Copyright (C) 2013 by Daniel Nicoletti <dantti12@gmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#ifndef AVAILABLECONNECTIONSMODEL_H
#define AVAILABLECONNECTIONSMODEL_H

#include <QStandardItemModel>
#include <NetworkManagerQt/device.h>
#include <NetworkManagerQt/wirelessnetwork.h>
#include <NetworkManagerQt/wimaxnsp.h>

class AvailableConnectionsModel : public QStandardItemModel
{
    Q_OBJECT
    Q_ENUMS(ConnectionRoles)
    Q_ENUMS(Kind)
public:
    enum ConnectionRoles {
        RoleConectionPath = Qt::UserRole + 1,
        RoleKind,
        RoleNetworkID,
        RoleMacAddress,
        RoleSignalStrength,
        RoleSecurity
    };

    enum Kind {
        Connection = 1 << 0,
        Network    = 1 << 1
    };
    Q_DECLARE_FLAGS(Kinds, Kind)

    explicit AvailableConnectionsModel(QObject *parent = 0);
    void setDevice(const NetworkManager::Device::Ptr &device);

private slots:
    void availableConnectionChanged();
    void connectionAdded(const QString &path);
    void connectionChanged();
    void connectionRemoved(const QString &path);
    void addConnection(const NetworkManager::Settings::Connection::Ptr &connection);
    void changeConnection(QStandardItem *stdItem, const NetworkManager::Settings::Connection::Ptr &connection);
    void addNetwork(const NetworkManager::WirelessNetwork::Ptr &network);
    void addNspNetwork(const NetworkManager::WimaxNsp::Ptr &nsp);

private:
    QStandardItem *findConnectionItem(const QString &path);
    QStandardItem *findNetworkItem(const QString &ssid);
    QStandardItem *findNspNetworkItem(const QString &name);

    NetworkManager::Device::Ptr m_device;
};

#endif // AVAILABLECONNECTIONSMODEL_H
