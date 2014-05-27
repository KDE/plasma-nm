/*
    Copyright 2008-2010 Sebastian Kügler <sebas@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Own
#include "uiutils.h"

// KDE
#include <KDebug>
#include <KLocale>
#include <KGlobal>

#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/manager.h>
#include <ModemManagerQt/modem.h>
#include <ModemManagerQt/modemdevice.h>
#include <ModemManagerQt/modem3gpp.h>
#include <ModemManagerQt/modemcdma.h>
#endif

// Qt
#include <QSizeF>
#include <QHostAddress>

#include <QString>

using namespace NetworkManager;

QString UiUtils::interfaceTypeLabel(const NetworkManager::Device::Type type, const NetworkManager::Device::Ptr iface)
{
    QString deviceText;
    switch (type) {
    case NetworkManager::Device::Ethernet:
        deviceText = i18nc("title of the interface widget in nm's popup", "Wired Ethernet");
        break;
    case NetworkManager::Device::Wifi:
        deviceText = i18nc("title of the interface widget in nm's popup", "Wireless 802.11");
        break;
    case NetworkManager::Device::Bluetooth:
        deviceText = i18nc("title of the interface widget in nm's popup", "Bluetooth");
        break;
    case NetworkManager::Device::Wimax:
        deviceText = i18nc("title of the interface widget in nm's popup", "WiMAX");
        break;
    case NetworkManager::Device::InfiniBand:
        deviceText = i18nc("title of the interface widget in nm's popup", "Infiniband");
        break;
    case NetworkManager::Device::Adsl:
        deviceText = i18nc("title of the interface widget in nm's popup", "ADSL");
        break;
    case NetworkManager::Device::Bond:
        deviceText = i18nc("title of the interface widget in nm's popup", "Virtual (bond)");
        break;
    case NetworkManager::Device::Bridge:
        deviceText = i18nc("title of the interface widget in nm's popup", "Virtual (bridge)");
        break;
    case NetworkManager::Device::Vlan:
        deviceText = i18nc("title of the interface widget in nm's popup", "Virtual (vlan)");
        break;
    case NetworkManager::Device::Modem: {
        const NetworkManager::ModemDevice::Ptr nmModemIface = iface.objectCast<NetworkManager::ModemDevice>();
        if (nmModemIface) {
            switch(modemSubType(nmModemIface->currentCapabilities())) {
            case NetworkManager::ModemDevice::Pots:
                deviceText = i18nc("title of the interface widget in nm's popup", "Serial Modem");
                break;
            case NetworkManager::ModemDevice::GsmUmts:
            case NetworkManager::ModemDevice::CdmaEvdo:
            case NetworkManager::ModemDevice::Lte:
                deviceText = i18nc("title of the interface widget in nm's popup", "Mobile Broadband");
                break;
            case NetworkManager::ModemDevice::NoCapability:
                kWarning() << "Unhandled modem sub type: NetworkManager::ModemDevice::NoCapability";
                break;
            }
        }
    }
        break;
    default:
        deviceText = i18nc("title of the interface widget in nm's popup", "Wired Ethernet");
        break;
    }
    return deviceText;
}

#if 0
QString UiUtils::iconName(const NetworkManager::Device::Ptr &device)
{
    if (!device) {
        return QLatin1String("dialog-error");
    }
    QString icon;

    switch (device->type()) {
        case NetworkManager::Device::Ethernet: {
            icon = "network-wired";

            NetworkManager::WiredDevice::Ptr wiredIface = device.objectCast<NetworkManager::WiredDevice>();
            if (wiredIface && wiredIface->carrier()) {
                icon = "network-wired-activated";
            }
            break;
        }
        case NetworkManager::Device::Wifi: {
            QString strength = "00";
            NetworkManager::WirelessDevice::Ptr wiface = device.objectCast<NetworkManager::WirelessDevice>();

            if (wiface) {
                NetworkManager::AccessPoint::Ptr ap = wiface->activeAccessPoint();
                if (ap) {
                    int s = ap->signalStrength();
                    if (s < 13) {
                        strength = "00";
                    } else if (s < 38) {
                        strength = "25";
                    } else if (s < 63) {
                        strength = "50";
                    } else if (s < 88) {
                        strength = "75";
                    } else if (s >= 88) {
                        strength = "100";
                    }
                } else {
                        strength = "00";
                }
            }
            icon = "network-wireless-" + strength;
            break;
        }
        case NetworkManager::Device::Bluetooth:
            icon = "preferences-system-bluetooth";
            break;
        case NetworkManager::Device::Modem:
            icon = "phone";
            break;
        default:
            icon = "network-wired";
            break;
    }
    kDebug() << "icon:" << icon;
    return icon;
}
#endif

QString UiUtils::iconAndTitleForConnectionSettingsType(NetworkManager::ConnectionSettings::ConnectionType type, QString &title)
{
    QString text;
    QString icon = "network-wired";
    switch (type) {
    case ConnectionSettings::Adsl:
        text = i18n("ADSL");
        icon = "modem";
        break;
    case ConnectionSettings::Pppoe:
        text = i18n("DSL");
        icon = "modem";
        break;
    case ConnectionSettings::Bluetooth:
        text = i18n("Bluetooth");
        icon = "preferences-system-bluetooth";
        break;
    case ConnectionSettings::Bond:
        text = i18n("Bond");
        break;
    case ConnectionSettings::Bridge:
        text = i18n("Bridge");
        break;
    case ConnectionSettings::Gsm:
    case ConnectionSettings::Cdma:
        text = i18n("Mobile broadband");
        icon = "phone";
        break;
    case ConnectionSettings::Infiniband:
        text = i18n("Infiniband");
        break;
    case ConnectionSettings::OLPCMesh:
        text = i18n("Olpc mesh");
        break;
    case ConnectionSettings::Vlan:
        text = i18n("VLAN");
        break;
    case ConnectionSettings::Vpn:
        text = i18n("VPN");
        icon = "secure-card";
        break;
    case ConnectionSettings::Wimax:
        text = i18n("WiMAX");
        icon = "network-wireless";
        break;
    case ConnectionSettings::Wired:
        text = i18n("Wired");
        icon = "network-wired";
        break;
    case ConnectionSettings::Wireless:
        text = i18n("Wireless");
        icon = "network-wireless";
        break;
    default:
        text = i18n("Unknown connection type");
        break;
    }
    title = text;
    return icon;
}

QString UiUtils::prettyInterfaceName(NetworkManager::Device::Type type, const QString &interfaceName)
{
    QString ret;
    switch (type) {
    case NetworkManager::Device::Wifi:
        ret = i18n("Wireless Interface (%1)", interfaceName);
        break;
    case NetworkManager::Device::Ethernet:
        ret = i18n("Wired Interface (%1)", interfaceName);
        break;
    case NetworkManager::Device::Bluetooth:
        ret = i18n("Bluetooth (%1)", interfaceName);
        break;
    case NetworkManager::Device::Modem:
        ret = i18n("Modem (%1)", interfaceName);
        break;
    case NetworkManager::Device::Adsl:
        ret = i18n("ADSL (%1)", interfaceName);
        break;
    case NetworkManager::Device::Vlan:
        ret = i18n("VLan (%1)", interfaceName);
        break;
    case NetworkManager::Device::Bridge:
        ret = i18n("Bridge (%1)", interfaceName);
        break;
    default:
        ret = interfaceName;
    }
    return ret;
}

QString UiUtils::connectionStateToString(NetworkManager::Device::State state, const QString &connectionName)
{
    QString stateString;
    switch (state) {
        case NetworkManager::Device::UnknownState:
            stateString = i18nc("description of unknown network interface state", "Unknown");
            break;
        case NetworkManager::Device::Unmanaged:
            stateString = i18nc("description of unmanaged network interface state", "Unmanaged");
            break;
        case NetworkManager::Device::Unavailable:
            stateString = i18nc("description of unavailable network interface state", "Unavailable");
            break;
        case NetworkManager::Device::Disconnected:
            stateString = i18nc("description of unconnected network interface state", "Not connected");
            break;
        case NetworkManager::Device::Preparing:
            stateString = i18nc("description of preparing to connect network interface state", "Preparing to connect");
            break;
        case NetworkManager::Device::ConfiguringHardware:
            stateString = i18nc("description of configuring hardware network interface state", "Configuring interface");
            break;
        case NetworkManager::Device::NeedAuth:
            stateString = i18nc("description of waiting for authentication network interface state", "Waiting for authorization");
            break;
        case NetworkManager::Device::ConfiguringIp:
            stateString = i18nc("network interface doing dhcp request in most cases", "Setting network address");
            break;
        case NetworkManager::Device::CheckingIp:
            stateString = i18nc("is other action required to fully connect? captive portals, etc.", "Checking further connectivity");
            break;
        case NetworkManager::Device::WaitingForSecondaries:
            stateString = i18nc("a secondary connection (e.g. VPN) has to be activated first to continue", "Waiting for a secondary connection");
            break;
        case NetworkManager::Device::Activated:
            if (connectionName.isEmpty()) {
                stateString = i18nc("network interface connected state label", "Connected");
            } else {
                stateString = i18nc("network interface connected state label", "Connected to %1", connectionName);
            }
            break;
        case NetworkManager::Device::Deactivating:
            stateString = i18nc("network interface disconnecting state label", "Deactivating connection");
            break;
        case NetworkManager::Device::Failed:
            stateString = i18nc("network interface connection failed state label", "Connection Failed");
            break;
        default:
            stateString = i18nc("interface state", "Error: Invalid state");
    }
    return stateString;
}

QString UiUtils::vpnConnectionStateToString(VpnConnection::State state)
{
    QString stateString;
    switch (state) {
        case VpnConnection::Unknown:
            stateString = i18nc("The state of the VPN connection is unknown", "Unknown");
            break;
        case VpnConnection::Prepare:
            stateString = i18nc("The VPN connection is preparing to connect", "Preparing to connect");
            break;
        case VpnConnection::NeedAuth:
            stateString = i18nc("The VPN connection needs authorization credentials", "Needs authorization");
            break;
        case VpnConnection::Connecting:
            stateString = i18nc("The VPN connection is being established", "Connecting");
            break;
        case VpnConnection::GettingIpConfig:
            stateString = i18nc("The VPN connection is getting an IP address", "Setting network address");
            break;
        case VpnConnection::Activated:
            stateString = i18nc("The VPN connection is active", "Activated");
            break;
        case VpnConnection::Failed:
            stateString = i18nc("The VPN connection failed", "Failed");
            break;
        case VpnConnection::Disconnected:
            stateString = i18nc("The VPN connection is disconnected", "Failed");
            break;
        default:
            stateString = i18nc("interface state", "Error: Invalid state");    }
    return stateString;
}

QString UiUtils::operationModeToString(NetworkManager::WirelessDevice::OperationMode mode)
{
    QString modeString;
    switch (mode) {
        case NetworkManager::WirelessDevice::WirelessDevice::Unknown:
            modeString = i18nc("wireless network operation mode", "Unknown");
            break;
        case NetworkManager::WirelessDevice::Adhoc:
            modeString = i18nc("wireless network operation mode", "Adhoc");
            break;
        case NetworkManager::WirelessDevice::WirelessDevice::Infra:
            modeString = i18nc("wireless network operation mode", "Infrastructure");
            break;
        case NetworkManager::WirelessDevice::WirelessDevice::ApMode:
            modeString = i18nc("wireless network operation mode", "Access point");
            break;
        default:
            modeString = I18N_NOOP("INCORRECT MODE FIX ME");
    }
    return modeString;
}

QStringList UiUtils::wpaFlagsToStringList(NetworkManager::AccessPoint::WpaFlags flags)
{
    /* for testing purposes
    flags = NetworkManager::AccessPoint::PairWep40
            | NetworkManager::AccessPoint::PairWep104
            | NetworkManager::AccessPoint::PairTkip
            | NetworkManager::AccessPoint::PairCcmp
            | NetworkManager::AccessPoint::GroupWep40
            | NetworkManager::AccessPoint::GroupWep104
            | NetworkManager::AccessPoint::GroupTkip
            | NetworkManager::AccessPoint::GroupCcmp
            | NetworkManager::AccessPoint::KeyMgmtPsk
            | NetworkManager::AccessPoint::KeyMgmt8021x; */

    QStringList flagList;

    if (flags.testFlag(NetworkManager::AccessPoint::PairWep40))
        flagList.append(i18nc("wireless network cipher", "Pairwise WEP40"));
    if (flags.testFlag(NetworkManager::AccessPoint::PairWep104))
        flagList.append(i18nc("wireless network cipher", "Pairwise WEP104"));
    if (flags.testFlag(NetworkManager::AccessPoint::PairTkip))
        flagList.append(i18nc("wireless network cipher", "Pairwise TKIP"));
    if (flags.testFlag(NetworkManager::AccessPoint::PairCcmp))
        flagList.append(i18nc("wireless network cipher", "Pairwise CCMP"));
    if (flags.testFlag(NetworkManager::AccessPoint::GroupWep40))
        flagList.append(i18nc("wireless network cipher", "Group WEP40"));
    if (flags.testFlag(NetworkManager::AccessPoint::GroupWep104))
        flagList.append(i18nc("wireless network cipher", "Group WEP104"));
    if (flags.testFlag(NetworkManager::AccessPoint::GroupTkip))
        flagList.append(i18nc("wireless network cipher", "Group TKIP"));
    if (flags.testFlag(NetworkManager::AccessPoint::GroupCcmp))
        flagList.append(i18nc("wireless network cipher", "Group CCMP"));
    if (flags.testFlag(NetworkManager::AccessPoint::KeyMgmtPsk))
        flagList.append(i18nc("wireless network cipher", "PSK"));
    if (flags.testFlag(NetworkManager::AccessPoint::KeyMgmt8021x))
        flagList.append(i18nc("wireless network cipher", "802.1x"));

    return flagList;
}


