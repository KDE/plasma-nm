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

#define DEVICE_ICON_SIZE 64

using namespace NetworkManager;

TabDeviceInfo::TabDeviceInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabDeviceInfo),
    m_ip4Visible(true),
    m_ip6Visible(true)
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
        m_availableConnectionsModel->setDevice(device);

        connect(device.data(), SIGNAL(activeConnectionChanged()),
                this, SLOT(updateActiveConnection()));
        updateActiveConnection();

        connect(device.data(), SIGNAL(ipV4ConfigChanged()),
                this, SLOT(updateIpV4Config()));
        updateIpV4Config();

        connect(device.data(), SIGNAL(ipV6ConfigChanged()),
                this, SLOT(updateIpV6Config()));
        updateIpV6Config();

        if (device->type() == Device::Wifi) {
            ui->turnOff->setEnabled(NetworkManager::isWirelessHardwareEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
                    ui->turnOff, SLOT(setEnabled(bool)));
            setTurnOffWifiText(NetworkManager::isWirelessEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
                    this, SLOT(setTurnOffWifiText(bool)));
            WirelessDevice::Ptr wifi = device.objectCast<WirelessDevice>();
            connect(wifi.data(), SIGNAL(activeAccessPointChanged(QString)),
                    this, SLOT(activeAccessPointChanged(QString)));
            AccessPoint::Ptr accessPoint = wifi->activeAccessPoint();
            if (accessPoint) {
                connect(accessPoint.data(), SIGNAL(signalStrengthChanged(int)),
                        this, SLOT(signalStrengthChanged()), Qt::UniqueConnection);
            }
        } else if (device->type() == Device::Wimax) {
            ui->turnOff->setEnabled(NetworkManager::isWimaxHardwareEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wimaxHardwareEnabledChanged(bool)),
                    ui->turnOff, SLOT(setEnabled(bool)));
            setTurnOffWimaxText(NetworkManager::isWimaxEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wimaxEnabledChanged(bool)),
                    this, SLOT(setTurnOffWimaxText(bool)));
        } else if (device->type() == Device::Modem) {
            ui->turnOff->setEnabled(NetworkManager::isWwanHardwareEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wimaxHardwareEnabledChanged(bool)),
                    ui->turnOff, SLOT(setEnabled(bool)));
            setTurnOffWwanText(NetworkManager::isWwanEnabled());
            connect(NetworkManager::notifier(), SIGNAL(wwanEnabledChanged(bool)),
                    this, SLOT(setTurnOffWwanText(bool)));
        } else {
            ui->turnOff->setVisible(false);
        }

        ui->deviceL->setText(UiUtils::prettyInterfaceName(device->type(), device->interfaceName()));
        connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
                this, SLOT(updateState()));
        updateState();
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

        QPixmap icon = KIconLoader::global()->loadIcon(UiUtils::iconName(m_device),
                                                       KIconLoader::NoGroup,
                                                       DEVICE_ICON_SIZE, // a not so huge icon
                                                       KIconLoader::DefaultState);
        ui->iconL->setPixmap(icon);
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
    updateIpConfig(m_device->ipV4Config(), true);
}

void TabDeviceInfo::updateIpV6Config()
{
    updateIpConfig(m_device->ipV6Config(), false);
}

