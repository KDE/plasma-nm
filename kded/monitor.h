/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_MONITOR_H
#define PLASMA_NM_MONITOR_H

#include <QObject>
#include <QDBusPendingCallWatcher>

#include "bluetoothmonitor.h"
#if WITH_MODEMMANAGER_SUPPORT
#include "modemmonitor.h"
#endif
#include <config.h>

class Q_DECL_EXPORT Monitor : public QObject
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.kde.plasmanetworkmanagement")
public:
    explicit Monitor(QObject * parent);
    virtual ~Monitor();

public Q_SLOTS:
    Q_SCRIPTABLE bool bluetoothConnectionExists(const QString &bdAddr, const QString &service);
    Q_SCRIPTABLE void addBluetoothConnection(const QString &bdAddr, const QString &service, const QString &connectionName);
#if WITH_MODEMMANAGER_SUPPORT
    Q_SCRIPTABLE void unlockModem(const QString &modem);
#endif
private:
    BluetoothMonitor * m_bluetoothMonitor;
#if WITH_MODEMMANAGER_SUPPORT
    ModemMonitor * m_modemMonitor;
#endif
};


#endif // PLASMA_NM_MONITOR_H
