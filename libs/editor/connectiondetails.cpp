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

QList<ConnectionDetailSection>
getConnectionDetails(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device)
{
    QList<ConnectionDetailSection> sections;

    if (!connection || !device) {
        return sections;
    }

    NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
    if (!settings) {
        return sections;
    }

    const bool isActive = (device->state() == NetworkManager::Device::Activated);
    const NetworkManager::ConnectionSettings::ConnectionType type = settings->connectionType();

    // Hardware/Connection type specific details
    if (type == NetworkManager::ConnectionSettings::Wired) {
        QList<QPair<QString, QString>> details;
        NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        if (wiredDevice) {
            if (isActive) {
                details.append({i18n("Connection speed"), UiUtils::connectionSpeed(wiredDevice->bitRate())});
            }
            details.append({i18n("MAC Address"), wiredDevice->hardwareAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Hardware"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Wireless) {
        QList<QPair<QString, QString>> details;
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        NetworkManager::WirelessSetting::Ptr wirelessSetting =
            settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
        QString ssid;
        NetworkManager::WirelessSetting::NetworkMode mode = NetworkManager::WirelessSetting::Infrastructure;
        if (wirelessSetting) {
            ssid = wirelessSetting->ssid();
            mode = wirelessSetting->mode();
        }
        details.append({i18n("Access Point (SSID)"), ssid});
        if (mode == NetworkManager::WirelessSetting::Infrastructure) {
            if (wirelessDevice) {
                NetworkManager::AccessPoint::Ptr ap = wirelessDevice->activeAccessPoint();
                if (ap) {
                    details.append({i18n("Signal Strength"), i18nc("WiFi signal strength percentage indicator", "%1%", ap->signalStrength())});
                }
            }
        }
        details.append({i18n("Security Type"),
                        UiUtils::labelFromWirelessSecurity(NetworkManager::securityTypeFromConnectionSetting(settings))});
        if (wirelessDevice) {
            if (isActive) {
                details.append({i18n("Connection Speed"), UiUtils::connectionSpeed(wirelessDevice->bitRate())});
            }
            const NetworkManager::AccessPoint::Ptr accessPoint = wirelessDevice->activeAccessPoint();

            if (accessPoint) {
                const int channel = NetworkManager::findChannel(accessPoint->frequency());
                const QString frequencyString = UiUtils::wirelessFrequencyToString(accessPoint->frequency());
                if (channel > 0) {
                    details.append({i18n("Frequency"), i18nc("Frequency (Channel)", "%1 (Channel %2)", frequencyString, channel)});
                } else {
                    details.append({i18n("Frequency"), frequencyString});
                }
                details.append({i18n("BSSID"), accessPoint->hardwareAddress()});
            }
            details.append({i18n("MAC Address"), wirelessDevice->hardwareAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Wi-Fi"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Gsm || type == NetworkManager::ConnectionSettings::Cdma) {
        QList<QPair<QString, QString>> details;
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
                        details.append({i18n("Operator"), gsmNet->operatorName()});
                    }
                } else {
                    ModemManager::ModemCdma::Ptr cdmaNet =
                        modem->interface(ModemManager::ModemDevice::CdmaInterface).objectCast<ModemManager::ModemCdma>();
                    details.append({i18n("Network ID"), QStringLiteral("%1").arg(cdmaNet->nid())});
                }

                if (modemNetwork) {
                    details.append({i18n("Signal Quality"), QStringLiteral("%1%").arg(modemNetwork->signalQuality().signal)});
                    details.append({i18n("Access Technology"),
                                    UiUtils::convertAccessTechnologyToString(modemNetwork->accessTechnologies())});
                }
            }
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Mobile Broadband"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Vpn) {
        QList<QPair<QString, QString>> details;
        // Get VPN plugin type from VPN setting
        NetworkManager::VpnSetting::Ptr vpnSetting = settings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
        QString vpnType;
        if (vpnSetting) {
            vpnType = vpnSetting->serviceType();
        }

        details.append({i18n("VPN Plugin"), vpnType});

        if (isActive) {
            NetworkManager::ActiveConnection::Ptr active = device->activeConnection();
            NetworkManager::VpnConnection::Ptr vpnConnection;

            if (active) {
                vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()));
            }

            if (vpnConnection && !vpnConnection->banner().isEmpty()) {
                details.append({i18n("Banner"), vpnConnection->banner().simplified()});
            }
        }
        if (!details.isEmpty()) {
            sections.append({i18n("VPN"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::WireGuard) {
        // From NetworkManager perspective, WireGuard is not a VPN connection,
        // so there are no specific VpnConnection settings to be fetched.
        QList<QPair<QString, QString>> details;
        details.append({i18n("VPN Plugin"), i18n("WireGuard")});
        sections.append({i18n("VPN"), details});
    } else if (type == NetworkManager::ConnectionSettings::Bluetooth) {
        QList<QPair<QString, QString>> details;
        NetworkManager::BluetoothDevice::Ptr bluetoothDevice = device.objectCast<NetworkManager::BluetoothDevice>();
        if (bluetoothDevice) {
            details.append({i18n("Name"), bluetoothDevice->name()});
            if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Pan) {
                details.append({i18n("Capabilities"), QStringLiteral("PAN")});
            } else if (bluetoothDevice->bluetoothCapabilities() == NetworkManager::BluetoothDevice::Dun) {
                details.append({i18n("Capabilities"), QStringLiteral("DUN")});
            }
            details.append({i18n("MAC Address"), bluetoothDevice->hardwareAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Bluetooth"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Infiniband) {
        QList<QPair<QString, QString>> details;
        NetworkManager::InfinibandDevice::Ptr infinibandDevice = device.objectCast<NetworkManager::InfinibandDevice>();
        details.append({i18n("Type"), i18n("Infiniband")});
        if (infinibandDevice) {
            details.append({i18n("MAC Address"), infinibandDevice->hwAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Infiniband"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Bond) {
        QList<QPair<QString, QString>> details;
        NetworkManager::BondDevice::Ptr bondDevice = device.objectCast<NetworkManager::BondDevice>();
        details.append({i18n("Type"), i18n("Bond")});
        if (bondDevice) {
            details.append({i18n("MAC Address"), bondDevice->hwAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Bond"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Bridge) {
        QList<QPair<QString, QString>> details;
        NetworkManager::BridgeDevice::Ptr bridgeDevice = device.objectCast<NetworkManager::BridgeDevice>();
        details.append({i18n("Type"), i18n("Bridge")});
        if (bridgeDevice) {
            details.append({i18n("MAC Address"), bridgeDevice->hwAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Bridge"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Vlan) {
        QList<QPair<QString, QString>> details;
        NetworkManager::VlanDevice::Ptr vlanDevice = device.objectCast<NetworkManager::VlanDevice>();
        details.append({i18n("Type"), i18n("Vlan")});
        if (vlanDevice) {
            details.append({i18n("Vlan ID"), QString("%1").arg(vlanDevice->vlanId())});
            details.append({i18n("MAC Address"), vlanDevice->hwAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Vlan"), details});
        }
    } else if (type == NetworkManager::ConnectionSettings::Adsl) {
        QList<QPair<QString, QString>> details;
        details.append({i18n("Type"), i18n("Adsl")});
        sections.append({i18n("Adsl"), details});
    } else if (type == NetworkManager::ConnectionSettings::Team) {
        QList<QPair<QString, QString>> details;
        NetworkManager::TeamDevice::Ptr teamDevice = device.objectCast<NetworkManager::TeamDevice>();
        details.append({i18n("Type"), i18n("Team")});
        if (teamDevice) {
            details.append({i18n("MAC Address"), teamDevice->hwAddress()});
        }
        if (!details.isEmpty()) {
            sections.append({i18n("Team"), details});
        }
    }

    // Add device interface name to the last section if active
    if (device && isActive && !sections.isEmpty()) {
        sections.last().details.append({i18n("Device"), device->interfaceName()});
    } else if (device && isActive && sections.isEmpty()) {
        // WireGuard or other types might not have created a section yet
        QList<QPair<QString, QString>> details;
        details.append({i18n("Device"), device->interfaceName()});
        sections.append({i18n("General"), details});
    }

    // Get IPv4 Address and related nameservers + IPv4 default gateway
    if (device && device->ipV4Config().isValid() && isActive) {
        QList<QPair<QString, QString>> details;
        if (!device->ipV4Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV4Config().addresses().first().ip();
            if (!addr.isNull() && addr.isGlobal()) {
                details.append({i18n("IPv4 Address"), addr.toString()});
            }
        }
        if (!device->ipV4Config().gateway().isEmpty()) {
            QString addr = device->ipV4Config().gateway();
            if (!addr.isNull()) {
                details.append({i18n("IPv4 Default Gateway"), addr});
            }
        }
        if (!device->ipV4Config().nameservers().isEmpty()) {
            QHostAddress addr1 = device->ipV4Config().nameservers().first();
            QHostAddress addr2 = device->ipV4Config().nameservers().last();
            if (!addr1.isNull()) {
                details.append({i18n("IPv4 Primary Nameserver"), addr1.toString()});
            }
            if (!addr2.isNull() && !addr1.isNull()) {
                if (addr2 != addr1) {
                    details.append({i18n("IPv4 Secondary Nameserver"), addr2.toString()});
                }
            }
        }
        if (!details.isEmpty()) {
            sections.append({i18n("IPv4"), details});
        }
    }

    // Get IPv6 Address and related nameservers + IPv6 default gateway
    if (device && device->ipV6Config().isValid() && isActive) {
        QList<QPair<QString, QString>> details;
        if (!device->ipV6Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV6Config().addresses().first().ip();
            if (!addr.isNull() && addr.isGlobal() && !addr.isUniqueLocalUnicast()) {
                details.append({i18n("IPv6 Address"), addr.toString()});
            } else if (!addr.isNull() && addr.isGlobal() && addr.isUniqueLocalUnicast()) {
                details.append({i18n("IPv6 ULA Address"), addr.toString()});
            }
        }
        if (!device->ipV6Config().gateway().isEmpty()) {
            QString addr = device->ipV6Config().gateway();
            if (!addr.isNull()) {
                details.append({i18n("IPv6 Default Gateway"), addr});
            }
        }
        if (!device->ipV6Config().nameservers().isEmpty()) {
            QHostAddress addr1 = device->ipV6Config().nameservers().first();
            QHostAddress addr2 = device->ipV6Config().nameservers().last();
            if (!addr1.isNull()) {
                details.append({i18n("IPv6 Primary Nameserver"), addr1.toString()});
            }
            if (!addr2.isNull() && !addr1.isNull()) {
                if (addr2 != addr1) {
                    details.append({i18n("IPv6 Secondary Nameserver"), addr2.toString()});
                }
            }
        }
        if (!details.isEmpty()) {
            sections.append({i18n("IPv6"), details});
        }
    }

    return sections;
}

} // namespace ConnectionDetails
