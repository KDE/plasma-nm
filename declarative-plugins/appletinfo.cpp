/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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


#include "appletinfo.h"

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/connection.h>

#include "debug.h"

AppletInfo::AppletInfo(QObject* parent):
    QObject(parent)
{
}

AppletInfo::~AppletInfo()
{
}

void AppletInfo::initIconInfo()
{
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));

    setAppletInfo();
}

void AppletInfo::initNetworkInfo()
{
    connect(NetworkManager::notifier(), SIGNAL(networkingEnabledChanged(bool)),
            SIGNAL(networkingEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
            SIGNAL(wirelessEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanEnabledChanged(bool)),
            SIGNAL(wwanEnabled(bool)));

    networkingEnabled(NetworkManager::isNetworkingEnabled());
    wirelessEnabled(NetworkManager::isWirelessEnabled());
    wwanEnabled(NetworkManager::isWwanEnabled());
}

void AppletInfo::activeConnectionsChanged()
{
    QList<NetworkManager::ActiveConnection*> actives = NetworkManager::activeConnections();

    foreach (NetworkManager::ActiveConnection * active, actives) {
        connect(active, SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)));

        if (active->state() == NetworkManager::ActiveConnection::Activating) {
            NMDebug() << "Applet info: Emit signal activatingConnection(" << active->connection()->id() << ")";
            Q_EMIT activatingConnection(active->connection()->uuid());
        }
    }

    setAppletInfo();
}

void AppletInfo::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    NetworkManager::ActiveConnection * active = qobject_cast<NetworkManager::ActiveConnection*>(sender());

    if (state == NetworkManager::ActiveConnection::Deactivated ||
        state == NetworkManager::ActiveConnection::Activated) {
        NMDebug() << "Applet info: Emit signal stopActivatingConnection(" << active->connection()->id() << ")";
        Q_EMIT stopActivatingConnection(active->connection()->uuid());
    }

    setAppletInfo();
}

void AppletInfo::setAppletInfo()
{
    bool main = false;
    bool second = false;

    QList<NetworkManager::ActiveConnection*> actives = NetworkManager::activeConnections();

    foreach (NetworkManager::ActiveConnection * active, actives) {
        if ((active->default4() || active->default6()) && active->state() == NetworkManager::ActiveConnection::Activated) {
            NetworkManager::Settings::ConnectionSettings settings;
            settings.fromMap(active->connection()->settings());

            switch (settings.connectionType()) {
                case NetworkManager::Settings::ConnectionSettings::Adsl:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Bluetooth:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Bond:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Bridge:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Cdma:
                    main = true;
                    NMDebug() << "Monitor: Emit signal mainConnectionIcon(phone)";
                    Q_EMIT mainConnectionIcon(QString("phone"));
                    break;
                case NetworkManager::Settings::ConnectionSettings::Gsm:
                    main = true;
                    NMDebug() << "Monitor: Emit signal mainConnectionIcon(phone)";
                    Q_EMIT mainConnectionIcon(QString("phone"));
                    break;
                case NetworkManager::Settings::ConnectionSettings::Infiniband:
                    break;
                case NetworkManager::Settings::ConnectionSettings::OLPCMesh:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Pppoe:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Vlan:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Vpn:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Wimax:
                    break;
                case NetworkManager::Settings::ConnectionSettings::Wired:
                    main = true;
                    NMDebug() << "Monitor: Emit signal mainConnectionIcon(network-wired-activated)";
                    Q_EMIT mainConnectionIcon(QString("network-wired-activated"));
                    break;
                case NetworkManager::Settings::ConnectionSettings::Wireless:
                    main = true;
                    NMDebug() << "Monitor: Emit signal mainConnectionIcon(network-wireless)";
                    Q_EMIT mainConnectionIcon(QString("network-wireless"));
                    break;
                default:
                    break;
            }
        }

        if (active->vpn() && active->state() == NetworkManager::ActiveConnection::Activated) {
            second = true;
            NMDebug() << "Monitor: Emit signal secondConnectionIcon(object-locked)";
            Q_EMIT secondConnectionIcon(QString("object-locked"));
        }
    }

    if (!main) {
        Q_EMIT mainConnectionIcon(QString("network-wired"));
    }

    if (!second) {
        Q_EMIT secondConnectionIcon(QString());
    }
}

