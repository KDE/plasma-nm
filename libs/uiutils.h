/*
    SPDX-FileCopyrightText: 2008-2010 Sebastian Kügler <sebas@kde.org>
    SPDX-FileCopyrightText: 2013-2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_UIUTILS_H
#define PLASMA_NM_UIUTILS_H


#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/VpnConnection>
#include <NetworkManagerQt/VpnSetting>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManager/ModemManager.h>
#include <ModemManagerQt/modem.h>
#endif

class Q_DECL_EXPORT UiUtils
{
public:
    enum SortedConnectionType {
        Wired,
        Wireless,
        Gsm,
        Cdma,
        Pppoe,
        Adsl,
        Infiniband,
        OLPCMesh,
        Bluetooth,
        Wireguard,
        Vpn,
        Vlan,
        Bridge,
        Bond,
        Team,
        Unknown
    };

    /*
     * @return sorted connection type used to prioritize specific connection types
     */
    static SortedConnectionType connectionTypeToSortedType(NetworkManager::ConnectionSettings::ConnectionType type);

    /*
     * @return whether given connection type is supported
     * Currently ignored connection types: Bond, Bridge, Generic, Infiniband, Team, Vlan, Tun
     */
    static bool isConnectionTypeSupported(NetworkManager::ConnectionSettings::ConnectionType type);

    /**
     * @return true if the connection is virtual.
     * @param type Type of the network connection
     */
    static bool isConnectionTypeVirtual(NetworkManager::ConnectionSettings::ConnectionType type);

    /**
     * @return a human-readable description for the network interface type for use as label
     * @param type the type of the network interface
     */
    static QString interfaceTypeLabel(const NetworkManager::Device::Type type, const NetworkManager::Device::Ptr iface);

    /**
     * @return a human-readable name for a given network interface according to the configured
     * naming style
     * @param type type of the network interface
     * @param interfaceName name of the network interface (eg eth0)
     */
    static QString prettyInterfaceName(NetworkManager::Device::Type type, const QString &interfaceName);

    /**
     * @return a human-readable description of the connection state of a given network interface
     * @param state The connection state
     */
    static QString connectionStateToString(NetworkManager::Device::State state, const QString &connectionName = QString());

    static QString vpnConnectionStateToString(NetworkManager::VpnConnection::State state);

    static QString iconAndTitleForConnectionSettingsType(NetworkManager::ConnectionSettings::ConnectionType type,
                                                         QString &title);
    /**
     * @return a human-readable description of operation mode.
     * @param mode the operation mode
     */
    static QString operationModeToString(NetworkManager::WirelessDevice::OperationMode mode);

    /**
     * @return string list with a human-readable description of wpa flags.
     * @param flags the wpa flags
     */
    static QStringList wpaFlagsToStringList(NetworkManager::AccessPoint::WpaFlags flags);

    /**
     * @return localized string showing a human-readable connection speed. 1000 is used as base.
     * @param bitrate bitrate of the connection per second
     */
    static QString connectionSpeed(double bitrate);

    /**
     * @param band The band of a wireless network. The value corresponds to the type enum in Knm::WirelessSetting::EnumBand
     * @return A string representation
     */
    static QString wirelessBandToString(NetworkManager::WirelessSetting::FrequencyBand band);

#if WITH_MODEMMANAGER_SUPPORT
    static QString convertAllowedModeToString(ModemManager::Modem::ModemModes mode);
    static QString convertAccessTechnologyToString(ModemManager::Modem::AccessTechnologies tech);
    static QString convertLockReasonToString(MMModemLock reason);
#endif
    static NetworkManager::ModemDevice::Capability modemSubType(NetworkManager::ModemDevice::Capabilities modemCaps);
    static QString labelFromWirelessSecurity(NetworkManager::WirelessSecurityType type);

    static QString formatDateRelative(const QDateTime & lastUsed);
    static QString formatLastUsedDateRelative(const QDateTime & lastUsed);
};
#endif // UIUTILS_H
