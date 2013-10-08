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

#include <QDebug>

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
        addDhcpItems(device->dhcp4Config()->options());
    }

    if (device->dhcp6Config()) {
        addDhcpItems(device->dhcp6Config()->options());
    }
}

void TabDeviceAdvanced::addDhcpItems(const QVariantMap &options)
{
    QVariantMap::ConstIterator i = options.constBegin();
    while (i != options.constEnd()) {
        QString name = i.key();
        if (name == QLatin1String("broadcast_address")) {
            addItem(i18n("Broadcast address"), i.value().toString());
        } else if (name == QLatin1String("dhcp_lease_time")) {
            addItem(i18n("DHCP lease time"),
                    KGlobal::locale()->prettyFormatDuration(i.value().toInt() * 1000));
        } else if (name == QLatin1String("dhcp_message_type")) {
            addItem(i18n("DHCP message type"), i.value().toString());
        } else if (name == QLatin1String("dhcp_rebinding_time")) {
            addItem(i18n("DHCP rebinding time"),
                    KGlobal::locale()->prettyFormatDuration(i.value().toInt() * 1000));
        } else if (name == QLatin1String("dhcp_renewal_time")) {
            addItem(i18n("DHCP renewal time"),
                    KGlobal::locale()->prettyFormatDuration(i.value().toInt() * 1000));
        } else if (name == QLatin1String("dhcp_server_identifier")) {
            addItem(i18n("DHCP server identifier"), i.value().toString());
        } else if (name == QLatin1String("domain_name")) {
            addItem(i18n("Domain name"), i.value().toString());
        } else if (name == QLatin1String("domain_name_servers")) {
            addItem(i18n("DNS"), i.value().toString());
        } else if (name == QLatin1String("expiry")) {
            QDateTime dateTime;
            dateTime = QDateTime::fromMSecsSinceEpoch(i.value().toLongLong() * 1000);
            addItem(i18n("Expiry"),
                    KGlobal::locale()->formatDateTime(dateTime));
        } else if (name == QLatin1String("ip_address")) {
            addItem(i18n("IP Address"), i.value().toString());
        } else if (name == QLatin1String("network_number")) {
            addItem(i18n("Network"), i.value().toString());
        } else if (name == QLatin1String("routers")) {
            addItem(i18n("Routers"), i.value().toString());
        } else if (name == QLatin1String("subnet_mask")) {
            addItem(i18n("Subnet mask"), i.value().toString());
        } else {
            addItem(i.key(), i.value().toString());
        }
        ++i;
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
