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

#include "globalstatus.h"

#include <QDBusConnection>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>

#include <KLocalizedString>

#include "debug.h"

GlobalStatus::GlobalStatus(QObject* parent):
    QObject(parent)
{
}

GlobalStatus::~GlobalStatus()
{
}

void GlobalStatus::init()
{
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)));
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));

    statusChanged(NetworkManager::status());
}

void GlobalStatus::activeConnectionsChanged()
{
    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        connect(active.data(), SIGNAL(default4Changed(bool)),
                SLOT(defaultChanged()), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(default6Changed(bool)),
                SLOT(defaultChanged()), Qt::UniqueConnection);
    }
}

void GlobalStatus::defaultChanged()
{
    statusChanged(NetworkManager::status());
}

void GlobalStatus::statusChanged(NetworkManager::Status status)
{
    QString statusMsg;
    bool connected = false;
    bool inProgress = false;

    if (status == NetworkManager::Connected ||
        status == NetworkManager::ConnectedLinkLocal ||
        status == NetworkManager::ConnectedSiteOnly) {

        QString name;
        foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
            if (active->default4() || active->default6()) {
                name = active->connection()->name();
                break;
            }
        }
        statusMsg = i18n("Connected to %1", name);
        connected = true;
        inProgress = false;
    } else {
        switch (status) {
            case NetworkManager::Asleep:
                statusMsg = i18n("Inactive");
                connected = false;
                inProgress = false;
                break;
            case NetworkManager::Disconnected:
                statusMsg = i18n("Disconnected");
                connected = false;
                inProgress = false;
                break;
            case NetworkManager::Disconnecting:
                statusMsg = i18n("Disconnecting");
                connected = true;
                inProgress = true;
                break;
            case NetworkManager::Connecting:
                statusMsg = i18n("Connecting");
                connected = false;
                inProgress = true;
                break;
            default:
                statusMsg = checkUnknownReason();
                connected = false;
                inProgress = false;
                break;
        }
    }

    NMAppletDebug() << "Emit signal setGlobalStatus(" << statusMsg << ", " << connected << ", " << inProgress << ")";
    Q_EMIT setGlobalStatus(statusMsg, connected, inProgress);
}


QString GlobalStatus::checkUnknownReason() const
{
    // check if NM is running
    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(NM_DBUS_INTERFACE)) {
        return i18n("NetworkManager not running");
    }
    // check if it has the correct version
    else if (NetworkManager::compareVersion(0, 9, 8) == -1) {
        return i18n("Incompatible NM version, 0.9.8 required");
    }

    return i18nc("global connection state", "Unknown");
}
