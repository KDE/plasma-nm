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
#include <NetworkManagerQt/settings/Connection>
#include <NetworkManagerQt/settings/Wireless>

#include "debug.h"

ConnectionIcon::ConnectionIcon(QObject* parent):
    QObject(parent),
    m_signal(0),
    m_modemNetwork(0)
{
}

ConnectionIcon::~ConnectionIcon()
{
}

void ConnectionIcon::init()
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
        if (active->state() == NetworkManager::ActiveConnection::Activating) {
            NMAppletDebug() << "Emit signal showConnectionIndicator()";
            connect(active.data(), SIGNAL(destroyed(QObject*)),
                    SIGNAL(hideConnectingIndicator()));
            Q_EMIT showConnectingIndicator();
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
        NMAppletDebug() << "Emit signal hideConnectionIndicator()";
        Q_EMIT hideConnectingIndicator();
    }

    setIcons();
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

    if (m_modemNetwork) {
        disconnect(m_modemNetwork.data(), 0, this, 0);
        m_modemNetwork.clear();
    }

    if (m_wirelessNetwork) {
        disconnect(m_wirelessNetwork.data(), 0, this, 0);
        m_wirelessNetwork.clear();
    }

    NetworkManager::ActiveConnection::List actives = NetworkManager::activeConnections();

    foreach (const NetworkManager::ActiveConnection::Ptr & active, actives) {
        if ((active->default4() || active->default6()) && active->state() == NetworkManager::ActiveConnection::Activated) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
            if (device) {
                NetworkManager::Device::Type type = device->type();

                if (type == NetworkManager::Device::Wifi) {
                    NetworkManager::Settings::ConnectionSettings::Ptr settings;
                    settings = active->connection()->settings();
                    NetworkManager::Settings::WirelessSetting::Ptr wirelessSetting = settings->setting(NetworkManager::Settings::Setting::Wireless).dynamicCast<NetworkManager::Settings::WirelessSetting>();
                    setWirelessIcon(device, wirelessSetting->ssid());
                    connectionFound = true;
                } else if (type == NetworkManager::Device::Ethernet) {
                    connectionFound = true;
                    NMAppletDebug() << "Emit signal setConnectionIcon(network-wired-activated)";
                    Q_EMIT setConnectionIcon("network-wired-activated");
                    Q_EMIT setTooltipIcon("network-wired-activated");
                } else if (type == NetworkManager::Device::Modem) {
                    connectionFound = true;
                    setModemIcon(device);
                } else if (type == NetworkManager::Device::Bluetooth) {
                    NetworkManager::BluetoothDevice::Ptr btDevice = device.objectCast<NetworkManager::BluetoothDevice>();
                    if (btDevice) {
                        connectionFound = true;
                        if (btDevice->bluetoothCapabilities().testFlag(NetworkManager::BluetoothDevice::Dun)) {
                            setModemIcon(device);
                        } else {
                            NMAppletDebug() << "Emit signal setStatiConnectionIcon(preferences-system-bluetooth)";
                            Q_EMIT setStaticConnectionIcon("preferences-system-bluetooth");
                        }
                    }
                }
            }
        }

        if (active->vpn() && active->state() == NetworkManager::ActiveConnection::Activated) {
            vpnFound = true;
            NMAppletDebug() << "Emit signal setHoverIcon(object-locked)";
            Q_EMIT setHoverIcon("object-locked");
        }
    }

    if (!connectionFound) {
        setDisconnectedIcon();
    }

    if (!vpnFound && connectionFound) {
        NMAppletDebug() << "Emit signal unsetHoverIcon()";
        Q_EMIT unsetHoverIcon();
    }
}

void ConnectionIcon::setDisconnectedIcon()
{
    if (NetworkManager::status() == NetworkManager::Unknown ||
        NetworkManager::status() == NetworkManager::Asleep) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wired)";
        Q_EMIT setConnectionIcon("network-wired");
        Q_EMIT setTooltipIcon("network-wired");
        NMAppletDebug() << "Emit signal setHoverIcon(dialog-error)";
        Q_EMIT setHoverIcon("dialog-error");
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
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wired)";
        Q_EMIT setConnectionIcon("network-wired");
        Q_EMIT setTooltipIcon("network-wired");
        Q_EMIT setHoverIcon("dialog-cancel");
        return;
    } else if (modem) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-mobile)";
        Q_EMIT setStaticConnectionIcon("phone");
        Q_EMIT setTooltipIcon("phone");
        Q_EMIT setHoverIcon("dialog-cancel");
        return;
    } else if (wireless) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wireless-0)";
        Q_EMIT setConnectionIcon("network-wireless-0");
        Q_EMIT setTooltipIcon("network-wireless-connected-00");
        Q_EMIT setHoverIcon("dialog-cancel");
        return;
    }  else {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wired)";
        Q_EMIT setConnectionIcon("network-wired");
        Q_EMIT setTooltipIcon("network-wired");
        Q_EMIT setHoverIcon("dialog-error");
        return;
    }
}

void ConnectionIcon::setModemIcon(const NetworkManager::Device::Ptr & device)
{
    NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();

    if (!modemDevice) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-mobile-100)";
        Q_EMIT setConnectionIcon("network-mobile-100");

        return;
    }

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
    } else {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-mobile)";
        Q_EMIT setConnectionIcon("network-mobile");
        Q_EMIT setTooltipIcon("phone");
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
        m_signal = m_modemNetwork->getSignalQuality();
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

    int accesstechnology = m_modemNetwork->getAccessTechnology();

    QString result;;

    switch(accesstechnology) {
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
            result = "network-mobile-%1-hspa";
            break;
        default:
            result = "network-mobile-%1";
            break;
    }

    NMAppletDebug() << "Emit signal setConnectionIcon(" << QString(result).arg(strength) << ")";
    Q_EMIT setConnectionIcon(QString(result).arg(strength));
    Q_EMIT setTooltipIcon("phone");
}

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
    int diff = m_signal - strength;

    if (diff >= 10 ||
        diff <= -10) {
        int iconStrenght = 100;

        if (strength < 20) {
            iconStrenght = 20;
            Q_EMIT setTooltipIcon("network-wireless-connected-25");
        } else if (strength < 25) {
            iconStrenght = 25;
            Q_EMIT setTooltipIcon("network-wireless-connected-25");
        } else if (strength < 40) {
            iconStrenght = 40;
            Q_EMIT setTooltipIcon("network-wireless-connected-50");
        } else if (strength < 50) {
            iconStrenght = 50;
            Q_EMIT setTooltipIcon("network-wireless-connected-50");
        } else if (strength < 60) {
            iconStrenght = 60;
            Q_EMIT setTooltipIcon("network-wireless-connected-75");
        } else if (strength < 75) {
            iconStrenght = 75;
            Q_EMIT setTooltipIcon("network-wireless-connected-75");
        } else if (strength < 80) {
            iconStrenght = 80;
            Q_EMIT setTooltipIcon("network-wireless-connected-100");
        } else {
            Q_EMIT setTooltipIcon("network-wireless-connected-100");
        }

        m_signal = iconStrenght;

        QString icon = QString("network-wireless-%1").arg(iconStrenght);

        NMAppletDebug() << "Emit signal setConnectionIcon(" << icon << ")";
        Q_EMIT setConnectionIcon(icon);
    }
}
