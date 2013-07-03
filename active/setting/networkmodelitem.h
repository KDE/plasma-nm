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

#ifndef PLASMA_NM_SETTINGS_NETWORK_MODEL_ITEM_H
#define PLASMA_NM_SETTINGS_NETWORK_MODEL_ITEM_H

#include <QObject>

class NetworkModelItem : public QObject
{
Q_OBJECT
public:
    enum NetworkType { Ethernet, Modem, Vpn, Wifi };

    explicit NetworkModelItem(NetworkType type, const QString & path = QString(), QObject * parent = 0);
    virtual ~NetworkModelItem();

    // Basic properties
    NetworkType type() const;
    void setType(NetworkType type);

    QString path() const;
    void setPath(const QString & path);

    QString icon() const;
    QString name() const;

    bool operator==(const NetworkModelItem * item) const;

private:
    NetworkType m_type;
    QString m_path;
};

#endif // PLASMA_NM_SETTINGS_NETWORK_MODEL_ITEM_H
