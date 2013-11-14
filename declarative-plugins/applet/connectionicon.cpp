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
#include <NetworkManagerQt/VpnConnection>

#include "debug.h"

ConnectionIcon::ConnectionIcon(QObject* parent)
    : QObject(parent)
    , m_signal(0)
    , m_connecting(false)
#if WITH_MODEMMANAGER_SUPPORT
    , m_modemNetwork(0)
#endif
{
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(setIcons()));
    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            SLOT(deviceRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
            SLOT(setIcons()));
    connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
            SLOT(setIcons()));
    connect(NetworkManager::notifier(), SIGNAL(wwanEnabledChanged(bool)),
            SLOT(setIcons()));
    connect(NetworkManager::notifier(), SIGNAL(wwanHardwareEnabledChanged(bool)),
            SLOT(setIcons()));
    connect(NetworkManager::notifier(), SIGNAL(networkingEnabledChanged(bool)),
            SLOT(setIcons()));

    setIcons();
}

ConnectionIcon::~ConnectionIcon()
{
}

bool ConnectionIcon::connecting() const
{
    return m_connecting;
}

QString ConnectionIcon::connectionSvgIcon() const
{
    return m_connectionSvgIcon;
}

QString ConnectionIcon::connectionIndicatorIcon() const
{
    return m_connectionIndicatorIcon;
}

QString ConnectionIcon::connectionPixmapIcon() const
{
    return m_connectionPixmapIcon;
}

void ConnectionIcon::activeConnectionsChanged()
{
    NetworkManager::ActiveConnection::List actives = NetworkManager::activeConnections();

    foreach (const NetworkManager::ActiveConnection::Ptr & active, actives) {
        connect(active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(default4Changed(bool)),
                SLOT(setIcons()), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(default6Changed(bool)),
                SLOT(setIcons()), Qt::UniqueConnection);

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
            m_connecting = true;
            NMAppletDebug() << "Emit signal connectingChanged(true)";
            Q_EMIT connectingChanged(true);
        }
    }

    setIcons();
}

void ConnectionIcon::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    if (state == NetworkManager::ActiveConnection::Deactivated ||
        state == NetworkManager::ActiveConnection::Deactivating ||
        state == NetworkManager::ActiveConnection::Activated ||
        state == NetworkManager::ActiveConnection::Unknown) {
        m_connecting = false;
        NMAppletDebug() << "Emit signal connectingChanged(false)";
        Q_EMIT connectingChanged(false);
    }

    setIcons();
}

void ConnectionIcon::activeConnectionDestroyed()
{
    m_connecting = false;
    NMAppletDebug() << "Emit signal connectingChanged(false)";
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
        setIcons();
    }
}

