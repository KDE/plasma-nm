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

#ifndef PLASMA_NM_MODEL_WIRELESS_ITEM_H
#define PLASMA_NM_MODEL_WIRELESS_ITEM_H

#include <NetworkManagerQt/WirelessNetwork>

#include "model/modelitem.h"

class ModelWirelessItem : public ModelItem
{
Q_OBJECT
public:
    explicit ModelWirelessItem(const NetworkManager::Device::Ptr &device = NetworkManager::Device::Ptr(), QObject * parent = 0);
    ~ModelWirelessItem();

    // Basic properties

    QString icon() const;
    QString ssid() const;
    int signal() const;
    bool secure() const;

    // Objects

    void setConnection(const NetworkManager::Settings::Connection::Ptr & connection);

    void setWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network);
    NetworkManager::WirelessNetwork::Ptr wirelessNetwork() const;

    // Object paths

    QString specificPath() const;

private Q_SLOTS:
    void onAccessPointChanged(const QString & accessPoint);
    void onSignalStrengthChanged(int strength);
    void wirelessNetworkRemoved();

protected:
    void updateDetails();
    void setConnectionSettings(const NetworkManager::Settings::ConnectionSettings::Ptr &settings);
    bool shouldBeRemovedSpecific() const;

private:
    NetworkManager::WirelessNetwork::Ptr m_network;

    QString m_ssid;
    int m_previousSignal;
    int m_signal;
    bool m_secure;
};

#endif // PLASMA_NM_CONNECTION_WIRELESS_ITEM_H
