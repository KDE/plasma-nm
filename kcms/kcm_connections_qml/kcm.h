/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_KCM_QML_H
#define PLASMA_NM_KCM_QML_H

#include "handler.h"
#include "security8021xsetting.h"
#include "wifisecuritysetting.h"

#include <KQuickConfigModule>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <QTimer>

class KCMNetworkManagementQml : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(WifiSecuritySetting *wifiSecurity READ wifiSecurity CONSTANT)
    Q_PROPERTY(Handler *handler READ handler CONSTANT)
    Q_PROPERTY(Security8021xSetting *security8021xSetting READ security8021xSetting CONSTANT)
    Q_PROPERTY(bool useApMode READ useApMode CONSTANT)
    Q_PROPERTY(int connectionType READ connectionType NOTIFY connectionTypeChanged)

public:
    explicit KCMNetworkManagementQml(QObject *parent, const KPluginMetaData &metaData);
    ~KCMNetworkManagementQml() override;

    int connectionType() const;
    Handler *handler() const;
    WifiSecuritySetting *wifiSecurity() const;
    Security8021xSetting *security8021xSetting() const;
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

private Q_SLOTS:
    void onConnectionAdded(const QString &connection);

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
    void resetSelection();

    // ImportResult importVpn();
    // ImportResult importVpnFile(const QString &fileName);
    NetworkManager::ConnectionSettings::Ptr m_pendingNewSettings;
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType = NetworkManager::ConnectionSettings::Unknown;

    QString m_currentConnectionPath;
    QString m_createdConnectionUuid;

    Handler *const m_handler;
    WifiSecuritySetting *const m_wifiSecurity;
    Security8021xSetting *const m_security8021xSetting;

    bool m_useApMode = false;

    QTimer *m_timer = nullptr;
};

#endif
