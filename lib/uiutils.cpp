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
#if 0
#include <KIconLoader>
#include <kdeversion.h>
#endif

#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

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
        deviceText = i18nc("title of the interface widget in nm's popup", "Wimax");
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
            QString specific = "connected-100";
            NetworkManager::WirelessDevice::Ptr wiface = device.objectCast<NetworkManager::WirelessDevice>();

            if (wiface) {
                NetworkManager::AccessPoint::Ptr ap = wiface->activeAccessPoint();
                if (ap) {
                    int s = ap->signalStrength();
                    if (s < 13) {
                        specific = "connected-00";
                    } else if (s < 38) {
                        specific = "connected-25";
                    } else if (s < 63) {
                        specific = "connected-50";
                    } else if (s < 88) {
                        specific = "connected-75";
                    } else if (s >= 88) {
                        specific = "connected-100";
                    }
                } else {
                        specific = "disconnected";
                }
            }
            icon = "network-wireless-" + specific;
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

QString UiUtils::iconAndTitleForConnectionSettingsType(NetworkManager::ConnectionSettings::ConnectionType type, QString &title)
{
    QString icon;
    switch (type) {
    case ConnectionSettings::Adsl:
        title = i18n("ADSL");
        icon = "modem";
        break;
    case ConnectionSettings::Pppoe:
        title = i18n("DSL");
        icon = "modem";
        break;
    case ConnectionSettings::Bluetooth:
        title = i18n("Bluetooth");
        icon = "preferences-system-bluetooth";
        break;
    case ConnectionSettings::Bond:
        title = i18n("Bond");
        break;
    case ConnectionSettings::Bridge:
        title = i18n("Bridge");
        break;
    case ConnectionSettings::Gsm:
    case ConnectionSettings::Cdma:
        title = i18n("Mobile broadband");
        icon = "phone";
        break;
    case ConnectionSettings::Infiniband:
        title = i18n("Infiniband");
        break;
    case ConnectionSettings::OLPCMesh:
        title = i18n("Olpc mesh");
        break;
    case ConnectionSettings::Vlan:
        title = i18n("VLAN");
        break;
    case ConnectionSettings::Vpn:
        title = i18n("VPN");
        icon = "secure-card";
        break;
    case ConnectionSettings::Wimax:
        title = i18n("WiMAX");
        icon = "network-wireless";
        break;
    case ConnectionSettings::Wired:
        title = i18n("Wired");
        icon = "network-wired";
        break;
    case ConnectionSettings::Wireless:
        title = i18n("Wireless");
        icon = "network-wireless";
        break;
    default:
        title = i18n("Unknown connection type");
        break;
    }
    return icon;
}

#if 0
int UiUtils::iconSize(const QSizeF size)
{
    int s = qMin(size.width(), size.height());
    // return the biggest fitting icon size from KIconLoader
    if (s >= KIconLoader::SizeEnormous) { // 128
        s = KIconLoader::SizeEnormous;
    } else if (s >= KIconLoader::SizeHuge) { // 64
        s = KIconLoader::SizeHuge;
    } else if (s >= KIconLoader::SizeLarge) { // 48
        s = KIconLoader::SizeLarge;
    } else if (s >= KIconLoader::SizeMedium) { // 32
        s = KIconLoader::SizeMedium;
    } else if (s >= KIconLoader::SizeSmallMedium) { // 22
        s = KIconLoader::SizeSmallMedium;
    } else { // 16
        s = KIconLoader::SizeSmall;
    }
    return s;
}
#endif


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

#if 0
QString UiUtils::connectionStateToString(Knm::InterfaceConnection::ActivationState state, const QString &connectionName)
{
    NetworkManager::Device::State s = NetworkManager::Device::UnknownState;

    switch(state) {
    case Knm::InterfaceConnection::Unknown:
        s = NetworkManager::Device::UnknownState;
        break;
    case Knm::InterfaceConnection::Activating:
        s = NetworkManager::Device::ConfiguringHardware;
        break;
    case Knm::InterfaceConnection::Activated:
        s = NetworkManager::Device::Activated;
        break;
    }

    return connectionStateToString(s, connectionName);
}

Solid::Device* UiUtils::findSolidDevice(const QString & uni)
{
    NetworkManager::Device * iface = NetworkManager::findNetworkInterface(uni);

    if (!iface) {
        return 0;
    }

    QList<Solid::Device> list = Solid::Device::listFromQuery(QString::fromLatin1("NetworkInterface.ifaceName == '%1'").arg(iface->interfaceName()));
    QList<Solid::Device>::iterator it = list.begin();

    if (it != list.end()) {
        Solid::Device* dev = new Solid::Device(*it);
        return dev;
    }

    return 0;
}

