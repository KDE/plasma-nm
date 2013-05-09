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

#include "TabDeviceInfo.h"
#include "ui_TabDeviceInfo.h"

#include "AvailableConnectionsModel.h"
#include "AvailableConnectionsSortModel.h"
#include "AvailableConnectionsDelegate.h"

#include <uiutils.h>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/IpConfig>
#include <NetworkManagerQt/WimaxDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WimaxSetting>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/ActiveConnection>

#include <KMessageBox>
#include <KDebug>

using namespace NetworkManager;

TabDeviceInfo::TabDeviceInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabDeviceInfo)
{
    ui->setupUi(this);

    ui->connectionCB->setMaxVisibleItems(20);
    ui->connectionCB->setItemDelegate(new AvailableConnectionsDelegate(this));
    m_availableConnectionsModel = new AvailableConnectionsModel(this);
    m_availableConnectionsSortModel = new AvailableConnectionsSortModel(this);
    m_availableConnectionsSortModel->setSourceModel(m_availableConnectionsModel);
    ui->connectionCB->setModel(m_availableConnectionsSortModel);
}

TabDeviceInfo::~TabDeviceInfo()
{
    delete ui;
}

void TabDeviceInfo::setDevice(const NetworkManager::Device::Ptr &device)
{
    if (m_device && m_device->uni() == device->uni()) {
        return;
    }

    if (m_device) {
        m_device->disconnect(this);
        NetworkManager::notifier()->disconnect(this);
        NetworkManager::notifier()->disconnect(ui->turnOff);
    }
    m_device = device;
    if (device) {
        connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
                this, SLOT(updateState()));
        updateState();

        m_availableConnectionsModel->setDevice(device);

        connect(device.data(), SIGNAL(activeConnectionChanged()),
                this, SLOT(updateActiveConnection()));
        updateActiveConnection();

        connect(device.data(), SIGNAL(ipV4AddressChanged()),
                this, SLOT(updateIpV4Config()));
        updateIpV4Config();

        if (device->type() == Device::Wifi) {
            ui->turnOff->setEnabled(NetworkManager::isWirelessHardwareEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
                    ui->turnOff, SLOT(setEnabled(bool)));
            setTurnOffWifiText(NetworkManager::isWirelessEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
                    this, SLOT(setTurnOffWifiText(bool)));
        } else if (device->type() == Device::Wimax) {
            ui->turnOff->setEnabled(NetworkManager::isWimaxHardwareEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wimaxHardwareEnabledChanged(bool)),
                    ui->turnOff, SLOT(setEnabled(bool)));
            setTurnOffWimaxText(NetworkManager::isWimaxEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wimaxEnabledChanged(bool)),
                    this, SLOT(setTurnOffWimaxText(bool)));
        } else {
            ui->turnOff->setVisible(false);
        }
    }
}

void TabDeviceInfo::updateState()
{
    if (m_device) {
        ui->statusL->setText(UiUtils::connectionStateToString(m_device->state()));
        switch (m_device->state()) {
        case Device::Disconnected:
        case Device::Unavailable:
        case Device::Unmanaged:
            ui->disconnectPB->setEnabled(false);
            break;
        default:
            ui->disconnectPB->setEnabled(true);
            break;
        }
    }
}

void TabDeviceInfo::updateActiveConnection()
{
    int active = -1;
    if (m_device->activeConnection()) {
        QString activeConnectionPath = m_device->activeConnection()->connection()->path();
        active = ui->connectionCB->findData(activeConnectionPath, AvailableConnectionsModel::RoleConectionPath);
    }
    ui->connectionCB->setCurrentIndex(active);
}

void TabDeviceInfo::updateIpV4Config()
{
    QStringList routers;
    QStringList addresses;
    foreach (const NetworkManager::IpAddress &address, m_device->ipV4Config().addresses()) {
        addresses << address.ip().toString() % QLatin1Char('/') % QString::number(address.prefixLength());
        routers << address.gateway().toString();
    }
    ui->ipv4AddressL->setText(addresses.join(QLatin1String("\n")));

    foreach (const NetworkManager::IpRoute &route, m_device->ipV4Config().routes()) {
        routers << i18n("%1/%2, nexthop %3 metric %4",
                        route.ip().toString(),
                        QString::number(route.prefixLength()),
                        route.nextHop().toString(),
                        QString::number(route.metric()));
    }
    ui->ipv4RouterL->setText(routers.join(QLatin1String("\n")));

    QStringList nameservers;
    foreach (const QHostAddress &nameserver, m_device->ipV4Config().nameservers()) {
        nameservers << nameserver.toString();
    }
    ui->ipv4DnsServerL->setText(nameservers.join(QLatin1String(", ")));

    ui->ipv4SearchDomainsL->setText(m_device->ipV4Config().domains().join(QLatin1String("\n")));
}

