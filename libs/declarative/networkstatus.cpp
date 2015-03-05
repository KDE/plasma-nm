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
    Q_FOREACH (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
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
            m_networkStatus = i18nc("A network device is connected, but there is only link-local connectivity", "Connected");
            break;
        case NetworkManager::ConnectedSiteOnly:
            m_networkStatus = i18nc("A network device is connected, but there is only site-local connectivity", "Connected");
            break;
        case NetworkManager::Connected:
            m_networkStatus = i18nc("A network device is connected, with global network connectivity", "Connected");
            break;
        case NetworkManager::Asleep:
            m_networkStatus = i18nc("Networking is inactive and all devices are disabled", "Inactive");
            break;
        case NetworkManager::Disconnected:
            m_networkStatus = i18nc("There is no active network connection", "Disconnected");
            break;
        case NetworkManager::Disconnecting:
            m_networkStatus = i18nc("Network connections are being cleaned up", "Disconnecting");
            break;
        case NetworkManager::Connecting:
            m_networkStatus = i18nc("A network device is connecting to a network and there is no other available network connection", "Connecting");
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

    QString activeConnections = QStringLiteral("<qt>");
    const QString format = QStringLiteral("%1: %2<br>");
    const QString formatDefault = QStringLiteral("%1: %2<br>");

    Q_FOREACH (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        if (!active->devices().isEmpty()) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
#if NM_CHECK_VERSION(0, 9, 10)
            if (device && device->type() != NetworkManager::Device::Generic && device->type() <= NetworkManager::Device::Team) {
#else
            if (device) {
#endif
                bool connecting = false;
                bool connected = false;
                QString conType;
                QString status;
                NetworkManager::VpnConnection::Ptr vpnConnection;

                if (active->vpn()) {
                    conType = i18n("VPN");
                    vpnConnection = active.objectCast<NetworkManager::VpnConnection>();
                } else {
                    conType = UiUtils::interfaceTypeLabel(device->type(), device);
                }

                if (vpnConnection && active->vpn()) {
                    if (vpnConnection->state() >= NetworkManager::VpnConnection::Prepare &&
                        vpnConnection->state() <= NetworkManager::VpnConnection::GettingIpConfig) {
                        connecting = true;
                    } else if (vpnConnection->state() == NetworkManager::VpnConnection::Activated) {
                        connected = true;
                    }
                } else {
                    if (active->state() == NetworkManager::ActiveConnection::Activated) {
                        connected = true;
                    } else if (active->state() == NetworkManager::ActiveConnection::Activating) {
                        connecting = true;
                    }
                }

                const QString connectionName = active->connection()->name().replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;");
                if (connecting) {
                    status = i18n("Connecting to %1", connectionName);
                } else if (connected) {
                    status = i18n("Connected to %1", connectionName);
                }

                if (active->default4() || active->default6()) {
                    activeConnections += formatDefault.arg(conType, status);
                } else {
                    activeConnections += format.arg(conType, status);
                }
            }
        }
    }

    activeConnections += "</qt>";
    // Remove the last two new lines
    activeConnections.replace("<br></qt>", "</qt>");

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
