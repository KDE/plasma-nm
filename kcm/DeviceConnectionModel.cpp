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

#include "DeviceConnectionModel.h"

#include <uiutils.h>

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/activeconnection.h>

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>
#include <QStringBuilder>

#include <KDebug>
#include <KLocale>
#include <KDateTime>
#include <KIcon>

using namespace NetworkManager;

DeviceConnectionModel::DeviceConnectionModel(QObject *parent) :
    QStandardItemModel(parent),
    m_handleConnections(false)
{
    connect(NetworkManager::notifier(), SIGNAL(serviceAppeared()),
            this, SLOT(initConnections()));
    connect(NetworkManager::notifier(), SIGNAL(serviceDisappeared()),
            this, SLOT(removeConnections()));
    connect(NetworkManager::Settings::notifier(), SIGNAL(connectionAdded(QString)),
            this, SLOT(connectionAdded(QString)));
    connect(NetworkManager::Settings::notifier(), SIGNAL(connectionRemoved(QString)),
            this, SLOT(connectionRemoved(QString)));

    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            this, SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            this, SLOT(deviceRemoved(QString)));
    foreach (const Device::Ptr &device, NetworkManager::networkInterfaces()) {
        addDevice(device);
    }
}

void DeviceConnectionModel::setHandleConnections(bool handleConnections)
{
    if (m_handleConnections != handleConnections) {
        m_handleConnections = handleConnections;
        if (handleConnections) {
            initConnections();
        } else {
            removeConnections();
        }
    }
}

Qt::ItemFlags DeviceConnectionModel::flags(const QModelIndex &index) const
{
    QStandardItem *stdItem = itemFromIndex(index);
    if (stdItem && stdItem->isCheckable() && stdItem->checkState() == Qt::Unchecked) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


void DeviceConnectionModel::initConnections()
{
    if (m_handleConnections) {
        foreach (const Settings::Connection::Ptr &connection, Settings::listConnections()) {
            addConnection(connection);
        }
    }
}

void DeviceConnectionModel::removeConnections()
{
    int i = 0;
    while (i < rowCount()) {
        if (item(i)->data(RoleIsDevice).toBool() == false) {
            removeRow(i);
        } else {
            ++i;
        }
    }
}

void DeviceConnectionModel::deviceAdded(const QString &uni)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    if (device) {
        addDevice(device);
    }
}

void DeviceConnectionModel::deviceChanged()
{
    NetworkManager::Device *caller = qobject_cast<NetworkManager::Device*>(sender());
    if (caller) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(caller->uni());
        if (device) {
            QStandardItem *stdItem = findDeviceItem(device->uni());
            if (!stdItem) {
                kWarning() << "Device not found" << device->uni();
                return;
            }
            changeDevice(stdItem, device);
        }
    }
}

void DeviceConnectionModel::deviceRemoved(const QString &uni)
{
    QStandardItem *stdItem = findDeviceItem(uni);
    if (stdItem) {
        removeRow(stdItem->row());
    }
}

void DeviceConnectionModel::addDevice(const NetworkManager::Device::Ptr &device)
{
    QStandardItem *stdItem = findDeviceItem(device->uni());
    if (stdItem) {
        return;
    }

    connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
            this, SLOT(deviceChanged()));

    stdItem = new QStandardItem;
    stdItem->setData(true, RoleIsDevice);
    stdItem->setData(device->uni(), RoleDeviceUNI);
    changeDevice(stdItem, device);
    appendRow(stdItem);
}

void DeviceConnectionModel::changeDevice(QStandardItem *stdItem, const NetworkManager::Device::Ptr &device)
{
    stdItem->setIcon(KIcon(UiUtils::iconName(device)));

    QString connectionName;
    NetworkManager::ActiveConnection::Ptr activeConnection = device->activeConnection();
    if (activeConnection) {
        connectionName = activeConnection->connection()->name();
    }
    stdItem->setData(UiUtils::connectionStateToString(device->state(), connectionName), RoleState);

    switch (device->type()) {
    case NetworkManager::Device::Wifi:
        stdItem->setText(i18n("Wireless Interface (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Ethernet:
        stdItem->setText(i18n("Wired Interface (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Bluetooth:
        stdItem->setText(i18n("Bluetooth (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Modem:
        stdItem->setText(i18n("Modem (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Adsl:
        stdItem->setText(i18n("ADSL (%1)", device->interfaceName()));
        break;
    default:
        stdItem->setText(device->interfaceName());
    }
}

void DeviceConnectionModel::connectionAdded(const QString &path)
{
    if (m_handleConnections) {
        Settings::Connection::Ptr connection = Settings::findConnection(path);
        if (connection) {
            addConnection(connection);
        }
    }
}

void DeviceConnectionModel::connectionChanged()
{
    if (m_handleConnections) {
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
}

void DeviceConnectionModel::connectionRemoved(const QString &path)
{
    if (m_handleConnections) {
        QStandardItem *stdItem = findConnectionItem(path);
        if (stdItem) {
            removeRow(stdItem->row());
        }
    }
}

void DeviceConnectionModel::addConnection(const Settings::Connection::Ptr &connection)
{
    QStandardItem *stdItem = findConnectionItem(connection->path());
    if (stdItem) {
        return;
    }

    connect(connection.data(), SIGNAL(updated()),
            this, SLOT(connectionChanged()));

    stdItem = new QStandardItem;
    stdItem->setData(false, RoleIsDevice);
    stdItem->setData(connection->path(), RoleConectionPath);
    changeConnection(stdItem, connection);
    appendRow(stdItem);
}

void DeviceConnectionModel::changeConnection(QStandardItem *stdItem, const Settings::Connection::Ptr &connection)
{
    kDebug() << connection->uuid() << connection->path() << connection->name();
    if (connection->active()) {
        stdItem->setIcon(KIcon("network-connect"));
    } else {
        stdItem->setIcon(KIcon("network-disconnect"));
    }

    stdItem->setText(connection->name());
}

QStandardItem *DeviceConnectionModel::findDeviceItem(const QString &uni)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i);
        if (stdItem->data(RoleIsDevice).toBool() == true &&
                stdItem->data(RoleDeviceUNI).toString() == uni) {
            return stdItem;
        }
    }

    return 0;
}

QStandardItem *DeviceConnectionModel::findConnectionItem(const QString &path)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i);
        if (stdItem->data(RoleIsDevice).toBool() == false &&
                stdItem->data(RoleConectionPath).toString() == path) {
            return stdItem;
        }
    }

    return 0;
}