QString UiUtils::interfaceNameLabel(const QString & uni, const KNetworkManagerServicePrefs::InterfaceNamingChoices interfaceNamingStyle)
{
    QString label;
    NetworkManager::Device * iface = NetworkManager::findNetworkInterface(uni);

    switch (interfaceNamingStyle) {
        case KNetworkManagerServicePrefs::SystemNames:
            if (iface) {
                label = iface->interfaceName();
            }
            break;
        case KNetworkManagerServicePrefs::VendorProductNames: {
            Solid::Device* dev = findSolidDevice(uni);
            if (dev) {
                if (!dev->vendor().isEmpty() && !dev->product().isEmpty()) {
                    label = i18nc("Format for <Vendor> <Product>", "%1 - %2", dev->vendor(), dev->product());
                }
                delete dev;
            }
            break;
        }
        case KNetworkManagerServicePrefs::TypeNames:
        default:
            break;
    }

    if (label.isEmpty()) {
        // if we don't get sensible information from Solid,
        // let's try to use the type of the interface
        if (iface) {
            label = UiUtils::interfaceTypeLabel(iface->type(), iface);
        }
    }
    return label;
}

QString UiUtils::interfaceNameLabel(const QString & uni)
{
    return interfaceNameLabel(uni, static_cast<KNetworkManagerServicePrefs::InterfaceNamingChoices>(KNetworkManagerServicePrefs::self()->interfaceNamingStyle()));
}

qreal UiUtils::interfaceState(const NetworkManager::Device *interface)
{
    if (!interface) {
        return 0;
    }

    // from libs/types.h
    switch (interface->state()) {
        case NetworkManager::Device::Preparing:
            return 0.15;
            break;
        case NetworkManager::Device::ConfiguringHardware:
            return 0.30;
            break;
        case NetworkManager::Device::NeedAuth:
            return 0.45;
            break;
        case NetworkManager::Device::ConfiguringIp:
            return 0.60;
            break;
        case NetworkManager::Device::CheckingIp:
            return 0.75;
            break;
        case NetworkManager::Device::WaitingForSecondaries:
            return 0.90;
            break;
        case NetworkManager::Device::Activated:
            return 1.0;
            break;
        default:
            return 0;
            break;
    }
    return 0;
}

qreal UiUtils::interfaceConnectionState(const RemoteInterfaceConnection *ic)
{
    if (!ic) {
        return 0;
    }
    switch (ic->activationState()) {
    case Knm::InterfaceConnection::Activating:
        return 0.33;
        break;
    case Knm::InterfaceConnection::Activated:
        return 1.0;
        break;
    case Knm::InterfaceConnection::Unknown:
    default:
        return 0;
        break;
    }
    return 0;
}

#endif

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

QString UiUtils::convertTypeToString(const ModemManager::ModemInterface::Type type)
{
    switch (type) {
        case ModemManager::ModemInterface::UnknownType: return i18nc("Unknown cellular type","Unknown");
        case ModemManager::ModemInterface::GsmType: return i18nc("Gsm cellular type","Gsm");
        case ModemManager::ModemInterface::CdmaType: return i18nc("Cdma cellular type","Cdma");
    }

    return i18nc("Unknown cellular type","Unknown");
}

QString UiUtils::convertBandToString(const ModemManager::ModemInterface::Band band)
{
    switch (band) {
        case ModemManager::ModemInterface::UnknownBand: return i18nc("Unknown cellular frequency band","Unknown");
        case ModemManager::ModemInterface::AnyBand: return i18nc("Any cellular frequency band","Any");
        case ModemManager::ModemInterface::Egsm: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 900 MHz");
        case ModemManager::ModemInterface::Dcs: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 1800 MHz");
        case ModemManager::ModemInterface::Pcs: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 1900 MHz");
        case ModemManager::ModemInterface::G850: return i18nc("Cellular frequency band","GSM/GPRS/EDGE 850 MHz");
        case ModemManager::ModemInterface::U2100: return i18nc("Cellular frequency band","WCDMA 2100 MHz (Class I)");
        case ModemManager::ModemInterface::U1800: return i18nc("Cellular frequency band","WCDMA 3GPP 1800 MHz (Class III)");
        case ModemManager::ModemInterface::U17IV: return i18nc("Cellular frequency band","WCDMA 3GPP AWS 1700/2100 MHz (Class IV)");
        case ModemManager::ModemInterface::U800: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 800 MHz (Class VI)");
        case ModemManager::ModemInterface::U850: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 850 MHz (Class V)");
        case ModemManager::ModemInterface::U900: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 900 MHz (Class VIII)");
        case ModemManager::ModemInterface::U17IX: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 1700 MHz (Class IX)");
        case ModemManager::ModemInterface::U19IX: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 1900 MHz (Class II)");
        case ModemManager::ModemInterface::U2600: return i18nc("Cellular frequency band","WCDMA 3GPP UMTS 2600 MHz (Class VII)");
    }

    return i18nc("Unknown cellular frequency band","Unknown");
}

