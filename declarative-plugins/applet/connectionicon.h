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

#ifndef PLASMA_NM_CONNECTION_ICON_H
#define PLASMA_NM_CONNECTION_ICON_H

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/activeconnection.h>
#include <QtNetworkManager/wirelessnetwork.h>

#include <QtModemManager/modemgsmnetworkinterface.h>

class ConnectionIcon : public QObject
{
Q_OBJECT
public:
    ConnectionIcon(QObject* parent = 0);
    virtual ~ConnectionIcon();

public Q_SLOTS:
    void init();

private Q_SLOTS:
    void activeConnectionsChanged();
    void activeConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    void carrierChanged(bool carrier);
    void deviceAdded(const QString & device);
    void deviceRemoved(const QString & device);
    void modemNetworkRemoved();
    void modemSignalChanged(uint signal);
    void setIcons();
    void setWirelessIconForSignalStrenght(int strenght);
    void setIconForModem();

Q_SIGNALS:
    void hideConnectingIndicator();
    void showConnectingIndicator();
    void setConnectionIcon(const QString & icon);
    void setHoverIcon(const QString & icon);
    void setTooltipIcon(const QString & icon);
    void unsetHoverIcon();

private:
    int m_signal;
    NetworkManager::WirelessNetwork::Ptr m_wirelessNetwork;
    ModemManager::ModemGsmNetworkInterface::Ptr m_modemNetwork;

    void setDisconnectedIcon();
    void setModemIcon(const NetworkManager::Device::Ptr & device);
    void setWirelessIcon(const NetworkManager::Device::Ptr & device, const QString & ssid);
};

#endif // PLASMA_NM_CONNECTION_ICON_H