void ConnectionIcon::setIcons()
{
    bool connectionFound = false;
    bool vpnFound = false;

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

    NetworkManager::ActiveConnection::List actives = NetworkManager::activeConnections();

    bool defaultRouteExists = false;
    foreach (const NetworkManager::ActiveConnection::Ptr & active, actives) {
        if (active->default4() || active->default6()) {
            defaultRouteExists = true;
            break;
        }
    }

    foreach (const NetworkManager::ActiveConnection::Ptr & active, actives) {
        if (((active->default4() || active->default6()) && active->state() == NetworkManager::ActiveConnection::Activated) || !defaultRouteExists) {
            NetworkManager::ActiveConnection::Ptr activeConnection;
            if (active->vpn()) {
                NetworkManager::ActiveConnection::Ptr activeTmp;
                activeTmp = NetworkManager::findActiveConnection(active->specificObject());
                if (activeTmp) {
                    activeConnection = activeTmp;
                }

                vpnFound = true;
                m_connectionIndicatorIcon = "object-locked";
                NMAppletDebug() << "Emit signal connectionIndicatorIconChanged(object-locked)";
                Q_EMIT connectionIndicatorIconChanged("object-locked");
            } else {
                activeConnection = active;
            }
            if (activeConnection->devices().isEmpty()) {
                continue;
            }
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(activeConnection->devices().first());
            if (device) {
                NetworkManager::Device::Type type = device->type();

                if (type == NetworkManager::Device::Wifi) {
                    NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
                    if (!wifiDevice) {
                        continue;
                    }
                    if (wifiDevice->mode() == NetworkManager::WirelessDevice::Adhoc) {
                        setWirelessIconForSignalStrength(100);
                        connectionFound = true;
                    } else {
                        NetworkManager::AccessPoint::Ptr ap = wifiDevice->activeAccessPoint();
                        if (ap) {
                            setWirelessIcon(device, ap->ssid());
                            connectionFound = true;
                        }
                    }
                } else if (type == NetworkManager::Device::Ethernet) {
                    connectionFound = true;
                    m_connectionSvgIcon = "network-wired-activated";
                    NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-wired-activated)";
                    Q_EMIT connectionSvgIconChanged("network-wired-activated");
                    m_connectionPixmapIcon = "network-wired-activated";
                    Q_EMIT connectionPixmapIconChanged("network-wired-activated");
                } else if (type == NetworkManager::Device::Modem) {
                    connectionFound = true;
#if WITH_MODEMMANAGER_SUPPORT
                    setModemIcon(device);
#else
                    m_connectionSvgIcon = "network-mobile-0";
                    NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-mobile-0)";
                    Q_EMIT connectionSvgIconChanged("network-mobile-0");
                    m_connectionPixmapIcon = "phone";
                    Q_EMIT connectionPixmapIconChanged("phone");
#endif
                } else if (type == NetworkManager::Device::Bluetooth) {
                    NetworkManager::BluetoothDevice::Ptr btDevice = device.objectCast<NetworkManager::BluetoothDevice>();
                    if (btDevice) {
                        connectionFound = true;
                        if (btDevice->bluetoothCapabilities().testFlag(NetworkManager::BluetoothDevice::Dun)) {
#if WITH_MODEMMANAGER_SUPPORT
                            setModemIcon(device);
#else
                        m_connectionSvgIcon = "network-mobile-0";
                        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-mobile-0)";
                        Q_EMIT connectionSvgIconChanged("network-mobile-0");
                        m_connectionPixmapIcon = "phone";
                        Q_EMIT connectionPixmapIconChanged("phone");
#endif
                        } else {
                            m_connectionSvgIcon = "bluetooth";
                            NMAppletDebug() << "Emit signal connectionSvgIconChanged(bluetooth)";
                            Q_EMIT connectionSvgIconChanged("bluetooth");
                            m_connectionPixmapIcon = "preferences-system-bluetooth";
                            Q_EMIT connectionPixmapIconChanged("preferences-system-bluetooth");
                        }
                    }
                }
            }
        } else if (active->vpn() && active->state() == NetworkManager::ActiveConnection::Activated) {
            vpnFound = true;
            m_connectionIndicatorIcon = "object-locked";
            NMAppletDebug() << "Emit signal connectionIndicatorIconChanged(object-locked)";
            Q_EMIT connectionIndicatorIconChanged("object-locked");
        }
    }

    if (!connectionFound) {
        setDisconnectedIcon();
    }

    if (!vpnFound && connectionFound) {
        m_connectionIndicatorIcon.clear();
        NMAppletDebug() << "Emit signal connectionIndicatorIconChanged()";
        Q_EMIT connectionIndicatorIconChanged(QString());
    }
}

