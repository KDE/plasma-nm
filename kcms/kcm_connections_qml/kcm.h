/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_KCM_QML_H
#define PLASMA_NM_KCM_QML_H

#include "connectionstatus.h"
#include "enums.h"
#include "generalsettings.h"
#include "handler.h"
#include "ipv4settings.h"
#include "ipv6settings.h"
#include "security8021xsetting.h"
#include "wifisecuritysetting.h"
#include "wifisetting.h"
#include "wiredsettings.h"

#include <KQuickConfigModule>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <QHash>
#include <QTimer>

class KCMNetworkManagementQml : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(WifiSecuritySetting *wifiSecurity READ wifiSecurity CONSTANT)
    Q_PROPERTY(Handler *handler READ handler CONSTANT)
    Q_PROPERTY(Security8021xSetting *security8021xSetting READ security8021xSetting CONSTANT)
    Q_PROPERTY(bool useApMode READ useApMode CONSTANT)
    Q_PROPERTY(ConnectionStatus *connectionStatus READ connectionStatus CONSTANT)
    Q_PROPERTY(Enums::ConnectionType connectionType READ connectionType NOTIFY connectionTypeChanged)
    Q_PROPERTY(GeneralSetting *generalSettings READ generalSettings CONSTANT)
    Q_PROPERTY(WifiSetting *wifiSetting READ wifiSetting CONSTANT)
    Q_PROPERTY(WiredSetting *wiredSetting READ wiredSetting CONSTANT)
    Q_PROPERTY(bool wiredSecurityEnabled READ wiredSecurityEnabled WRITE setWiredSecurityEnabled NOTIFY wiredSecurityEnabledChanged)
    Q_PROPERTY(IPv4Settings *ipv4Settings READ ipv4Settings CONSTANT)
    Q_PROPERTY(IPv6Settings *ipv6Settings READ ipv6Settings CONSTANT)

public:
    explicit KCMNetworkManagementQml(QObject *parent, const KPluginMetaData &metaData);
    ~KCMNetworkManagementQml() override;

    Enums::ConnectionType connectionType() const;
    Handler *handler() const;
    WifiSecuritySetting *wifiSecurity() const;
    Security8021xSetting *security8021xSetting() const;
    ConnectionStatus *connectionStatus() const;
    GeneralSetting *generalSettings() const;
    WifiSetting *wifiSetting() const;
    WiredSetting *wiredSetting() const;
    bool wiredSecurityEnabled() const;
    void setWiredSecurityEnabled(bool enabled);
    IPv4Settings *ipv4Settings() const;
    IPv6Settings *ipv6Settings() const;
    bool useApMode() const;

    Q_INVOKABLE void onRequestCreateConnection(int connectionType, const QString &vpnType, const QString &specificType, bool shared);
    Q_INVOKABLE void onSelectedConnectionChanged(const QString &connectionPath);
    // Q_INVOKABLE void onRequestExportConnection(const QString &connectionPath);
    Q_INVOKABLE void onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath);
    Q_INVOKABLE void onRequestDuplicateConnection(const QString &connectionPath);

public Q_SLOTS:
    void defaults() override;
    void load() override;
    void save() override;

Q_SIGNALS:
    void connectionLoaded(const QString &path);
    void saveSucceeded();
    void saveFailed(const QString &errorMessage);
    void kcmChangedStateChanged(bool changed);
    void connectionTypeChanged();
    void wiredSecurityEnabledChanged();

private Q_SLOTS:
    void onConnectionAdded(const QString &connection);
    void onSecretsArrived(QDBusPendingCallWatcher *watcher);

private:
    struct ImportResult {
        bool success;
        QString errorMessage;

        static ImportResult pass();
        static ImportResult fail(const QString &message);
    };

private:
    void applyWirelessSetting(NMVariantMapMap &map);
    void addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void kcmChanged(bool kcmChanged);
    void loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void requestSecrets(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void resetSelection();
    void applyTypeSettings(NMVariantMapMap &map, NetworkManager::ConnectionSettings::ConnectionType type);

    // ImportResult importVpn();
    // ImportResult importVpnFile(const QString &fileName);
    NetworkManager::ConnectionSettings::Ptr m_pendingNewSettings;
    NetworkManager::ConnectionSettings::Ptr m_currentSettings;

    QHash<QString, std::function<void(const NetworkManager::Setting::Ptr &)>> m_secretsHandlers;
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType = NetworkManager::ConnectionSettings::Unknown;

    QString m_currentConnectionPath;
    QString m_createdConnectionUuid;

    Handler *const m_handler;
    WifiSecuritySetting *const m_wifiSecurity;
    Security8021xSetting *const m_security8021xSetting;
    ConnectionStatus *const m_connectionStatus;
    GeneralSetting *const m_generalSettings;
    WifiSetting *const m_wifiSetting;
    WiredSetting *const m_wiredSetting;
    IPv4Settings *const m_ipv4Settings;
    IPv6Settings *const m_ipv6Settings;

    bool m_useApMode = false;
    bool m_wiredSecurityEnabled = false;

    QTimer *m_timer = nullptr;
};

#endif