QString UiUtils::connectionSpeed(double bitrate)
{
    QString out;
    if (bitrate < 1000) {
        out = i18nc("connection speed", "<numid>%1</numid> Bit/s", bitrate);
    } else if (bitrate < 1000000) {
        out = i18nc("connection speed", "<numid>%1</numid> MBit/s", bitrate/1000);
    } else {
        out = i18nc("connection speed", "<numid>%1</numid> GBit/s", bitrate/1000000);
    }
    return out;
}

QString UiUtils::wirelessBandToString(NetworkManager::WirelessSetting::FrequencyBand band)
{
    switch (band) {
        case NetworkManager::WirelessSetting::Automatic:
            return QLatin1String("automatic");
            break;
        case NetworkManager::WirelessSetting::A:
            return QLatin1String("a");
            break;
        case NetworkManager::WirelessSetting::Bg:
            return QLatin1String("b/g");
            break;
    }

    return QString();
}

#if WITH_MODEMMANAGER_SUPPORT
QString UiUtils::convertAllowedModeToString(ModemManager::Modem::ModemModes modes)
{
    if (modes.testFlag(MM_MODEM_MODE_4G)) {
        return i18nc("Gsm modes (2G/3G/any)","LTE");
    } else if (modes.testFlag(MM_MODEM_MODE_3G)) {
        return i18nc("Gsm modes (2G/3G/any)","UMTS/HSxPA");
    } else if (modes.testFlag(MM_MODEM_MODE_2G)) {
        return i18nc("Gsm modes (2G/3G/any)","GPRS/EDGE");
    } else if (modes.testFlag(MM_MODEM_MODE_CS)) {
        return i18nc("Gsm modes (2G/3G/any)","GSM");
    } else if (modes.testFlag(MM_MODEM_MODE_ANY)) {
        return i18nc("Gsm modes (2G/3G/any)","Any");
    }

    return i18nc("Gsm modes (2G/3G/any)","Any");
}

