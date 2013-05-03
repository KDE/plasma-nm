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

#include <NetworkManagerQt/settings.h>
#include <NetworkManagerQt/connection.h>
#include <NetworkManagerQt/activeconnection.h>

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
using namespace NetworkManager::Settings;

DeviceConnectionModel::DeviceConnectionModel(QObject *parent) :
    QStandardItemModel(parent)
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
}

void DeviceConnectionModel::init()
{
    foreach (const Device::Ptr &device, NetworkManager::networkInterfaces()) {
        addDevice(device);
    }
    initConnections();
}

Qt::ItemFlags DeviceConnectionModel::flags(const QModelIndex &index) const
{
    QStandardItem *stdItem = itemFromIndex(index);
    if (stdItem->flags() == Qt::NoItemFlags) {
        return Qt::NoItemFlags;
    } else if (stdItem && stdItem->isCheckable() && stdItem->checkState() == Qt::Unchecked) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void DeviceConnectionModel::initConnections()
{
    foreach (const Settings::Connection::Ptr &connection, Settings::listConnections()) {
        addConnection(connection);
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
    stdItem->setText(UiUtils::prettyInterfaceName(device->type(), device->interfaceName()));
}

void DeviceConnectionModel::connectionAdded(const QString &path)
{
    Settings::Connection::Ptr connection = Settings::findConnection(path);
    if (connection) {
        addConnection(connection);
    }
}

void DeviceConnectionModel::connectionChanged()
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

void DeviceConnectionModel::connectionRemoved(const QString &path)
{
    QStandardItem *stdItem = findConnectionItem(path);
    if (stdItem) {
        removeRow(stdItem->row());
    }
}

void DeviceConnectionModel::addConnection(const Settings::Connection::Ptr &connection)
{
    QStandardItem *stdItem = findConnectionItem(connection->path());
    if (stdItem) {
        return;
    }

    Settings::ConnectionSettings::ConnectionType type = connection->settings()->connectionType();
    QStandardItem *parentItem = findOrCreateConnectionType(type);
    connect(connection.data(), SIGNAL(updated()),
            this, SLOT(connectionChanged()));

    stdItem = new QStandardItem;
    stdItem->setData(true, RoleIsConnection);
    stdItem->setData(connection->path(), RoleConectionPath);
    changeConnection(stdItem, connection);
    parentItem->appendRow(stdItem);
}

void DeviceConnectionModel::changeConnection(QStandardItem *stdItem, const Settings::Connection::Ptr &connection)
{
    kDebug() << connection->uuid() << connection->path() << connection->name();
    if (connection->active()) {
        stdItem->setIcon(KIcon("network-connect"));
    } else {
        stdItem->setIcon(KIcon("network-disconnect"));
    }
    stdItem->setData(connection->active(), RoleConnectionActive);

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
        if (stdItem->data(RoleIsConnection).toBool() == true &&
                stdItem->data(RoleConectionPath).toString() == path) {
            return stdItem;
        }
    }

    return 0;
}

QStandardItem *DeviceConnectionModel::findOrCreateConnectionType(Settings::ConnectionSettings::ConnectionType type)
{
    QStandardItem *parentItem = 0;
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *stdItem = item(i);
        if (stdItem->data(RoleIsConnectionParent).toBool()) {
            parentItem = stdItem;
            break;
        }
    }

    if (!parentItem) {
        parentItem = new QStandardItem;
        parentItem->setData(true, RoleIsConnectionParent);
        parentItem->setSelectable(false);
        parentItem->setEnabled(false);
        parentItem->setFlags(Qt::NoItemFlags);
        parentItem->setSizeHint(QSize(0, 0));
        appendRow(parentItem);
        emit parentAdded(parentItem->index());
    }

    for (int i = 0; i < parentItem->rowCount(); ++i) {
        QStandardItem *stdItem = parentItem->child(i);
        uint itemType = stdItem->data(RoleIsConnectionCategory).toUInt();
        if (itemType == type ||
                (type == ConnectionSettings::Gsm && itemType == ConnectionSettings::Cdma) ||
                (type == ConnectionSettings::Cdma && itemType == ConnectionSettings::Gsm)) {
            return stdItem;
        }
    }

    QStandardItem *ret = new QStandardItem();
    QString text;
    KIcon icon;
    switch (type) {
    case ConnectionSettings::Adsl:
        text = i18n("ADSL");
        icon = KIcon("modem");
        break;
    case ConnectionSettings::Pppoe:
        text = i18n("DSL");
        icon = KIcon("modem");
        break;
    case ConnectionSettings::Bluetooth:
        text = i18n("Bluetooth");
        icon = KIcon("preferences-system-bluetooth");
        break;
    case ConnectionSettings::Bond:
        text = i18n("Bond");
        break;
    case ConnectionSettings::Bridge:
        text = i18n("Bridge");
        break;
    case ConnectionSettings::Gsm:
    case ConnectionSettings::Cdma:
        text = i18n("Mobile broadband");
        icon = KIcon("phone");
        break;
    case ConnectionSettings::Infiniband:
        text = i18n("Infiniband");
        break;
    case ConnectionSettings::OLPCMesh:
        text = i18n("Olpc mesh");
        break;
    case ConnectionSettings::Vlan:
        text = i18n("VLAN");
        break;
    case ConnectionSettings::Vpn:
        text = i18n("VPN");
        break;
    case ConnectionSettings::Wimax:
        text = i18n("WiMAX");
        icon = KIcon("network-wireless");
        break;
    case ConnectionSettings::Wired:
        text = i18n("Wired");
        icon = KIcon("network-wired");
        break;
    case ConnectionSettings::Wireless:
        text = i18n("Wi-Fi");
        icon = KIcon("network-wireless");
        break;
    default:
        text = i18n("Unknown connection type");
        break;
    }

    ret->setText(text);
    ret->setIcon(icon);
    ret->setData(type, RoleIsConnectionCategory);
    parentItem->appendRow(ret);
    emit parentAdded(ret->index());

    return ret;
}
