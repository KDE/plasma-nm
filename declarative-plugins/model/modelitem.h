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

#ifndef PLASMA_NM_MODEL_ITEM_H
#define PLASMA_NM_MODEL_ITEM_H

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Utils>

#include "model.h"

class ModelItem : public QObject
{
Q_OBJECT
public:
    Q_ENUMS(Detail)
    explicit ModelItem(const QString& device = QString(), QObject * parent = 0);
    virtual ~ModelItem();

    QString details() const;
    QString icon() const;
    QString lastUsed() const;
    QString name() const;
    QString originalName() const;
    int signal() const;
    QString sectionType() const;
    QString speed() const;
    QString ssid() const;
    QString uni() const;
    QString uuid() const;

    // Object paths
    QString activeConnectionPath() const;
    QString connectionPath() const;
    QString devicePath() const;
    QString nspPath() const;
    QString specificPath() const;

    NetworkManager::ActiveConnection::State connectionState() const;
    NetworkManager::ConnectionSettings::ConnectionType type() const;
    NetworkManager::Utils::WirelessSecurityType securityType() const;

    bool operator==(const ModelItem * item) const;

    // Objects
    void setActiveConnection(const QString& active);
    void setConnection(const QString& connection);
    void setDevice(const QString& device);
    void setNsp(const QString& nsp);
    void setWirelessNetwork(const QString& ssid);

public Q_SLOTS:
    void updateAccessPoint(const QString& ap);
    void updateActiveConnectionState(NetworkManager::ActiveConnection::State state);
    void updateBitrate(int bitrate);
    void updateDetails();
    void updateSignalStrength(int strength);

private:
    QString m_activePath;
    QString m_accessPointPath;
    QString m_connectionPath;
    QString m_devicePath;
    QString m_device;
    QString m_details;
    QString m_name;
    QString m_nspPath;
    QString m_nsp;
    QString m_ssid;
    QString m_uuid;

    int m_bitrate;
    NetworkManager::ActiveConnection::State m_connectionState;
    QDateTime m_lastUsed;
    NetworkManager::Utils::WirelessSecurityType m_securityType;
    int m_signal;
    NetworkManager::ConnectionSettings::ConnectionType m_type;

    void setConnectionSettings(const NetworkManager::ConnectionSettings::Ptr& settings);
};

#endif // PLASMA_NM_CONNECTION_ITEM_H
