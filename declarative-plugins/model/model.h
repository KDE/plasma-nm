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

#ifndef PLASMA_NM_MODEL_H
#define PLASMA_NM_MODEL_H

#include <QtCore/QAbstractListModel>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Device>

class ModelItem;

#include "monitor.h"
#include "modelitems.h"

class Model : public QAbstractListModel
{
Q_OBJECT
public:
    enum ItemRole {
        /**
         * Connection state, one of Unknown, Active, Activating, Deactivating, Deactivated
         */
        ConnectionStateRole = Qt::UserRole + 1,
        /**
         * D-bus path of the connection associated with this item
         */
        ConnectionPathRole,
        /**
         * SVG icon of the item from plasma-nm icon set, except icons for VPN, Adsl, Pppoe (for these we don't have SVG icons)
         */
        ConnectionIconRole,
        /**
         * Formated info about the connection, i.e IPv4 addresses, SSID etc.
         */
        ConnectionDetailsRole,
        /**
         * Device name of the device associated with this item
         */
        DeviceNameRole,
        /**
         * D-bus path of the device associated with this item
         */
        DevicePathRole,
        /**
         * Item name, it uses the name of associated connection or the name of associated AP (when there is no connection)
         */
        NameRole,
        /**
         * Item security, one of None, StaticWep, DynamicWep, Leap, WpaPsk, WpaEap, Wpa2Psk, Wpa2Eap
         */
        SecurityTypeRole,
        /**
         * Item section name, one of Active connections (the item is activated), Previous connections (there is some associated connection)
         * or Uknown connections (for available accesspoints without associated connections)
         */
        SectionRole,
        /**
         * Item signal strength, used only for wireless and gsm/cdma connections
         */
        SignalRole,
        /**
         * Item ssid, used only for wireless connections
         */
        SsidRole,
        /**
         * Used only for wireless (D-bus path of the associated AP) and wimax connections (D-bus path of the associated NSP)
         */
        SpecificPathRole,
        /**
         * Item uuid, valid only for items with associated connection
         */
        UuidRole,
        /**
         * Item unique identifier
         * For wireless connections is "ssid%devicePath", for the rest is "connectionName%devicePath"
         */
        UniRole,
        /**
         * Connection type of the item
         */
        TypeRole
    };

    explicit Model(QObject* parent = 0);
    virtual ~Model();

    int rowCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

public Q_SLOTS:
    void updateItems();

private Q_SLOTS:
    void addActiveConnection(const QString& active);
    void activeConnectionStateChanged(const QString& active, NetworkManager::ActiveConnection::State state);
    void addConnection(const QString& connection, const QString& device);
    void addVpnConnection(const QString& connection);
    void addWimaxNsp(const QString& nsp, const QString& device);
    void addWirelessNetwork(const QString& ssid, const QString& device);
    void connectionUpdated(const QString& connection);
#if WITH_MODEMMANAGER_SUPPORT
    void modemPropertiesChanged(const QString& modem);
    void modemSignalQualityChanged(uint signal, const QString& modem);
#endif
    void removeActiveConnection(const QString& active);
    void removeAvailableConnection(const QString& connection, const QString& device);
    void removeConnection(const QString& connection);
    void removeConnectionsByDevice(const QString& device);
    void removeVpnConnections();
    void removeWimaxNsp(const QString& nsp, const QString& device);
    void removeWimaxNsps();
    void removeWirelessNetwork(const QString& ssid,const QString& device);
    void removeWirelessNetworks();
    void wimaxNspSignalChanged(const QString& nsp, int strength);
    void wirelessNetworkSignalChanged(const QString& ssid, int strength);
    void wirelessNetworkApChanged(const QString& ssid, const QString& ap);
private:
    Monitor * m_monitor;
    ModelItems m_items;
    QStringList m_keys;

    void insertItem(ModelItem * item);
    bool removeItem(ModelItem * item);
    bool updateItem(ModelItem * item);
};

#endif // PLASMA_NM_MODEL_H
