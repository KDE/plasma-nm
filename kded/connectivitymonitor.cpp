/*
 * Copyright 2016  Jan Grulich <jgrulich@redhat.com>
 * Copyright 2020  Kai Uwe Broulik <kde@broulik.de>
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

#include "connectivitymonitor.h"

#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDesktopServices>

#include <KLocalizedString>

#include <NetworkManagerQt/ActiveConnection>

#include "debug.h"

ConnectivityMonitor::ConnectivityMonitor(QObject *parent)
    : QObject(parent)
{
    m_limitedConnectivityTimer.setSingleShot(true);
    m_limitedConnectivityTimer.setInterval(10000);
    connect(&m_limitedConnectivityTimer, &QTimer::timeout, this, &ConnectivityMonitor::showLimitedConnectivityNotification);

    checkConnectivity();

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::connectivityChanged, this, &ConnectivityMonitor::connectivityChanged);
}

ConnectivityMonitor::~ConnectivityMonitor()
{
    if (m_notification) {
        m_notification->close();
    }
}

void ConnectivityMonitor::connectivityChanged(NetworkManager::Connectivity connectivity)
{
    if (m_notification && m_notification->property("nm_connectivity") != connectivity) {
        m_notification->close();
        m_notification = nullptr;
    }

    if (connectivity == NetworkManager::Limited) {
        qCDebug(PLASMA_NM) << "Network connectivity limited, scheduling notification";
        if (!m_limitedConnectivityTimer.isActive()) {
            m_limitedConnectivityTimer.start();
        }
    } else {
        m_limitedConnectivityTimer.stop();

        if (connectivity == NetworkManager::Portal) {
            qCDebug(PLASMA_NM) << "Detected captive portal";
            const NetworkManager::ActiveConnection::Ptr primaryConnection = NetworkManager::primaryConnection();
            const QString title = primaryConnection ? primaryConnection->id() : i18n("Network authentication");

            if (m_notification) {
                m_notification->setTitle(title);
                m_notification->update();
            } else {
                m_notification = new KNotification(QStringLiteral("CaptivePortal"), KNotification::Persistent);
                m_notification->setActions(QStringList{i18n("Log in")});
                m_notification->setComponentName(QStringLiteral("networkmanagement"));
                m_notification->setTitle(title);
                m_notification->setText(i18n("You need to log in to this network"));
                connect(m_notification, &KNotification::action1Activated, this, [this] () {
                    QDesktopServices::openUrl(QUrl("http://networkcheck.kde.org"));
                });
                m_notification->sendEvent();
            }
        }
    }
}

void ConnectivityMonitor::showLimitedConnectivityNotification()
{
    if (m_notification) {
        return;
    }

    m_notification = new KNotification(QStringLiteral("LimitedConnectivity"));
    m_notification->setComponentName(QStringLiteral("networkmanagement"));
    m_notification->setTitle(i18n("Limited Connectivity"));
    m_notification->setText(i18n("This device appears to be connected to a network but is unable to reach the internet."));
    m_notification->sendEvent();
}

void ConnectivityMonitor::checkConnectivity()
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
