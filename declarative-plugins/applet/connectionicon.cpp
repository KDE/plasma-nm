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

#include "connectionicon.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WirelessSetting>

#include "debug.h"

ConnectionIcon::ConnectionIcon(QObject* parent)
    : QObject(parent)
    , m_signal(0)
    , m_wirelessNetwork(0)
    , m_connecting(false)
    , m_vpn(false)
#if WITH_MODEMMANAGER_SUPPORT
    , m_modemNetwork(0)
#endif
{
    connect(NetworkManager::notifier(), SIGNAL(primaryConnectionChanged(QString)),
            SLOT(primaryConnectionChanged(QString)));
    connect(NetworkManager::notifier(), SIGNAL(activatingConnectionChanged(QString)),
            SLOT(activatingConnectionChanged(QString)));
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionAdded(QString)),
            SLOT(activeConnectionAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            SLOT(deviceRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
            SLOT(wirelessEnabledChanged(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
            SLOT(wirelessEnabledChanged(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanEnabledChanged(bool)),
            SLOT(wwanEnabledChanged(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanHardwareEnabledChanged(bool)),
            SLOT(wwanEnabledChanged(bool)));
    connect(NetworkManager::notifier(), SIGNAL(networkingEnabledChanged(bool)),
            SLOT(networkingEnabledChanged(bool)));
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)));

    foreach (NetworkManager::Device::Ptr device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Ethernet) {
            NetworkManager::WiredDevice::Ptr wiredDevice = device.staticCast<NetworkManager::WiredDevice>();
            if (wiredDevice) {
                connect(wiredDevice.data(), SIGNAL(carrierChanged(bool)),
                        SLOT(carrierChanged(bool)));
            }
        }
    }

    foreach (NetworkManager::ActiveConnection::Ptr activeConnection, NetworkManager::activeConnections()) {
        if (activeConnection->vpn()) {
            NetworkManager::VpnConnection::Ptr vpnConnection;
            vpnConnection = activeConnection.objectCast<NetworkManager::VpnConnection>();
            if (vpnConnection) {
                connect(vpnConnection.data(), SIGNAL(stateChanged(NetworkManager::VpnConnection::State,NetworkManager::VpnConnection::StateChangeReason)),
                        SLOT(vpnConnectionStateChanged(NetworkManager::VpnConnection::State,NetworkManager::VpnConnection::StateChangeReason)), Qt::UniqueConnection);
                if (vpnConnection->state() == NetworkManager::VpnConnection::Activated) {
                    m_vpn = true;
                }
            }
        }
    }

    setIcons();
}

ConnectionIcon::~ConnectionIcon()
{
}

bool ConnectionIcon::connecting() const
{
    return m_connecting;
}

QString ConnectionIcon::connectionIcon() const
{
    if (m_vpn ) {
        return m_connectionIcon + "-locked";
    }
    return m_connectionIcon;
}

QString ConnectionIcon::connectionTooltipIcon() const
{
    return m_connectionTooltipIcon;
}

void ConnectionIcon::activatingConnectionChanged(const QString& connection)
{
    if (connection != "/") {
        setIcons();
    }
}

void ConnectionIcon::activeConnectionAdded(const QString &activeConnection)
{
    NetworkManager::ActiveConnection::Ptr active = NetworkManager::findActiveConnection(activeConnection);

    if (active) {
        NetworkManager::VpnConnection::Ptr vpnConnection;
        if (active->vpn()) {
            vpnConnection = active.objectCast<NetworkManager::VpnConnection>();
        }
        if ((active->state() == NetworkManager::ActiveConnection::Activating) ||
            (vpnConnection && (vpnConnection->state() == NetworkManager::VpnConnection::Prepare ||
                               vpnConnection->state()  == NetworkManager::VpnConnection::NeedAuth ||
                               vpnConnection->state()  == NetworkManager::VpnConnection::Connecting ||
                               vpnConnection->state()  == NetworkManager::VpnConnection::GettingIpConfig))) {
            connect(active.data(), SIGNAL(destroyed(QObject*)),
                    SLOT(activeConnectionDestroyed()));
            if (vpnConnection) {
                connect(vpnConnection.data(), SIGNAL(stateChanged(NetworkManager::VpnConnection::State,NetworkManager::VpnConnection::StateChangeReason)),
                        SLOT(vpnConnectionStateChanged(NetworkManager::VpnConnection::State,NetworkManager::VpnConnection::StateChangeReason)), Qt::UniqueConnection);
            } else {
                connect(active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                        SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
            }
            NMAppletDebug() << "Emit signal connectingChanged(true)";
            m_connecting = true;
            Q_EMIT connectingChanged(true);
        }
    }
}

void ConnectionIcon::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    if (state == NetworkManager::ActiveConnection::Deactivated ||
        state == NetworkManager::ActiveConnection::Deactivating ||
        state == NetworkManager::ActiveConnection::Activated ||
        state == NetworkManager::ActiveConnection::Unknown) {
        NMAppletDebug() << "Emit signal connectingChanged(false)";
        m_connecting = false;
        Q_EMIT connectingChanged(false);
    }
}

