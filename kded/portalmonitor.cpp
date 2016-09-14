/*
 * Copyright 2016  Jan Grulich <jgrulich@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "portalmonitor.h"

#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDesktopServices>

#include <KLocalizedString>
#include <KNotification>

#include <NetworkManagerQt/ActiveConnection>

PortalMonitor::PortalMonitor(QObject *parent)
    : QObject(parent)
{
    checkConnectivity();

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::connectivityChanged, this, &PortalMonitor::connectivityChanged);
}

PortalMonitor::~PortalMonitor()
{
}

void PortalMonitor::connectivityChanged(NetworkManager::Connectivity connectivity)
{
    if (connectivity == NetworkManager::Portal) {
        NetworkManager::ActiveConnection::Ptr primaryConnection = NetworkManager::primaryConnection();
        KNotification *notification = new KNotification(QStringLiteral("CaptivePortal"), KNotification::CloseOnTimeout, this);
        notification->setActions(QStringList{i18n("Log in")});
        notification->setComponentName(QStringLiteral("networkmanagement"));
        if (primaryConnection) {
            notification->setTitle(primaryConnection->id());
        } else {
            notification->setTitle(i18n("Network authentication"));
        }
        notification->setText(i18n("You need to log in to this network"));
        notification->sendEvent();
        connect(notification, &KNotification::action1Activated, this, [] () {
            QDesktopServices::openUrl(QUrl("https://kde.org"));
        });
    }
}

void PortalMonitor::checkConnectivity()
{
    QDBusPendingReply<uint> pendingReply = NetworkManager::checkConnectivity();
    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(pendingReply);
    connect(callWatcher, &QDBusPendingCallWatcher::finished, this, [this] (QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<uint> reply = *watcher;
        if (reply.isValid()) {
            connectivityChanged((NetworkManager::Connectivity)reply.value());
        }
        watcher->deleteLater();
    });
}
