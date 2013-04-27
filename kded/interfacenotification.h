/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2013 by Daniel Nicoletti <dantti12@gmail.com>

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

#ifndef PLASMA_NM_INTERFACE_NOTIFICATION_H
#define PLASMA_NM_INTERFACE_NOTIFICATION_H

#include <QObject>

#include <NetworkManagerQt/device.h>

class KNotification;
class InterfaceNotification : public QObject
{
    Q_OBJECT
public:
    explicit InterfaceNotification(QObject *parent = 0);

private slots:
    void deviceAdded(const QString &uni);
    void addDevice(const NetworkManager::Device::Ptr &device);
    void stateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason);
    void notificationClosed();

private:
    QHash<QString, KNotification*> m_notifications;
};

#endif // PLASMA_NM_INTERFACE_NOTIFICATION_H
