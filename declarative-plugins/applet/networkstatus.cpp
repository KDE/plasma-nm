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

#include "networkstatus.h"
#include "debug.h"
#include "uiutils.h"

#include <QDBusConnection>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>

#include <KLocalizedString>



NetworkStatus::NetworkStatus(QObject* parent):
    QObject(parent)
{
}

NetworkStatus::~NetworkStatus()
{
}

void NetworkStatus::init()
{
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)));
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));

    activeConnectionsChanged();
    statusChanged(NetworkManager::status());
}

void NetworkStatus::activeConnectionsChanged()
{
    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        connect(active.data(), SIGNAL(default4Changed(bool)),
                SLOT(defaultChanged()), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(default6Changed(bool)),
                SLOT(defaultChanged()), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(changeTooltip()));
    }

    changeTooltip();
}

void NetworkStatus::defaultChanged()
{
    statusChanged(NetworkManager::status());
}

void NetworkStatus::statusChanged(NetworkManager::Status status)
{
    QString statusMsg;
    bool connected = false;
    bool inProgress = false;

    if (status == NetworkManager::Connected ||
        status == NetworkManager::ConnectedLinkLocal ||
        status == NetworkManager::ConnectedSiteOnly) {

        statusMsg = i18n("Connected");
        connected = true;
        inProgress = false;
        changeTooltip();
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

        Q_EMIT setTooltip(statusMsg);
    }

    NMAppletDebug() << "Emit signal setNetworkStatus(" << statusMsg << ", " << connected << ", " << inProgress << ")";
    Q_EMIT setGlobalStatus(statusMsg, connected, inProgress);
}

void NetworkStatus::changeTooltip()
{
    if (NetworkManager::status() != NetworkManager::Connected &&
        NetworkManager::status() != NetworkManager::ConnectedLinkLocal &&
        NetworkManager::status() != NetworkManager::ConnectedSiteOnly) {
        return;
    }

    QString tooltip = "<qt>";
    QString format = "<b>%1 - %2</b><br>%3<br><br>";
    QString formatDefault = "<b>%1 - %2</b><br><b>%3</b><br><br>";

    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        if (!active->devices().isEmpty()) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());

            if (device) {
                QString devName;
                QString conType;
                QString status;
                if (device->ipInterfaceName().isEmpty()) {
                    devName = device->interfaceName();
                } else {
                    devName = device->ipInterfaceName();
                }
//                 conType = NetworkManager::ConnectionSettings::typeAsString(active->connection()->settings()->connectionType());
                if (active->vpn()) {
                    conType = i18n("VPN Connection");
                } else {
                    conType = UiUtils::interfaceTypeLabel(device->type(), device);
                }
                if (active->state() == NetworkManager::ActiveConnection::Activated) {
                    status = i18n("Connected to %1", active->connection()->name());
                } else if (active->state() == NetworkManager::ActiveConnection::Activating) {
                    status = i18n("Connecting to %1", active->connection()->name());
                }
                if (active->default4() || active->default6()) {
                    tooltip += QString(formatDefault).arg(devName, conType, status);
                } else {
                    tooltip += QString(format).arg(devName, conType, status);
                }
            }
        }
    }

    tooltip += "</qt>";
    // Remove the last two new lines
    tooltip.replace("<br><br></qt>", "</qt>");

    Q_EMIT setTooltip(tooltip);
}

QString NetworkStatus::checkUnknownReason() const
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