QString UiUtils::convertAccessTechnologyToString(ModemManager::Modem::AccessTechnologies tech)
{
    if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_LTE)) {
        return i18nc("Cellular access technology","LTE");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_EVDOB)) {
        return i18nc("Cellular access technology","CDMA2000 EVDO revision B");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_EVDOA)) {
        return i18nc("Cellular access technology","CDMA2000 EVDO revision A");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_EVDO0)) {
        return i18nc("Cellular access technology","CDMA2000 EVDO revision 0");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_1XRTT)) {
        return i18nc("Cellular access technology","CDMA2000 1xRTT");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS)) {
        return i18nc("Cellular access technology","HSPA+");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_HSPA)) {
        return i18nc("Cellular access technology","HSPA");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_HSUPA)) {
        return i18nc("Cellular access technology","HSUPA");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_HSDPA)) {
        return i18nc("Cellular access technology","HSDPA");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_UMTS)) {
        return i18nc("Cellular access technology","UMTS");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_EDGE)) {
        return i18nc("Cellular access technology","EDGE");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_GPRS)) {
        return i18nc("Cellular access technology","GPRS");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT)) {
        return i18nc("Cellular access technology","Compact GSM");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_GSM)) {
        return i18nc("Cellular access technology","GSM");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_POTS)) {
        return i18nc("Analog wireline modem","Analog");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN)) {
        return i18nc("Unknown cellular access technology","Unknown");
    } else if (tech.testFlag(MM_MODEM_ACCESS_TECHNOLOGY_ANY)) {
        return i18nc("Any cellular access technology","Any");
    }

    return i18nc("Unknown cellular access technology","Unknown");
}

