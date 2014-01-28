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

#ifndef PLASMA_NM_NETWORK_MODEL_H
#define PLASMA_NM_NETWORK_MODEL_H

#include <QtCore/QAbstractListModel>

#include "config.h"
#include "networkitemslist.h"
#include "plasmanm_export.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/VpnConnection>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Utils>

#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
#include <ModemManagerQt/modem.h>
#else
#include <ModemManagerQt/modeminterface.h>
#endif
#endif

class PLASMA_NM_EXPORT NetworkModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit NetworkModel(QObject* parent = 0);
    virtual ~NetworkModel();

    enum ItemRole {
        ConnectionDetailsRole = Qt::UserRole + 1,
        ConnectionIconRole,
        ConnectionPathRole,
        ConnectionStateRole,
        DevicePathRole,
        DeviceStateRole,
        ItemTypeRole,
        LastUsedRole,
        NameRole,
        SecurityTypeRole,
        SecurityTypeStringRole,
        SectionRole,
        SignalRole,
        SsidRole,
        SpeedRole,
        SpecificPathRole,
        TypeRole,
        UuidRole,
        UniRole,
        VpnState,
        SlaveRole,
        TimeStamp
    };

    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

    void setDetails(const QStringList& list);
    void updateItems();

private Q_SLOTS:
    void activeConnectionAdded(const QString& activeConnection);
    void activeConnectionRemoved(const QString& activeConnection);
    void activeConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    void activeVpnConnectionStateChanged(NetworkManager::VpnConnection::State state,NetworkManager::VpnConnection::StateChangeReason reason);
    void availableConnectionAppeared(const QString& connection);
    void availableConnectionDisappeared(const QString& connection);
    void bitrateChanged(int bitrate);
    void connectionAdded(const QString& connection);
    void connectionRemoved(const QString& connection);
    void connectionUpdated();
    void deviceAdded(const QString& device);
    void deviceRemoved(const QString& device);
    void deviceStateChanged(NetworkManager::Device::State state, NetworkManager::Device::State oldState, NetworkManager::Device::StateChangeReason reason);
#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
    void gsmNetworkAccessTechnologyChanged(ModemManager::Modem::AccessTechnologies technology);
    void gsmNetworkCurrentModesChanged();
#else
    void gsmNetworkAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology technology);
    void gsmNetworkAllowedModeChanged(ModemManager::ModemInterface::AllowedMode mode);
#endif
    void gsmNetworkSignalQualityChanged(uint signal);
#endif
    void statusChanged(NetworkManager::Status status);
    void wirelessNetworkAppeared(const QString& ssid);
    void wirelessNetworkDisappeared(const QString& ssid);
    void wirelessNetworkSignalChanged(int signal);
    void wirelessNetworkReferenceApChanged(const QString& accessPoint);

    void initialize();

private:
    QStringList m_detailsList;
    NetworkItemsList m_list;

    void addActiveConnection(const NetworkManager::ActiveConnection::Ptr& activeConnection);
    void addAvailableConnection(const QString& connection, const NetworkManager::Device::Ptr& device);
    void addConnection(const NetworkManager::Connection::Ptr& connection);
    void addDevice(const NetworkManager::Device::Ptr& device);
    void addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr& network, const NetworkManager::WirelessDevice::Ptr& device);
    void initializeSignals();
    void initializeSignals(const NetworkManager::ActiveConnection::Ptr& activeConnection);
    void initializeSignals(const NetworkManager::Connection::Ptr& connection);
    void initializeSignals(const NetworkManager::Device::Ptr& device);
    void initializeSignals(const NetworkManager::WirelessNetwork::Ptr& network);
    void updateItem(NetworkModelItem * item);

    NetworkManager::Utils::WirelessSecurityType alternativeWirelessSecurity(const NetworkManager::Utils::WirelessSecurityType type);
};

#endif // PLASMA_NM_NETWORK_MODEL_H
