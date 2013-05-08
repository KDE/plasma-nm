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

#include "modelitem.h"

#include <KLocalizedString>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/VpnConnection>

#include <NetworkManagerQt/settings/VpnSetting>
#include <NetworkManagerQt/settings/WirelessSetting>

#include <ModemManagerQt/modemgsmnetworkinterface.h>

#include "model.h"
#include "uiutils.h"

#include "debug.h"

ModelItem::ModelItem(const QString& device, QObject * parent):
    QObject(parent),
    m_connected(false),
    m_connecting(false),
    m_secure(false),
    m_signal(0),
    m_sectionType(ModelItem::Unknown),
    m_type(NetworkManager::Settings::ConnectionSettings::Unknown)
{
    if (!device.isEmpty()) {
        setDevice(device);
    }
}

ModelItem::~ModelItem()
{
}

bool ModelItem::connected() const
{
    return m_connected;
}

bool ModelItem::connecting() const
{
    return m_connecting;
}

bool ModelItem::secure() const
{
    return m_secure;
}

QString ModelItem::details() const
{
    return m_details;
}

QString ModelItem::deviceName() const
{
    return m_device;
}

QString ModelItem::icon() const
{
    switch (m_type) {
        case NetworkManager::Settings::ConnectionSettings::Adsl:
            return "modem";
            break;
        case NetworkManager::Settings::ConnectionSettings::Bluetooth:
            return "preferences-system-bluetooth";
            break;
        case NetworkManager::Settings::ConnectionSettings::Bond:
            break;
        case NetworkManager::Settings::ConnectionSettings::Bridge:
            break;
        case NetworkManager::Settings::ConnectionSettings::Cdma:
            return "phone";
            break;
        case NetworkManager::Settings::ConnectionSettings::Gsm:
            return "phone";
            break;
        case NetworkManager::Settings::ConnectionSettings::Infiniband:
            break;
        case NetworkManager::Settings::ConnectionSettings::OLPCMesh:
            break;
        case NetworkManager::Settings::ConnectionSettings::Pppoe:
            return "modem";
            break;
        case NetworkManager::Settings::ConnectionSettings::Vlan:
            break;
        case NetworkManager::Settings::ConnectionSettings::Vpn:
            return "secure-card";
            break;
        case NetworkManager::Settings::ConnectionSettings::Wimax:
            break;
        case NetworkManager::Settings::ConnectionSettings::Wired:
            if (connected()) {
                return "network-wired-activated";
            } else {
                return "network-wired";
            }
            break;
        case NetworkManager::Settings::ConnectionSettings::Wireless:
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
            break;
        default:
            return "network-wired";
            break;
    }

    return "network-wired";
}

QString ModelItem::name() const
{
    return m_name;
}

QString ModelItem::uuid() const
{
    return m_uuid;
}

QString ModelItem::ssid() const
{
    return m_ssid;
}

QString ModelItem::sectionType() const
{
    if (m_connected) {
        return i18n("Active connections");
    } else if (!m_uuid.isEmpty()) {
        return i18n("Previous connections");
    } else {
        return i18n("Unknown connections");
    }
}

QString ModelItem::activeConnectionPath() const
{
    return m_activePath;
}

QString ModelItem::connectionPath() const
{
    return m_connectionPath;
}

QString ModelItem::devicePath() const
{
    return m_devicePath;
}

QString ModelItem::specificPath() const
{
    return m_accessPointPath;
}

int ModelItem::signal() const
{
    return m_signal;
}

NetworkManager::Settings::ConnectionSettings::ConnectionType ModelItem::type() const
{
    return m_type;
}

