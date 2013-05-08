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

#include "TabDeviceAdvanced.h"
#include "ui_TabDeviceAdvanced.h"

#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>

#include <KDebug>

TabDeviceAdvanced::TabDeviceAdvanced(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabDeviceAdvanced)
{
    ui->setupUi(this);

    m_model = new QStandardItemModel(this);
    QStringList headers;
    headers << i18n("Setting");
    headers << i18n("Value");
    m_model->setHorizontalHeaderLabels(headers);
    ui->treeView->setModel(m_model);
    ui->treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
}

TabDeviceAdvanced::~TabDeviceAdvanced()
{
    delete ui;
}

void TabDeviceAdvanced::setDevice(const NetworkManager::Device::Ptr &device)
{
    m_model->removeRows(0, m_model->rowCount());

    addItem(i18n("Managed"), device->managed() ? i18n("Yes") : i18n("No"));
    addItem(i18n("Auto connect"), device->autoconnect() ? i18n("Yes") : i18n("No"));
    addItem(i18n("Interface name"), device->interfaceName());
    addItem(i18n("Driver"), device->driver());
    addItem(i18n("Driver version"), device->driverVersion());
    if (!device->firmwareVersion().isEmpty()) {
        addItem(i18n("Firmware version"), device->firmwareVersion());
    }

    if (device->designSpeed()) {
        addItem(i18n("Speed"), QString::number(device->designSpeed()));
    }

    NetworkManager::WirelessDevice::Ptr wifi = device.dynamicCast<NetworkManager::WirelessDevice>();
    if (wifi) {
        addItem(i18n("Hardware address"), wifi->permanentHardwareAddress());
        if (wifi->hardwareAddress() != wifi->permanentHardwareAddress()) {
            addItem(i18n("Custom hardware address"), wifi->hardwareAddress());
        }
    }

    NetworkManager::WiredDevice::Ptr wired = device.dynamicCast<NetworkManager::WiredDevice>();
    if (wired) {
        addItem(i18n("Hardware address"), wired->permanentHardwareAddress());
        if (wired->hardwareAddress() != wired->permanentHardwareAddress()) {
            addItem(i18n("Custom hardware address"), wired->hardwareAddress());
        }
    }

    if (device->dhcp4Config()) {
        QVariantMap options = device->dhcp4Config()->options();
        QVariantMap::ConstIterator i = options.constBegin();
        while (i != options.constEnd()) {
            QString name = i.key();
            if (name == QLatin1String("broadcast_address")) {
                addItem(i18n("Broadcast address:"), i.value().toString());
            } else {
                addItem(i.key(), i.value().toString());
            }
            ++i;
        }
        kDebug() << device->dhcp4Config()->options();
    }

    if (device->dhcp6Config()) {
        QVariantMap options = device->dhcp6Config()->options();
        QVariantMap::ConstIterator i = options.constBegin();
        while (i != options.constEnd()) {
            QString name = i.key();
            if (name == QLatin1String("broadcast_address")) {
                addItem(i18n("Broadcast Address:"), i.value().toString());
            } else {
                addItem(i.key(), i.value().toString());
            }
            ++i;
        }
        kDebug() << device->dhcp6Config()->options();
    }
}

void TabDeviceAdvanced::addItem(const QString &key, const QString &value)
{
    QList<QStandardItem*> items;
    QStandardItem *item = new QStandardItem(key);
    item->setEditable(false);
    items << item;

    item = new QStandardItem(value);
    items << item;

    m_model->appendRow(items);
}