QString UiUtils::convertLockReasonToString(MMModemLock reason)
{
    switch (reason) {
    case MM_MODEM_LOCK_NONE:
        return i18nc("possible SIM lock reason", "Modem is unlocked.");
    case MM_MODEM_LOCK_SIM_PIN:
        return i18nc("possible SIM lock reason", "SIM requires the PIN code.");
    case MM_MODEM_LOCK_SIM_PIN2:
        return i18nc("possible SIM lock reason", "SIM requires the PIN2 code.");
    case MM_MODEM_LOCK_SIM_PUK:
        return i18nc("possible SIM lock reason", "SIM requires the PUK code.");
    case MM_MODEM_LOCK_SIM_PUK2:
        return i18nc("possible SIM lock reason", "SIM requires the PUK2 code.");
    case MM_MODEM_LOCK_PH_SP_PIN:
        return i18nc("possible SIM lock reason", "Modem requires the service provider PIN code.");
    case MM_MODEM_LOCK_PH_SP_PUK:
        return i18nc("possible SIM lock reason", "Modem requires the service provider PUK code.");
    case MM_MODEM_LOCK_PH_NET_PIN:
        return i18nc("possible SIM lock reason", "Modem requires the network PIN code.");
    case MM_MODEM_LOCK_PH_NET_PUK:
        return i18nc("possible SIM lock reason", "Modem requires the network PUK code.");
    case MM_MODEM_LOCK_PH_SIM_PIN:
        return i18nc("possible SIM lock reason", "Modem requires the PIN code.");
    case MM_MODEM_LOCK_PH_CORP_PIN:
        return i18nc("possible SIM lock reason", "Modem requires the corporate PIN code.");
    case MM_MODEM_LOCK_PH_CORP_PUK:
        return i18nc("possible SIM lock reason", "Modem requires the corporate PUK code.");
    case MM_MODEM_LOCK_PH_FSIM_PIN:
        return i18nc("possible SIM lock reason", "Modem requires the PH-FSIM PIN code.");
    case MM_MODEM_LOCK_PH_FSIM_PUK:
        return i18nc("possible SIM lock reason", "Modem requires the PH-FSIM PUK code.");
    case MM_MODEM_LOCK_PH_NETSUB_PIN:
        return i18nc("possible SIM lock reason", "Modem requires the network subset PIN code.");
    case MM_MODEM_LOCK_PH_NETSUB_PUK:
        return i18nc("possible SIM lock reason", "Modem requires the network subset PUK code.");
    case MM_MODEM_LOCK_UNKNOWN:
    default:
        return i18nc("possible SIM lock reason", "Lock reason unknown.");
    }
}
#endif

