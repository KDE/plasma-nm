/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_NETWORK_STATUS_H
#define PLASMA_NM_NETWORK_STATUS_H

#include <QObject>

#include <NetworkManagerQt/Manager>

class NetworkStatus : public QObject
{
/**
 * Returns a formated list of active connections or NM status when there is no active connection
 */
Q_PROPERTY(QString activeConnections READ activeConnections NOTIFY activeConnectionsChanged)
/**
 * Returns the current status of NetworkManager
 */
Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY networkStatusChanged)
Q_OBJECT
public:
    enum SortedConnectionType {
        Wired,
        Wireless,
        Gsm,
        Cdma,
        Pppoe,
        Adsl,
        Infiniband,
        OLPCMesh,
        Bluetooth,
        Wireguard,
        Vpn,
        Other,
    };

    static SortedConnectionType connectionTypeToSortedType(NetworkManager::ConnectionSettings::ConnectionType type);

    explicit NetworkStatus(QObject* parent = nullptr);
    ~NetworkStatus() override;

    QString activeConnections() const;
    QString networkStatus() const;

private Q_SLOTS:
    void activeConnectionsChanged();
    void defaultChanged();
    void statusChanged(NetworkManager::Status status);
    void changeActiveConnections();

Q_SIGNALS:
    void activeConnectionsChanged(const QString & activeConnections);
    void networkStatusChanged(const QString & status);

private:
    QString m_activeConnections;
    QString m_networkStatus;

    QString checkUnknownReason() const;
};

#endif // PLAMA_NM_NETWORK_STATUS_H
