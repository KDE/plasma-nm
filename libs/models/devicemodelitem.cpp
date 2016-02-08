/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

#include "devicemodelitem.h"
#include "uiutils.h"

#include <KLocalizedString>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/manager.h>
#include <ModemManagerQt/modem.h>
#include <ModemManagerQt/modemdevice.h>
#include <ModemManagerQt/modem3gpp.h>
#include <ModemManagerQt/modemcdma.h>
#endif

DeviceModelItem::DeviceModelItem(QObject* parent)
    : QObject(parent)
    , m_deviceState(NetworkManager::Device::UnknownState)
{
}

DeviceModelItem::~DeviceModelItem()
{
}

QString DeviceModelItem::activeConnection() const
{
    return m_activeConnection;
}

void DeviceModelItem::setActiveConnection(const QString& name)
{
    m_activeConnection = name;
}

QStringList DeviceModelItem::details() const
{
    return m_details;
}

QString DeviceModelItem::devicePath() const
{
    return m_devicePath;
}

QString DeviceModelItem::deviceName() const
{
    return m_deviceName;
}

void DeviceModelItem::setDeviceName(const QString& name)
{
    m_deviceName = name;
}

void DeviceModelItem::setDevicePath(const QString& path)
{
    m_devicePath = path;
}

NetworkManager::Device::State DeviceModelItem::deviceState() const
{
    return m_deviceState;
}

void DeviceModelItem::setDeviceState(const NetworkManager::Device::State state)
{
    m_deviceState = state;
}

QString DeviceModelItem::deviceUdi() const
{
    return m_deviceUdi;
}

void DeviceModelItem::setDeviceUdi(const QString& udi)
{
    m_deviceUdi = udi;
}

NetworkManager::Device::Type DeviceModelItem::deviceType() const
{
    return m_deviceType;
}

void DeviceModelItem::setDeviceType(const NetworkManager::Device::Type type)
{
    m_deviceType = type;
}

QString DeviceModelItem::deviceLabel() const
{
    return m_deviceLabel;
}

void DeviceModelItem::setDeviceLabel(const QString& label)
{
    m_deviceLabel = label;
}

QString DeviceModelItem::deviceSubLabel() const
{
    if (m_deviceState == NetworkManager::Device::Activated) {
        return i18n("Connected to '%1'").arg(m_activeConnection);
    } else {
        return UiUtils::connectionStateToString(m_deviceState);
    }
}

QString DeviceModelItem::icon() const
{
    switch (m_deviceType) {
        case NetworkManager::Device::Ethernet:
            if (m_deviceState == NetworkManager::Device::Activated) {
                return QStringLiteral("network-wired-activated");
            } else {
                return QStringLiteral("network-wired");
            }
            break;
        case NetworkManager::Device::Wifi:
            return QStringLiteral("network-wireless");
            break;
        case NetworkManager::Device::Modem:
            return QStringLiteral("network-modem");
            break;
        case NetworkManager::Device::Bluetooth:
            return QStringLiteral("network-bluetooth");
            break;
        default:
            break;
    }

    if (m_deviceState == NetworkManager::Device::Activated) {
        return QStringLiteral("network-wired-activated");
    } else {
        return QStringLiteral("network-wired");
    }
}

bool DeviceModelItem::operator==(const DeviceModelItem* item) const
{
    return item->devicePath() == devicePath();
}