QString UiUtils::convertNspTypeToString(WimaxNsp::NetworkType type)
{
    switch (type) {
        case WimaxNsp::Unknown: return i18nc("Unknown", "Unknown Wimax NSP type");
        case WimaxNsp::Home: return i18n("Home");
        case WimaxNsp::Partner: return i18n("Partner");
        case WimaxNsp::RoamingPartner: return i18n("Roaming partner");
    }

    return i18nc("Unknown", "Unknown Wimax NSP type");
}

NetworkManager::ModemDevice::Capability UiUtils::modemSubType(NetworkManager::ModemDevice::Capabilities modemCaps)
{
    if (modemCaps & NetworkManager::ModemDevice::Lte) {
        return NetworkManager::ModemDevice::Lte;
    } else if (modemCaps & NetworkManager::ModemDevice::CdmaEvdo) {
        return NetworkManager::ModemDevice::CdmaEvdo;
    } else if (modemCaps & NetworkManager::ModemDevice::GsmUmts) {
        return NetworkManager::ModemDevice::GsmUmts;
    } else if (modemCaps & NetworkManager::ModemDevice::Pots) {
        return NetworkManager::ModemDevice::Pots;
    }
    return NetworkManager::ModemDevice::NoCapability;
}

QString UiUtils::labelFromWirelessSecurity(NetworkManager::Utils::WirelessSecurityType type)
{
    QString tip;
    switch (type) {
        case NetworkManager::Utils::None:
            tip = i18nc("@label no security", "Insecure");
            break;
        case NetworkManager::Utils::StaticWep:
            tip = i18nc("@label WEP security", "WEP");
            break;
        case NetworkManager::Utils::Leap:
            tip = i18nc("@label LEAP security", "LEAP");
            break;
        case NetworkManager::Utils::DynamicWep:
            tip = i18nc("@label Dynamic WEP security", "Dynamic WEP");
            break;
        case NetworkManager::Utils::WpaPsk:
            tip = i18nc("@label WPA-PSK security", "WPA-PSK");
            break;
        case NetworkManager::Utils::WpaEap:
            tip = i18nc("@label WPA-EAP security", "WPA-EAP");
            break;
        case NetworkManager::Utils::Wpa2Psk:
            tip = i18nc("@label WPA2-PSK security", "WPA2-PSK");
            break;
        case NetworkManager::Utils::Wpa2Eap:
            tip = i18nc("@label WPA2-EAP security", "WPA2-EAP");
            break;
        default:
            tip = i18nc("@label unknown security", "Unknown security type");
            break;
    }
    return tip;
}

