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

#ifndef PLASMA_NM_AVAILABLE_DEVICES_H
#define PLASMA_NM_AVAILABLE_DEVICES_H

#include <QObject>

#include <NetworkManagerQt/Device>

class AvailableDevices : public QObject
{
Q_PROPERTY(QString wiredDevicePath READ wiredDevicePath NOTIFY wiredDevicePathChanged)
Q_PROPERTY(QString wirelessDevicePath READ wirelessDevicePath NOTIFY wirelessDevicePathChanged)
Q_PROPERTY(QString wimaxDevicePath READ wimaxDevicePath NOTIFY wimaxDevicePathChanged)
Q_PROPERTY(QString wwanDevicePath READ wwanDevicePath NOTIFY wwanDevicePathChanged)
Q_PROPERTY(bool wiredAvailable READ isWiredAvailable NOTIFY wiredAvailableChanged)
Q_PROPERTY(bool wirelessAvailable READ isWirelessAvailable NOTIFY wirelessAvailableChanged)
Q_PROPERTY(bool wimaxAvailable READ isWimaxAvailable NOTIFY wimaxAvailableChanged)
Q_PROPERTY(bool wwanAvailable READ isWwanAvailable NOTIFY wwanAvailableChanged)
Q_OBJECT
public:
    explicit AvailableDevices(QObject* parent = 0);
    virtual ~AvailableDevices();

public Q_SLOTS:
    void init();

    QString wiredDevicePath() const;
    QString wirelessDevicePath() const;
    QString wimaxDevicePath() const;
    QString wwanDevicePath() const;

    bool isWiredAvailable() const;
    bool isWirelessAvailable() const;
    bool isWimaxAvailable() const;
    bool isWwanAvailable() const;

private Q_SLOTS:
    void deviceAdded(const QString& dev);
    void deviceRemoved();

Q_SIGNALS:
    void wiredDevicePathChanged(const QString & path);
    void wirelessDevicePathChanged(const QString & path);
    void wimaxDevicePathChanged(const QString & path);
    void wwanDevicePathChanged(const QString & path);

    void wiredAvailableChanged(bool available);
    void wirelessAvailableChanged(bool available);
    void wimaxAvailableChanged(bool available);
    void wwanAvailableChanged(bool available);

private:
    QString m_wiredDevicePath;
    QString m_wirelessDevicePath;
    QString m_wwanDevicePath;
    QString m_wimaxDevicePath;

    bool m_wiredAvailable;
    bool m_wirelessAvailable;
    bool m_wimaxAvailable;
    bool m_wwanAvailable;

};

#endif // PLASMA_NM_AVAILABLE_DEVICES_H