void ModelItem::updateDetails()
{
    QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    m_details = "<qt><table>";

    // Initialize objects
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

    foreach (const QString& key, m_detailKeys) {
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
                if (device->ipV4Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }
        } else if (key == "ipv4:gateway") {
            if (device && device->ipV4Config().isValid() && m_connected) {
                if (device->ipV4Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV4Config().addresses().first().gateway();
                m_details += QString(format).arg(i18n("IPv4 Gateway:"), addr.toString());
            }
        } else if (key == "ipv6:address") {
            if (device && device->ipV6Config().isValid() && m_connected) {
                if (device->ipV6Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }
        } else if (key == "ipv6:gateway") {
            if (device && device->ipV6Config().isValid() && m_connected) {
                if (device->ipV6Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV6Config().addresses().first().gateway();
                m_details += QString(format).arg(i18n("IPv6 Gateway:"), addr.toString());
            }
        } else if (key == "interface:driver") {
            if (device) {
                m_details += QString(format).arg(i18n("Driver:"), device->driver());
            }
        }
    }

    if (m_type == NetworkManager::Settings::ConnectionSettings::Bluetooth) {
        NetworkManager::BluetoothDevice::Ptr btDevice;
        if (device) {
            btDevice = device.objectCast<NetworkManager::BluetoothDevice>();
        }

        foreach (const QString& key, m_detailKeys) {
            if (key == "bluetooth:name") {
                if (btDevice) {
                    m_details += QString(format).arg(i18n("Name:"), btDevice->name());
                }
            } else if (key == "interface:hardwareAddress") {
                if (btDevice) {
                    m_details += QString(format).arg(i18n("MAC Address:"), btDevice->hardwareAddress());
                }
            } else if (key == "interface:driver") {
                if (btDevice) {
                    m_details += QString(format).arg(i18n("Driver:"), device->driver());
                }
            }
        }
    } else if (m_type == NetworkManager::Settings::ConnectionSettings::Gsm) {
        NetworkManager::ModemDevice::Ptr modemDevice;
        ModemManager::ModemGsmNetworkInterface::Ptr modemNetwork;
        if (device) {
            modemDevice = device.objectCast<NetworkManager::ModemDevice>();
        }
        if (modemDevice) {
            modemNetwork = modemDevice->getModemNetworkIface().objectCast<ModemManager::ModemGsmNetworkInterface>();
        }

        foreach (const QString& key, m_detailKeys) {
            if (key == "mobile:operator") {
                if (modemNetwork) {
                    m_details += QString(format).arg(i18n("Operator:"), modemNetwork->getRegistrationInfo().operatorName);
                }
            } else if (key == "mobile:quality") {
                if (modemNetwork) {
                    m_details += QString(format).arg(i18n("Signal quality:"), QString("%1%").arg(modemNetwork->getSignalQuality()));
                }
            } else if (key == "mobile:technology") {
                if (modemNetwork) {
                    m_details += QString(format).arg(i18n("Access technology:"), QString("%1/%2").arg(UiUtils::convertTypeToString(modemNetwork->type()), UiUtils::convertAccessTechnologyToString(modemNetwork->getAccessTechnology())));
                }
            } else if (key == "mobile:mode") {
                if (modemNetwork) {
                    m_details += QString(format).arg(i18n("Allowed Mode"), UiUtils::convertAllowedModeToString(modemNetwork->getAllowedMode()));
                }
            } else if (key == "mobile:band") {
                if (modemNetwork) {
                    m_details += QString(format).arg(i18n("Frequency Band:"), UiUtils::convertBandToString(modemNetwork->getBand()));
                }
            } else if (key == "mobile:unlock") {
                if (modemNetwork) {
                    m_details += QString(format).arg(i18n("Unlock Required:"), modemNetwork->unlockRequired());
                }
            } else if (key == "mobile:imei") {
                if (modemDevice) {
                    ModemManager::ModemGsmCardInterface::Ptr modemCard;
                    modemCard = modemDevice->getModemCardIface();
                    if (modemCard) {
                        m_detailKeys += QString(format).arg(i18n("IMEI:"), modemCard->getImei());
                    }
                }
            } else if (key == "mobile:imsi") {
                if (modemDevice) {
                    ModemManager::ModemGsmCardInterface::Ptr modemCard;
                    modemCard = modemDevice->getModemCardIface();
                    if (modemCard) {
                        m_detailKeys += QString(format).arg(i18n("IMSI:"), modemCard->getImsi());
                    }
                }
            }
        }
    } else if (m_type == NetworkManager::Settings::ConnectionSettings::Wired) {
        NetworkManager::WiredDevice::Ptr wiredDevice;
        if (device) {
            wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        }

        foreach (const QString& key, m_detailKeys) {
            if (key == "interface:bitrate") {
                if (wiredDevice && m_connected) {
                    m_details += QString(format).arg(i18n("Connection speed:"), UiUtils::connectionSpeed(wiredDevice->bitRate()));
                }
            } else if (key == "interface:hardwareaddress") {
                if (wiredDevice) {
                    m_details += QString(format).arg(i18n("MAC Address:"), wiredDevice->permanentHardwareAddress());
                }
            }
        }
    } else if (m_type == NetworkManager::Settings::ConnectionSettings::Wireless) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice;
        if (device) {
            wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        }
        NetworkManager::WirelessNetwork::Ptr network;
        if (wirelessDevice) {
            network = wirelessDevice->findNetwork(m_ssid);
        }
        NetworkManager::AccessPoint::Ptr ap;
        if (network) {
            ap = network->referenceAccessPoint();
        }

        foreach (const QString& key, m_detailKeys) {
            if (key == "interface:bitrate") {
                if (wirelessDevice && m_connected) {
                    m_details += QString(format).arg(i18n("Connection speed:"), UiUtils::connectionSpeed(wirelessDevice->bitRate()));
                }
            } else if (key == "interface:hardwareaddress") {
                if (wirelessDevice) {
                    m_details += QString(format).arg(i18n("MAC Address:"), wirelessDevice->permanentHardwareAddress());
                }
            } else if (key == "wireless:signal") {
                if (network) {
                    m_details += QString(format).arg(i18n("Signal strength:"), i18n("%1%", network->signalStrength()));
                }
            } else if (key == "wireless:ssid") {
                if (network) {
                    m_details += QString(format).arg(i18n("Access point (SSID):"), network->ssid());
                }
            } else if (key == "wireless:accesspoint") {
                if (ap) {
                    m_details += QString(format).arg(i18n("Access point (BSSID):"), ap->hardwareAddress());
                }
            } else if (key == "wireless:channel") {
                if (ap) {
                    m_details += QString(format).arg(i18nc("Wifi AP channel and frequency", "Channel:"), i18n("%1 (%2 MHz)", UiUtils::findChannel(ap->frequency()), ap->frequency()));
                }
            }
        }
    } else if (m_type == NetworkManager::Settings::ConnectionSettings::Vpn) {
        NetworkManager::ActiveConnection::Ptr active = NetworkManager::findActiveConnection(m_activePath);
        NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnection(m_connectionPath);
        NetworkManager::Settings::ConnectionSettings::Ptr connectionSettings;
        NetworkManager::Settings::VpnSetting::Ptr vpnSetting;
        NetworkManager::VpnConnection::Ptr vpnConnection;

        if (connection) {
            connectionSettings = connection->settings();
        }
        if (connectionSettings) {
            vpnSetting = connectionSettings->setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
        }

        if (active) {
            vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()));
        }

        foreach (const QString& key, m_detailKeys) {
            if (key == "vpn:plugin") {
                if (vpnSetting) {
                    m_details += QString(format).arg(i18n("VPN plugin:"), vpnSetting->serviceType().section('.', -1));
                }
            } else if (key == "vpn:banner") {
                if (vpnConnection) {
                    m_details += QString(format).arg(i18n("Banner:"), vpnConnection->banner().simplified());
                }
            }
        }
    }

    m_details += "</table></qt>";
}