QString UiUtils::shortToolTipFromWirelessSecurity(NetworkManager::Utils::WirelessSecurityType type)
{
    QString tip;
    switch (type) {
        case NetworkManager::Utils::None:
            tip = i18nc("@info:tooltip no security", "Insecure");
            break;
        case NetworkManager::Utils::StaticWep:
            tip = i18nc("@info:tooltip WEP security", "WEP");
            break;
        case NetworkManager::Utils::Leap:
            tip = i18nc("@info:tooltip LEAP security", "LEAP");
            break;
        case NetworkManager::Utils::DynamicWep:
            tip = i18nc("@info:tooltip Dynamic WEP security", "Dynamic WEP");
            break;
        case NetworkManager::Utils::WpaPsk:
            tip = i18nc("@info:tooltip WPA-PSK security", "WPA-PSK");
            break;
        case NetworkManager::Utils::WpaEap:
            tip = i18nc("@info:tooltip WPA-EAP security", "WPA-EAP");
            break;
        case NetworkManager::Utils::Wpa2Psk:
            tip = i18nc("@info:tooltip WPA2-PSK security", "WPA2-PSK");
            break;
        case NetworkManager::Utils::Wpa2Eap:
            tip = i18nc("@info:tooltip WPA2-EAP security", "WPA2-EAP");
            break;
        default:
            tip = i18nc("@info:tooltip unknown security", "Unknown security type");
            break;
    }
    return tip;
}

QString UiUtils::connectionDetails(const Device::Ptr& device, const Connection::Ptr& connection, const QStringList& keys)
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;

    const bool connected = device && connection && device->activeConnection() &&
                           device->activeConnection()->connection() == connection && device->activeConnection()->state() == ActiveConnection::Activated;

    foreach (const QString& key, keys) {
        if (key == "interface:name") {
            if (device) {
                QString name;
                if (device->ipInterfaceName().isEmpty()) {
                    name = device->interfaceName();
                } else {
                    name = device->ipInterfaceName();
                }
                details += QString(format).arg(i18n("System name:"), name);
            }
        } else if (key == "ipv4:address") {
            if (device && device->ipV4Config().isValid() && connected) {
                if (device->ipV4Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                if (!addr.isNull()) {
                    details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
                }
            }
        } else if (key == "ipv4:gateway") {
            if (device && device->ipV4Config().isValid() && connected) {
                if (device->ipV4Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV4Config().addresses().first().gateway();
                if (!addr.isNull()) {
                    details += QString(format).arg(i18n("IPv4 Gateway:"), addr.toString());
                }
            }
        } else if (key == "ipv6:address") {
            if (device && device->ipV6Config().isValid() && connected) {
                if (device->ipV6Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                if (!addr.isNull()) {
                    details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
                }
            }
        } else if (key == "ipv6:gateway") {
            if (device && device->ipV6Config().isValid() && connected) {
                if (device->ipV6Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV6Config().addresses().first().gateway();
                if (!addr.isNull() && addr.toString() != "::") {
                    details += QString(format).arg(i18n("IPv6 Gateway:"), addr.toString());
                }
            }
        } else if (key == "interface:driver") {
            if (device) {
                details += QString(format).arg(i18n("Driver:"), device->driver());
            }
        }
    }

    return details;
}

QString UiUtils::bluetoothDetails(const BluetoothDevice::Ptr& btDevice, const QStringList& keys)
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;

    foreach (const QString& key, keys) {
        if (key == "bluetooth:name") {
            if (btDevice) {
                details += QString(format).arg(i18nc("Name", "Bluetooth name"), btDevice->name());
            }
        } else if (key == "interface:hardwareAddress") {
            if (btDevice) {
                details += QString(format).arg(i18n("MAC Address:"), btDevice->hardwareAddress());
            }
        }
    }

    return details;
}

QString UiUtils::modemDetails(const ModemDevice::Ptr& modemDevice, const QStringList& keys)
{
#if WITH_MODEMMANAGER_SUPPORT
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;
    ModemManager::Modem::Ptr modemNetwork;
    ModemManager::Modem3gpp::Ptr gsmNet;
    ModemManager::ModemCdma::Ptr cdmaNet;

    ModemManager::ModemDevice::Ptr modem = ModemManager::findModemDevice(modemDevice->udi());
    if (modem) {
        modemNetwork = modem->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
        gsmNet = modem->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
        cdmaNet = modem->interface(ModemManager::ModemDevice::CdmaInterface).objectCast<ModemManager::ModemCdma>();
    }

    foreach (const QString& key, keys) {
        if (key == "mobile:operator") {
            if (gsmNet) {
                details += QString(format).arg(i18n("Operator:"), gsmNet->operatorName());
            } else if (cdmaNet) {
                details += QString(format).arg(i18n("Network ID:"), cdmaNet->nid());
            }
        } else if (key == "mobile:quality") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Signal Quality:"), QString("%1%").arg(modemNetwork->signalQuality().signal));
            }
        } else if (key == "mobile:technology") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Access Technology:"), UiUtils::convertAccessTechnologyToString(modemNetwork->accessTechnologies()));
            }
        } else if (key == "mobile:mode") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Allowed Mode:"), UiUtils::convertAllowedModeToString(modemNetwork->currentModes().allowed));
            }
        } else if (key == "mobile:unlock") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Unlock Required:"), UiUtils::convertLockReasonToString(modemNetwork->unlockRequired()));
            }
        } else if (key == "mobile:imei") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("IMEI:"), modemNetwork->equipmentIdentifier());
            }
        } else if (key == "mobile:imsi") {
            if (modemDevice) {
                ModemManager::Sim::Ptr simCard;
                simCard = modemDevice->getModemCardIface();
                if (simCard) {
                    details += QString(format).arg(i18n("IMSI:"), simCard->imsi());
                }
            }
        }
    }
    return details;