QString UiUtils::convertAllowedModeToString(const ModemManager::ModemInterface::AllowedMode mode)
{
    switch (mode) {
        case ModemManager::ModemInterface::AnyModeAllowed: return i18nc("Allowed Gsm modes (2G/3G/any)","Any");
        case ModemManager::ModemInterface::Prefer2g: return i18nc("Allowed Gsm modes (2G/3G/any)","Prefer 2G");
        case ModemManager::ModemInterface::Prefer3g: return i18nc("Allowed Gsm modes (2G/3G/any)","Prefer 3G");
        case ModemManager::ModemInterface::UseOnly2g: return i18nc("Allowed Gsm modes (2G/3G/any)","Only 2G");
        case ModemManager::ModemInterface::UseOnly3g: return i18nc("Allowed Gsm modes (2G/3G/any)","Only 3G");
    }

    return i18nc("Allowed Gsm modes (2G/3G/any)","Any");
}

QString UiUtils::convertAccessTechnologyToString(const ModemManager::ModemInterface::AccessTechnology tech)
{
    switch (tech) {
        case ModemManager::ModemInterface::UnknownTechnology: return i18nc("Unknown cellular access technology","Unknown");
        case ModemManager::ModemInterface::Gsm: return i18nc("Cellular access technology","GSM");
        case ModemManager::ModemInterface::GsmCompact: return i18nc("Cellular access technology","Compact GSM");
        case ModemManager::ModemInterface::Gprs: return i18nc("Cellular access technology","GPRS");
        case ModemManager::ModemInterface::Edge: return i18nc("Cellular access technology","EDGE");
        case ModemManager::ModemInterface::Umts: return i18nc("Cellular access technology","UMTS");
        case ModemManager::ModemInterface::Hsdpa: return i18nc("Cellular access technology","HSDPA");
        case ModemManager::ModemInterface::Hsupa: return i18nc("Cellular access technology","HSUPA");
        case ModemManager::ModemInterface::Hspa: return i18nc("Cellular access technology","HSPA");
        case ModemManager::ModemInterface::HspaPlus: return i18nc("Cellular access technology","HSPA+");
        case ModemManager::ModemInterface::Lte: return i18nc("Cellular access technology","LTE");
    }

    return i18nc("Unknown cellular access technology","Unknown");
}