bool ModelItem::operator==(const ModelItem* item) const
{
    if (((item->uuid() == this->uuid() && !item->uuid().isEmpty() && !this->uuid().isEmpty()) ||
         (item->name() == this->name() && !item->name().isEmpty() && !this->name().isEmpty() && item->type() == this->type()) ||
         (item->ssid() == this->ssid() && !item->ssid().isEmpty() && !this->ssid().isEmpty())) &&
         ((item->devicePath() == this->devicePath() && !item->devicePath().isEmpty() && !this->devicePath().isEmpty()) ||
          (item->type() == NetworkManager::Settings::ConnectionSettings::Vpn && this->type() == NetworkManager::Settings::ConnectionSettings::Vpn))) {
        return true;
    }
    return false;
}

void ModelItem::setActiveConnection(const QString& active)
{
    m_activePath = active;

    NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(active);

    if (activeConnection) {
        if (activeConnection->state() == NetworkManager::ActiveConnection::Activating) {
            m_connecting = true;
            m_connected = false;
            NMItemDebug() << name() << ": activating";
        } else if (activeConnection->state() == NetworkManager::ActiveConnection::Activated) {
            NMItemDebug() << name() << ": activated";
            m_connected = true;
            m_connecting = false;
        }
    } else {
        m_connecting = false;
        m_connected = false;
    }

    updateDetails();
}