#else
    Q_UNUSED(modemDevice)
    Q_UNUSED(keys)
    return QString();
#endif
}

QString UiUtils::vpnDetails(const VpnConnection::Ptr& vpnConnection, const VpnSetting::Ptr& vpnSetting, const QStringList& keys)
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;

    foreach (const QString& key, keys) {
        if (key == "vpn:plugin") {
            if (vpnSetting) {
                details += QString(format).arg(i18n("VPN plugin:"), vpnSetting->serviceType().section('.', -1));
            }
        } else if (key == "vpn:banner") {
            if (vpnConnection) {
                details += QString(format).arg(i18n("Banner:"), vpnConnection->banner().simplified());
            }
        }
    }

    return details;
}

QString UiUtils::wimaxDetails(const NetworkManager::WimaxDevice::Ptr& wimaxDevice, const WimaxNsp::Ptr& wimaxNsp, const NetworkManager::Connection::Ptr& connection, const QStringList& keys)
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;

    const bool connected = wimaxDevice && connection && wimaxDevice->activeConnection() &&
                           wimaxDevice->activeConnection()->connection() == connection && wimaxDevice->activeConnection()->state() == ActiveConnection::Activated;

    foreach (const QString& key, keys) {
        if (key == "wimax:bsid") {
            if (connected && wimaxDevice) {
                details += QString(format).arg(i18n("Bsid:"), wimaxDevice->bsid());
            }
        } else if (key == "wimax:nsp") {
            if (wimaxNsp) {
                details += QString(format).arg(i18n("NSP Name:"), wimaxNsp->name());
            }
        } else if (key == "wimax:signal") {
            if (wimaxNsp) {
                details += QString(format).arg(i18n("Signal Quality:"), i18n("%1%", wimaxNsp->signalQuality()));
            }
        } else if (key == "wimax:type") {
            if (wimaxNsp) {
                details += QString(format).arg(i18n("Network Type:"), UiUtils::convertNspTypeToString(wimaxNsp->networkType()));
            }
        }
    }

    return details;
}

QString UiUtils::wiredDetails(const WiredDevice::Ptr& wiredDevice, const NetworkManager::Connection::Ptr& connection, const QStringList& keys)
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;

    const bool connected = wiredDevice && connection && wiredDevice->activeConnection() &&
                           wiredDevice->activeConnection()->connection() == connection && wiredDevice->activeConnection()->state() == ActiveConnection::Activated;

    foreach (const QString& key, keys) {
        if (key == "interface:bitrate") {
            if (wiredDevice && connected) {
                details += QString(format).arg(i18n("Connection speed:"), UiUtils::connectionSpeed(wiredDevice->bitRate()));
            }
        } else if (key == "interface:hardwareaddress") {
            if (wiredDevice) {
                details += QString(format).arg(i18n("MAC Address:"), wiredDevice->permanentHardwareAddress());
            }
        }
    }

    return details;
}