void TabDeviceInfo::updateIpConfig(const IpConfig &ipConfig, bool ipv4)
{
    QStringList addresses;
    QStringList routers;
    QStringList nameservers;
    QStringList domains;
    if (ipConfig.isValid()) {
        foreach (const NetworkManager::IpAddress &address, ipConfig.addresses()) {
            addresses << address.ip().toString() % QLatin1Char('/') % QString::number(address.prefixLength());
            routers << address.gateway().toString();
        }

        foreach (const NetworkManager::IpRoute &route, ipConfig.routes()) {
            routers << i18n("%1/%2, nexthop %3 metric %4",
                            route.ip().toString(),
                            QString::number(route.prefixLength()),
                            route.nextHop().toString(),
                            QString::number(route.metric()));
        }

        foreach (const QHostAddress &nameserver, ipConfig.nameservers()) {
            nameservers << nameserver.toString();
        }
        domains = ipConfig.domains();
    }

    if (ipv4) {
        ui->ipv4AddressL->setText(addresses.join(QLatin1String("\n")));
        ui->ipv4RouterL->setText(routers.join(QLatin1String("\n")));
        ui->ipv4DnsL->setText(nameservers.join(QLatin1String(", ")));
        ui->ipv4SearchDomainsL->setText(domains.join(QLatin1String("\n")));
        showIp4Config(ipConfig.isValid());
    } else {
        ui->ipv6AddressL->setText(addresses.join(QLatin1String("\n")));
        ui->ipv6RouterL->setText(routers.join(QLatin1String("\n")));
        ui->ipv6DnsL->setText(nameservers.join(QLatin1String(", ")));
        ui->ipv6SearchDomainsL->setText(domains.join(QLatin1String("\n")));
        showIp6Config(ipConfig.isValid());
    }

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

    QString name = modelIndex.data().toString();
    QString newConnectionPath = modelIndex.data(AvailableConnectionsModel::RoleConectionPath).toString();
    if (newConnectionPath.isEmpty()) {
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
        updateActiveConnection();
        if (reply.isError()) {
            KMessageBox::error(this,
                               i18n("Failed to activate network %1:\n%2", name, reply.error().message()));
        }
    } else {
        QString oldConnectionPath;
        if (m_device->activeConnection()) {
            oldConnectionPath = m_device->activeConnection()->connection()->path();
        }

        if (newConnectionPath != oldConnectionPath) {
            QDBusPendingReply<QDBusObjectPath> reply;
            reply = NetworkManager::activateConnection(newConnectionPath, m_device->uni(), QString());
            reply.waitForFinished();
            updateActiveConnection();
            if (reply.isError()) {
                KMessageBox::error(this,
                                   i18n("Failed to activate network %1:\n%2", name, reply.error().message()));
            }
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

void TabDeviceInfo::activeAccessPointChanged(const QString &uni)
{
    WirelessDevice *wifi = qobject_cast<WirelessDevice*>(sender());
    AccessPoint::Ptr accessPoint = wifi->findAccessPoint(uni);
    if (accessPoint) {
        connect(accessPoint.data(), SIGNAL(signalStrengthChanged(int)),
                this, SLOT(signalStrengthChanged()), Qt::UniqueConnection);
    }
}

void TabDeviceInfo::signalStrengthChanged()
{
    NetworkManager::AccessPoint *accessPoint = qobject_cast<NetworkManager::AccessPoint*>(sender());
    if (accessPoint) {
        WirelessDevice::Ptr wifi = m_device.dynamicCast<WirelessDevice>();
        if (wifi->activeAccessPoint() && wifi->activeAccessPoint()->uni() == accessPoint->uni()) {
            updateState();
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

void TabDeviceInfo::setTurnOffWwanText(bool enabled)
{
    if (enabled) {
        ui->turnOff->setText(i18n("Turn off mobile broadband"));
    } else {
        ui->turnOff->setText(i18n("Turn on mobile broadband"));
    }
}

void TabDeviceInfo::showIp4Config(bool show)
{
    kDebug() << m_ip4Visible << show;
    if (m_ip4Visible != show) {
        m_ip4Visible = show;
        ui->ip4L->setEnabled(show);
        ui->ip4AddressL->setEnabled(show);
        ui->ipv4AddressL->setEnabled(show);
        ui->ip4DnsL->setEnabled(show);
        ui->ipv4DnsL->setEnabled(show);
        ui->ip4RouterL->setEnabled(show);
        ui->ipv4RouterL->setEnabled(show);
        ui->ip4SearchDomainsL->setEnabled(show);
        ui->ipv4SearchDomainsL->setEnabled(show);
    }
}

void TabDeviceInfo::showIp6Config(bool show)
{
    kDebug() << m_ip6Visible << show;
    if (m_ip6Visible != show) {
        m_ip6Visible = show;
        ui->ip6L->setEnabled(show);
        ui->ip6AddressL->setEnabled(show);
        ui->ipv6AddressL->setEnabled(show);
        ui->ip6DnsL->setEnabled(show);
        ui->ipv6DnsL->setEnabled(show);
        ui->ip6RouterL->setEnabled(show);
        ui->ipv6RouterL->setEnabled(show);
        ui->ip6SearchDomainsL->setEnabled(show);
        ui->ipv6SearchDomainsL->setEnabled(show);
    }
}
