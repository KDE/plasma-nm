/*
    Copyright 2011 Lamarque Souza <lamarque@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "../lib/mobileconnectionwizard.h"

#include <QDBusObjectPath>
#include <ModemManagerQt/manager.h>

class BluetoothMonitor: public QObject
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.kde.plasmanetworkmanagement")
public:
    explicit BluetoothMonitor(QObject * parent);
    ~BluetoothMonitor();

public Q_SLOTS:
    Q_SCRIPTABLE void addBluetoothConnection(const QString & bdAddr, const QString & service);
private Q_SLOTS:
    void init();
    void modemAdded(const QString &udi);
private:
    QString mBdaddr;
    QString mService;
    QString mDunDevice;
    QString mDevicePath;
    QString mDeviceName;
    QWeakPointer<MobileConnectionWizard> mobileConnectionWizard;
};
#endif
