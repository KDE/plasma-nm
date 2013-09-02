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

#ifndef PLASMA_NM_CONNECTION_SETTINGS_HANDLER_H
#define PLASMA_NM_CONNECTION_SETTINGS_HANDLER_H

#include <QObject>

#include <NetworkManagerQt/generic-types.h>

class ConnectionSettingsHandler : public QObject
{
Q_OBJECT
public:
    explicit ConnectionSettingsHandler(QObject* parent = 0);
    virtual ~ConnectionSettingsHandler();

public Q_SLOTS:
    QVariantMap loadSettings(const QString & uuid);
    void addConnection(const QVariantMap & map);
    void addAndActivateConnection(const QVariantMap & map, const QString & device, const QString & specificPath);
    void saveSettings(const QVariantMap & map, const QString & connection = QString());

private Q_SLOTS:
    void gotSecrets(const QString & id, bool success, const NMVariantMapMap & secrets, const QString & msg);

Q_SIGNALS:
    void loadSecrets(const QVariantMap & secrets);
private:
    NMVariantMapMap nmVariantMapMap(const QVariantMap & map);
};

#endif // PLASMA_NM_CONNECTION_SETTINGS_HANDLER_H