void ModelItem::setDevice(const QString& device)
{
    m_devicePath = device;
    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);

    if (dev) {
        if (dev->ipInterfaceName().isEmpty()) {
            m_device = dev->interfaceName();
        } else {
            m_device = dev->ipInterfaceName();
        }
        m_devicePath = dev->uni();
        updateDetails();
    } else {
        m_devicePath.clear();
    }
}

void ModelItem::setConnection(const QString& connection)
{
    m_connectionPath = connection;
    NetworkManager::Settings::Connection::Ptr con = NetworkManager::Settings::findConnection(m_connectionPath);

    if (con) {
        setConnectionSettings(con->settings());
    } else {
        m_connectionPath.clear();
        m_name.clear();
        m_uuid.clear();
        m_activePath.clear();

        if (!m_ssid.isEmpty()) {
            m_name = m_ssid;
        }
    }
}

void ModelItem::setDetailKeys(const QStringList& keys)
{
    m_detailKeys = keys;

    updateDetails();
}

void ModelItem::setConnectionSettings(const NetworkManager::Settings::ConnectionSettings::Ptr& settings)
{
    m_uuid = settings->uuid();
    m_name = settings->id();
    m_type = settings->connectionType();

    if (type() == NetworkManager::Settings::ConnectionSettings::Wireless) {
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
                setConnection(QString());
                NetworkManager::WirelessNetwork::Ptr wifiNetwork = wifiDevice->findNetwork(previousSsid);
                if (wifiNetwork) {
                    setWirelessNetwork(previousSsid);
                }
            } else {
                setWirelessNetwork(m_ssid);
            }
        }
    }

    updateDetails();
}

void ModelItem::setWirelessNetwork(const QString& ssid)
{
    m_ssid = ssid;

    NetworkManager::WirelessNetwork::Ptr network;
    NetworkManager::WirelessDevice::Ptr wifiDevice = NetworkManager::findNetworkInterface(m_devicePath).objectCast<NetworkManager::WirelessDevice>();

    if (wifiDevice) {
        network = wifiDevice->findNetwork(ssid);
    }

    if (network) {
        m_accessPointPath = network->referenceAccessPoint()->uni();
        m_ssid = network->ssid();
        m_signal = network->signalStrength();
        m_type = NetworkManager::Settings::ConnectionSettings::Wireless;

        if (m_name.isEmpty() || m_connectionPath.isEmpty()) {
            m_name = m_ssid;
        }

        NetworkManager::AccessPoint::Ptr ap = wifiDevice->findAccessPoint(m_accessPointPath);

        if (ap && ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
            m_secure = true;
        }
    } else {
        m_ssid.clear();
        m_signal = 0;
        m_type = NetworkManager::Settings::ConnectionSettings::Unknown;
        m_secure = false;
    }

    updateDetails();
}

void ModelItem::updateActiveConnectionState(NetworkManager::ActiveConnection::State state)
{
    if (state == NetworkManager::ActiveConnection::Deactivated ||
        state == NetworkManager::ActiveConnection::Deactivating) {
        NMItemDebug() << name() << ": disconnected";
        m_connecting = false;
        m_connected = false;
    } else if (state == NetworkManager::ActiveConnection::Activated) {
        NMItemDebug() << name() << ": activated";
        m_connecting = false;
        m_connected = true;
    } else if (state == NetworkManager::ActiveConnection::Activating) {
        NMItemDebug() << name() << ": activating";
        m_connecting = true;
        m_connected = false;
    }

    updateDetails();

    NMItemDebug() << name() << ": state has been changed to " << state;
}

void ModelItem::updateAccessPoint(const QString& ap)
{
    m_accessPointPath = ap;

    updateDetails();

    NMItemDebug() << name() << ": access point changed to " << m_accessPointPath;
}

void ModelItem::updateSignalStrenght(int strength)
{
    m_signal = strength;

    updateDetails();

    NMItemDebug() << name() << ": signal strength changed to " << m_signal;
}
