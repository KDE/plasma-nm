/***************************************************************************
 *   Copyright (C) 2012 by Daniel Nicoletti                                *
 *   dantti12@gmail.com                                                    *
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

#ifndef DEVICE_MODEL_H
#define DEVICE_MODEL_H

#include <QStandardItemModel>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusMessage>

#include <NetworkManagerQt/Manager>

typedef QMap<QString, QString>  StringStringMap;
typedef QList<QDBusObjectPath> ObjectPathList;

class DeviceConnectionModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum DeviceRoles {
        RoleIsDevice = Qt::UserRole + 1,
        RoleIsDeviceParent,
        RoleIsConnection,
        RoleIsVpnConnection,
        RoleIsConnectionParent,
        RoleIsConnectionCategory,
        RoleIsVpnConnectionCategory,
        RoleIsConnectionCategoryActiveCount,
        RoleDeviceUNI,
        RoleConnectionPath,
        RoleState,
        RoleConnectionActivePath,
        RoleSort
    };
    explicit DeviceConnectionModel(QObject *parent = 0);
    void init();

    Qt::ItemFlags flags(const QModelIndex &index) const;

signals:
    void parentAdded(const QModelIndex &index);

private slots:
    void initConnections();
    void removeConnections();

    void deviceAdded(const QString &uni);
    void deviceChanged();
    void deviceRemoved(const QString &uni);
    void addDevice(const NetworkManager::Device::Ptr &device);
    void changeDevice(QStandardItem *stdItem, const NetworkManager::Device::Ptr &device);

    void connectionAdded(const QString &path);
    void connectionUpdated();
    void connectionRemoved(const QString &path);
    void activeConnectionAdded(const QString &path);
    void activeConnectionRemoved(const QString &path);
    void addConnection(const NetworkManager::Connection::Ptr &connection);
    void changeConnectionActive(QStandardItem *stdItem, const QString &activePath = QString());
    void signalStrengthChanged();
    void activeAccessPointChanged(const QString &uni);

private:
    QStandardItem *findDeviceItem(const QString &uni);
    QStandardItem *findConnectionItem(const QString &path, DeviceRoles role);
    QStandardItem *findOrCreateConnectionType(NetworkManager::ConnectionSettings::ConnectionType type);
};

#endif // DEVICE_MODEL_H
