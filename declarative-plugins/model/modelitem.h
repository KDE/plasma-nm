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

#include <NetworkManagerQt/activeconnection.h>
#include <NetworkManagerQt/connection.h>
#include <NetworkManagerQt/settings/connection.h>
#include <NetworkManagerQt/device.h>

#include "model.h"

class ModelItem : public QObject
{
Q_OBJECT
public:
    Q_ENUMS(Detail)
    enum SectionType { Connected, Known, Unknown };

    explicit ModelItem(const NetworkManager::Device::Ptr &device = NetworkManager::Device::Ptr(), QObject * parent = 0);
    virtual ~ModelItem();

    // Basic properties

    bool connected() const;
    bool connecting() const;
    virtual QString icon() const;
    QString name() const;
    QString uuid() const;
    virtual QString ssid() const;
    virtual int signal() const;
    virtual bool secure() const;
    QString sectionType() const;
    bool shouldBeRemoved() const;
    NetworkManager::Settings::ConnectionSettings::ConnectionType type() const;

    bool operator==(const ModelItem * item) const;

    // Detail info

    QString detailInformations() const;

    void setDetailFlags(Model::Details flags);

    // Objects

    virtual void setActiveConnection(const NetworkManager::ActiveConnection::Ptr & active);
    NetworkManager::ActiveConnection::Ptr activeConnection() const;

    virtual void setConnection(const NetworkManager::Settings::Connection::Ptr & connection);
    NetworkManager::Settings::Connection::Ptr connection() const;

    void addDevice(const QString & device);
    void removeDevice(const QString & device);

    QString activeDevicePath() const;

    // Object paths

    QString connectionPath() const;
    QStringList devicePaths() const;
    virtual QString specificParameter() const;

Q_SIGNALS:
    void itemChanged();

private Q_SLOTS:
    void onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    void onConnectionUpdated();
    void onDefaultRouteChanged(bool defaultRoute);

protected:
    NetworkManager::ActiveConnection::Ptr m_active;
    NetworkManager::Settings::Connection::Ptr m_connection;

    Model::Details m_flags;

    bool m_connected;
    bool m_connecting;
    QString m_connectionPath;
    QStringList m_devicePaths;
    QString m_activeDevicePath;
    QString m_details;
    QString m_name;
    SectionType m_sectionType;
    QString m_uuid;
    NetworkManager::Settings::ConnectionSettings::ConnectionType m_type;

    virtual void addSpecificDevice(const NetworkManager::Device::Ptr & device);
    virtual void updateDetails();
    virtual void setConnectionSettings(const NetworkManager::Settings::ConnectionSettings::Ptr &settings);
    virtual bool shouldBeRemovedSpecific() const;
};

#endif // PLASMA_NM_CONNECTION_ITEM_H
