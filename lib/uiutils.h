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

#ifndef PLASMA_NM_UIUTILS_H
#define PLASMA_NM_UIUTILS_H

class QSizeF;

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/WimaxNsp>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/WimaxDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/VpnConnection>
#include <NetworkManagerQt/VpnSetting>

#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
#include <ModemManager/ModemManager.h>
#include <ModemManagerQt/modem.h>
#endif
#endif

#include <kdemacros.h>

#include "config.h"

class KDE_EXPORT UiUtils
{
public:

    /**
     * @return a human-readable description for the network interface type for use as label
     * @param type the type of the network interface
     */
    static QString interfaceTypeLabel(const NetworkManager::Device::Type type, const NetworkManager::Device::Ptr iface);
#if 0

    /**
     * @return a human-readable name for a given network interface according to the configured
     * naming style
     * @param uni uni of the network interface
     */
    static QString interfaceNameLabel(const QString & uni);

    /**
     * @return a human-readable name for a given network interface according to the configured
     * naming style
     * @param uni uni of the network interface
     * @param interfaceNamingStyle name style to use when generating the name
     */
    static QString interfaceNameLabel(const QString & uni, const KNetworkManagerServicePrefs::InterfaceNamingChoices interfaceNamingStyle);
#endif
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
#if 0
    /**
     * @return a human-readable description of the connection state of a given interface connection
     * @param state The connection state
     */
    static QString connectionStateToString(Knm::InterfaceConnection::ActivationState state, const QString &connectionName = QString());
#endif

    /**
     * @return an icon name suitable for the interface type
     * @param iface the network interface
     */
    static QString iconName(const NetworkManager::Device::Ptr &device);

    static QString iconAndTitleForConnectionSettingsType(NetworkManager::ConnectionSettings::ConnectionType type,
                                                         QString &title);
#if 0
    /** This method can be used to retrieve an icon size that fits into a given size.
     * The resulting size can be used to render Pixmaps from KIconLoader without
     * rescaling them (and thereby losing quality)
     *
     * @return a size available in KIconLoader.
     * @param size The size of the rect it should fit in
     */
    static int iconSize(const QSizeF size);

    /** This method can be used to retrieve the progress of a connection attempt
     * as a qreal, for painting progress bars.
     *
     * @return the progress between 0 (disconnected) and 1 (activated).
     * @param interface the network interface
     */
    static qreal interfaceState(const NetworkManager::Device *interface);

    /** This method can be used to retrieve the progress of a connection attempt
     * as a qreal, for painting progress bars. This is mostly used by VPN connections,
     * which do not have NetworkManager::Device associated to them.
     *
     * @return the progress between 0 (unknown) and 1 (activated).
     * @param remote interface connection
     */
    static qreal interfaceConnectionState(const RemoteInterfaceConnection *ic);
#endif
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
#ifdef MODEMMANAGERQT_ONE
//     static QString convertTypeToString(ModemManager::ModemDevice::InterfaceType type);
    static QString convertBandsToString(const QList<MMModemBand> & band);
    static QString convertAllowedModeToString(ModemManager::Modem::ModemModes mode);
    static QString convertAccessTechnologyToString(ModemManager::Modem::AccessTechnologies tech);
    static QString convertLockReasonToString(MMModemLock reason);
#else
    static QString convertTypeToString(const ModemManager::ModemInterface::Type type);
    static QString convertBandToString(const ModemManager::ModemInterface::Band band);
    static QString convertAllowedModeToString(const ModemManager::ModemInterface::AllowedMode mode);
    static QString convertAccessTechnologyToString(const ModemManager::ModemInterface::AccessTechnology tech);
#endif
#endif
    static NetworkManager::ModemDevice::Capability modemSubType(NetworkManager::ModemDevice::Capabilities modemCaps);
    static QString convertNspTypeToString(NetworkManager::WimaxNsp::NetworkType type);

    static QString shortToolTipFromWirelessSecurity(NetworkManager::Utils::WirelessSecurityType type);
    static QString labelFromWirelessSecurity(NetworkManager::Utils::WirelessSecurityType type);

    static QString connectionDetails(const NetworkManager::Device::Ptr & device, const NetworkManager::Connection::Ptr & connection, const QStringList & keys);
    static QString bluetoothDetails(const NetworkManager::BluetoothDevice::Ptr & btDevice, const QStringList & keys);
    static QString modemDetails(const NetworkManager::ModemDevice::Ptr & modemDevice, const QStringList & keys);
    static QString vpnDetails(const NetworkManager::VpnConnection::Ptr & vpnConnection, const NetworkManager::VpnSetting::Ptr & vpnSetting, const QStringList & keys);
    static QString wimaxDetails(const NetworkManager::WimaxDevice::Ptr & wimaxDevice, const NetworkManager::WimaxNsp::Ptr & wimaxNsp, const NetworkManager::Connection::Ptr & connection, const QStringList & keys);
    static QString wiredDetails(const NetworkManager::WiredDevice::Ptr & wiredDevice, const NetworkManager::Connection::Ptr & connection, const QStringList & keys);
    static QString wirelessDetails(const NetworkManager::WirelessDevice::Ptr & wirelessDevice, const NetworkManager::WirelessNetwork::Ptr & network, const NetworkManager::Connection::Ptr & connection, const QStringList & keys);
};
#endif // UIUTILS_H