QString UiUtils::convertNspTypeToString(WimaxNsp::NetworkType type)
{
    switch (type) {
        case WimaxNsp::Unknown: return i18n("Unknown");
        case WimaxNsp::Home: return i18n("Home");
        case WimaxNsp::Partner: return i18n("Partner");
        case WimaxNsp::RoamingPartner: return i18n("Roaming partner");
    }

    return i18n("Unknown");
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

QString UiUtils::deviceDetails(const Device::Ptr& device, ConnectionSettings::ConnectionType type, bool connected, bool connecting, const QStringList& keys, const QString& format)
{
    QString details;

    foreach (const QString& key, keys) {
        if (key == "interface:type") {
            if (type != NetworkManager::ConnectionSettings::Unknown) {
                details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::ConnectionSettings::typeAsString(type));
            }
        } else if (key == "interface:status") {
            QString status = i18n("Disconnected");
            if (connecting) {
                status = i18n("Connecting");
            } else if (connected) {
                status = i18n("Connected");
            }
            details += QString(format).arg(i18n("Status:"), status);
        } else if (key == "interface:name") {
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
                details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }
        } else if (key == "ipv4:gateway") {
            if (device && device->ipV4Config().isValid() && connected) {
                if (device->ipV4Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV4Config().addresses().first().gateway();
                details += QString(format).arg(i18n("IPv4 Gateway:"), addr.toString());
            }
        } else if (key == "ipv6:address") {
            if (device && device->ipV6Config().isValid() && connected) {
                if (device->ipV6Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }
        } else if (key == "ipv6:gateway") {
            if (device && device->ipV6Config().isValid() && connected) {
                if (device->ipV6Config().addresses().isEmpty()) {
                    continue;
                }
                QHostAddress addr = device->ipV6Config().addresses().first().gateway();
                details += QString(format).arg(i18n("IPv6 Gateway:"), addr.toString());
            }
        } else if (key == "interface:driver") {
            if (device) {
                details += QString(format).arg(i18n("Driver:"), device->driver());
            }
        }
    }

    return details;
}

QString UiUtils::bluetoothDetails(const BluetoothDevice::Ptr& btDevice, const QStringList& keys, const QString& format)
{
    QString details;

    foreach (const QString& key, keys) {
        if (key == "bluetooth:name") {
            if (btDevice) {
                details += QString(format).arg(i18n("Name:"), btDevice->name());
            }
        } else if (key == "interface:hardwareAddress") {
            if (btDevice) {
                details += QString(format).arg(i18n("MAC Address:"), btDevice->hardwareAddress());
            }
        } else if (key == "interface:driver") {
            if (btDevice) {
                details += QString(format).arg(i18n("Driver:"), btDevice->driver());
            }
        }
    }

    return details;
}

QString UiUtils::modemDetails(const ModemDevice::Ptr& modemDevice, const QStringList& keys, const QString& format)
{
    QString details;

    ModemManager::ModemGsmNetworkInterface::Ptr modemNetwork = modemDevice->getModemNetworkIface().objectCast<ModemManager::ModemGsmNetworkInterface>();

    foreach (const QString& key, keys) {
        if (key == "mobile:operator") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Operator:"), modemNetwork->getRegistrationInfo().operatorName);
            }
        } else if (key == "mobile:quality") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Signal quality:"), QString("%1%").arg(modemNetwork->getSignalQuality()));
            }
        } else if (key == "mobile:technology") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Access technology:"), QString("%1/%2").arg(UiUtils::convertTypeToString(modemNetwork->type()), UiUtils::convertAccessTechnologyToString(modemNetwork->getAccessTechnology())));
            }
        } else if (key == "mobile:mode") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Allowed Mode"), UiUtils::convertAllowedModeToString(modemNetwork->getAllowedMode()));
            }
        } else if (key == "mobile:band") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Frequency Band:"), UiUtils::convertBandToString(modemNetwork->getBand()));
            }
        } else if (key == "mobile:unlock") {
            if (modemNetwork) {
                details += QString(format).arg(i18n("Unlock Required:"), modemNetwork->unlockRequired());
            }
        } else if (key == "mobile:imei") {
            if (modemDevice) {
                ModemManager::ModemGsmCardInterface::Ptr modemCard;
                modemCard = modemDevice->getModemCardIface();
                if (modemCard) {
                    details += QString(format).arg(i18n("IMEI:"), modemCard->getImei());
                }
            }
        } else if (key == "mobile:imsi") {
            if (modemDevice) {
                ModemManager::ModemGsmCardInterface::Ptr modemCard;
                modemCard = modemDevice->getModemCardIface();
                if (modemCard) {
                    details += QString(format).arg(i18n("IMSI:"), modemCard->getImsi());
                }
            }
        }
    }

    return details;
}

QString UiUtils::vpnDetails(const VpnConnection::Ptr& vpnConnection, const VpnSetting::Ptr& vpnSetting, const QStringList& keys, const QString& format)
{
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

QString UiUtils::wimaxDetails(const NetworkManager::WimaxDevice::Ptr& wimaxDevice, const WimaxNsp::Ptr& wimaxNsp, bool connected, const QStringList& keys, const QString& format)
{
    QString details;

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

QString UiUtils::wiredDetails(const WiredDevice::Ptr& wiredDevice, bool connected, const QStringList& keys, const QString& format)
{
    QString details;

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

QString UiUtils::wirelessDetails(const WirelessDevice::Ptr& wirelessDevice, const WirelessNetwork::Ptr& network, const AccessPoint::Ptr& ap, bool connected, const QStringList& keys, const QString& format)
{
    QString details;

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
            if (network) {
                details += QString(format).arg(i18n("Signal strength:"), i18n("%1%", network->signalStrength()));
            }
        } else if (key == "wireless:ssid") {
            if (network) {
                details += QString(format).arg(i18n("Access point (SSID):"), network->ssid());
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
            if (ap) {
                NetworkManager::Utils::WirelessSecurityType security = NetworkManager::Utils::findBestWirelessSecurity(wirelessDevice->wirelessCapabilities(), true, (wirelessDevice->mode() == NetworkManager::WirelessDevice::Adhoc),
                                                                                                                        ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());
                details += QString(format).arg(i18n("Security:"), UiUtils::labelFromWirelessSecurity(security));
            }
        } else if (key == "wireless:band") {
            if (ap) {
                details += QString(format).arg(i18n("Frequency band:"), UiUtils::wirelessBandToString(NetworkManager::Utils::findFrequencyBand(ap->frequency())));
            }
        }
    }

    return details;
}

