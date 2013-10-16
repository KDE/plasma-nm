/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_MODEM_MONITOR_H
#define PLASMA_NM_MODEM_MONITOR_H

#include <QObject>
#include <QDBusPendingCallWatcher>

#include "config.h"

#ifdef MODEMMANAGERQT_ONE
#include <ModemManager/ModemManager.h>
#include <ModemManagerQt/modem.h>
#endif
#include <kdemacros.h>

class ModemMonitorPrivate;

/**
 * Monitors modem hardware and provides a PIN unlock dialog
 */
class KDE_EXPORT ModemMonitor : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ModemMonitor)
public:
    explicit ModemMonitor(QObject * parent);
    virtual ~ModemMonitor();
private slots:
    void modemAdded(const QString&);
#ifdef MODEMMANAGERQT_ONE
    void requestPin(MMModemLock lock);
#else
    void requestPin(const QString &);
#endif
    void onSendPinArrived(QDBusPendingCallWatcher *);
private:
    ModemMonitorPrivate * d_ptr;
};

#endif // PLASMA_NM_MODEM_MONITOR_H
