/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_MODEL_NETWORK_MODEL_ITEM_H
#define PLASMA_NM_MODEL_NETWORK_MODEL_ITEM_H

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Utils>

#include <Plasma/DataEngine>

#include "networkmodel.h"
#include "plasmanm_export.h"

class PLASMA_NM_EXPORT NetworkModelItem : public QObject
{
Q_OBJECT
public:

    enum ItemType { UnavailableConnection, AvailableConnection, AvailableAccessPoint };

    explicit NetworkModelItem(QObject * parent = 0);
    virtual ~NetworkModelItem();

    QString activeConnectionPath() const;
    void setActiveConnectionPath(const QString& path);

    QString connectionPath() const;
    void setConnectionPath(const QString& path);

    NetworkManager::ActiveConnection::State connectionState() const;
    void setConnectionState(NetworkManager::ActiveConnection::State state);

    QString details() const;
    void setDetailsList(const QStringList& list);

    QString devicePath() const;
    void setDeviceName(const QString& name);
    void setDevicePath(const QString& path);

    QString deviceState() const;
    void setDeviceState(const NetworkManager::Device::State state);

    QString download() const;

    QString icon() const;

    ItemType itemType() const;

    NetworkManager::WirelessSetting::NetworkMode mode() const;
    void setMode(const NetworkManager::WirelessSetting::NetworkMode mode);

    QString name() const;
    void setName(const QString& name);

    QString originalName() const;

    QString sectionType() const;

    NetworkManager::Utils::WirelessSecurityType securityType() const;
    void setSecurityType(NetworkManager::Utils::WirelessSecurityType type);

    int signal() const;
    void setSignal(int signal);

    bool slave() const;
    void setSlave(bool slave);

    QString specificPath() const;
    void setSpecificPath(const QString& path);

    QString speed() const;
    void setSpeed(int speed);

    QString ssid() const;
    void setSsid(const QString& ssid);

    QDateTime timestamp() const;
    void setTimestamp(const QDateTime& date);

    NetworkManager::ConnectionSettings::ConnectionType type() const;
    void setType(NetworkManager::ConnectionSettings::ConnectionType type);

    QString uni() const;

    QString upload() const;

    QString uuid() const;
    void setUuid(const QString& uuid);

    QString vpnState() const;
    void setVpnState(NetworkManager::VpnConnection::State state);

    bool operator==(const NetworkModelItem * item) const;

public Q_SLOTS:
    void dataUpdated(const QString & sourceName, const Plasma::DataEngine::Data & data);
    void updateDetails();

Q_SIGNALS:
    void itemUpdated();

private:
    void initializeDataEngine();
    void removeDataEngine();
    void setUpdateEnabled(bool enabled);

    QString m_activeConnectionPath;
    int m_bitrate;
    QString m_connectionPath;
    NetworkManager::ActiveConnection::State m_connectionState;
    QString m_devicePath;
    QString m_deviceName;
    NetworkManager::Device::State m_deviceState;
    QString m_details;
    QStringList m_detailsList;
    QString m_download;
    QString m_downloadSource;
    QString m_downloadUnit;
    Plasma::DataEngine * m_engine;
    NetworkManager::WirelessSetting::NetworkMode m_mode;
    QString m_name;
    QString m_nsp;
    NetworkManager::Utils::WirelessSecurityType m_securityType;
    int m_signal;
    bool m_slave;
    QString m_specificPath;
    QString m_ssid;
    QDateTime m_timestamp;
    NetworkManager::ConnectionSettings::ConnectionType m_type;
    QString m_upload;
    QString m_uploadSource;
    QString m_uploadUnit;
    bool m_updateEnabled;
    QString m_uuid;
    NetworkManager::VpnConnection::State m_vpnState;
};

#endif // PLASMA_NM_MODEL_NETWORK_MODEL_ITEM_H
