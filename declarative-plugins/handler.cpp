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

#include "handler.h"

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/accesspoint.h>
#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/activeconnection.h>

#include "debug.h"

Handler::Handler(QObject* parent):
    QObject(parent)
{
}

Handler::~Handler()
{
}

void Handler::activateConnection(const QString& connection, const QString& device, const QString& specificObject)
{
    NetworkManager::Settings::Connection * con = NetworkManager::Settings::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to activate this connection";
        return;
    }
        NMHandlerDebug() << "Activating " << con->id() << " connection";

    NetworkManager::activateConnection(connection, device, specificObject);
}

void Handler::addAndActivateConnection(const QString& connection, const QString& device, const QString& specificObject)
{
    Q_UNUSED(connection);
    Q_UNUSED(device);
    Q_UNUSED(specificObject);

    NMHandlerDebug() << "Add and activate connection";
}

void Handler::deactivateConnection(const QString& connection)
{
    NetworkManager::Settings::Connection * con = NetworkManager::Settings::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to deactivate this connection";
        return;
    }

    foreach (NetworkManager::ActiveConnection * active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid()) {
            if (active->vpn()) {
                NetworkManager::deactivateConnection(active->path());
            } else {
                active->devices().first()->disconnectInterface();
            }
        }
    }

    NMHandlerDebug() << "Deactivating " << con->id() << " connection";
}

void Handler::enableNetworking(bool enable)
{
    NMHandlerDebug() << "Networking enabled: " << enable;
    NetworkManager::setNetworkingEnabled(enable);
}

void Handler::enableWireless(bool enable)
{
    NMHandlerDebug() << "Wireless enabled: " << enable;
    NetworkManager::setWirelessEnabled(enable);
}

void Handler::enableWwan(bool enable)
{
    NMHandlerDebug() << "Wwan enabled: " << enable;
    NetworkManager::setWwanEnabled(enable);
}

void Handler::removeConnection(const QString& connection)
{
    NetworkManager::Settings::Connection * con = NetworkManager::Settings::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to remove this connection";
        return;
    }

    con->remove();
}
