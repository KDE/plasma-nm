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

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/wireddevice.h>
#include <QtNetworkManager/wirelessdevice.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/settings/802-11-wireless.h>

#include "debug.h"

ConnectionIcon::ConnectionIcon(QObject* parent):
    QObject(parent),
    m_wirelessSignal(0),
    m_wirelessNetwork(0),
    m_wirelessEnvironment(0)
{
}

ConnectionIcon::~ConnectionIcon()
{
    if (m_wirelessEnvironment)
        delete m_wirelessEnvironment;
}

void ConnectionIcon::init()
{
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));

    setIcons();
}

void ConnectionIcon::activeConnectionsChanged()
{
    QList<NetworkManager::ActiveConnection*> actives = NetworkManager::activeConnections();

    foreach (NetworkManager::ActiveConnection * active, actives) {
        connect(active, SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)));

        if (active->state() == NetworkManager::ActiveConnection::Activating) {
            NMAppletDebug() << "Emit signal showConnectionIndicator()";
            Q_EMIT showConnectingIndicator();
        }
    }

    setIcons();
}

void ConnectionIcon::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    if (state == NetworkManager::ActiveConnection::Deactivated ||
        state == NetworkManager::ActiveConnection::Activated) {
        NMAppletDebug() << "Emit signal hideConnectionIndicator()";
        Q_EMIT hideConnectingIndicator();
    }

    setIcons();
}

void ConnectionIcon::setIcons()
{
bool connectionFound = false;
    bool vpnFound = false;

    if (m_wirelessEnvironment) {
        delete m_wirelessEnvironment;
        m_wirelessEnvironment = 0;
        m_wirelessNetwork = 0;
    }
    m_wirelessSignal = 0;

    QList<NetworkManager::ActiveConnection*> actives = NetworkManager::activeConnections();

    foreach (NetworkManager::ActiveConnection * active, actives) {
        if ((active->default4() || active->default6()) && active->state() == NetworkManager::ActiveConnection::Activated) {
            NetworkManager::Settings::ConnectionSettings settings;
            settings.fromMap(active->connection()->settings());

            if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Cdma ||
                settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Gsm) {
                connectionFound = true;
                setModemIcon();
            } else if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Wired) {
                connectionFound = true;
                NMAppletDebug() << "Emit signal setConnectionIcon(network-wired-activated)";
                Q_EMIT setConnectionIcon(QString("network-wired-activated"));
            } else if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
                connectionFound = true;
                NetworkManager::Settings::WirelessSetting * wirelessSetting = dynamic_cast<NetworkManager::Settings::WirelessSetting*>(settings.setting(NetworkManager::Settings::Setting::Wireless));
                setWirelessIcon(active->devices().first(), wirelessSetting->ssid());
            }
        }

        if (active->vpn() && active->state() == NetworkManager::ActiveConnection::Activated) {
            vpnFound = true;
            NMAppletDebug() << "Emit signal setVpnIcon()";
            Q_EMIT setVpnIcon();
        }
    }

    if (!connectionFound) {
        setDisconnectedIcon();
    }

    if (!vpnFound) {
        NMAppletDebug() << "Emit signal unsetVpnIcon()";
        Q_EMIT unsetVpnIcon();
    }
}

void ConnectionIcon::setDisconnectedIcon()
{
    bool wired = false;
    bool wireless = false;
    bool modem = false;

    foreach (NetworkManager::Device * device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Ethernet) {
            NetworkManager::WiredDevice * wiredDev = qobject_cast<NetworkManager::WiredDevice*>(device);
            if (wiredDev->carrier()) {
                wired = true;
                disconnect(wiredDev, SIGNAL(carrierChanged(bool)));
                connect(wiredDev, SIGNAL(carrierChanged(bool)),
                        SLOT(setIcons()));
            }
        } else if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice * wirelessDev = qobject_cast<NetworkManager::WirelessDevice*>(device);
            if (!wirelessDev->accessPoints().isEmpty()) {
                wireless = true;
                disconnect(wirelessDev, SIGNAL(accessPointAppeared(QString)));
                connect(wirelessDev, SIGNAL(accessPointAppeared(QString)),
                        SLOT(setIcons()));
            }
        } else if (device->type() == NetworkManager::Device::Modem) {
            modem = true;
        }
    }

    if (wired) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wired)";
        Q_EMIT setConnectionIcon(QString("network-wired"));
    } else if (wireless) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wireless-0)";
        Q_EMIT setConnectionIcon(QString("network-wireless-0"));
    } else if (modem) {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-mobile-0-none)";
        Q_EMIT setConnectionIcon(QString("network-mobile-0-none"));
    } else {
        NMAppletDebug() << "Emit signal setConnectionIcon(network-wired)";
        Q_EMIT setConnectionIcon(QString("network-wired"));
    }
}

void ConnectionIcon::setModemIcon()
{
    // TODO
    NMAppletDebug() << "Emit signal setConnectionIcon(network-mobile-100)";
    Q_EMIT setConnectionIcon(QString("network-mobile-100"));
}

void ConnectionIcon::setWirelessIcon(NetworkManager::Device* device, const QString& ssid)
{
    NetworkManager::WirelessDevice * wirelessDevice = qobject_cast<NetworkManager::WirelessDevice*>(device);
    m_wirelessEnvironment = new NetworkManager::WirelessNetworkInterfaceEnvironment(wirelessDevice);
    m_wirelessNetwork = m_wirelessEnvironment->findNetwork(ssid);

    if (m_wirelessNetwork) {
        connect(m_wirelessNetwork, SIGNAL(signalStrengthChanged(int)),
                SLOT(setWirelessIconForSignalStrenght(int)));

        setWirelessIconForSignalStrenght(m_wirelessNetwork->signalStrength());
    } else {
        setDisconnectedIcon();
    }
}

void ConnectionIcon::setWirelessIconForSignalStrenght(int strenght)
{
    int diff = m_wirelessSignal - strenght;

    if (diff >= 10 ||
        diff <= -10) {
        int iconStrenght = 100;

        if (strenght < 20) {
            iconStrenght = 20;
        } else if (strenght < 25) {
            iconStrenght = 25;
        } else if (strenght < 40) {
            iconStrenght = 40;
        } else if (strenght < 50) {
            iconStrenght = 50;
        } else if (strenght < 60) {
            iconStrenght = 60;
        } else if (strenght < 75) {
            iconStrenght = 75;
        } else if (strenght < 80) {
            iconStrenght = 80;
        }

        m_wirelessSignal = iconStrenght;

        QString icon = QString("network-wireless-%1").arg(iconStrenght);

        NMAppletDebug() << "Emit signal setConnectionIcon(" << icon << ")";
        Q_EMIT setConnectionIcon(icon);
    }
}
