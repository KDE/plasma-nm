/*
    Copyright 2013 Dan Vratil <dvratil@redhat.com>
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

#ifndef PLASMA_NM_GLOBAL_CONFIG_H
#define PLASMA_NM_GLOBAL_CONFIG_H

#include <QObject>
#include <QStringList>

#include "plasmanm_export.h"

class PLASMA_NM_EXPORT GlobalConfig : public QObject
{
Q_OBJECT
Q_PROPERTY(bool airplaneModeEnabled
           READ airplaneModeEnabled
           WRITE setAirplaneModeEnabled
           NOTIFY airplaneModeEnabledChanged)
Q_PROPERTY(QStringList detailKeys
           READ detailKeys
           WRITE setDetailKeys
           NOTIFY detailKeysChanged)
Q_PROPERTY(NetworkSpeedUnit networkSpeedUnit
           READ networkSpeedUnit
           WRITE setNetworkSpeedUnit
           NOTIFY networkSpeedUnitChanged)
public:
    enum NetworkSpeedUnit { KBytes, KBits };
    Q_ENUMS(NetworkSpeedUnit)

    explicit GlobalConfig();
    virtual ~GlobalConfig();

public Q_SLOTS:
    bool airplaneModeEnabled() const;
    void setAirplaneModeEnabled(bool enabled);

    QStringList detailKeys() const;
    void setDetailKeys(const QStringList& keys);

    NetworkSpeedUnit networkSpeedUnit() const;
    void setNetworkSpeedUnit(NetworkSpeedUnit unit);

Q_SIGNALS:
    void airplaneModeEnabledChanged();
    void detailKeysChanged();
    void networkSpeedUnitChanged();

private:
    explicit GlobalConfig(void *dummy);
    GlobalConfig * instance();

    bool m_airplaneMode;
    QStringList m_keys;
    NetworkSpeedUnit m_unit;
};

#endif // PLASMA_NM_GLOBAL_CONFIG_H
