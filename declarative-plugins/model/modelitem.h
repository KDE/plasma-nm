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

#include <QtNetworkManager/activeconnection.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/vpnconnection.h>
#include <QtNetworkManager/wirelessnetwork.h>
#include <QtNetworkManager/settings/connection.h>

class ModelItem : public QObject
{
Q_OBJECT
public:
    ModelItem(NetworkManager::Device * device = 0, QObject * parent = 0);
    ~ModelItem();

    // Basic properties

    QString name() const;
    QString uuid() const;
    bool connected() const;
    bool connecting() const;
    QString deviceUdi() const;
    QString ssid() const;
    int signal() const;
    bool secure() const;
    NetworkManager::Settings::ConnectionSettings::ConnectionType type() const;

    bool operator==(ModelItem * item);

    // Detail info

    QString detailInformations() const;

    // Objects

    void setActiveConnection(NetworkManager::ActiveConnection * active);
    NetworkManager::ActiveConnection * activeConnection() const;

    void setConnection(NetworkManager::Settings::Connection * connection);
    NetworkManager::Settings::Connection * connection() const;

    void setDevice(NetworkManager::Device * device);
    NetworkManager::Device * device() const;

    void setWirelessNetwork(NetworkManager::WirelessNetwork * network);
    NetworkManager::WirelessNetwork * wirelessNetwork() const;

    // Object paths

    QString connectionPath() const;
    QString devicePath() const;
    QString specificPath() const;

Q_SIGNALS:
    void accessPointChanged();
    void connectionChanged();
    void defaultRouteChanged();
    void signalChanged();
    void stateChanged();

private Q_SLOTS:
    void onAccessPointChanged(const QString & accessPoint);
    void onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    void onConnectionUpdated(const QVariantMapMap & map);
    void onDefaultRouteChanged(bool defaultRoute);
    void onSignalStrengthChanged(int strength);
    void onVpnConnectionStateChanged(NetworkManager::VpnConnection::State state);

private:
    NetworkManager::ActiveConnection * m_active;
    NetworkManager::Settings::Connection * m_connection;
    NetworkManager::Device * m_device;
    NetworkManager::WirelessNetwork * m_network;
    NetworkManager::VpnConnection * m_vpn;

    QString m_name;
    QString m_uuid;
    bool m_connected;
    bool m_conecting;
    QString m_details;
    QString m_deviceUdi;
    QString m_ssid;
    int m_signal;
    bool m_secure;
    NetworkManager::Settings::ConnectionSettings::ConnectionType m_type;

    void updateDetails();
    void setConnectionSettings(const QVariantMapMap & map);
};

#endif // PLASMA_NM_CONNECTION_ITEM_H