void ConnectionIcon::activeConnectionDestroyed()
{
    NMAppletDebug() << "Emit signal connectingChanged(false)";
    m_connecting = false;
    Q_EMIT connectingChanged(false);
}

void ConnectionIcon::carrierChanged(bool carrier)
{
    Q_UNUSED(carrier);
    if (NetworkManager::status() == NetworkManager::Disconnected) {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::deviceAdded(const QString& device)
{
    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);

    if (!dev) {
        return;
    }

    if (dev->type() == NetworkManager::Device::Ethernet) {
        NetworkManager::WiredDevice::Ptr wiredDev = dev.objectCast<NetworkManager::WiredDevice>();
        connect(wiredDev.data(), SIGNAL(carrierChanged(bool)),
                SLOT(carrierChanged(bool)));
    }
}

void ConnectionIcon::deviceRemoved(const QString& device)
{
    Q_UNUSED(device);

    if (NetworkManager::status() == NetworkManager::Disconnected) {
        setDisconnectedIcon();
    }
}

#if WITH_MODEMMANAGER_SUPPORT
void ConnectionIcon::modemNetworkRemoved()
{
    m_modemNetwork.clear();
}

void ConnectionIcon::modemSignalChanged(uint signal)
{
    int diff = m_signal - signal;

    if (diff >= 10 ||
        diff <= -10) {
        m_signal = signal;

        setIconForModem();
    }
}
#endif
void ConnectionIcon::networkingEnabledChanged(bool enabled)
{
    if (!enabled) {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-unavailable)";
        m_connectionIcon = "network-unavailable";
        Q_EMIT connectionIconChanged("network-unavailable");
    }
}

void ConnectionIcon::primaryConnectionChanged(const QString& connection)
{
    if (!connection.isEmpty()) {
        setIcons();
    }
}

