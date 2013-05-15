/*
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

#ifndef PLASMA_NM_HANDLER_H
#define PLASMA_NM_HANDLER_H

#include <NetworkManagerQt/Connection>

class Handler : public QObject
{
Q_OBJECT

public:
    explicit Handler(QObject* parent = 0);
    virtual ~Handler();

public Q_SLOTS:
    void activateConnection(const QString & connection, const QString & device, const QString & specificParameter);
    void addAndActivateConnection(const QString & device, const QString & specificParameter);
    void deactivateConnection(const QString & connection);
    void enableNetworking(bool enable);
    void enableWireless(bool enable);
    void enableWimax(bool enable);
    void enableWwan(bool enable);
    void editConnection(const QString & uuid);
    void removeConnection(const QString & connection);
    void openEditor();
};

#endif // PLASMA_NM_HANDLER_H
