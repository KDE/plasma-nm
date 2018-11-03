/*
    Copyright 2011 Lamarque Souza <lamarque@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>
    Copyright 2015 David Rosca <nowrep@gmail.com>

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

#ifndef PLASMA_NM_BLUETOOTH_MONITOR_H
#define PLASMA_NM_BLUETOOTH_MONITOR_H
#if WITH_MODEMMANAGER_SUPPORT
#include "mobileconnectionwizard.h"
#include <ModemManagerQt/manager.h>
#endif

#include <QObject>

class BluetoothMonitor: public QObject
{
Q_OBJECT
public:
    explicit BluetoothMonitor(QObject * parent);
    ~BluetoothMonitor() override;

    bool bluetoothConnectionExists(const QString &bdAddr, const QString &service);
    void addBluetoothConnection(const QString &bdAddr, const QString &service, const QString &connectionName);
};
#endif
