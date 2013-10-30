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
#include <NetworkManagerQt/WimaxDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/VpnConnection>

#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WirelessSetting>

#include <KGlobal>
#include <KLocale>
#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
#include <ModemManagerQt/manager.h>
#include <ModemManagerQt/modem.h>
#endif
#endif
#include "model.h"
#include "uiutils.h"
#include "applet/globalconfig.h"

#include "debug.h"

ModelItem::ModelItem(const QString& device, QObject * parent)
    : QObject(parent)
    , m_signal(0)
    , m_sectionType(ModelItem::Unknown)
    , m_connectionState(NetworkManager::ActiveConnection::Unknown)
    , m_type(NetworkManager::ConnectionSettings::Unknown)
    , m_securityType(NetworkManager::Utils::None)
{
    if (!device.isEmpty()) {
        setDevice(device);
    }
}

ModelItem::~ModelItem()
{
}

NetworkManager::ActiveConnection::State ModelItem::connectionState() const
{
    return m_connectionState;
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
        case NetworkManager::ConnectionSettings::Adsl:
            return "modem";
            break;
        case NetworkManager::ConnectionSettings::Bluetooth:
            if (connectionState() == NetworkManager::ActiveConnection::Activated) {
                return "bluetooth";
            } else {
                return "bluetooth-inactive";
            }
            break;
        case NetworkManager::ConnectionSettings::Bond:
            break;
        case NetworkManager::ConnectionSettings::Bridge:
            break;
        case NetworkManager::ConnectionSettings::Cdma:
            if (m_signal == 0 ) {
                return "network-mobile-0";
            } else if (m_signal < 20) {
                return "network-mobile-20";
            } else if (m_signal < 40) {
                return "network-mobile-40";
            } else if (m_signal < 60) {
                return "network-mobile-60";
            } else if (m_signal < 80) {
                return "network-mobile-80";
            } else {
                return "network-mobile-100";
            }
            break;
        case NetworkManager::ConnectionSettings::Gsm:
            if (m_signal == 0 ) {
                return "network-mobile-0";
            } else if (m_signal < 20) {
                return "network-mobile-20";
            } else if (m_signal < 40) {
                return "network-mobile-40";
            } else if (m_signal < 60) {
                return "network-mobile-60";
            } else if (m_signal < 80) {
                return "network-mobile-80";
            } else {
                return "network-mobile-100";
            }
            break;
        case NetworkManager::ConnectionSettings::Infiniband:
            break;
        case NetworkManager::ConnectionSettings::OLPCMesh:
            break;
        case NetworkManager::ConnectionSettings::Pppoe:
            return "modem";
            break;
        case NetworkManager::ConnectionSettings::Vlan:
            break;
        case NetworkManager::ConnectionSettings::Vpn:
            return "secure-card";
            break;
        case NetworkManager::ConnectionSettings::Wimax:
            break;
        case NetworkManager::ConnectionSettings::Wired:
            if (connectionState() == NetworkManager::ActiveConnection::Activated) {
                return "network-wired-activated";
            } else {
                return "network-wired";
            }
            break;
        case NetworkManager::ConnectionSettings::Wireless:
            if (m_signal == 0 ) {
                if (!m_connectionPath.isEmpty()) {
                    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(m_connectionPath);
                    if (con) {
                        NetworkManager::WirelessSetting::Ptr wirelessSetting;
                        wirelessSetting = con->settings()->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                        if (wirelessSetting && (wirelessSetting->mode() == NetworkManager::WirelessSetting::Adhoc ||
                                                wirelessSetting->mode() == NetworkManager::WirelessSetting::Ap)) {
                            return "network-wireless-100";
                        }
                    }
                }
                return "network-wireless-00";
            } else if (m_signal < 20) {
                return "network-wireless-20";
            } else if (m_signal < 25) {
                return "network-wireless-25";
            } else if (m_signal < 40) {
                return "network-wireless-40";
            } else if (m_signal < 50) {
                return "network-wireless-50";
            } else if (m_signal < 60) {
                return "network-wireless-60";
            } else if (m_signal < 75) {
                return "network-wireless-75";
            } else if (m_signal < 80) {
                return "network-wireless-80";
            } else {
                return "network-wireless-100";
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

QString ModelItem::nspPath() const
{
    return m_nspPath;
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
    if (connectionState() == NetworkManager::ActiveConnection::Activated) {
        return i18n("Active connections");
    } else if (!m_uuid.isEmpty()) {
        return i18n("Previous connections");
    } else {
        return i18n("Unknown connections");
    }
}

QString ModelItem::originalName() const
{
    return name() + " (" + deviceName() + ')';
}

QString ModelItem::uni() const
{
    if (m_type == NetworkManager::ConnectionSettings::Wireless && m_uuid.isEmpty()) {
        return m_ssid + '%' + m_devicePath;
    } else {
        return m_connectionPath + '%' + m_devicePath;
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
    if (type() == NetworkManager::ConnectionSettings::Wimax) {
        return m_nspPath;
    } else if (type() == NetworkManager::ConnectionSettings::Wireless) {
        return m_accessPointPath;
    }

    return QString();
}

int ModelItem::signal() const
{
    return m_signal;
}

NetworkManager::ConnectionSettings::ConnectionType ModelItem::type() const
{
    return m_type;
}

NetworkManager::Utils::WirelessSecurityType ModelItem::securityType() const
{
    return m_securityType;
}

void ModelItem::updateDetails()
{
    m_details = "<qt><table>";

    QStringList detailKeys = GlobalConfig().detailKeys();

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_connectionPath);
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

    m_details += UiUtils::connectionDetails(device, connection, detailKeys);

    if (m_type == NetworkManager::ConnectionSettings::Bluetooth) {
        NetworkManager::BluetoothDevice::Ptr btDevice = device.objectCast<NetworkManager::BluetoothDevice>();
        m_details += UiUtils::bluetoothDetails(btDevice, detailKeys);
    } else if (m_type == NetworkManager::ConnectionSettings::Gsm) {
        NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();
        m_details += UiUtils::modemDetails(modemDevice, detailKeys);
    } else if (m_type == NetworkManager::ConnectionSettings::Wimax) {
        NetworkManager::WimaxNsp::Ptr wimaxNsp;
        NetworkManager::WimaxDevice::Ptr wimaxDevice = device.objectCast<NetworkManager::WimaxDevice>();
        wimaxNsp = wimaxDevice->findNsp(m_nspPath);
        if (wimaxDevice && wimaxNsp) {
            m_details += UiUtils::wimaxDetails(wimaxDevice, wimaxNsp, connection, detailKeys);
        }
    } else if (m_type == NetworkManager::ConnectionSettings::Wired) {
        NetworkManager::WiredDevice::Ptr wiredDevice;
        if (device) {
            wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        }
        m_details += UiUtils::wiredDetails(wiredDevice, connection, detailKeys);
    } else if (m_type == NetworkManager::ConnectionSettings::Wireless) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice;
        if (device) {
            wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        }
        NetworkManager::WirelessNetwork::Ptr network;
        if (wirelessDevice) {
            network = wirelessDevice->findNetwork(m_ssid);
        }
        m_details += UiUtils::wirelessDetails(wirelessDevice, network, connection, detailKeys);
    } else if (m_type == NetworkManager::ConnectionSettings::Vpn) {
        NetworkManager::ActiveConnection::Ptr active = NetworkManager::findActiveConnection(m_activePath);
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_connectionPath);
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
        m_details += UiUtils::vpnDetails(vpnConnection, vpnSetting, detailKeys);
    }

    m_details += "</table></qt>";
}

