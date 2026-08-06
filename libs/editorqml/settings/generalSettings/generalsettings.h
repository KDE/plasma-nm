/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_GENERAL_SETTING_H
#define PLASMA_NM_GENERAL_SETTING_H

#include "plasmanm_editorqml_export.h"
#include "wifisecuritysetting.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <qqmlregistration.h>

class PLASMANM_EDITORQML_EXPORT GeneralSetting : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(bool autoconnect READ autoconnect WRITE setAutoconnect NOTIFY autoconnectChanged)
    Q_PROPERTY(int autoconnectPriority READ autoconnectPriority WRITE setAutoconnectPriority NOTIFY autoconnectPriorityChanged)
    Q_PROPERTY(bool allUsers READ allUsers WRITE setAllUsers NOTIFY allUsersChanged)
    Q_PROPERTY(bool autoconnectVpn READ autoconnectVpn WRITE setAutoconnectVpn NOTIFY autoconnectVpnChanged)
    Q_PROPERTY(QString vpnUuid READ vpnUuid WRITE setVpnUuid NOTIFY vpnUuidChanged)
    Q_PROPERTY(QString firewallZone READ firewallZone WRITE setFirewallZone NOTIFY firewallZoneChanged)
    Q_PROPERTY(int metered READ metered WRITE setMetered NOTIFY meteredChanged)
    Q_PROPERTY(QVariantMap permissions READ permissions WRITE setPermissions NOTIFY permissionsChanged)

    Q_PROPERTY(bool isVpnConnection READ isVpnConnection NOTIFY isVpnConnectionChanged)
    Q_PROPERTY(QVariantList vpnConnections READ vpnConnections NOTIFY vpnConnectionsChanged)
    Q_PROPERTY(QStringList firewallZones READ firewallZones NOTIFY firewallZonesChanged)

public:
    explicit GeneralSetting(QObject *parent = nullptr);

    bool autoconnect() const;
    void setAutoconnect(bool value);
    int autoconnectPriority() const;
    void setAutoconnectPriority(int value);
    bool allUsers() const;
    void setAllUsers(bool value);
    bool autoconnectVpn() const;
    void setAutoconnectVpn(bool value);
    QString vpnUuid() const;
    void setVpnUuid(const QString &uuid);
    QString firewallZone() const;
    void setFirewallZone(const QString &zone);
    int metered() const;
    void setMetered(int value);
    QVariantMap permissions() const;
    void setPermissions(const QVariantMap &permissions);

    bool isVpnConnection() const;
    QVariantList vpnConnections() const;
    QStringList firewallZones() const;

    void loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings);
    void applyTo(const NetworkManager::ConnectionSettings::Ptr &settings, WifiSecuritySetting *wifiSecurity) const;

Q_SIGNALS:
    void autoconnectChanged();
    void autoconnectPriorityChanged();
    void allUsersChanged();
    void autoconnectVpnChanged();
    void vpnUuidChanged();
    void firewallZoneChanged();
    void meteredChanged();
    void permissionsChanged();
    void isVpnConnectionChanged();
    void vpnConnectionsChanged();
    void firewallZonesChanged();
    void validChanged();

private:
    void reloadVpnConnections();
    void reloadFirewallZones();
    void setIsVpnConnection(bool value);

    bool m_autoconnect = true;
    int m_autoconnectPriority = 0;
    bool m_allUsers = false;
    bool m_autoconnectVpn = false;
    QString m_vpnUuid;
    QString m_firewallZone;
    int m_metered = 0;
    QVariantMap m_permissions;

    bool m_isVpnConnection = false;
    QVariantList m_vpnConnections;
    QStringList m_firewallZones;

    // WifiSecuritySetting *m_wifisecurity = nullptr;
};

#endif // PLASMA_NM_GENERAL_SETTING_H