void ConnectionIcon::setDisconnectedIcon()
{
    if (NetworkManager::status() == NetworkManager::Unknown ||
        NetworkManager::status() == NetworkManager::Asleep) {
        m_connectionSvgIcon = "network-wired";
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-wired)";
        Q_EMIT connectionSvgIconChanged("network-wired");
        m_connectionPixmapIcon = "network-wired";
        Q_EMIT connectionPixmapIconChanged("network-wired");
        NMAppletDebug() << "Emit signal setHoverIcon(dialog-error)";
        m_connectionIndicatorIcon = "dialog-error";
        Q_EMIT connectionIndicatorIconChanged("dialog-error");
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
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-wired)";
        m_connectionSvgIcon = "network-wired";
        Q_EMIT connectionSvgIconChanged("network-wired");
        m_connectionPixmapIcon = "network-wired";
        Q_EMIT connectionPixmapIconChanged("network-wired");
        NMAppletDebug() << "Emit signal connectionIndicatorIconChanged(dialog-cancel)";
        m_connectionIndicatorIcon = "dialog-cancel";
        Q_EMIT connectionIndicatorIconChanged("dialog-cancel");
        return;
    } else if (modem) {
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-mobile-0)";
        m_connectionSvgIcon = "network-mobile-0";
        Q_EMIT connectionSvgIconChanged("network-mobile-0");
        m_connectionPixmapIcon = "phone";
        Q_EMIT connectionPixmapIconChanged("phone");
        NMAppletDebug() << "Emit signal connectionIndicatorIconChanged(dialog-cancel)";
        m_connectionIndicatorIcon = "dialog-cancel";
        Q_EMIT connectionIndicatorIconChanged("dialog-cancel");
        return;
    } else if (wireless) {
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-wireless-0)";
        m_connectionSvgIcon = "network-wireless-0";
        Q_EMIT connectionSvgIconChanged("network-wireless-0");
        m_connectionPixmapIcon = "network-wireless-connected-00";
        Q_EMIT connectionPixmapIconChanged("network-wireless-connected-00");
        NMAppletDebug() << "Emit signal connectionIndicatorIconChanged(dialog-cancel)";
        m_connectionIndicatorIcon = "dialog-cancel";
        Q_EMIT connectionIndicatorIconChanged("dialog-cancel");
        return;
    }  else {
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-wired)";
        m_connectionSvgIcon = "network-wired";
        Q_EMIT connectionSvgIconChanged("network-wired");
        m_connectionPixmapIcon = "network-wired";
        Q_EMIT connectionPixmapIconChanged("network-wired");
        NMAppletDebug() << "Emit signal connectionIndicatorIconChanged(dialog-cancel)";
        m_connectionIndicatorIcon = "dialog-cancel";
        Q_EMIT connectionIndicatorIconChanged("dialog-cancel");
    }
}
#if WITH_MODEMMANAGER_SUPPORT
void ConnectionIcon::setModemIcon(const NetworkManager::Device::Ptr & device)
{
    NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();

    if (!modemDevice) {
        m_connectionSvgIcon = "network-mobile-100";
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-mobile-100)";
        Q_EMIT connectionSvgIconChanged("network-mobile-100");

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
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(network-mobile)";
        m_connectionSvgIcon = "network-mobile-0";
        Q_EMIT connectionSvgIconChanged("network-mobile-0");
        m_connectionPixmapIcon = "phone";
        Q_EMIT connectionPixmapIconChanged("phone");
        return;
    }
}

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

    if (m_signal < 13) {
        strength = '0';
    } else if (m_signal < 30) {
        strength = "20";
    } else if (m_signal < 50) {
        strength = "40";
    } else if (m_signal < 70) {
        strength = "60";
    } else if (m_signal < 90) {
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
    NMAppletDebug() << "Emit signal connectionSvgIconChanged(" << QString(result).arg(strength) << ")";
    m_connectionSvgIcon = QString(result).arg(strength);
    Q_EMIT connectionSvgIconChanged(QString(result).arg(strength));
    m_connectionPixmapIcon = "phone";
    Q_EMIT connectionPixmapIconChanged("phone");
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

        if (strength < 20) {
            iconStrength = 20;
            m_connectionPixmapIcon = "network-wireless-connected-25";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-25");
        } else if (strength < 25) {
            iconStrength = 25;
            m_connectionPixmapIcon = "network-wireless-connected-25";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-25");
        } else if (strength < 40) {
            iconStrength = 40;
            m_connectionPixmapIcon = "network-wireless-connected-50";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-50");
        } else if (strength < 50) {
            iconStrength = 50;
            m_connectionPixmapIcon = "network-wireless-connected-50";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-50");
        } else if (strength < 60) {
            iconStrength = 60;
            m_connectionPixmapIcon = "network-wireless-connected-75";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-75");
        } else if (strength < 75) {
            iconStrength = 75;
            m_connectionPixmapIcon = "network-wireless-connected-75";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-75");
        } else if (strength < 80) {
            iconStrength = 80;
            m_connectionPixmapIcon = "network-wireless-connected-100";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-100");
        } else {
            m_connectionPixmapIcon = "network-wireless-connected-100";
            Q_EMIT connectionPixmapIconChanged("network-wireless-connected-100");
        }

        m_signal = iconStrength;

        QString icon = QString("network-wireless-%1").arg(iconStrength);

        m_connectionSvgIcon = icon;
        NMAppletDebug() << "Emit signal connectionSvgIconChanged(" << icon << ")";
        Q_EMIT connectionSvgIconChanged(icon);
    }
}
