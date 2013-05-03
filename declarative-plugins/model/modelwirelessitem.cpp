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
#include "modelwirelessitem.h"
#include "uiutils.h"
#include "debug.h"

#include <NetworkManagerQt/wirelessdevice.h>
#include <NetworkManagerQt/settings/802-11-wireless.h>
#include <NetworkManagerQt/manager.h>

#include <KLocalizedString>

ModelWirelessItem::ModelWirelessItem(const NetworkManager::Device::Ptr &device, QObject* parent):
    ModelItem(device, parent),
    m_previousSignal(0),
    m_signal(0),
    m_secure(false)
{
    m_type = NetworkManager::Settings::ConnectionSettings::Wireless;
}

ModelWirelessItem::~ModelWirelessItem()
{
}

QString ModelWirelessItem::icon() const
{
    if (m_signal < 13) {
        return "network-wireless-connected-00";
    } else if (m_signal < 38) {
        return "network-wireless-connected-25";
    } else if (m_signal < 63) {
        return "network-wireless-connected-50";
    } else if (m_signal < 88) {
        return "network-wireless-connected-75";
    } else {
        return "network-wireless-connected-100";
    }
}

int ModelWirelessItem::signal() const
{
    return m_signal;
}

QString ModelWirelessItem::ssid() const
{
    return m_ssid;
}

bool ModelWirelessItem::secure() const
{
    return m_secure;
}

QString ModelWirelessItem::specificPath() const
{
    if (m_network) {
        return m_network->referenceAccessPoint()->uni();
    }

    return QString();
}

bool ModelWirelessItem::shouldBeRemovedSpecific() const
{
    return m_network.isNull();
}

void ModelWirelessItem::updateDetails()
{
    QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    m_details = "<qt><table>";

    // Initialize objects
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);
    NetworkManager::WirelessDevice::Ptr wireless;
    if (device) {
        wireless = device.objectCast<NetworkManager::WirelessDevice>();
    }
    NetworkManager::WirelessNetwork::Ptr network;
    if (wireless) {
        network = wireless->findNetwork(m_ssid);
    }
    NetworkManager::AccessPoint::Ptr ap;
    if (network) {
        ap = network->referenceAccessPoint();
    }

    foreach (const QString & key, m_detailKeys) {
        if (key == "interface:type") {
            if (m_type != NetworkManager::Settings::ConnectionSettings::Unknown) {
                m_details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::Settings::ConnectionSettings::typeAsString(m_type));
            }
        } else if (key == "interface:status") {
            QString status = i18n("Disconnected");
            if (m_connecting) {
                status = i18n("Connecting");
            } else if (m_connected) {
                status = i18n("Connected");
            }
            m_details += QString(format).arg(i18n("Status"), status);
        } else if (key == "interface:name") {
            if (device) {
                QString name;
                if (device->ipInterfaceName().isEmpty()) {
                    name = device->interfaceName();
                } else {
                    name = device->ipInterfaceName();
                }
                m_details += QString(format).arg(i18n("System name:"), name);
            }
        } else if (key == "ipv4:address") {
            if (device && device->ipV4Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }
        } else if (key == "ipv4:gateway") {
            if (device && device->ipV4Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV4Config().addresses().first().gateway();
                m_details += QString(format).arg(i18n("IPv4 Gateway:"), addr.toString());
            }
        } else if (key == "ipv6:address") {
            if (device && device->ipV6Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }
        } else if (key == "ipv6:gateway") {
            if (device && device->ipV6Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV6Config().addresses().first().gateway();
                m_details += QString(format).arg(i18n("IPv6 Gateway:"), addr.toString());
            }
        } else if (key == "interface:driver") {
            if (device) {
                m_details += QString(format).arg(i18n("Driver:"), device->driver());
            }
        } else if (key == "interface:bitrate") {
            if (wireless && m_connected) {
                m_details += QString(format).arg(i18n("Connection speed:"), UiUtils::connectionSpeed(wireless->bitRate()));
            }
        } else if (key == "interface:hardwareaddress") {
            if (wireless) {
                m_details += QString(format).arg(i18n("MAC Address:"), wireless->permanentHardwareAddress());
            }
        } else if (key == "wireless:signal") {
            if (network) {
                m_details += QString(format).arg(i18n("Signal strength:"), i18n("%1%", network->signalStrength()));
            }
        } else if (key == "wireless:ssid") {
            if (network) {
                m_details += QString(format).arg(i18n("Access point (SSID):"), network->ssid());
            }
        } else if (key == "wireless:bssid") {
            if (ap) {
                m_details += QString(format).arg(i18n("Access point (BSSID):"), ap->hardwareAddress());
            }
        } else if (key == "wireless:channel") {
            if (ap) {
                m_details += QString(format).arg(i18nc("Wifi AP channel and frequency", "Channel:"), i18n("%1 (%2 MHz)", UiUtils::findChannel(ap->frequency()), ap->frequency()));
            }
        }
    }

    m_details += "</table></qt>";

    Q_EMIT itemChanged();
}

