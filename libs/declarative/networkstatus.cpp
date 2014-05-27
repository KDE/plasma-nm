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
#include "uiutils.h"

#include <QDBusConnection>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>

#include <KLocalizedString>

NetworkStatus::NetworkStatus(QObject* parent)
    : QObject(parent)
{
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)));
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionsChanged()),
            SLOT(activeConnectionsChanged()));

    activeConnectionsChanged();
    statusChanged(NetworkManager::status());
}

NetworkStatus::~NetworkStatus()
{
}

QString NetworkStatus::activeConnections() const
{
    return m_activeConnections;
}

QString NetworkStatus::networkStatus() const
{
    return m_networkStatus;
}

void NetworkStatus::activeConnectionsChanged()
{
    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        connect(active.data(), SIGNAL(default4Changed(bool)),
                SLOT(defaultChanged()), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(default6Changed(bool)),
                SLOT(defaultChanged()), Qt::UniqueConnection);
        connect(active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(changeActiveConnections()));
    }

    changeActiveConnections();
}

void NetworkStatus::defaultChanged()
{
    statusChanged(NetworkManager::status());
}

void NetworkStatus::statusChanged(NetworkManager::Status status)
{
    switch (status) {
        case NetworkManager::ConnectedLinkLocal:
            m_networkStatus = i18nc("Connected", "A network device is connected, but there is only link-local connectivity");
            break;
        case NetworkManager::ConnectedSiteOnly:
            m_networkStatus = i18nc("Connected", "A network device is connected, but there is only site-local connectivity");
            break;
        case NetworkManager::Connected:
            m_networkStatus = i18nc("Connected", "A network device is connected, with global network connectivity");
            break;
        case NetworkManager::Asleep:
            m_networkStatus = i18nc("Inactive", "Networking is inactive and all devices are disabled");
            break;
        case NetworkManager::Disconnected:
            m_networkStatus = i18nc("Disconnected", "There is no active network connection");
            break;
        case NetworkManager::Disconnecting:
            m_networkStatus = i18nc("Disconnecting", "Network connections are being cleaned up");
            break;
        case NetworkManager::Connecting:
            m_networkStatus = i18nc("Connecting", "A network device is connecting to a network and there is no other available network connection");
            break;
        default:
            m_networkStatus = checkUnknownReason();
            break;
    }

    if (status == NetworkManager::ConnectedLinkLocal ||
        status == NetworkManager::ConnectedSiteOnly ||
        status == NetworkManager::Connected) {
        changeActiveConnections();
    } else {
        m_activeConnections = m_networkStatus;
        Q_EMIT activeConnectionsChanged(m_activeConnections);
    }

    Q_EMIT networkStatusChanged(m_networkStatus);
}

void NetworkStatus::changeActiveConnections()
{
    if (NetworkManager::status() != NetworkManager::Connected &&
        NetworkManager::status() != NetworkManager::ConnectedLinkLocal &&
        NetworkManager::status() != NetworkManager::ConnectedSiteOnly) {
        return;
    }

    QString activeConnections = "<qt>";
    const QString format = "<b>%1 - %2</b><br>%3<br><br>";
    const QString formatDefault = "<b>%1 - %2</b><br><b>%3</b><br><br>";

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
                    activeConnections += QString(formatDefault).arg(devName, conType, status);
                } else {
                    activeConnections += QString(format).arg(devName, conType, status);
                }
            }
        }
    }

    activeConnections += "</qt>";
    // Remove the last two new lines
    activeConnections.replace("<br><br></qt>", "</qt>");

    m_activeConnections = activeConnections;
    Q_EMIT activeConnectionsChanged(activeConnections);
}

QString NetworkStatus::checkUnknownReason() const
{
    // Check if NetworkManager is running.
    if (!QDBusConnection::systemBus().interface()->isServiceRegistered(NM_DBUS_INTERFACE)) {
        return i18n("NetworkManager not running");
    }

    // Check for compatible NetworkManager version.
    if (NetworkManager::compareVersion(0, 9, 8) < 0) {
        return i18n("NetworkManager 0.9.8 required, found %1.", NetworkManager::version());
    }

    return i18nc("global connection state", "Unknown");
}
