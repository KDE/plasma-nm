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

QMap<QString, QMap<QString, QString>>
getConnectionDetails(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device)
{
    QMap<QString, QMap<QString, QString>> sections;
    QString currentSection;

    if (!connection || !device) {
        return sections;
    }

    NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
    if (!settings) {
        return sections;
    }

    const bool isActive = (device->state() == NetworkManager::Device::Activated);
    const NetworkManager::ConnectionSettings::ConnectionType type = settings->connectionType();

    // Get IPv[46]Address and related nameservers + IPv[46] default gateway
    if (device && device->ipV4Config().isValid() && isActive) {
        currentSection = i18n("IPv4");
        if (!device->ipV4Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV4Config().addresses().first().ip();
            if (!addr.isNull() && addr.isGlobal()) {
                sections[currentSection].insert(i18n("IPv4 Address"), addr.toString());
            }
        }
        if (!device->ipV4Config().gateway().isEmpty()) {
            QString addr = device->ipV4Config().gateway();
            if (!addr.isNull()) {
                sections[currentSection].insert(i18n("IPv4 Default Gateway"), addr);
            }
        }
        if (!device->ipV4Config().nameservers().isEmpty()) {
            QHostAddress addr1 = device->ipV4Config().nameservers().first();
            QHostAddress addr2 = device->ipV4Config().nameservers().last();
            if (!addr1.isNull()) {
                sections[currentSection].insert(i18n("IPv4 Primary Nameserver"), addr1.toString());
            }
            if (!addr2.isNull() && !addr1.isNull()) {
                if (addr2 != addr1) {
                    sections[currentSection].insert(i18n("IPv4 Secondary Nameserver"), addr2.toString());
                }
            }
        }
    }

    if (device && device->ipV6Config().isValid() && isActive) {
        currentSection = i18n("IPv6");
        if (!device->ipV6Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV6Config().addresses().first().ip();
            if (!addr.isNull() && addr.isGlobal() && !addr.isUniqueLocalUnicast()) {
                sections[currentSection].insert(i18n("IPv6 Address"), addr.toString());
            } else if (!addr.isNull() && addr.isGlobal() && addr.isUniqueLocalUnicast()) {
                sections[currentSection].insert(i18n("IPv6 ULA Address"), addr.toString());
            }
        }
        if (!device->ipV6Config().gateway().isEmpty()) {
            QString addr = device->ipV6Config().gateway();
            if (!addr.isNull()) {
                sections[currentSection].insert(i18n("IPv6 Default Gateway"), addr);
            }
        }
        if (!device->ipV6Config().nameservers().isEmpty()) {
            QHostAddress addr1 = device->ipV6Config().nameservers().first();
            QHostAddress addr2 = device->ipV6Config().nameservers().last();
            if (!addr1.isNull()) {
                sections[currentSection].insert(i18n("IPv6 Primary Nameserver"), addr1.toString());
            }
            if (!addr2.isNull() && !addr1.isNull()) {
                if (addr2 != addr1) {
                    sections[currentSection].insert(i18n("IPv6 Secondary Nameserver"), addr2.toString());
                }
            }
        }
    }

    if (type == NetworkManager::ConnectionSettings::Wired) {
        currentSection = i18n("Hardware");
        NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        if (wiredDevice) {
            if (isActive) {
                sections[currentSection].insert(i18n("Connection speed"), UiUtils::connectionSpeed(wiredDevice->bitRate()));
            }
            sections[currentSection].insert(i18n("MAC Address"), wiredDevice->hardwareAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Wireless) {
        currentSection = i18n("Wi-Fi");
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        NetworkManager::WirelessSetting::Ptr wirelessSetting =
            settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
        QString ssid;
        NetworkManager::WirelessSetting::NetworkMode mode = NetworkManager::WirelessSetting::Infrastructure;
        if (wirelessSetting) {
            ssid = wirelessSetting->ssid();
            mode = wirelessSetting->mode();
        }
        sections[currentSection].insert(i18n("Access Point (SSID)"), ssid);
        if (mode == NetworkManager::WirelessSetting::Infrastructure) {
            if (wirelessDevice) {
                NetworkManager::AccessPoint::Ptr ap = wirelessDevice->activeAccessPoint();
                if (ap) {
                    sections[currentSection].insert(i18n("Signal Strength"), i18nc("WiFi signal strength percentage indicator", "%1%", ap->signalStrength()));
                }
            }
        }
        sections[currentSection].insert(i18n("Security Type"),
                                        UiUtils::labelFromWirelessSecurity(NetworkManager::securityTypeFromConnectionSetting(settings)));
        if (wirelessDevice) {
            if (isActive) {
                sections[currentSection].insert(i18n("Connection Speed"), UiUtils::connectionSpeed(wirelessDevice->bitRate()));
            }
            const NetworkManager::AccessPoint::Ptr accessPoint = wirelessDevice->activeAccessPoint();

            if (accessPoint) {
                const int channel = NetworkManager::findChannel(accessPoint->frequency());
                const QString frequencyString = UiUtils::wirelessFrequencyToString(accessPoint->frequency());
                if (channel > 0) {
                    sections[currentSection].insert(i18n("Frequency"), i18nc("Frequency (Channel)", "%1 (Channel %2)", frequencyString, channel));
                } else {
                    sections[currentSection].insert(i18n("Frequency"), frequencyString);
                }
                sections[currentSection].insert(i18n("BSSID"), accessPoint->hardwareAddress());
            }
            sections[currentSection].insert(i18n("MAC Address"), wirelessDevice->hardwareAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Gsm || type == NetworkManager::ConnectionSettings::Cdma) {
        currentSection = i18n("Mobile Broadband");
        NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();
        if (modemDevice) {
            ModemManager::ModemDevice::Ptr modem = ModemManager::findModemDevice(modemDevice->udi());
            if (modem) {
                ModemManager::Modem::Ptr modemNetwork =
                    modem->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();

                if (type == NetworkManager::ConnectionSettings::Gsm) {
                    ModemManager::Modem3gpp::Ptr gsmNet =
                        modem->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
                    if (gsmNet) {
                        sections[currentSection].insert(i18n("Operator"), gsmNet->operatorName());
                    }
                } else {
                    ModemManager::ModemCdma::Ptr cdmaNet =
                        modem->interface(ModemManager::ModemDevice::CdmaInterface).objectCast<ModemManager::ModemCdma>();
                    sections[currentSection].insert(i18n("Network ID"), QStringLiteral("%1").arg(cdmaNet->nid()));
                }

                if (modemNetwork) {
                    sections[currentSection].insert(i18n("Signal Quality"), QStringLiteral("%1%").arg(modemNetwork->signalQuality().signal));
                    sections[currentSection].insert(i18n("Access Technology"),
                                                    UiUtils::convertAccessTechnologyToString(modemNetwork->accessTechnologies()));
                }
            }
        }
    } else if (type == NetworkManager::ConnectionSettings::Vpn) {
        currentSection = i18n("VPN");
        // Get VPN plugin type from VPN setting
        NetworkManager::VpnSetting::Ptr vpnSetting = settings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
        QString vpnType;
        if (vpnSetting) {
            vpnType = vpnSetting->serviceType();
        }

        sections[currentSection].insert(i18n("VPN Plugin"), vpnType);

        if (isActive) {
            NetworkManager::ActiveConnection::Ptr active = device->activeConnection();
            NetworkManager::VpnConnection::Ptr vpnConnection;

            if (active) {
                vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()));
            }

            if (vpnConnection && !vpnConnection->banner().isEmpty()) {
                sections[currentSection].insert(i18n("Banner"), vpnConnection->banner().simplified());
            }
        }
    } else if (type == NetworkManager::ConnectionSettings::WireGuard) {
        // From NetworkManager perspective, WireGuard is not a VPN connection,
        // so there are no specific VpnConnection settings to be fetched.
        sections[i18n("VPN")].insert(i18n("VPN Plugin"), i18n("WireGuard"));
    } else if (type == NetworkManager::ConnectionSettings::Bluetooth) {
        currentSection = i18n("Bluetooth");
        NetworkManager::BluetoothDevice::Ptr bluetoothDevice = device.objectCast<NetworkManager::BluetoothDevice>();
        if (bluetoothDevice) {
            sections[currentSection].insert(i18n("Name"), bluetoothDevice->name());
            if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Pan) {
                sections[currentSection].insert(i18n("Capabilities"), QStringLiteral("PAN"));
            } else if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Dun) {
                sections[currentSection].insert(i18n("Capabilities"), QStringLiteral("DUN"));
            }
            sections[currentSection].insert(i18n("MAC Address"), bluetoothDevice->hardwareAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Infiniband) {
        currentSection = i18n("Infiniband");
        NetworkManager::InfinibandDevice::Ptr infinibandDevice = device.objectCast<NetworkManager::InfinibandDevice>();
        sections[currentSection].insert(i18n("Type"), i18n("Infiniband"));
        if (infinibandDevice) {
            sections[currentSection].insert(i18n("MAC Address"), infinibandDevice->hwAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Bond) {
        currentSection = i18n("Bond");
        NetworkManager::BondDevice::Ptr bondDevice = device.objectCast<NetworkManager::BondDevice>();
        sections[currentSection].insert(i18n("Type"), i18n("Bond"));
        if (bondDevice) {
            sections[currentSection].insert(i18n("MAC Address"), bondDevice->hwAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Bridge) {
        currentSection = i18n("Bridge");
        NetworkManager::BridgeDevice::Ptr bridgeDevice = device.objectCast<NetworkManager::BridgeDevice>();
        sections[currentSection].insert(i18n("Type"), i18n("Bridge"));
        if (bridgeDevice) {
            sections[currentSection].insert(i18n("MAC Address"), bridgeDevice->hwAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Vlan) {
        currentSection = i18n("Vlan");
        NetworkManager::VlanDevice::Ptr vlanDevice = device.objectCast<NetworkManager::VlanDevice>();
        sections[currentSection].insert(i18n("Type"), i18n("Vlan"));
        if (vlanDevice) {
            sections[currentSection].insert(i18n("Vlan ID"), QString("%1").arg(vlanDevice->vlanId()));
            sections[currentSection].insert(i18n("MAC Address"), vlanDevice->hwAddress());
        }
    } else if (type == NetworkManager::ConnectionSettings::Adsl) {
        currentSection = i18n("Adsl");
        sections[currentSection].insert(i18n("Type"), i18n("Adsl"));
    } else if (type == NetworkManager::ConnectionSettings::Team) {
        currentSection = i18n("Team");
        NetworkManager::TeamDevice::Ptr teamDevice = device.objectCast<NetworkManager::TeamDevice>();
        sections[currentSection].insert(i18n("Type"), i18n("Team"));
        if (teamDevice) {
            sections[currentSection].insert(i18n("MAC Address"), teamDevice->hwAddress());
        }
    }

    if (device && isActive) {
        if (currentSection.isEmpty()) {
            // Can happen for WireGuard
            currentSection = i18n("General");
        }
        sections[currentSection].insert(i18n("Device"), device->interfaceName());
    }

    return sections;
}

} // namespace ConnectionDetails