void ModelWirelessItem::setConnection(const NetworkManager::Settings::Connection::Ptr & connection)
{
    ModelItem::setConnection(connection);

    if (!m_connection) {
        if (m_network) {
            m_name = m_ssid;
        } else {
            m_ssid.clear();
            m_secure = false;
        }
    }
}

void ModelWirelessItem::setConnectionSettings(const NetworkManager::Settings::ConnectionSettings::Ptr &settings)
{
    ModelItem::setConnectionSettings(settings);
    bool changed = false;
    QString previousSsid;
    if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
        NetworkManager::Settings::WirelessSetting::Ptr wirelessSetting;
        wirelessSetting = settings->setting(NetworkManager::Settings::Setting::Wireless).dynamicCast<NetworkManager::Settings::WirelessSetting>();
        if (m_ssid != wirelessSetting->ssid()) {
            if (!m_ssid.isEmpty()) {
                changed = true;
            }
            previousSsid = m_ssid;
            m_ssid = wirelessSetting->ssid();
            if (!wirelessSetting->security().isEmpty()) {
                m_secure = true;
            }
        }

        if (!changed) {
            return;
        }

        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(devicePath());
        if (!device) {
           return;
        }
        NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
        if (!wifiDevice) {
            return;
        }
        NetworkManager::WirelessNetwork::Ptr newWifiNetwork = wifiDevice->findNetwork(m_ssid);

        if (!newWifiNetwork) {
            setConnection(NetworkManager::Settings::Connection::Ptr());
            NetworkManager::WirelessNetwork::Ptr wifiNetwork = wifiDevice->findNetwork(previousSsid);
            if (wifiNetwork) {
                setWirelessNetwork(wifiNetwork);
            }
        } else {
            setWirelessNetwork(newWifiNetwork);
        }
    }

    updateDetails();
}

void ModelWirelessItem::setWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network)
{
    m_network = network;

    if (m_network) {
        m_ssid = network->ssid();
        m_previousSignal = m_signal;
        m_signal = m_network->signalStrength();
        m_type = NetworkManager::Settings::ConnectionSettings::Wireless;

        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

        if (device) {
            NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
            NetworkManager::AccessPoint::Ptr ap = wifiDev->findAccessPoint(m_network->referenceAccessPoint()->uni());

            if (ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
                m_secure = true;
            }
        }

        if (!m_connection) {
            m_name = ssid();
        }

        connect(m_network.data(), SIGNAL(signalStrengthChanged(int)),
                SLOT(onSignalStrengthChanged(int)), Qt::UniqueConnection);
        connect(m_network.data(), SIGNAL(referenceAccessPointChanged(QString)),
                SLOT(onAccessPointChanged(QString)), Qt::UniqueConnection);
    } else {
        m_ssid.clear();
        m_signal = 0;
        m_type = NetworkManager::Settings::ConnectionSettings::Unknown;
        m_secure = false;
    }

    updateDetails();
}

NetworkManager::WirelessNetwork::Ptr ModelWirelessItem::wirelessNetwork() const
{
    return m_network;
}

void ModelWirelessItem::onAccessPointChanged(const QString& accessPoint)
{
    Q_UNUSED(accessPoint);

    updateDetails();

    NMItemDebug() << name() << ": access point changed";
}

void ModelWirelessItem::onSignalStrengthChanged(int strength)
{
    m_previousSignal = m_signal;
    m_signal = strength;

    updateDetails();

    NMItemDebug() << name() << ": strength changed to " << strength;
}
