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

    foreach (const Settings::Connection::Ptr &con, Settings::listConnections()) {
        addConnection(con);
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
            int row = findDeviceItem(device->uni());
            if (row == -1) {
                kWarning() << "Device not found" << device->uni();
                return;
            }
            changeDevice(item(row), device);
        }
    }
}

void DeviceConnectionModel::deviceRemoved(const QString &uni)
{
    kWarning() << "Device not found" << uni;
    int row = findDeviceItem(uni);
    kWarning() << "Device not found" << row;
    if (row != -1) {
        removeRow(row);
    }
}

void DeviceConnectionModel::addDevice(const NetworkManager::Device::Ptr &device)
{
    int row = findDeviceItem(device->uni());
    if (row != -1) {
        return;
    }

    connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
            this, SLOT(deviceChanged()));

    QStandardItem *stdItem = new QStandardItem;
    stdItem->setData(device->uni(), DeviceUNI);
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
    stdItem->setData(UiUtils::connectionStateToString(device->state(), connectionName), StateRole);

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

QStandardItem *DeviceConnectionModel::findProfile(QStandardItem *parent, const QDBusObjectPath &objectPath)
{
    return 0;
}

void DeviceConnectionModel::removeProfilesNotInList(QStandardItem *parent, const ObjectPathList &profiles)
{
}

int DeviceConnectionModel::findDeviceItem(const QString &uni)
{
    for (int i = 0; i < rowCount(); ++i) {
        if (item(i)->data(DeviceUNI).toString() == uni) {
            return i;
        }
    }

    return -1;
}

QVariant DeviceConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return i18n("Devices");
    }
    return QVariant();
}

bool DeviceConnectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value)
    Q_UNUSED(role)



    // We return false since colord will emit a DeviceChanged signal telling us about this change
    return false;
}

Qt::ItemFlags DeviceConnectionModel::flags(const QModelIndex &index) const
{
    QStandardItem *stdItem = itemFromIndex(index);
    if (stdItem && stdItem->isCheckable() && stdItem->checkState() == Qt::Unchecked) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
