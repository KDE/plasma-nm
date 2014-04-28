/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>
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

#ifndef PLASMA_NM_CONNECTION_EDITOR_H
#define PLASMA_NM_CONNECTION_EDITOR_H

#include <QObject>

#include <NetworkManagerQt/ConnectionSettings>

class ConnectionEditor : public QObject
{
Q_OBJECT
Q_PROPERTY(QStringList availableVpnPlugins READ availableVpnPlugins)

public:
    explicit ConnectionEditor(QObject* parent = 0);
    virtual ~ConnectionEditor();

    Q_INVOKABLE void addConnection(uint connectionType, bool shared);
    Q_INVOKABLE void addVpnConnection(const QString& plugin);
    Q_INVOKABLE void exportVpn(const QString& connectionUuid);
    Q_INVOKABLE void importVpn();
    QStringList availableVpnPlugins() const;

private:
    QMap<QString, QString> m_vpnPlugins;
//     void importSecretsFromPlainTextFiles();
//     void storeSecrets(const QMap<QString, QMap<QString, QString> > & map);
};

#endif // PLASMA_NM_CONNECTION_EDITOR_H
