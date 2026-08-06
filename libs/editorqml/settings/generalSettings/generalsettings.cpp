/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
      SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "generalsettings.h"
#include "wifisecuritysetting.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>

#include <KUser>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <qtmetamacros.h>

GeneralSetting::GeneralSetting(QObject *parent)
    : QObject(parent)
{
}

void GeneralSetting::reloadVpnConnections()
{
    m_vpnConnections.clear();

    const NetworkManager::Connection::List list = NetworkManager::listConnections();

    for (const NetworkManager::Connection::Ptr &conn : list) {
        const NetworkManager::ConnectionSettings::Ptr conSet = conn->settings();

        if (conSet->connectionType() == NetworkManager::ConnectionSettings::Vpn || conSet->connectionType() == NetworkManager::ConnectionSettings::WireGuard) {
            m_vpnConnections.append(QVariantMap{{QStringLiteral("text"), conSet->id()}, {QStringLiteral("uuid"), conSet->uuid()}});
        }
    }

    Q_EMIT vpnConnectionsChanged();
}

void GeneralSetting::reloadFirewallZones()
{
    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.fedoraproject.FirewallD1"),
                                                      QStringLiteral("/org/fedoraproject/FirewallD1"),
                                                      QStringLiteral("org.fedoraproject.FirewallD1.zone"),
                                                      QStringLiteral("getZones"));
    QDBusPendingReply<QStringList> reply = QDBusConnection::systemBus().asyncCall(msg);
    reply.waitForFinished();
    m_firewallZones = reply.isValid() ? reply.value() : QStringList{};
    Q_EMIT firewallZonesChanged();
}

void GeneralSetting::loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings)
{
    if (!settings) {
        return;
    }

    setIsVpnConnection(settings->connectionType() == NetworkManager::ConnectionSettings::Vpn);

    QVariantMap perms;
    const QHash<QString, QString> nmPerms = settings->permissions();
    for (auto it = nmPerms.constBegin(); it != nmPerms.constEnd(); it++) {
        perms.insert(it.key(), it.value());
    }
    setPermissions(perms);

    setFirewallZone(settings->zone());

    reloadVpnConnections();

    const QStringList secondaries = settings->secondaries();
    QString foundVpn;
    for (const QVariant &entry : std::as_const(m_vpnConnections)) {
        const QString uuid = entry.toMap().value(QStringLiteral("uuid")).toString();
        if (!uuid.isEmpty() && secondaries.contains(uuid)) {
            foundVpn = uuid;
            break;
        }
    }
    setAutoconnectVpn(!foundVpn.isEmpty());
    setVpnUuid(foundVpn);

    setAutoconnect(settings->autoconnect());
    setAutoconnectPriority(settings->autoconnectPriority());
    setMetered(static_cast<int>(settings->metered()));

    reloadFirewallZones();

    Q_EMIT validChanged();
}

void GeneralSetting::applyTo(const NetworkManager::ConnectionSettings::Ptr &settings, WifiSecuritySetting *wifiSecurity) const
{
    if (!settings) {
        return;
    }

    settings->setAutoconnect(m_autoconnect);

    if (m_allUsers) {
        settings->setPermissions(QHash<QString, QString>());
        if (wifiSecurity) {
            wifiSecurity->setStoreSecretsSystemWide(m_allUsers);
        }
    } else if (m_permissions.isEmpty()) {
        settings->addToPermissions(KUser().loginName(), QString());
    } else {
        QHash<QString, QString> nmPerms;
        for (auto it = m_permissions.constBegin(); it != m_permissions.constEnd(); ++it) {
            nmPerms.insert(it.key(), it.value().toString());
        }
        settings->setPermissions(nmPerms);
    }

    settings->setSecondaries(m_autoconnectVpn && !m_vpnUuid.isEmpty() ? QStringList{m_vpnUuid} : QStringList{});

    if (!m_firewallZone.isEmpty()) {
        settings->setZone(m_firewallZone);
    }

    if (m_autoconnect) {
        settings->setAutoconnectPriority(m_autoconnectPriority);
    }

    settings->setMetered(static_cast<NetworkManager::ConnectionSettings::Metered>(m_metered));
}

bool GeneralSetting::autoconnect() const
{
    return m_autoconnect;
}
void GeneralSetting::setAutoconnect(bool v)
{
    if (m_autoconnect != v) {
        m_autoconnect = v;
        Q_EMIT autoconnectChanged();
        Q_EMIT validChanged();
    }
}

int GeneralSetting::autoconnectPriority() const
{
    return m_autoconnectPriority;
}
void GeneralSetting::setAutoconnectPriority(int v)
{
    if (m_autoconnectPriority != v) {
        m_autoconnectPriority = v;
        Q_EMIT autoconnectPriorityChanged();
        Q_EMIT validChanged();
    }
}

bool GeneralSetting::allUsers() const
{
    return m_allUsers;
}
void GeneralSetting::setAllUsers(bool v)
{
    if (m_allUsers != v) {
        m_allUsers = v;
        Q_EMIT allUsersChanged();
        Q_EMIT validChanged();
    }
}

bool GeneralSetting::autoconnectVpn() const
{
    return m_autoconnectVpn;
}
void GeneralSetting::setAutoconnectVpn(bool v)
{
    if (m_autoconnectVpn != v) {
        m_autoconnectVpn = v;
        Q_EMIT autoconnectVpnChanged();
        Q_EMIT validChanged();
    }
}

QString GeneralSetting::vpnUuid() const
{
    return m_vpnUuid;
}
void GeneralSetting::setVpnUuid(const QString &v)
{
    if (m_vpnUuid != v) {
        m_vpnUuid = v;
        Q_EMIT vpnUuidChanged();
        Q_EMIT validChanged();
    }
}

QString GeneralSetting::firewallZone() const
{
    return m_firewallZone;
}
void GeneralSetting::setFirewallZone(const QString &v)
{
    if (m_firewallZone != v) {
        m_firewallZone = v;
        Q_EMIT firewallZoneChanged();
        Q_EMIT validChanged();
    }
}

int GeneralSetting::metered() const
{
    return m_metered;
}
void GeneralSetting::setMetered(int v)
{
    if (m_metered != v) {
        m_metered = v;
        Q_EMIT meteredChanged();
        Q_EMIT validChanged();
    }
}

QVariantMap GeneralSetting::permissions() const
{
    return m_permissions;
}
void GeneralSetting::setPermissions(const QVariantMap &p)
{
    m_permissions = p;
    setAllUsers(p.isEmpty());
    Q_EMIT permissionsChanged();
    Q_EMIT validChanged();
}

bool GeneralSetting::isVpnConnection() const
{
    return m_isVpnConnection;
}
void GeneralSetting::setIsVpnConnection(bool v)
{
    if (m_isVpnConnection != v) {
        m_isVpnConnection = v;
        Q_EMIT isVpnConnectionChanged();
        Q_EMIT validChanged();
    }
}

QVariantList GeneralSetting::vpnConnections() const
{
    return m_vpnConnections;
}
QStringList GeneralSetting::firewallZones() const
{
    return m_firewallZones;
}

// void GeneralSetting::setWifiSecurity(WifiSecuritySetting *wifiSecurity)
//{
//     m_wifisecurity = wifiSecurity;
//     qDebug() << m_wifisecurity;
// }

#include "moc_generalsettings.cpp"
