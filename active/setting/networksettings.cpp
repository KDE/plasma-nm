/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "networksettings.h"
#include "networkmodel.h"
#include "uiutils.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>

#include <KDebug>
#include <KLocale>

class NetworkSettingsPrivate {
public:
    NetworkSettings *q;
    QObject *networkModel;
    QString details;
    QString icon;
    QString settingName;
    QString status;

    QString path;
    NetworkModelItem::NetworkType type;

    void initNetwork();
};

NetworkSettings::NetworkSettings()
{
    d = new NetworkSettingsPrivate;
    d->q = this;
    d->networkModel = 0;
    d->settingName = i18n("Network Setting");
    d->status = i18n("Network status and control");
    d->type = NetworkModelItem::Undefined;

    d->initNetwork();
    kDebug() << "NetworkSettings module loaded.";
}

NetworkSettings::~NetworkSettings()
{
    kDebug() << "========================== NetworkSettings destroyed";
    delete d;
}

void NetworkSettingsPrivate::initNetwork()
{
    QAbstractItemModel * _networkModel = new NetworkModel(q);
    networkModel = _networkModel;
}

QObject* NetworkSettings::networkModel()
{
    return d->networkModel;
}

void NetworkSettings::setNetworkModel(QObject *networkModel)
{
    if ( d->networkModel != networkModel) {
        d->networkModel = networkModel;
        emit networkModelChanged();
    }
}

QString NetworkSettings::details() const
{
    return d->details;
}

void NetworkSettings::setDetails(const QString &details)
{
    if (d->details != details) {
        d->details = details;
        emit detailsChanged();
    }
}

QString NetworkSettings::icon() const
{
    return d->icon;
}

void NetworkSettings::setIcon(const QString &icon)
{
    if (d->icon != icon) {
        d->icon = icon;
        emit iconChanged();
    }
}

QString NetworkSettings::settingName() const
{
    return d->settingName;
}

void NetworkSettings::setSettingName(const QString &name)
{
    if (d->settingName != name) {
        d->settingName = name;
        emit settingNameChanged();
    }
}

QString NetworkSettings::status() const
{
    return d->status;
}

void NetworkSettings::setStatus(const QString &status)
{
    if (d->status != status) {
        d->status = status;
        emit statusChanged();
    }
}

void NetworkSettings::setNetwork(uint type, const QString &path)
{
    if (d->type == NetworkModelItem::Vpn) {
        disconnect(NetworkManager::notifier(), 0, this, 0);
    } else if (d->type != NetworkModelItem::Undefined) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);
        if (device) {
            disconnect(device.data(), 0, this, 0);
        }
    }

    d->type = (NetworkModelItem::NetworkType) type;
    d->path = path;

    if (d->type == NetworkModelItem::Ethernet ||
        d->type == NetworkModelItem::Modem ||
        d->type == NetworkModelItem::Wifi) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);

        if (device) {
            connect(device.data(), SIGNAL(connectionStateChanged()),
                    SLOT(updateStatus()), Qt::UniqueConnection);
            connect(device.data(), SIGNAL(activeConnectionChanged()),
                    SLOT(updateStatus()), Qt::UniqueConnection);
            connect(device.data(), SIGNAL(connectionStateChanged()),
                    SLOT(updateDetails()), Qt::UniqueConnection);
            connect(device.data(), SIGNAL(activeConnectionChanged()),
                    SLOT(updateDetails()), Qt::UniqueConnection);
        }
    }

    if (d->type == NetworkModelItem::Vpn) {
        foreach (const NetworkManager::ActiveConnection::Ptr & activeConnection, NetworkManager::activeConnections()) {
            if (activeConnection && activeConnection->vpn()) {
                connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                        SLOT(updateStatus()), Qt::UniqueConnection);
                connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                        SLOT(updateDetails()), Qt::UniqueConnection);
            }
        }

        connect(NetworkManager::notifier(), SIGNAL(activeConnectionAdded(QString)),
                SLOT(activeConnectionAdded(QString)), Qt::UniqueConnection);
    }

    updateDetails();
    updateIcon();
    updateSettingName();
    updateStatus();
}

void NetworkSettings::activeConnectionAdded(const QString &active)
{
    NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(active);

    if (activeConnection && activeConnection->vpn()) {
        connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(updateStatus()), Qt::UniqueConnection);
        connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(updateDetails()), Qt::UniqueConnection);
        updateDetails();
        updateStatus();
    }
}