void ConnectionIcon::statusChanged(NetworkManager::Status status)
{
    if (status == NetworkManager::Disconnected) {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::vpnConnectionStateChanged(NetworkManager::VpnConnection::State state, NetworkManager::VpnConnection::StateChangeReason reason)
{
    Q_UNUSED(reason);
    if (state == NetworkManager::VpnConnection::Activated ||
        state == NetworkManager::VpnConnection::Failed ||
        state == NetworkManager::VpnConnection::Disconnected) {
        NMAppletDebug() << "Emit signal connectingChanged(false)";
        m_connecting = false;
        Q_EMIT connectingChanged(false);
    }

    if (state == NetworkManager::VpnConnection::Activated) {
        m_vpn = true;
        setIcons();
    } else if (state == NetworkManager::VpnConnection::Failed ||
               state == NetworkManager::VpnConnection::Disconnected) {
        m_vpn = false;
        setIcons();
    }
}

void ConnectionIcon::wirelessEnabledChanged(bool enabled)
{
    Q_UNUSED(enabled);
    if (NetworkManager::status() == NetworkManager::Disconnected) {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::wwanEnabledChanged(bool enabled)
{
    Q_UNUSED(enabled);
    if (NetworkManager::status() == NetworkManager::Disconnected) {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::setIcons()
{
    m_signal = 0;
#if WITH_MODEMMANAGER_SUPPORT
    if (m_modemNetwork) {
        disconnect(m_modemNetwork.data(), 0, this, 0);
        m_modemNetwork.clear();
    }
#endif
    if (m_wirelessNetwork) {
        disconnect(m_wirelessNetwork.data(), 0, this, 0);
        m_wirelessNetwork.clear();
    }

    NetworkManager::ActiveConnection::Ptr connection = NetworkManager::activatingConnection();
    if (!connection) {
        connection = NetworkManager::primaryConnection();
    }

    if (connection) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(connection->devices().first());
        if (device) {
            NetworkManager::Device::Type type = device->type();
            if (type == NetworkManager::Device::Wifi) {
                NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
                if (wifiDevice->mode() == NetworkManager::WirelessDevice::Adhoc) {
                    setWirelessIconForSignalStrength(100);
                } else {
                    NetworkManager::AccessPoint::Ptr ap = wifiDevice->activeAccessPoint();
                    if (ap) {
                        setWirelessIcon(device, ap->ssid());
                    }
                }
            } else if (type == NetworkManager::Device::Ethernet) {
                NMAppletDebug() << "Emit signal connectionIconChanged(network-wired-activated)";
                m_connectionIcon = "network-wired-activated";
                m_connectionTooltipIcon = "network-wired-activated";
                Q_EMIT connectionIconChanged("network-wired-activated");
                Q_EMIT connectionTooltipIconChanged("network-wired-activated");
            } else if (type == NetworkManager::Device::Modem) {
#if WITH_MODEMMANAGER_SUPPORT
                setModemIcon(device);
#else
                NMAppletDebug() << "Emit signal connectionIconChanged(network-mobile-0)";
                m_connectionIcon = "network-mobile-0";
                m_connectionTooltipIcon = "phone";
                Q_EMIT connectionIconChanged("network-mobile-0");
                Q_EMIT connectionTooltipIconChanged("phone");
#endif
            } else if (type == NetworkManager::Device::Bluetooth) {
                NetworkManager::BluetoothDevice::Ptr btDevice = device.objectCast<NetworkManager::BluetoothDevice>();
                if (btDevice) {
                    if (btDevice->bluetoothCapabilities().testFlag(NetworkManager::BluetoothDevice::Dun)) {
#if WITH_MODEMMANAGER_SUPPORT
                        setModemIcon(device);
#else
                        NMAppletDebug() << "Emit signal connectionIconChanged(network-mobile-0)";
                        m_connectionIcon = "network-mobile-0";
                        m_connectionTooltipIcon = "phone";
                        Q_EMIT connectionIconChanged("network-mobile-0");
                        Q_EMIT connectionTooltipIconChanged("phone");
#endif
                    } else {
                        NMAppletDebug() << "Emit signal connectionIconChanged(bluetooth)";
                        m_connectionIcon = "bluetooth";
                        m_connectionTooltipIcon = "preferences-system-bluetooth";
                        Q_EMIT connectionIconChanged("bluetooth");
                        Q_EMIT connectionTooltipIconChanged("preferences-system-bluetooth");
                    }
                }
            } else {
                NMAppletDebug() << "Emit signal connectionIconChanged(network-wired-activated)";
                m_connectionIcon = "network-wired-activated";
                m_connectionTooltipIcon = "network-wired-activated";
                Q_EMIT connectionIconChanged("network-wired-activated");
                Q_EMIT connectionTooltipIconChanged("network-wired-activated");
            }
        }
    } else {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::setDisconnectedIcon()
{
    if (NetworkManager::status() == NetworkManager::Unknown ||
        NetworkManager::status() == NetworkManager::Asleep) {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-unavailable)";
        m_connectionIcon = "network-unavailable";
        Q_EMIT connectionIconChanged("network-unavailable");
        return;
    }

    bool wired = false;
    bool wireless = false;
    bool modem = false;

    foreach (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Ethernet) {
            NetworkManager::WiredDevice::Ptr wiredDev = device.objectCast<NetworkManager::WiredDevice>();
            if (wiredDev->carrier()) {
                wired = true;
            }
        } else if (device->type() == NetworkManager::Device::Wifi &&
                   NetworkManager::isWirelessEnabled() &&
                   NetworkManager::isWirelessHardwareEnabled()) {
            wireless = true;
        } else if (device->type() == NetworkManager::Device::Modem &&
                   NetworkManager::isWwanEnabled() &&
                   NetworkManager::isWwanHardwareEnabled()) {
            modem = true;
        }
    }

    if (wired) {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-wired-available)";
        m_connectionIcon = "network-wired-available";
        m_connectionTooltipIcon = "network-wired";
        Q_EMIT connectionIconChanged("network-wired-available");
        Q_EMIT connectionTooltipIconChanged("network-wired");
        return;
    } else if (wireless) {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-wireless-available)";
        m_connectionIcon = "network-wireless-available";
        m_connectionTooltipIcon = "network-wireless-connected-00";
        Q_EMIT connectionIconChanged("network-wireless-available");
        Q_EMIT connectionTooltipIconChanged("network-wireless-connected-00");
        return;
    } else if (modem) {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-mobile-available)";
        m_connectionIcon = "network-mobile-available";
        m_connectionTooltipIcon = "phone";
        Q_EMIT connectionIconChanged("network-mobile-available");
        Q_EMIT connectionTooltipIconChanged("phone");
        return;
    }  else {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-unavailable)";
        m_connectionIcon = "network-unavailable";
        m_connectionTooltipIcon = "network-wired";
        Q_EMIT connectionIconChanged("network-unavailable");
        Q_EMIT connectionTooltipIconChanged("network-wired");
    }
}
#if WITH_MODEMMANAGER_SUPPORT
void ConnectionIcon::setModemIcon(const NetworkManager::Device::Ptr & device)
{
    NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();

    if (!modemDevice) {
        m_connectionIcon = "network-mobile-100";
        NMAppletDebug() << "Emit signal connectionIconChanged(network-mobile-100)";
        Q_EMIT connectionIconChanged("network-mobile-100");

        return;
    }

#ifdef MODEMMANAGERQT_ONE
    m_modemNetwork = modemDevice->getModemNetworkIface();

    if (m_modemNetwork) {
        connect(m_modemNetwork.data(), SIGNAL(signalQualityChanged(uint)),
                SLOT(modemSignalChanged(uint)), Qt::UniqueConnection);
        connect(m_modemNetwork.data(), SIGNAL(accessTechnologyChanged(ModemManager::Modem::AccessTechnologies)),
                SLOT(setIconForModem()), Qt::UniqueConnection);
        connect(m_modemNetwork.data(), SIGNAL(destroyed(QObject*)),
                SLOT(modemNetworkRemoved()));

        m_signal = m_modemNetwork->signalQuality().signal;
        setIconForModem();
#else
    m_modemNetwork = modemDevice->getModemNetworkIface().objectCast<ModemManager::ModemGsmNetworkInterface>();

    if (m_modemNetwork) {
        connect(m_modemNetwork.data(), SIGNAL(signalQualityChanged(uint)),
                SLOT(modemSignalChanged(uint)), Qt::UniqueConnection);
        connect(m_modemNetwork.data(), SIGNAL(accessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)),
                SLOT(setIconForModem()), Qt::UniqueConnection);
        connect(m_modemNetwork.data(), SIGNAL(destroyed(QObject*)),
                SLOT(modemNetworkRemoved()));

        m_signal = m_modemNetwork->getSignalQuality();
        setIconForModem();
#endif
    } else {
        NMAppletDebug() << "Emit signal connectionIconChanged(network-mobile)";
        m_connectionIcon = "network-mobile-0";
        Q_EMIT connectionIconChanged("network-mobile-0");
        m_connectionTooltipIcon = "phone";
        Q_EMIT connectionTooltipIconChanged("phone");
        return;
    }
}

void ConnectionIcon::setIconForModem()
{
    if (!m_signal) {
#ifdef MODEMMANAGERQT_ONE
        m_signal = m_modemNetwork->signalQuality().signal;
#else
        m_signal = m_modemNetwork->getSignalQuality();
#endif
    }
    QString strength = "00";

    if (m_signal == 0) {
        strength = '0';
    } else if (m_signal < 20) {
        strength = "20";
    } else if (m_signal < 40) {
        strength = "40";
    } else if (m_signal < 60) {
        strength = "60";
    } else if (m_signal < 80) {
        strength = "80";
    } else {
        strength = "100";
    }

    QString result;;

#ifdef MODEMMANAGERQT_ONE
    switch(m_modemNetwork->accessTechnologies()) {
    case MM_MODEM_ACCESS_TECHNOLOGY_GSM:
    case MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT:
        result = "network-mobile-%1";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_GPRS:
        result = "network-mobile-%1-gprs";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_EDGE:
        result = "network-mobile-%1-edge";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_UMTS:
        result = "network-mobile-%1-umts";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_HSDPA:
        result = "network-mobile-%1-hsdpa";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_HSUPA:
        result = "network-mobile-%1-hsupa";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_HSPA:
    case MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS:
        result = "network-mobile-%1-hspa";
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_LTE:
        result = "network-mobile-%1-lte";
        break;
    default:
        result = "network-mobile-%1";
        break;
    }
#else
    switch(m_modemNetwork->getAccessTechnology()) {
        case ModemManager::ModemInterface::UnknownTechnology:
        case ModemManager::ModemInterface::Gsm:
        case ModemManager::ModemInterface::GsmCompact:
            result = "network-mobile-%1";
            break;
        case ModemManager::ModemInterface::Gprs:
            result = "network-mobile-%1-gprs";
            break;
        case ModemManager::ModemInterface::Edge:
            result = "network-mobile-%1-edge";
            break;
        case ModemManager::ModemInterface::Umts:
            result = "network-mobile-%1-umts";
            break;
        case ModemManager::ModemInterface::Hsdpa:
            result = "network-mobile-%1-hsdpa";
            break;
        case ModemManager::ModemInterface::Hsupa:
            result = "network-mobile-%1-hsupa";
            break;
        case ModemManager::ModemInterface::Hspa:
        case ModemManager::ModemInterface::HspaPlus:
            result = "network-mobile-%1-hspa";
            break;
        case ModemManager::ModemInterface::Lte:
            result = "network-mobile-%1-lte";
            break;
        default:
            result = "network-mobile-%1";
            break;
    }
#endif
    NMAppletDebug() << "Emit signal connectionIconChanged(" << QString(result).arg(strength) << ")";
    m_connectionIcon = QString(result).arg(strength);
    m_connectionTooltipIcon = "phone";
    Q_EMIT connectionIconChanged(QString(result).arg(strength));
    Q_EMIT connectionTooltipIconChanged("phone");
}
#endif
void ConnectionIcon::setWirelessIcon(const NetworkManager::Device::Ptr &device, const QString& ssid)
{
    NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
    if (device) {
        m_wirelessNetwork = wirelessDevice->findNetwork(ssid);
    } else {
        m_wirelessNetwork.clear();
    }

    if (m_wirelessNetwork) {
        connect(m_wirelessNetwork.data(), SIGNAL(signalStrengthChanged(int)),
                SLOT(setWirelessIconForSignalStrength(int)), Qt::UniqueConnection);

        setWirelessIconForSignalStrength(m_wirelessNetwork->signalStrength());
    } else {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::setWirelessIconForSignalStrength(int strength)
{
    const int diff = m_signal - strength;

    if (diff >= 10 ||
        diff <= -10) {
        int iconStrength = 100;

        if (strength == 0) {
            iconStrength = 0;
            m_connectionTooltipIcon = "network-wireless-connected-00";
            Q_EMIT connectionTooltipIconChanged("network-wireless-connected-00");
        } else if (strength < 20) {
            iconStrength = 20;
            m_connectionTooltipIcon = "network-wireless-connected-20";
            Q_EMIT connectionTooltipIconChanged("network-wireless-connected-20");
        } else if (strength < 40) {
            iconStrength = 40;
            m_connectionTooltipIcon = "network-wireless-connected-40";
            Q_EMIT connectionTooltipIconChanged("network-wireless-connected-40");
        } else if (strength < 60) {
            iconStrength = 60;
            m_connectionTooltipIcon = "network-wireless-connected-60";
            Q_EMIT connectionTooltipIconChanged("network-wireless-connected-60");
        } else if (strength < 80) {
            iconStrength = 80;
            m_connectionTooltipIcon = "network-wireless-connected-80";
            Q_EMIT connectionTooltipIconChanged("network-wireless-connected-80");
        } else if (strength < 100) {
            m_connectionTooltipIcon = "network-wireless-connected-100";
            Q_EMIT connectionTooltipIconChanged("network-wireless-connected-100");
        }

        m_signal = iconStrength;

        QString icon = QString("network-wireless-%1").arg(iconStrength);

        NMAppletDebug() << "Emit signal connectionIconChanged(" << icon << ")";
        m_connectionIcon = icon;
        Q_EMIT connectionIconChanged(icon);
    }
}
