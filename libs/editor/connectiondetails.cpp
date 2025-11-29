/*
    SPDX-FileCopyrightText: 2013-2018 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "connectiondetails.h"
#include "uiutils.h"

#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/AdslDevice>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/BondDevice>
#include <NetworkManagerQt/BridgeDevice>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/InfinibandDevice>
#include <NetworkManagerQt/IpConfig>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/TeamDevice>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/VlanDevice>
#include <NetworkManagerQt/VpnConnection>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WirelessSetting>

#include <KLocalizedString>
#include <QHostAddress>

#include <ModemManagerQt/Manager>
#include <ModemManagerQt/Modem3Gpp>
#include <ModemManagerQt/Modem>
#include <ModemManagerQt/ModemCdma>
#include <ModemManagerQt/ModemDevice>

namespace ConnectionDetails
{

QVariantList getConnectionDetails(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device)
{
    QStringList details;

    if (!connection || !device) {
        QVariantList result;
        return result;
    }

    NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
    if (!settings) {
        QVariantList result;
        return result;
    }

    const bool isActive = (device->state() == NetworkManager::Device::Activated);
    const NetworkManager::ConnectionSettings::ConnectionType type = settings->connectionType();

    // Get IPv[46]Address and related nameservers + IPv[46] default gateway
    if (device && device->ipV4Config().isValid() && isActive) {
        if (!device->ipV4Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV4Config().addresses().first().ip();
            if (!addr.isNull() && addr.isGlobal()) {
                details << i18n("IPv4 Address") << addr.toString();
            }
        }
        if (!device->ipV4Config().gateway().isEmpty()) {
            QString addr = device->ipV4Config().gateway();
            if (!addr.isNull()) {
                details << i18n("IPv4 Default Gateway") << addr;
            }
        }
        if (!device->ipV4Config().nameservers().isEmpty()) {
            QHostAddress addr1 = device->ipV4Config().nameservers().first();
            QHostAddress addr2 = device->ipV4Config().nameservers().last();
            if (!addr1.isNull()) {
                details << i18n("IPv4 Primary Nameserver") << addr1.toString();
            }
            if (!addr2.isNull() && !addr1.isNull()) {
                if (addr2 != addr1) {
                    details << i18n("IPv4 Secondary Nameserver") << addr2.toString();
                }
            }
        }
    }

    if (device && device->ipV6Config().isValid() && isActive) {
        if (!device->ipV6Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV6Config().addresses().first().ip();
            if (!addr.isNull() && addr.isGlobal() && !addr.isUniqueLocalUnicast()) {
                details << i18n("IPv6 Address") << addr.toString();
            } else if (!addr.isNull() && addr.isGlobal() && addr.isUniqueLocalUnicast()) {
                details << i18n("IPv6 ULA Address") << addr.toString();
            }
        }
        if (!device->ipV6Config().gateway().isEmpty()) {
            QString addr = device->ipV6Config().gateway();
            if (!addr.isNull()) {
                details << i18n("IPv6 Default Gateway") << addr;
            }
        }
        if (!device->ipV6Config().nameservers().isEmpty()) {
            QHostAddress addr1 = device->ipV6Config().nameservers().first();
            QHostAddress addr2 = device->ipV6Config().nameservers().last();
            if (!addr1.isNull()) {
                details << i18n("IPv6 Primary Nameserver") << addr1.toString();
            }
            if (!addr2.isNull() && !addr1.isNull()) {
                if (addr2 != addr1) {
                    details << i18n("IPv6 Secondary Nameserver") << addr2.toString();
                }
            }
        }
    }
    if (type == NetworkManager::ConnectionSettings::Wired) {
        NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        if (wiredDevice) {
            if (isActive) {
                details << i18n("Connection speed") << UiUtils::connectionSpeed(wiredDevice->bitRate());
            }
            details << i18n("MAC Address") << wiredDevice->hardwareAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Wireless) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        NetworkManager::WirelessSetting::Ptr wirelessSetting =
            settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
        QString ssid;
        NetworkManager::WirelessSetting::NetworkMode mode = NetworkManager::WirelessSetting::Infrastructure;
        if (wirelessSetting) {
            ssid = wirelessSetting->ssid();
            mode = wirelessSetting->mode();
        }
        details << i18n("Access Point (SSID)") << ssid;
        if (mode == NetworkManager::WirelessSetting::Infrastructure) {
            if (wirelessDevice) {
                NetworkManager::AccessPoint::Ptr ap = wirelessDevice->activeAccessPoint();
                if (ap) {
                    details << i18n("Signal Strength") << i18nc("WiFi signal strength percentage indicator", "%1%", ap->signalStrength());
                }
            }
        }
        details << i18n("Security Type") << UiUtils::labelFromWirelessSecurity(NetworkManager::securityTypeFromConnectionSetting(settings));
        if (wirelessDevice) {
            if (isActive) {
                details << i18n("Connection Speed") << UiUtils::connectionSpeed(wirelessDevice->bitRate());
            }
            const NetworkManager::AccessPoint::Ptr accessPoint = wirelessDevice->activeAccessPoint();

            if (accessPoint) {
                const int channel = NetworkManager::findChannel(accessPoint->frequency());
                const QString frequencyString = UiUtils::wirelessFrequencyToString(accessPoint->frequency());
                details << i18n("Frequency");
                if (channel > 0) {
                    details << i18nc("Frequency (Channel)", "%1 (Channel %2)", frequencyString, channel);
                } else {
                    details << frequencyString;
                }
                details << i18n("BSSID") << accessPoint->hardwareAddress();
            }
            details << i18n("MAC Address") << wirelessDevice->hardwareAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Gsm || type == NetworkManager::ConnectionSettings::Cdma) {
        NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();
        if (modemDevice) {
            ModemManager::ModemDevice::Ptr modem = ModemManager::findModemDevice(modemDevice->udi());
            if (modem) {
                ModemManager::Modem::Ptr modemNetwork = modem->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();

                if (type == NetworkManager::ConnectionSettings::Gsm) {
                    ModemManager::Modem3gpp::Ptr gsmNet = modem->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
                    if (gsmNet) {
                        details << i18n("Operator") << gsmNet->operatorName();
                    }
                } else {
                    ModemManager::ModemCdma::Ptr cdmaNet = modem->interface(ModemManager::ModemDevice::CdmaInterface).objectCast<ModemManager::ModemCdma>();
                    details << i18n("Network ID") << QStringLiteral("%1").arg(cdmaNet->nid());
                }

                if (modemNetwork) {
                    details << i18n("Signal Quality") << QStringLiteral("%1%").arg(modemNetwork->signalQuality().signal);
                    details << i18n("Access Technology") << UiUtils::convertAccessTechnologyToString(modemNetwork->accessTechnologies());
                }
            }
        }
    } else if (type == NetworkManager::ConnectionSettings::Vpn) {
        // Get VPN plugin type from VPN setting
        NetworkManager::VpnSetting::Ptr vpnSetting = settings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
        QString vpnType;
        if (vpnSetting) {
            vpnType = vpnSetting->serviceType();
        }

        details << i18n("VPN Plugin") << vpnType;

        if (isActive) {
            NetworkManager::ActiveConnection::Ptr active = device->activeConnection();
            NetworkManager::VpnConnection::Ptr vpnConnection;

            if (active) {
                vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()));
            }

            if (vpnConnection && !vpnConnection->banner().isEmpty()) {
                details << i18n("Banner") << vpnConnection->banner().simplified();
            }
        }
    } else if (type == NetworkManager::ConnectionSettings::WireGuard) {
        // From NetworkManager perspective, WireGuard is not a VPN connection,
        // so there are no specific VpnConnection settings to be fetched.
        details << i18n("VPN Plugin") << "WireGuard";
    } else if (type == NetworkManager::ConnectionSettings::Bluetooth) {
        NetworkManager::BluetoothDevice::Ptr bluetoothDevice = device.objectCast<NetworkManager::BluetoothDevice>();
        if (bluetoothDevice) {
            details << i18n("Name") << bluetoothDevice->name();
            if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Pan) {
                details << i18n("Capabilities") << QStringLiteral("PAN");
            } else if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Dun) {
                details << i18n("Capabilities") << QStringLiteral("DUN");
            }
            details << i18n("MAC Address") << bluetoothDevice->hardwareAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Infiniband) {
        NetworkManager::InfinibandDevice::Ptr infinibandDevice = device.objectCast<NetworkManager::InfinibandDevice>();
        details << i18n("Type") << i18n("Infiniband");
        if (infinibandDevice) {
            details << i18n("MAC Address") << infinibandDevice->hwAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Bond) {
        NetworkManager::BondDevice::Ptr bondDevice = device.objectCast<NetworkManager::BondDevice>();
        details << i18n("Type") << i18n("Bond");
        if (bondDevice) {
            details << i18n("MAC Address") << bondDevice->hwAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Bridge) {
        NetworkManager::BridgeDevice::Ptr bridgeDevice = device.objectCast<NetworkManager::BridgeDevice>();
        details << i18n("Type") << i18n("Bridge");
        if (bridgeDevice) {
            details << i18n("MAC Address") << bridgeDevice->hwAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Vlan) {
        NetworkManager::VlanDevice::Ptr vlanDevice = device.objectCast<NetworkManager::VlanDevice>();
        details << i18n("Type") << i18n("Vlan");
        if (vlanDevice) {
            details << i18n("Vlan ID") << QString("%1").arg(vlanDevice->vlanId());
            details << i18n("MAC Address") << vlanDevice->hwAddress();
        }
    } else if (type == NetworkManager::ConnectionSettings::Adsl) {
        details << i18n("Type") << i18n("Adsl");
    } else if (type == NetworkManager::ConnectionSettings::Team) {
        NetworkManager::TeamDevice::Ptr teamDevice = device.objectCast<NetworkManager::TeamDevice>();
        details << i18n("Type") << i18n("Team");
        if (teamDevice) {
            details << i18n("MAC Address") << teamDevice->hwAddress();
        }
    }

    if (device && isActive) {
        details << i18n("Device") << device->interfaceName();
    }

    // Convert QStringList to QVariantList with label/value pairs
    QVariantList result;
    for (int i = 0; i < details.size(); i += 2) {
        QString label = details.at(i);
        QString value = (i + 1 < details.size()) ? details.at(i + 1) : QString();
        result << QVariantMap{{QStringLiteral("label"), label}, {QStringLiteral("value"), value}};
    }
    return result;
}

} // namespace ConnectionDetails