void DeviceModelItem::updateDetails()
{
    m_details.clear();

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

    // Get IPv[46]Address
    if (device && device->ipV4Config().isValid() && m_deviceState == NetworkManager::Device::Activated) {
        if (!device->ipV4Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV4Config().addresses().first().ip();
            if (!addr.isNull()) {
                m_details << i18n("IPv4 Address") << addr.toString();
            }
        }
    }

    if (device && device->ipV6Config().isValid() && m_deviceState == NetworkManager::Device::Activated) {
        if (!device->ipV6Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV6Config().addresses().first().ip();
            if (!addr.isNull()) {
                m_details << i18n("IPv6 Address") << addr.toString();
            }
        }
    }

    if (m_deviceType == NetworkManager::Device::Ethernet) {
        NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        if (wiredDevice) {
            if (m_deviceState == NetworkManager::Device::Activated) {
                m_details << i18n("Connection speed") << UiUtils::connectionSpeed(wiredDevice->bitRate());
            }
            m_details << i18n("MAC Address") << wiredDevice->permanentHardwareAddress();
        }
    } else if (m_deviceType == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
//         m_details << i18n("Access point (SSID)") << m_ssid;
//         if (m_mode == NetworkManager::WirelessSetting::Infrastructure) {
//             m_details << i18n("Signal strength") << QString("%1%").arg(m_signal);
//         }
//         if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
//             m_details << i18n("Security type") << UiUtils::labelFromWirelessSecurity(m_securityType);
//         }
        if (wirelessDevice) {
            if (m_deviceState == NetworkManager::Device::Activated) {
                m_details << i18n("Connection speed") << UiUtils::connectionSpeed(wirelessDevice->bitRate());
            }
            m_details << i18n("MAC Address") << wirelessDevice->permanentHardwareAddress();
        }
    } else if (m_deviceType == NetworkManager::Device::Modem) {
#if WITH_MODEMMANAGER_SUPPORT
        NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();
        if (modemDevice) {
            ModemManager::ModemDevice::Ptr modem = ModemManager::findModemDevice(modemDevice->udi());
            if (modem) {
                ModemManager::Modem::Ptr modemNetwork = modem->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
/*
                if (m_type == NetworkManager::ConnectionSettings::Gsm) {
                    ModemManager::Modem3gpp::Ptr gsmNet = modem->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
                    if (gsmNet) {
                        m_details << i18n("Operator") << gsmNet->operatorName();
                    }
                } else {
                    ModemManager::ModemCdma::Ptr cdmaNet = modem->interface(ModemManager::ModemDevice::CdmaInterface).objectCast<ModemManager::ModemCdma>();
                    m_details << i18n("Network ID") << QString("%1").arg(cdmaNet->nid());
                }*/

                if (modemNetwork) {
                    m_details << i18n("Signal Quality") << QString("%1%").arg(modemNetwork->signalQuality().signal);
                    m_details << i18n("Access Technology") << UiUtils::convertAccessTechnologyToString(modemNetwork->accessTechnologies());
                }
            }
        }
#endif
//     } else if (m_type == NetworkManager::ConnectionSettings::Vpn) {
//         NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_connectionPath);
//         NetworkManager::ConnectionSettings::Ptr connectionSettings;
//         NetworkManager::VpnSetting::Ptr vpnSetting;
//
//         if (connection) {
//             connectionSettings = connection->settings();
//         }
//         if (connectionSettings) {
//             vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
//         }
//
//         if (vpnSetting) {
//             m_details << i18n("VPN plugin") << vpnSetting->serviceType().section('.', -1);
//         }
//
//         if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
//             NetworkManager::ActiveConnection::Ptr active = NetworkManager::findActiveConnection(m_activeConnectionPath);
//             NetworkManager::VpnConnection::Ptr vpnConnection;
//
//             if (active) {
//                 vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()), &QObject::deleteLater);
//             }
//
//             if (vpnConnection && !vpnConnection->banner().isEmpty()) {
//                 m_details << i18n("Banner") << vpnConnection->banner().simplified();
//             }
//         }
    } else if (m_deviceType == NetworkManager::Device::Bluetooth) {
        NetworkManager::BluetoothDevice::Ptr bluetoothDevice = device.objectCast<NetworkManager::BluetoothDevice>();
        if (bluetoothDevice) {
            m_details << i18n("Name") << bluetoothDevice->name();
            if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Pan) {
                m_details << i18n("Capabilities") << QStringLiteral("PAN");
            } else if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Dun) {
                m_details << i18n("Capabilities") << QStringLiteral("DUN");
            }
            m_details << i18n("MAC Address") << bluetoothDevice->hardwareAddress();

        }
    }
}