bool ModelItem::operator==(const ModelItem* item) const
{
    if (!item->uuid().isEmpty() && !uuid().isEmpty()) {
        if (item->devicePath() == devicePath() && item->uuid() == uuid()) {
            return true;
        }
    } else if (item->type() == NetworkManager::ConnectionSettings::Wireless && type() == NetworkManager::ConnectionSettings::Wireless) {
        if (item->ssid() == ssid() && item->devicePath() == devicePath()) {
            return true;
        }
    }

    return false;
}

void ModelItem::setActiveConnection(const QString& active)
{
    m_activePath = active;

    NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(active);

    if (activeConnection) {
        m_connectionState = activeConnection->state();
    } else {
        m_connectionState = NetworkManager::ActiveConnection::Unknown;
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

#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
        if (dev->type() == NetworkManager::Device::Modem) {
            ModemManager::ModemDevice::Ptr modemDevice = ModemManager::findModemDevice(dev->udi());
            if (modemDevice) {
                ModemManager::Modem::Ptr modemInterface = modemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
                if (modemInterface) {
                    updateSignalStrenght(modemInterface->signalQuality().signal);
                }
            }
        }
#endif
#endif
        updateDetails();
    } else {
        m_devicePath.clear();
    }
}

void ModelItem::setConnection(const QString& connection)
{
    m_connectionPath = connection;
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(m_connectionPath);

    if (con && !con->uuid().isEmpty() && !con->name().isEmpty()) {
        setConnectionSettings(con->settings());
    } else {
        m_connectionPath.clear();
        m_name.clear();
        m_uuid.clear();
        m_activePath.clear();
        m_connectionState = NetworkManager::ActiveConnection::Unknown;

        if (!m_ssid.isEmpty()) {
            m_name = m_ssid;
        } else if (!m_nsp.isEmpty()) {
            m_name = m_nsp;
        }
    }
}

