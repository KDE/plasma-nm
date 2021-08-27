/*
    SPDX-FileCopyrightText: 2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_MONITOR_H
#define PLASMA_NM_MONITOR_H

#include <QDBusPendingCallWatcher>
#include <QObject>

#include "bluetoothmonitor.h"
#if WITH_MODEMMANAGER_SUPPORT
#include "modemmonitor.h"
#endif

class Q_DECL_EXPORT Monitor : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasmanetworkmanagement")
public:
    explicit Monitor(QObject *parent);
    ~Monitor() override;

public Q_SLOTS:
    Q_SCRIPTABLE bool bluetoothConnectionExists(const QString &bdAddr, const QString &service);
    Q_SCRIPTABLE void addBluetoothConnection(const QString &bdAddr, const QString &service, const QString &connectionName);
#if WITH_MODEMMANAGER_SUPPORT
    Q_SCRIPTABLE void unlockModem(const QString &modem);
#endif
private:
    BluetoothMonitor *m_bluetoothMonitor;
#if WITH_MODEMMANAGER_SUPPORT
    ModemMonitor *m_modemMonitor;
#endif
};

#endif // PLASMA_NM_MONITOR_H
