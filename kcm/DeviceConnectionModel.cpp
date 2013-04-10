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
    QStandardItemModel(parent)
{
    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            this, SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            this, SLOT(deviceRemoved(QString)));

    foreach (const Device::Ptr &device, NetworkManager::networkInterfaces()) {
        addDevice(device);
    }

    foreach (const Settings::Connection::Ptr &connection, Settings::listConnections()) {
        if (connection.isNull()) {
            kWarning() << "NULLLL";
        }
        kWarning() << "NULLLL" << connection->name();
        addConnection(connection);
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

void DeviceConnectionModel::addConnection(const Settings::Connection::Ptr &connection)
{
    QStandardItem *stdItem = findConnectionItem(connection->path());
    if (stdItem) {
        return;
    }

    connect(connection.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
            this, SLOT(deviceChanged()));

    stdItem = new QStandardItem;
    stdItem->setData(false, RoleIsDevice);
    stdItem->setData(connection->path(), RoleConectionUNI);
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

void DeviceConnectionModel::serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(serviceName)
    if (newOwner.isEmpty() || oldOwner != newOwner) {
        // colord has quit or restarted
        removeRows(0, rowCount());
        emit changed();
    }
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

QStandardItem *DeviceConnectionModel::findConnectionItem(const QString &uni)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i);
        if (stdItem->data(RoleIsDevice).toBool() == false &&
                stdItem->data(RoleConectionUNI).toString() == uni) {
            return stdItem;
        }
    }

    return 0;
}

QVariant DeviceConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return i18n("Devices");
    }
    return QVariant();
}

Qt::ItemFlags DeviceConnectionModel::flags(const QModelIndex &index) const
{
    QStandardItem *stdItem = itemFromIndex(index);
    if (stdItem && stdItem->isCheckable() && stdItem->checkState() == Qt::Unchecked) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