QString UiUtils::wirelessDetails(const WirelessDevice::Ptr& wirelessDevice, const AccessPoint::Ptr& ap, const NetworkManager::Connection::Ptr& connection, const QStringList& keys)
{
    const QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    QString details;

    const bool connected = wirelessDevice && connection && wirelessDevice->activeConnection() &&
                           wirelessDevice->activeConnection()->connection() == connection && wirelessDevice->activeConnection()->state() == ActiveConnection::Activated;


    foreach (const QString& key, keys) {
        if (key == "interface:bitrate") {
            if (wirelessDevice && connected) {
                details += QString(format).arg(i18n("Connection speed:"), UiUtils::connectionSpeed(wirelessDevice->bitRate()));
            }
        } else if (key == "interface:hardwareaddress") {
            if (wirelessDevice) {
                details += QString(format).arg(i18n("MAC Address:"), wirelessDevice->permanentHardwareAddress());
            }
        } else if (key == "wireless:mode") {
            if (wirelessDevice) {
                details += QString(format).arg(i18n("Mode:"), UiUtils::operationModeToString(wirelessDevice->mode()));
            }
        } else if (key == "wireless:signal") {
            if (ap) {
                details += QString(format).arg(i18n("Signal strength:"), i18n("%1%", ap->signalStrength()));
            }
        } else if (key == "wireless:ssid") {
            if (ap) {
                details += QString(format).arg(i18n("Access point (SSID):"), ap->ssid());
            }
        } else if (key == "wireless:accesspoint") {
            if (ap) {
                details += QString(format).arg(i18n("Access point (BSSID):"), ap->hardwareAddress());
            }
        } else if (key == "wireless:channel") {
            if (ap) {
                details += QString(format).arg(i18nc("Wifi AP channel and frequency", "Channel:"), i18n("%1 (%2 MHz)", NetworkManager::Utils::findChannel(ap->frequency()), ap->frequency()));
            }
        } else if (key == "wireless:security") {
            NetworkManager::Utils::WirelessSecurityType security = Utils::Unknown;
            if (ap) {
                security = NetworkManager::Utils::findBestWirelessSecurity(wirelessDevice->wirelessCapabilities(), true, (wirelessDevice->mode() == NetworkManager::WirelessDevice::Adhoc),
                                                                           ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());
                if (security != Utils::Unknown) {
                    details += QString(format).arg(i18n("Security:"), UiUtils::labelFromWirelessSecurity(security));
                }
            } else if (connection) {
                // Necessary for example for AdHoc connections
                security = Utils::securityTypeFromConnectionSetting(connection->settings());
                if (security != Utils::Unknown) {
                    details += QString(format).arg(i18n("Security:"), UiUtils::labelFromWirelessSecurity(security));
                }
            }
        } else if (key == "wireless:band") {
            if (ap) {
                details += QString(format).arg(i18n("Frequency band:"), UiUtils::wirelessBandToString(NetworkManager::Utils::findFrequencyBand(ap->frequency())));
            }
        }
    }

    return details;
}

QString UiUtils::formatDateRelative(const QDateTime & lastUsed)
{
    QString lastUsedText;
    if (lastUsed.isValid()) {
        const QDateTime now = QDateTime::currentDateTime();
        if (lastUsed.daysTo(now) == 0 ) {
            int secondsAgo = lastUsed.secsTo(now);
            if (secondsAgo < (60 * 60 )) {
                int minutesAgo = secondsAgo / 60;
                lastUsedText = i18ncp(
                                   "Label for last used time for a network connection used in the last hour, as the number of minutes since usage",
                                   "One minute ago",
                                   "%1 minutes ago",
                                   minutesAgo);
            } else {
                int hoursAgo = secondsAgo / (60 * 60);
                lastUsedText = i18ncp(
                                   "Label for last used time for a network connection used in the last day, as the number of hours since usage",
                                   "One hour ago",
                                   "%1 hours ago",
                                   hoursAgo);
            }
        } else if (lastUsed.daysTo(now) == 1) {
            lastUsedText = i18nc("Label for last used time for a network connection used the previous day", "Yesterday");
        } else {
            lastUsedText = KGlobal::locale()->formatDate(lastUsed.date(), KLocale::ShortDate);
        }
    } else {
        lastUsedText =  i18nc("Label for last used time for a "
                              "network connection that has never been used", "Never");
    }
    return lastUsedText;
}

QString UiUtils::formatLastUsedDateRelative(const QDateTime & lastUsed)
{
    QString lastUsedText;
    if (lastUsed.isValid()) {
        const QDateTime now = QDateTime::currentDateTime();
        if (lastUsed.daysTo(now) == 0 ) {
            const int secondsAgo = lastUsed.secsTo(now);
            if (secondsAgo < (60 * 60 )) {
                const int minutesAgo = secondsAgo / 60;
                lastUsedText = i18ncp(
                                   "Label for last used time for a network connection used in the last hour, as the number of minutes since usage",
                                   "Last used one minute ago",
                                   "Last used %1 minutes ago",
                                   minutesAgo);
            } else {
                const int hoursAgo = secondsAgo / (60 * 60);
                lastUsedText = i18ncp(
                                   "Label for last used time for a network connection used in the last day, as the number of hours since usage",
                                   "Last used one hour ago",
                                   "Last used %1 hours ago",
                                   hoursAgo);
            }
        } else if (lastUsed.daysTo(now) == 1) {
            lastUsedText = i18nc("Label for last used time for a network connection used the previous day", "Last used yesterday");
        } else {
            lastUsedText = i18n("Last used on %1", KGlobal::locale()->formatDate(lastUsed.date(), KLocale::ShortDate));
        }
    } else {
        lastUsedText =  i18nc("Label for last used time for a "
                              "network connection that has never been used", "Never used");
    }
    return lastUsedText;
}