void NetworkSettings::updateDetails()
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details = "<qt><table>";

    QStringList detailKeys;
    detailKeys << "interface:status" << "interface:bitrate" << "interface:hardwareaddress" << "ipv4:address" << "ipv6:address" << "wireless:ssid" << "wireless:signal";
    detailKeys << "wireless:security" << "mobile:operator" << "mobile:quality" << "mobile:technology" << "vpn:plugin" << "vpn:banner";


    if (d->type != NetworkModelItem::Vpn) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(d->path);
        if (device) {
            const NetworkManager::Device::State state = device->state();
            const bool connected = (state == NetworkManager::Device::Activated);
            const bool connecting = (state >= NetworkManager::Device::CheckingIp && state <= NetworkManager::Device::Preparing);
            if (d->type == NetworkModelItem::Ethernet) {
                details += UiUtils::deviceDetails(device, NetworkManager::ConnectionSettings::Wired, connected, connecting, detailKeys, format);
                NetworkManager::WiredDevice::Ptr wiredDevice;
                wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
                details += UiUtils::wiredDetails(wiredDevice, connected, detailKeys, format);
            } else if (d->type == NetworkModelItem::Modem) {
                details += UiUtils::deviceDetails(device, NetworkManager::ConnectionSettings::Gsm, connected, connecting, detailKeys, format);
                NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();
                details += UiUtils::modemDetails(modemDevice, detailKeys, format);
            } else if (d->type == NetworkModelItem::Wifi) {
                details += UiUtils::deviceDetails(device, NetworkManager::ConnectionSettings::Wireless, connected, connecting, detailKeys, format);
                NetworkManager::WirelessDevice::Ptr wirelessDevice;
                wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
                QString ssid;
                NetworkManager::AccessPoint::Ptr ap = wirelessDevice->activeAccessPoint();
                if (ap) {
                    ssid = wirelessDevice->activeAccessPoint()->ssid();
                }
                NetworkManager::WirelessNetwork::Ptr network;
                if (wirelessDevice) {
                    network = wirelessDevice->findNetwork(ssid);
                }
                details += UiUtils::wirelessDetails(wirelessDevice, network, ap, connected, detailKeys, format);
            }
        }
    } else {
        NetworkManager::ActiveConnection::Ptr active;
        foreach (const NetworkManager::ActiveConnection::Ptr & activeConnection, NetworkManager::activeConnections()) {
            if (activeConnection && activeConnection->vpn() &&
                (activeConnection->state() == NetworkManager::ActiveConnection::Activated || activeConnection->state() == NetworkManager::ActiveConnection::Activating)) {
                active = activeConnection;
            }
        }

        if (active) {
            NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(active->connection()->path());
            NetworkManager::ConnectionSettings::Ptr connectionSettings;
            NetworkManager::VpnSetting::Ptr vpnSetting;
            NetworkManager::VpnConnection::Ptr vpnConnection;

            if (connection) {
                connectionSettings = connection->settings();
            }
            if (connectionSettings) {
                vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
            }

            if (active) {
                vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()), &QObject::deleteLater);
            }
            details += UiUtils::vpnDetails(vpnConnection, vpnSetting, detailKeys, format);
        }
    }
    details += "</table></qt>";

    setDetails(details);
}

void NetworkSettings::updateIcon()
{
    if (d->type != NetworkModelItem::Vpn) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(d->path);
        if (device) {
            setIcon(UiUtils::iconName(device));
        }
    } else {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(d->path);
        if (connection) {
            QString title;
            setIcon(UiUtils::iconAndTitleForConnectionSettingsType(NetworkManager::ConnectionSettings::Vpn, title));
        }
    }
}

void NetworkSettings::updateSettingName()
{
    switch (d->type) {
    case NetworkModelItem::Ethernet:
        setSettingName(i18n("Ethernet Setting"));
        break;
    case NetworkModelItem::Modem:
        setSettingName(i18n("Modem Setting"));
        break;
    case NetworkModelItem::Vpn:
        setSettingName(i18n("VPN Setting"));
        break;
    case NetworkModelItem::Wifi:
        setSettingName(i18n("Wireless Setting"));
        break;
    default:
        break;
    }
}

void NetworkSettings::updateStatus()
{
    if (d->type != NetworkModelItem::Vpn) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(d->path);
        if (device) {
            NetworkManager::ActiveConnection::Ptr activeConnection = device->activeConnection();
            if (activeConnection) {
                setStatus(UiUtils::connectionStateToString(device->state(), activeConnection->connection()->name()));
            } else {
                setStatus(UiUtils::connectionStateToString(device->state()));
            }
        }
    } else if (d->type == NetworkModelItem::Vpn) {
        // TODO: maybe check for the case when there are two active VPN connections
        foreach (const NetworkManager::ActiveConnection::Ptr & activeConnection, NetworkManager::activeConnections()) {
            if (activeConnection && activeConnection->vpn()) {
                if (activeConnection->state() == NetworkManager::ActiveConnection::Unknown) {
                    setStatus(UiUtils::connectionStateToString(NetworkManager::Device::UnknownState));
                } else if (activeConnection->state() == NetworkManager::ActiveConnection::Activating) {
                    setStatus(i18n("Connecting"));
                } else if (activeConnection->state() == NetworkManager::ActiveConnection::Activated) {
                    setStatus(UiUtils::connectionStateToString(NetworkManager::Device::Activated) % i18n(" to %1").arg(activeConnection->connection()->name()));
                } else if (activeConnection->state() == NetworkManager::ActiveConnection::Deactivating) {
                    setStatus(UiUtils::connectionStateToString(NetworkManager::Device::Deactivating));
                } else if (activeConnection->state() == NetworkManager::ActiveConnection::Deactivated) {
                    setStatus(UiUtils::connectionStateToString(NetworkManager::Device::Disconnected));
                }
            }
        }
    } else {
        setStatus(UiUtils::connectionStateToString(NetworkManager::Device::UnknownState));
    }
}

#include "networksettings.moc"
