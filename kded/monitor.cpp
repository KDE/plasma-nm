/*
    SPDX-FileCopyrightText: 2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
