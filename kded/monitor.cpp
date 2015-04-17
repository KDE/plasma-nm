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


#include "monitor.h"

#include <QDBusConnection>

Monitor::Monitor(QObject* parent)
    : QObject(parent)
{
#if WITH_MODEMMANAGER_SUPPORT
    m_modemMonitor = new ModemMonitor(this);
#endif
    m_bluetoothMonitor = new BluetoothMonitor(this);

    QDBusConnection::sessionBus().registerService("org.kde.plasmanetworkmanagement");
    QDBusConnection::sessionBus().registerObject("/org/kde/plasmanetworkmanagement", this, QDBusConnection::ExportScriptableContents);
}

Monitor::~Monitor()
{
    delete m_bluetoothMonitor;
#if WITH_MODEMMANAGER_SUPPORT
    delete m_modemMonitor;
#endif
}

bool Monitor::bluetoothConnectionExists(const QString &bdAddr, const QString &service)
{
    return m_bluetoothMonitor->bluetoothConnectionExists(bdAddr, service);
}

void Monitor::addBluetoothConnection(const QString &bdAddr, const QString &service, const QString &connectionName)
{
    m_bluetoothMonitor->addBluetoothConnection(bdAddr, service, connectionName);
}

#if WITH_MODEMMANAGER_SUPPORT
void Monitor::unlockModem(const QString& modem)
{
    qDebug() << "unlocking " << modem;
    m_modemMonitor->unlockModem(modem);
}
#endif
