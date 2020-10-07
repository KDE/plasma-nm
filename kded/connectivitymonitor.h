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

#ifndef PLASMA_NM_CONNECTIVITY_MONITOR_H
#define PLASMA_NM_CONNECTIVITY_MONITOR_H

#include <NetworkManagerQt/Manager>

#include <KNotification>
#include <QObject>
#include <QPointer>
#include <QTimer>

class ConnectivityMonitor : public QObject
{
    Q_OBJECT
public:
    explicit ConnectivityMonitor(QObject *parent);
    ~ConnectivityMonitor() override;

private Q_SLOTS:
    void connectivityChanged(NetworkManager::Connectivity connectivity);
    void checkConnectivity();

private:
    void showLimitedConnectivityNotification();

    QPointer<KNotification> m_notification;

    QTimer m_limitedConnectivityTimer;
};

#endif // PLASMA_NM_CONNECTIVITY_MONITOR_H