void TabDeviceInfo::on_disconnectPB_clicked()
{
    if (m_device) {
        m_device->disconnectInterface();
    }
}

void TabDeviceInfo::on_connectionCB_activated(int index)
{
    kDebug();
    QModelIndex modelIndex = m_availableConnectionsSortModel->mapToSource(m_availableConnectionsSortModel->index(index, 0));
    if (m_device.isNull() || !modelIndex.isValid()) {
        return;
    }

    QString newConnectionPath = modelIndex.data(AvailableConnectionsModel::RoleConectionPath).toString();
    if (newConnectionPath.isEmpty()) {
        QString name = modelIndex.data().toString();
        QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply;
        uint kind = modelIndex.data(AvailableConnectionsModel::RoleKinds).toUInt();
        if (kind & AvailableConnectionsModel::NetworkWireless) {
            NetworkManager::WirelessDevice::Ptr wifi = m_device.dynamicCast<NetworkManager::WirelessDevice>();
            if (wifi.isNull()) {
                return;
            }

            QByteArray ssid = modelIndex.data(AvailableConnectionsModel::RoleNetworkID).toByteArray();
            WirelessNetwork::Ptr network = wifi->findNetwork(ssid);
            if (network.isNull()) {
                return;
            }

            NetworkManager::ConnectionSettings settings(NetworkManager::ConnectionSettings::Wireless);
            settings.setId(name);
            settings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

            NetworkManager::WirelessSetting::Ptr wifiSetting;
            wifiSetting = settings.setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
            wifiSetting->setSsid(ssid);

            reply = NetworkManager::addAndActivateConnection(settings.toMap(), m_device->uni(), network->referenceAccessPoint()->uni());
        } else if (kind & AvailableConnectionsModel::NetworkNsp) {
            QString uni = modelIndex.data(AvailableConnectionsModel::RoleNetworkID).toString();

            NetworkManager::ConnectionSettings settings(NetworkManager::ConnectionSettings::Wimax);
            settings.setId(name);
            settings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

            NetworkManager::WimaxSetting::Ptr wimaxSetting;
            wimaxSetting = settings.setting(NetworkManager::Setting::Wimax).dynamicCast<NetworkManager::WimaxSetting>();
            wimaxSetting->setNetworkName(name);
            reply = NetworkManager::addAndActivateConnection(settings.toMap(), m_device->uni(), uni);
        }

        reply.waitForFinished();
        if (reply.isError()) {
            KMessageBox::error(this,
                               i18n("Failed to activate network %1:\n%2", name, reply.error().message()));
            updateActiveConnection();
        }
    } else {
        QString oldConnectionPath;
        if (m_device->activeConnection()) {
            oldConnectionPath = m_device->activeConnection()->connection()->path();
        }

        if (newConnectionPath != oldConnectionPath) {
            NetworkManager::activateConnection(newConnectionPath, m_device->uni(), QString());
        }
    }
}

void TabDeviceInfo::on_turnOff_clicked()
{
    if (m_device) {
        switch (m_device->type()) {
        case Device::Wifi:
            NetworkManager::setWirelessEnabled(!NetworkManager::isWirelessEnabled());
            break;
        case Device::Wimax:
            NetworkManager::setWimaxEnabled(!NetworkManager::isWimaxEnabled());
            break;
        default:
            break;
        }
    }
}

void TabDeviceInfo::setTurnOffWifiText(bool enabled)
{
    if (enabled) {
        ui->turnOff->setText(i18n("Turn off WiFi"));
    } else {
        ui->turnOff->setText(i18n("Turn on WiFi"));
    }
}

void TabDeviceInfo::setTurnOffWimaxText(bool enabled)
{
    if (enabled) {
        ui->turnOff->setText(i18n("Turn off WiMax"));
    } else {
        ui->turnOff->setText(i18n("Turn on WiMax"));
    }
}
