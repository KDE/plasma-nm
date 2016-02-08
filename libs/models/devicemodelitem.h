/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_MODEL_DEVICE_MODEL_ITEM_H
#define PLASMA_NM_MODEL_DEVICE_MODEL_ITEM_H

#include <NetworkManagerQt/Device>

#include "devicemodel.h"

class Q_DECL_EXPORT DeviceModelItem : public QObject
{
Q_OBJECT
public:
    explicit DeviceModelItem(QObject * parent = 0);
    virtual ~DeviceModelItem();

    QString activeConnection() const;
    void setActiveConnection(const QString& name);

    QStringList details() const;

    QString deviceName() const;
    void setDeviceName(const QString& name);

    QString devicePath() const;
    void setDevicePath(const QString& path);

    NetworkManager::Device::State deviceState() const;
    void setDeviceState(const NetworkManager::Device::State state);

    QString deviceUdi() const;
    void setDeviceUdi(const QString& udi);

    NetworkManager::Device::Type deviceType() const;
    void setDeviceType(const NetworkManager::Device::Type type);

    QString deviceLabel() const;
    void setDeviceLabel(const QString& label);

    QString deviceSubLabel() const;

    QString icon() const;

    bool operator==(const DeviceModelItem * item) const;

public Q_SLOTS:
    void updateDetails();

private:
    QString m_activeConnection;
    QString m_deviceLabel;
    QString m_deviceName;
    QString m_deviceOriginalName;
    QString m_devicePath;
    QString m_deviceUdi;
    QStringList m_details;
    NetworkManager::Device::State m_deviceState;
    NetworkManager::Device::Type m_deviceType;
};

#endif // PLASMA_NM_MODEL_DEVICE_MODEL_ITEM_H