void ModelItem::setConnectionSettings(const NetworkManager::ConnectionSettings::Ptr& settings)
{
    m_uuid = settings->uuid();
    m_name = settings->id();
    m_type = settings->connectionType();

    if (m_type == NetworkManager::ConnectionSettings::Wireless) {
        m_securityType = NetworkManager::Utils::securityTypeFromConnectionSetting(settings);
    }

    if (type() == NetworkManager::ConnectionSettings::Wireless) {
        bool changed = false;
        QString previousSsid;
        NetworkManager::WirelessSetting::Ptr wirelessSetting;
        wirelessSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();

        if (m_ssid != wirelessSetting->ssid()) {
            changed = true;
            previousSsid = m_ssid;
            m_ssid = wirelessSetting->ssid();
        }

        if (wirelessSetting->mode() == NetworkManager::WirelessSetting::Adhoc ||
            wirelessSetting->mode() == NetworkManager::WirelessSetting::Ap) {
            updateDetails();
            return;
        }

        if (!changed) {
            updateDetails();
            return;
        }

        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(devicePath());
        if (!device) {
            updateDetails();
            return;
        }

        NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
        if (!wifiDevice) {
            updateDetails();
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

    updateDetails();
}

void ModelItem::setNsp(const QString& nsp)
{
    NetworkManager::WimaxNsp::Ptr wimaxNsp;
    NetworkManager::WimaxDevice::Ptr wimaxDevice = NetworkManager::findNetworkInterface(m_devicePath).objectCast<NetworkManager::WimaxDevice>();

    if (wimaxDevice) {
        wimaxNsp = wimaxDevice->findNsp(nsp);
    }

    if (wimaxNsp) {
        m_nspPath = wimaxNsp->uni();
        m_nsp = wimaxNsp->name();
        m_signal = wimaxNsp->signalQuality();
        m_type = NetworkManager::ConnectionSettings::Wimax;

        if (m_name.isEmpty() || m_connectionPath.isEmpty()) {
            m_name = m_nsp;
        }
    } else {
        m_nsp.clear();
        m_signal = 0;
        m_type = NetworkManager::ConnectionSettings::Unknown;
    }

    updateDetails();
}

void ModelItem::setWirelessNetwork(const QString& ssid)
{
    NetworkManager::WirelessNetwork::Ptr network;
    NetworkManager::WirelessDevice::Ptr wifiDevice = NetworkManager::findNetworkInterface(m_devicePath).objectCast<NetworkManager::WirelessDevice>();

    if (wifiDevice) {
        network = wifiDevice->findNetwork(ssid);
    }

    if (network) {
        m_accessPointPath = network->referenceAccessPoint()->uni();
        m_ssid = network->ssid();
        m_signal = network->signalStrength();
        m_type = NetworkManager::ConnectionSettings::Wireless;

        if (m_name.isEmpty() || m_connectionPath.isEmpty()) {
            m_name = m_ssid;
        }

        NetworkManager::AccessPoint::Ptr ap = wifiDevice->findAccessPoint(m_accessPointPath);

        if (ap && ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
            m_securityType = NetworkManager::Utils::findBestWirelessSecurity(wifiDevice->wirelessCapabilities(), true, (wifiDevice->mode() == NetworkManager::WirelessDevice::Adhoc),
                                                                            ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());
        }
    } else {
        m_accessPointPath.clear();
        if (m_connectionPath.isEmpty()) {
            m_ssid.clear();
            m_type = NetworkManager::ConnectionSettings::Unknown;
            m_securityType = NetworkManager::Utils::Unknown;
        }
        m_signal = 0;
    }

    updateDetails();
}

void ModelItem::updateActiveConnectionState(NetworkManager::ActiveConnection::State state)
{
    m_connectionState = state;
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
