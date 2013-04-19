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

#include "AvailableConnectionsModel.h"

#include <NetworkManagerQt/settings/connection.h>
#include <NetworkManagerQt/activeconnection.h>
#include <NetworkManagerQt/device.h>
#include <NetworkManagerQt/settings.h>

#include <KDebug>

using namespace NetworkManager;

AvailableConnectionsModel::AvailableConnectionsModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

void AvailableConnectionsModel::setDevice(const NetworkManager::Device::Ptr &device)
{
    if (m_device && m_device->uni() == device->uni()) {
        return;
    } else if (m_device) {
        m_device->disconnect(this);
    }
    m_device = device;
    removeRows(0, rowCount());

    if (device->type() == NetworkManager::Device::Wifi) {

    } else if (device->type() == NetworkManager::Device::Wimax) {

    } else {
        connect(device.data(), SIGNAL(availableConnectionChanged()),
                this, SLOT(availableConnectionChanged()));
        foreach (const Settings::Connection::Ptr &connection, device->availableConnections()) {
            addConnection(connection);
        }
    }
}

void AvailableConnectionsModel::availableConnectionChanged()
{
    Device *device = qobject_cast<NetworkManager::Device*>(sender());
    if (device) {
        Settings::Connection::List connections = device->availableConnections();
        foreach (const Settings::Connection::Ptr &connection, connections) {
            qWarning() << "Connection" << connection->name();
            if (!findConnectionItem(connection->path())) {
                qWarning() << "New connection" << connection->name();
                addConnection(connection);
            }
        }

        int i = 0;
        while (i < rowCount()) {
            bool found = false;
            QString path = item(i)->data(RoleConectionPath).toString();
            foreach (const Settings::Connection::Ptr &connection, connections) {
                if (connection->path() == path) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                qWarning() << "Remove connection" << path;
                removeRow(i);
            } else {
                ++i;
            }
        }
    }
}

void AvailableConnectionsModel::connectionAdded(const QString &path)
{
    Settings::Connection::Ptr connection = Settings::findConnection(path);
    if (connection) {
        addConnection(connection);
    }
}

void AvailableConnectionsModel::connectionChanged()
{
    Settings::Connection *caller = qobject_cast<Settings::Connection*>(sender());
    if (caller) {
        Settings::Connection::Ptr connection = Settings::findConnection(caller->path());
        if (connection) {
            QStandardItem *stdItem = findConnectionItem(connection->path());
            if (!stdItem) {
                kWarning() << "Connection not found" << connection->path();
                return;
            }
            changeConnection(stdItem, connection);
        }
    }
}

void AvailableConnectionsModel::connectionRemoved(const QString &path)
{
    QStandardItem *stdItem = findConnectionItem(path);
    if (stdItem) {
        removeRow(stdItem->row());
    }
}

void AvailableConnectionsModel::addConnection(const NetworkManager::Settings::Connection::Ptr &connection)
{
    QStandardItem *stdItem = findConnectionItem(connection->path());
    if (stdItem) {
        return;
    }

    connect(connection.data(), SIGNAL(updated()),
            this, SLOT(connectionChanged()));

    stdItem = new QStandardItem;
    stdItem->setData(connection->path(), RoleConectionPath);
    changeConnection(stdItem, connection);
    appendRow(stdItem);
}

void AvailableConnectionsModel::changeConnection(QStandardItem *stdItem, const NetworkManager::Settings::Connection::Ptr &connection)
{
    kDebug() << connection->uuid() << connection->path() << connection->name();/*
    if (connection->active()) {
        stdItem->setIcon(KIcon("network-connect"));
    } else {
        stdItem->setIcon(KIcon("network-disconnect"));
    }*/

    stdItem->setText(connection->name());
}

QStandardItem *AvailableConnectionsModel::findConnectionItem(const QString &path)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i);
        if (stdItem->data(RoleConectionPath).toString() == path) {
            return stdItem;
        }
    }

    return 0;
}
