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

#include "config.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/WirelessNetwork>
#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
#include <ModemManagerQt/modem.h>
#else
#include <ModemManagerQt/modemgsmnetworkinterface.h>
#endif
#endif

class ConnectionIcon : public QObject
{
/**
 * Returns true when NetworkManager is trying to activate some connection
 */
Q_PROPERTY(bool connecting READ connecting NOTIFY connectingChanged)
/**
 * Returns an elementId of SVG icon in plasma-nm icon set for the main active connection
 */
Q_PROPERTY(QString connectionSvgIcon READ connectionSvgIcon NOTIFY connectionSvgIconChanged)
/**
 * Returns a pixmap icon name from Oxygen icon set for the main active connection
 */
Q_PROPERTY(QString connectionPixmapIcon READ connectionPixmapIcon NOTIFY connectionPixmapIconChanged)
/**
 * Current indicators:
 * 1) -locked - indicates active VPN connection
 * 2) dialog-cancel - indicates no active connection // TODO
 * 3) dialog-error - indicates that NetworkManager is not active // TODO
 */
Q_PROPERTY(QString connectionIndicatorIcon READ connectionIndicatorIcon NOTIFY connectionIndicatorIconChanged)
Q_OBJECT
public:
    explicit ConnectionIcon(QObject* parent = 0);
    virtual ~ConnectionIcon();

    bool connecting() const;
    QString connectionSvgIcon() const;
    QString connectionIndicatorIcon() const;
    QString connectionPixmapIcon() const;

private Q_SLOTS:
    void activeConnectionsChanged();
    void activeConnectionStateChanged(NetworkManager::ActiveConnection::State state);
    void activeConnectionDestroyed();
    void carrierChanged(bool carrier);
    void deviceAdded(const QString & device);
    void deviceRemoved(const QString & device);
    void setIcons();
    void setWirelessIconForSignalStrength(int strength);
#if WITH_MODEMMANAGER_SUPPORT
    void modemNetworkRemoved();
    void modemSignalChanged(uint signal);
    void setIconForModem();
#endif
Q_SIGNALS:
    void connectingChanged(bool connecting);
    void connectionSvgIconChanged(const QString & icon);
    void connectionIndicatorIconChanged(const QString & icon);
    void connectionPixmapIconChanged(const QString & icon);

private:
    uint m_signal;
    NetworkManager::WirelessNetwork::Ptr m_wirelessNetwork;

    bool m_connecting;
    QString m_connectionSvgIcon;
    QString m_connectionIndicatorIcon;
    QString m_connectionPixmapIcon;

    void setDisconnectedIcon();
    void setWirelessIcon(const NetworkManager::Device::Ptr & device, const QString & ssid);
#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
    ModemManager::Modem::Ptr m_modemNetwork;
#else
    ModemManager::ModemGsmNetworkInterface::Ptr m_modemNetwork;
#endif
    void setModemIcon(const NetworkManager::Device::Ptr & device);
#endif
};

#endif // PLASMA_NM_CONNECTION_ICON_H
