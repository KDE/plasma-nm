/*
    Copyright 2018 Jan Grulich <jgrulich@redhat.com>

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

#include "connectionsettings.h"
#include "setting.h"

#include "ipv4setting.h"
#include "ipv6setting.h"
#include "wiredsetting.h"
#include "wirelesssetting.h"
#include "wirelesssecuritysetting.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>

#include <KIconLoader>
#include <KNotification>
#include <KLocalizedString>
#include <KUser>

#include <QIcon>

class ConnectionSettingsPrivate
{
public:
    NetworkManager::ConnectionSettings::Ptr connectionSettings;

    QMap<NetworkManager::Setting::SettingType, Setting*> settings;

    int pendingReplies;
    bool valid;
    bool secretsLoaded;
    NMStringMap vpnConnections;
};

ConnectionSettings::ConnectionSettings(QObject *parent)
    : ConnectionSettings(NMVariantMapMap(), parent)
{
}

ConnectionSettings::ConnectionSettings(const NMVariantMapMap &settings, QObject *parent)
    : QObject(parent)
    , d_ptr(new ConnectionSettingsPrivate())
{
    Q_D(ConnectionSettings);

    // Load list of VPN connections
    NetworkManager::Connection::List list = NetworkManager::listConnections();

    for (const NetworkManager::Connection::Ptr & conn : list) {
        NetworkManager::ConnectionSettings::Ptr conSet = conn->settings();
        if (conSet->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
            d->vpnConnections.insert(conSet->uuid(), conSet->id());
        }
    }

    loadConfig(settings);
}

ConnectionSettings::~ConnectionSettings()
{
    delete d_ptr;
}

void ConnectionSettings::loadConfig(const NMVariantMapMap &settings)
{
    Q_D(ConnectionSettings);

    // Clear previous configuration
    d->pendingReplies = 0;
    d->valid = false;
    d->secretsLoaded = false;
    qDeleteAll(d->settings);
    d->settings.clear();

    d->connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(settings));

    // In case of new connection id will be empty
    if (d->connectionSettings->id().isEmpty()) {
        d->connectionSettings->addToPermissions(KUser().loginName(), QString());
    }

    // TODO add rest setting types
//     QString serviceType;
    if (d->connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Wired) {
        addSetting(NetworkManager::Setting::Ipv4, new Ipv4Setting(d->connectionSettings->setting(NetworkManager::Setting::Ipv4), this));
        addSetting(NetworkManager::Setting::Ipv6, new Ipv6Setting(d->connectionSettings->setting(NetworkManager::Setting::Ipv6), this));
        addSetting(NetworkManager::Setting::Wired, new WiredSetting(d->connectionSettings->setting(NetworkManager::Setting::Wired), this));;
    } else if (d->connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Wireless) {
        addSetting(NetworkManager::Setting::Ipv4, new Ipv4Setting(d->connectionSettings->setting(NetworkManager::Setting::Ipv4), this));
        addSetting(NetworkManager::Setting::Ipv6, new Ipv6Setting(d->connectionSettings->setting(NetworkManager::Setting::Ipv6), this));
        addSetting(NetworkManager::Setting::Wireless, new WirelessSetting(d->connectionSettings->setting(NetworkManager::Setting::Wireless), this));
        addSetting(NetworkManager::Setting::WirelessSecurity, new WirelessSecuritySetting(d->connectionSettings->setting(NetworkManager::Setting::WirelessSecurity), this));
//         WifiConnectionWidget *wifiWidget = new WifiConnectionWidget(m_connection->setting(NetworkManager::Setting::Wireless), this);
//         addSettingWidget(wifiWidget, i18n("Wi-Fi"));
//         WifiSecurity *wifiSecurity = new WifiSecurity(m_connection->setting(NetworkManager::Setting::WirelessSecurity),
//                 m_connection->setting(NetworkManager::Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>(),
//                 this);
//         addSettingWidget(wifiSecurity, i18n("Wi-Fi Security"));
//         connect(wifiWidget, static_cast<void (WifiConnectionWidget::*)(const QString &)>(&WifiConnectionWidget::ssidChanged), wifiSecurity, &WifiSecurity::onSsidChanged);
    }
//     else if (type == NetworkManager::ConnectionSettings::Pppoe) { // DSL
//         PppoeWidget *pppoeWidget = new PppoeWidget(m_connection->setting(NetworkManager::Setting::Pppoe), this);
//         addSettingWidget(pppoeWidget, i18n("DSL"));
//         WiredConnectionWidget *wiredWidget = new WiredConnectionWidget(m_connection->setting(NetworkManager::Setting::Wired), this);
//         addSettingWidget(wiredWidget, i18n("Wired"));
//     } else if (type == NetworkManager::ConnectionSettings::Gsm) { // GSM
//         GsmWidget *gsmWidget = new GsmWidget(m_connection->setting(NetworkManager::Setting::Gsm), this);
//         addSettingWidget(gsmWidget, i18n("Mobile Broadband (%1)", m_connection->typeAsString(m_connection->connectionType())));
//     } else if (type == NetworkManager::ConnectionSettings::Cdma) { // CDMA
//         CdmaWidget *cdmaWidget = new CdmaWidget(m_connection->setting(NetworkManager::Setting::Cdma), this);
//         addSettingWidget(cdmaWidget, i18n("Mobile Broadband (%1)", m_connection->typeAsString(m_connection->connectionType())));
//     } else if (type == NetworkManager::ConnectionSettings::Bluetooth) {  // Bluetooth
//         BtWidget *btWidget = new BtWidget(m_connection->setting(NetworkManager::Setting::Bluetooth), this);
//         addSettingWidget(btWidget, i18n("Bluetooth"));
//         NetworkManager::BluetoothSetting::Ptr btSetting = m_connection->setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
//         if (btSetting->profileType() == NetworkManager::BluetoothSetting::Dun) {
//             GsmWidget *gsmWidget = new GsmWidget(m_connection->setting(NetworkManager::Setting::Gsm), this);
//             addSettingWidget(gsmWidget, i18n("GSM"));
//             PPPWidget *pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Setting::Ppp), this);
//             addSettingWidget(pppWidget, i18n("PPP"));
//         }
//     } else if (type == NetworkManager::ConnectionSettings::Infiniband) { // Infiniband
//         InfinibandWidget *infinibandWidget = new InfinibandWidget(m_connection->setting(NetworkManager::Setting::Infiniband), this);
//         addSettingWidget(infinibandWidget, i18n("Infiniband"));
//     } else if (type == NetworkManager::ConnectionSettings::Bond) { // Bond
//         BondWidget *bondWidget = new BondWidget(m_connection->uuid(), m_connection->setting(NetworkManager::Setting::Bond), this);
//         addSettingWidget(bondWidget, i18n("Bond"));
//     } else if (type == NetworkManager::ConnectionSettings::Bridge) { // Bridge
//         BridgeWidget *bridgeWidget = new BridgeWidget(m_connection->uuid(), m_connection->setting(NetworkManager::Setting::Bridge), this);
//         addSettingWidget(bridgeWidget, i18n("Bridge"));
//     } else if (type == NetworkManager::ConnectionSettings::Vlan) { // Vlan
//         VlanWidget *vlanWidget = new VlanWidget(m_connection->setting(NetworkManager::Setting::Vlan), this);
//         addSettingWidget(vlanWidget, i18n("Vlan"));
//     } else if (type == NetworkManager::ConnectionSettings::Team) { // Team
//         TeamWidget *teamWidget = new TeamWidget(m_connection->uuid(), m_connection->setting(NetworkManager::Setting::Team), this);
//         addSettingWidget(teamWidget, i18n("Team"));
//     } else if (type == NetworkManager::ConnectionSettings::Vpn) { // VPN
//         QString error;
//         VpnUiPlugin *vpnPlugin = 0;
//         NetworkManager::VpnSetting::Ptr vpnSetting =
//             m_connection->setting(NetworkManager::Setting::Vpn).staticCast<NetworkManager::VpnSetting>();
//         if (!vpnSetting) {
//             qCWarning(PLASMA_NM) << "Missing VPN setting!";
//         } else {
//             serviceType = vpnSetting->serviceType();
//             vpnPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QString::fromLatin1("PlasmaNetworkManagement/VpnUiPlugin"),
//                         QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(serviceType),
//                         this, QVariantList(), &error);
//             if (vpnPlugin && error.isEmpty()) {
//                 const QString shortName = serviceType.section('.', -1);
//                 SettingWidget *vpnWidget = vpnPlugin->widget(vpnSetting, this);
//                 addSettingWidget(vpnWidget, i18n("VPN (%1)", shortName));
//             } else {
//                 qCWarning(PLASMA_NM) << error << ", serviceType == " << serviceType;
//             }
//         }
//     }
//
//     // PPP widget
//     if (type == NetworkManager::ConnectionSettings::Pppoe || type == NetworkManager::ConnectionSettings::Cdma || type == NetworkManager::ConnectionSettings::Gsm) {
//         PPPWidget *pppWidget = new PPPWidget(m_connection->setting(NetworkManager::Setting::Ppp), this);
//         addSettingWidget(pppWidget, i18n("PPP"));
//     }
//
//     // IPv4 widget
//     if (!m_connection->isSlave()) {
//         IPv4Widget *ipv4Widget = new IPv4Widget(m_connection->setting(NetworkManager::Setting::Ipv4), this);
//         addSettingWidget(ipv4Widget, i18n("IPv4"));
//     }
//
//     // IPv6 widget
//     if ((type == NetworkManager::ConnectionSettings::Wired
//             || type == NetworkManager::ConnectionSettings::Wireless
//             || type == NetworkManager::ConnectionSettings::Infiniband
// #if NM_CHECK_VERSION(0, 9, 10)
//             || type == NetworkManager::ConnectionSettings::Team
// #endif
//
// #if NM_CHECK_VERSION(1, 0, 0)
//             || type == NetworkManager::ConnectionSettings::Cdma
//             || type == NetworkManager::ConnectionSettings::Gsm
// #endif
//             || type == NetworkManager::ConnectionSettings::Bond
//             || type == NetworkManager::ConnectionSettings::Bridge
//             || type == NetworkManager::ConnectionSettings::Vlan
//             || (type == NetworkManager::ConnectionSettings::Vpn && serviceType == QLatin1String("org.freedesktop.NetworkManager.openvpn"))) && !m_connection->isSlave()) {
//         IPv6Widget *ipv6Widget = new IPv6Widget(m_connection->setting(NetworkManager::Setting::Ipv6), this);
//         addSettingWidget(ipv6Widget, i18n("IPv6"));
//     }

    bool valid = true;
    for (auto it = d->settings.constBegin(); it != d->settings.constEnd(); ++it) {
        valid = valid && it.value()->isValid();
        connect(it.value(), &Setting::validityChanged, this, &ConnectionSettings::onValidityChanged);
    }

    d->valid = valid;
    Q_EMIT validityChanged(valid);

    // If the connection is not empty (not new) we want to load its secrets
//     if (!emptyConnection) {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(d->connectionSettings->uuid());
        if (connection) {
            QStringList requiredSecrets;
            QString settingName;
            QVariantMap setting;
            QDBusPendingReply<NMVariantMapMap> reply;

//             if (m_connection->connectionType() == NetworkManager::ConnectionSettings::Adsl) {
//                 NetworkManager::AdslSetting::Ptr adslSetting = connection->settings()->setting(NetworkManager::Setting::Adsl).staticCast<NetworkManager::AdslSetting>();
//                 if (adslSetting && !adslSetting->needSecrets().isEmpty()) {
//                     requiredSecrets = adslSetting->needSecrets();
//                     setting = adslSetting->toMap();
//                     settingName = QLatin1String("adsl");
//                 }
//             } else if (m_connection->connectionType() == NetworkManager::ConnectionSettings::Bluetooth) {
//                 NetworkManager::GsmSetting::Ptr gsmSetting = connection->settings()->setting(NetworkManager::Setting::Gsm).staticCast<NetworkManager::GsmSetting>();
//                 if (gsmSetting && !gsmSetting->needSecrets().isEmpty()) {
//                     requiredSecrets = gsmSetting->needSecrets();
//                     setting = gsmSetting->toMap();
//                     settingName = QLatin1String("gsm");
//                 }
//             } else if (m_connection->connectionType() == NetworkManager::ConnectionSettings::Cdma) {
//                 NetworkManager::CdmaSetting::Ptr cdmaSetting = connection->settings()->setting(NetworkManager::Setting::Cdma).staticCast<NetworkManager::CdmaSetting>();
//                 if (cdmaSetting && !cdmaSetting->needSecrets().isEmpty()) {
//                     requiredSecrets = cdmaSetting->needSecrets();
//                     setting = cdmaSetting->toMap();
//                     settingName = QLatin1String("cdma");
//                 }
//             } else if (m_connection->connectionType() == NetworkManager::ConnectionSettings::Gsm) {
//                 NetworkManager::GsmSetting::Ptr gsmSetting = connection->settings()->setting(NetworkManager::Setting::Gsm).staticCast<NetworkManager::GsmSetting>();
//                 if (gsmSetting && !gsmSetting->needSecrets().isEmpty()) {
//                     requiredSecrets = gsmSetting->needSecrets();
//                     setting = gsmSetting->toMap();
//                     settingName = QLatin1String("gsm");
//                 }
//             } else if (m_connection->connectionType() == NetworkManager::ConnectionSettings::Pppoe) {
//                 NetworkManager::PppoeSetting::Ptr pppoeSetting = connection->settings()->setting(NetworkManager::Setting::Pppoe).staticCast<NetworkManager::PppoeSetting>();
//                 if (pppoeSetting && !pppoeSetting->needSecrets().isEmpty()) {
//                     requiredSecrets = pppoeSetting->needSecrets();
//                     setting = pppoeSetting->toMap();
//                     settingName = QLatin1String("pppoe");
//                 }
//             } else if (m_connection->connectionType() == NetworkManager::ConnectionSettings::Wired) {
//                 NetworkManager::Security8021xSetting::Ptr securitySetting = connection->settings()->setting(NetworkManager::Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>();
//                 if (securitySetting && !securitySetting->needSecrets().isEmpty()) {
//                     requiredSecrets = securitySetting->needSecrets();
//                     setting = securitySetting->toMap();
//                     settingName = QLatin1String("802-1x");
//                 }
//             } else
            if (d->connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Wireless) {
                NetworkManager::WirelessSecuritySetting::Ptr wifiSecuritySetting = d->connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();
                if (wifiSecuritySetting &&
                        (wifiSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::WpaEap ||
                        (wifiSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::WirelessSecuritySetting::Ieee8021x &&
                         wifiSecuritySetting->authAlg() != NetworkManager::WirelessSecuritySetting::Leap))) {
                    // TODO
//                     NetworkManager::Security8021xSetting::Ptr securitySetting = connection->settings()->setting(NetworkManager::Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>();
//                     if (securitySetting && !securitySetting->needSecrets().isEmpty()) {
//                         requiredSecrets = securitySetting->needSecrets();
//                         setting = securitySetting->toMap();
//                         settingName = QLatin1String("802-1x");
//
//                         if (requiredSecrets.contains(NM_SETTING_802_1X_PASSWORD_RAW)) {
//                             requiredSecrets.removeAll(NM_SETTING_802_1X_PASSWORD_RAW);
//                         }
//                     }
                } else {
                    if (!wifiSecuritySetting->needSecrets().isEmpty()) {
                        requiredSecrets = wifiSecuritySetting->needSecrets();
                        setting = wifiSecuritySetting->toMap();
                        settingName = QLatin1String("802-11-wireless-security");
                    }
                }
            } else if (d->connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
                settingName = QLatin1String("vpn");
            }

            if (!requiredSecrets.isEmpty() || d->connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
                bool requestSecrets = false;
                if (d->connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
                    requestSecrets = true;
                } else {
                    for (const QString &secret : requiredSecrets) {
                        if (setting.contains(secret + QLatin1String("-flags"))) {
                            NetworkManager::Setting::SecretFlagType secretFlag = (NetworkManager::Setting::SecretFlagType)setting.value(secret + QLatin1String("-flags")).toInt();
                            if (secretFlag == NetworkManager::Setting::None || secretFlag == NetworkManager::Setting::AgentOwned) {
                                requestSecrets = true;
                            }
                        } else {
                            requestSecrets = true;
                        }
                    }
                }

                if (requestSecrets) {
                    d->pendingReplies++;
                    reply = connection->secrets(settingName);
                    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
                    watcher->setProperty("connection", connection->name());
                    watcher->setProperty("settingName", settingName);
                    connect(watcher, &QDBusPendingCallWatcher::finished, this, &ConnectionSettings::onReplyFinished);
                    d->valid = false;
                    Q_EMIT validityChanged(false);
                    return;
                }
            }
        }
//     }

    // We should be now fully initialized as we don't wait for secrets
    if (d->pendingReplies == 0) {
        d->secretsLoaded = true;
    }
}

NMVariantMapMap ConnectionSettings::settingMap() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->toMap();
}

QString ConnectionSettings::id() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->id();
}

void ConnectionSettings::setId(const QString &id)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setId(id);

    Q_EMIT settingChanged();
}

QString ConnectionSettings::uuid() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->uuid();
}

void ConnectionSettings::setUuid(const QString &uuid)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setUuid(uuid);
}

uint ConnectionSettings::connectionType() const
{
    Q_D(const ConnectionSettings);

    return (uint)d->connectionSettings->connectionType();
}

bool ConnectionSettings::autoconnect() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->autoconnect();
}

void ConnectionSettings::setAutoconnect(bool autoconnect)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setAutoconnect(autoconnect);

    Q_EMIT settingChanged();
}

QStringList ConnectionSettings::permissions() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->permissions().keys();
}

void ConnectionSettings::setPermissions(const QStringList &permissions)
{
    Q_D(ConnectionSettings);

    if (permissions.isEmpty()) {
        d->connectionSettings->setPermissions({});
    } else {
        for (const QString &user : permissions) {
            if (user == QLatin1String("replace_current_user")) {
                d->connectionSettings->addToPermissions(KUser().loginName(), QString());
            } else {
                d->connectionSettings->addToPermissions(user, QString());
            }
        }
    }

    Q_EMIT settingChanged();
}

QString ConnectionSettings::secondaryConnection() const
{
    Q_D(const ConnectionSettings);

    if (!d->connectionSettings->secondaries().isEmpty()) {
        return d->vpnConnections.value(d->connectionSettings->secondaries().first());
    }

    return nullptr;
}

void ConnectionSettings::setSecondaryConnection(const QString &secondaryConnection)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setSecondaries({d->vpnConnections.key(secondaryConnection)});

    Q_EMIT settingChanged();
}

QString ConnectionSettings::zone() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->zone();
}

void ConnectionSettings::setZone(const QString &zone)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setZone(zone);

    Q_EMIT settingChanged();
}

int ConnectionSettings::priority() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->autoconnectPriority();
}

void ConnectionSettings::setPriority(int priority)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setAutoconnectPriority(priority);

    Q_EMIT settingChanged();
}

int ConnectionSettings::metered() const
{
    Q_D(const ConnectionSettings);

    return d->connectionSettings->metered();
}

void ConnectionSettings::setMetered(int metered)
{
    Q_D(ConnectionSettings);

    d->connectionSettings->setMetered(static_cast<NetworkManager::ConnectionSettings::Metered>(metered));

    Q_EMIT settingChanged();
}

QObject * ConnectionSettings::setting(uint type) const
{
    Q_D(const ConnectionSettings);

    return d->settings.value((NetworkManager::Setting::SettingType) type);
}

QList<NetworkManager::Setting::SettingType> ConnectionSettings::settingTypes() const
{
    Q_D(const ConnectionSettings);

    return d->settings.keys();
}

void ConnectionSettings::addSetting(NetworkManager::Setting::SettingType type, Setting *setting)
{
    Q_D(ConnectionSettings);

    d->settings.insert(type, setting);
}

bool ConnectionSettings::isInitialized() const
{
    Q_D(const ConnectionSettings);

    return d->secretsLoaded;
}

bool ConnectionSettings::isValid() const
{
    Q_D(const ConnectionSettings);

    return d->valid;
}

void ConnectionSettings::onReplyFinished(QDBusPendingCallWatcher *watcher)
{
    Q_D(ConnectionSettings);

    QDBusPendingReply<NMVariantMapMap> reply = *watcher;
    const QString settingName = watcher->property("settingName").toString();
    if (reply.isValid()) {
        NMVariantMapMap secrets = reply.argumentAt<0>();
        for (const QString &key : secrets.keys()) {
            if (key == settingName) {
                NetworkManager::Setting::Ptr setting = d->connectionSettings->setting(NetworkManager::Setting::typeFromString(key));
                if (setting) {
                    setting->secretsFromMap(secrets.value(key));
                    for (auto it = d->settings.constBegin(); it != d->settings.constEnd(); ++it) {
                        if (NetworkManager::Setting::typeFromString(key) == it.key()) {
                            it.value()->loadSecrets(setting);
                        }
                    }
                }
            }
        }
    } else {
        KNotification *notification = new KNotification("FailedToGetSecrets", KNotification::CloseOnTimeout);
        notification->setComponentName("networkmanagement");
        notification->setTitle(i18n("Failed to get secrets for %1", watcher->property("connection").toString()));
        notification->setText(reply.error().message());
        notification->setPixmap(QIcon::fromTheme("dialog-warning").pixmap(KIconLoader::SizeHuge));
        notification->sendEvent();
    }

    watcher->deleteLater();
    onValidityChanged(true);

    // We should be now fully with secrets
    d->pendingReplies--;
    d->secretsLoaded = true;
}

void ConnectionSettings::onValidityChanged(bool valid)
{
    Q_D(ConnectionSettings);

    if (!valid) {
        d->valid = false;
        Q_EMIT validityChanged(false);
        return;
    } else {
        for (auto it = d->settings.constBegin(); it != d->settings.constEnd(); ++it) {
            if (!it.value()->isValid()) {
                d->valid = false;
                Q_EMIT validityChanged(false);
                return;
            }
        }
    }

    d->valid = true;
    Q_EMIT validityChanged(true);
}
