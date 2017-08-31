/*
    Copyright 2017 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_CONFIGURATION_H
#define PLASMA_NM_CONFIGURATION_H

#include <QObject>

#include <NetworkManagerQt/Manager>

class Q_DECL_EXPORT Configuration : public QObject
{
    Q_PROPERTY(bool unlockModemOnDetection READ unlockModemOnDetection WRITE setUnlockModemOnDetection)
    Q_PROPERTY(bool manageVirtualConnections READ manageVirtualConnections WRITE setManageVirtualConnections)
    Q_OBJECT
public:
    static bool unlockModemOnDetection();
    static void setUnlockModemOnDetection(bool unlock);

    static bool manageVirtualConnections();
    static void setManageVirtualConnections(bool manage);
};

#endif // PLAMA_NM_CONFIGURATION_H

