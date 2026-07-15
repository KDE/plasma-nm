/*
      SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kcm.h"
#include "plasma_nm_kcm_qml.h"
#include "security8021xsetting.h"
#include "uiutils.h"
#include "wifisecuritysetting.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>

#include <kquickconfigmodule.h>
#include <networkmanagerqt/connectionsettings.h>

K_PLUGIN_CLASS_WITH_JSON(KCMNetworkManagementQml, "kcm_networkmanagement_qml.json")

KCMNetworkManagementQml::KCMNetworkManagementQml(QObject *parent, const KPluginMetaData &data)
    : KQuickConfigModule(parent, data)
    , m_handler(new Handler(this))
    , m_wifiSecurity(new WifiSecuritySetting(this))
    , m_security8021xSetting(new Security8021xSetting(this))
    , m_timer(new QTimer(this))
{
    // Check if we can use AP mode to identify security type
    bool foundInactive = false;
    NetworkManager::WirelessDevice::Ptr wifiDev;

    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
            if (wifiDev) {
                if (!wifiDev->isActive()) {
                    foundInactive = true;
                } else {
                    // Prefer previous device if it was inactive
                    if (foundInactive) {
                        break;
                    }
                }

                if (wifiDev->wirelessCapabilities().testFlag(NetworkManager::WirelessDevice::ApCap)) {
                    m_useApMode = true;
                }

                // We prefer inactive wireless card with AP capabilities
                if (foundInactive && m_useApMode) {
                    break;
                }
            }
        }
    }

    NetworkManager::Connection::Ptr selectedConnection;

    // Pre-select the currently active primary connection
    if (auto active = NetworkManager::primaryConnection(); active && active->isValid()) {
        selectedConnection = active->connection();
        if (selectedConnection) {
            qCDebug(PLASMA_NM_KCM_QML_LOG) << "Selecting active connection:" << selectedConnection->uuid();
        }
    }

    // Select the very first connection as a fallback
    if (!selectedConnection || !selectedConnection->isValid()) {
        NetworkManager::Connection::List connectionList = NetworkManager::listConnections();
        std::sort(connectionList.begin(), connectionList.end(), [](const NetworkManager::Connection::Ptr &left, const NetworkManager::Connection::Ptr &right) {
            const QString leftName = left->settings()->id();
            const UiUtils::SortedConnectionType leftType = UiUtils::connectionTypeToSortedType(left->settings()->connectionType());
            const QDateTime leftDate = left->settings()->timestamp();

            const QString rightName = right->settings()->id();
            const UiUtils::SortedConnectionType rightType = UiUtils::connectionTypeToSortedType(right->settings()->connectionType());
            const QDateTime rightDate = right->settings()->timestamp();

            if (leftType < rightType) {
                return true;
            } else if (leftType > rightType) {
                return false;
            }

            if (leftDate > rightDate) {
                return true;
            } else if (leftDate < rightDate) {
                return false;
            }

            if (QString::localeAwareCompare(leftName, rightName) > 0) {
                return true;
            } else {
                return false;
            }
        });

        for (const NetworkManager::Connection::Ptr &connection : connectionList) {
            const NetworkManager::ConnectionSettings::ConnectionType type = connection->settings()->connectionType();
            if (UiUtils::isConnectionTypeSupported(type)) {
                selectedConnection = connection;
                qCDebug(PLASMA_NM_KCM_QML_LOG) << "Selecting first connection:" << connection->uuid();
                break;
            }
        }
    }

    if (selectedConnection && selectedConnection->isValid()) {
        onSelectedConnectionChanged(selectedConnection->path());
    } else {
        qCDebug(PLASMA_NM_KCM_QML_LOG) << "Cannot preselect a connection";
    }

    connect(NetworkManager::settingsNotifier(),
            &NetworkManager::SettingsNotifier::connectionAdded,
            this,
            &KCMNetworkManagementQml::onConnectionAdded,
            Qt::UniqueConnection);

    // Initialize first scan and then scan every 15 seconds
    m_handler->requestScan();
    m_timer->setInterval(15000);
    connect(m_timer, &QTimer::timeout, this, [this]() {
        m_handler->requestScan();
    });
    m_timer->start();
}

KCMNetworkManagementQml::~KCMNetworkManagementQml() = default;

WifiSecuritySetting *KCMNetworkManagementQml::wifiSecurity() const
{
    return m_wifiSecurity;
}

Handler *KCMNetworkManagementQml::handler() const
{
    return m_handler;
}

Security8021xSetting *KCMNetworkManagementQml::security8021xSetting() const
{
    return m_security8021xSetting;
}

bool KCMNetworkManagementQml::useApMode() const
{
    return m_useApMode;
}
void KCMNetworkManagementQml::defaults()
{
    KQuickConfigModule::defaults();
}

int KCMNetworkManagementQml::connectionType() const
{
    return m_connectionType;
}
void KCMNetworkManagementQml::load()
{
    if (m_currentConnectionPath.isEmpty()) {
        return;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        loadConnectionSettings(connection->settings());
    }

    KQuickConfigModule::load();
}

void KCMNetworkManagementQml::applyWirelessSetting(NMVariantMapMap &map)
{
    if (!map.contains(QStringLiteral("802-11-wireless"))) {
        return;
    }

    NetworkManager::ConnectionSettings settings(NetworkManager::ConnectionSettings::Wireless);
    settings.fromMap(map);

    auto securitySetting = settings.setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();
    auto wirelessSetting = settings.setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();

    if (securitySetting && wirelessSetting) {
        if (securitySetting->keyMgmt() != NetworkManager::WirelessSecuritySetting::Unknown) {
            wirelessSetting->setSecurity(QStringLiteral("802-11-wireless-security"));
        }

        if (securitySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::SAE && wirelessSetting->mode() == NetworkManager::WirelessSetting::Adhoc) {
            // Ad-Hoc settings as specified by the supplicant
            // Proto
            QList<NetworkManager::WirelessSecuritySetting::WpaProtocolVersion> protoVersions = securitySetting->proto();
            protoVersions << NetworkManager::WirelessSecuritySetting::Rsn;
            securitySetting->setProto(protoVersions);
            // Pairwise
            QList<NetworkManager::WirelessSecuritySetting::WpaEncryptionCapabilities> pairwiseEncrypts = securitySetting->pairwise();
            pairwiseEncrypts << NetworkManager::WirelessSecuritySetting::Ccmp;
            securitySetting->setPairwise(pairwiseEncrypts);
            // Group
            QList<NetworkManager::WirelessSecuritySetting::WpaEncryptionCapabilities> groupEncrypts = securitySetting->group();
            groupEncrypts << NetworkManager::WirelessSecuritySetting::Ccmp;
            securitySetting->setGroup(groupEncrypts);
        }
    }

    map = settings.toMap();
}

void KCMNetworkManagementQml::save()
{
    // process the pending settings
    if (m_pendingNewSettings) {
        NMVariantMapMap map = m_pendingNewSettings->toMap();
        map.insert(QStringLiteral("802-11-wireless-security"), m_wifiSecurity->setting());
        if (m_wifiSecurity->enabled8021x()) {
            map.insert(QStringLiteral("802-1x"), m_wifiSecurity->setting8021x());
        }

        applyWirelessSetting(map);
        m_handler->addConnection(map);

        m_pendingNewSettings.reset();
        kcmChanged(false);
        KQuickConfigModule::save();
        return;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (!connection) {
        Q_EMIT saveFailed(QStringLiteral("No connection selected"));
        return;
    }

    NMVariantMapMap map = connection->settings()->toMap();
    if (m_wifiSecurity->securityType() != WifiSecuritySetting::None) {
        map.insert(QStringLiteral("802-11-wireless-security"), m_wifiSecurity->setting());
    } else {
        map.remove(QStringLiteral("802-11-wireless-security"));
    }
    if (m_wifiSecurity->enabled8021x()) {
        map.insert(QStringLiteral("802-1x"), m_wifiSecurity->setting8021x());
    } else {
        map.remove(QStringLiteral("802-1x"));
    }
    applyWirelessSetting(map);
    qDebug() << map;
    m_handler->updateConnection(connection, map);

    kcmChanged(false);
    KQuickConfigModule::save();
}
void KCMNetworkManagementQml::onSelectedConnectionChanged(const QString &connectionPath)
{
    if (connectionPath.isEmpty()) {
        resetSelection();
        return;
    }

    m_pendingNewSettings.reset();
    m_currentConnectionPath = connectionPath;
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(connectionPath);
    if (connection) {
        loadConnectionSettings(connection->settings());
    }
}
void KCMNetworkManagementQml::loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connectionSettings)
{
    if (!connectionSettings)
        return;

    m_connectionType = connectionSettings->connectionType();
    Q_EMIT connectionTypeChanged();
    // check wireless only for rn
    if (connectionSettings->connectionType() != NetworkManager::ConnectionSettings::Wireless) {
        kcmChanged(false);
        return;
    }

    NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity =
        connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();

    NetworkManager::Security8021xSetting::Ptr security8021x =
        connectionSettings->setting(NetworkManager::Setting::Security8021x).staticCast<NetworkManager::Security8021xSetting>();
    if (wifiSecurity || security8021x) {
        m_wifiSecurity->loadConfig(wifiSecurity);
        m_wifiSecurity->loadSecrets(wifiSecurity, security8021x);

        connect(m_wifiSecurity, &WifiSecuritySetting::validChanged, this, [this]() {
            if (m_wifiSecurity->isValid()) {
                kcmChanged(true);
            }
        });
    }

    Q_EMIT connectionLoaded(m_currentConnectionPath);

    kcmChanged(false);
}

void KCMNetworkManagementQml::onRequestCreateConnection(int connectionType, const QString &vpnType, const QString &specificType, bool shared)
{
    Q_UNUSED(vpnType)
    Q_UNUSED(specificType)

    auto type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connectionType);

    // only handle wireless for now
    if (type == NetworkManager::ConnectionSettings::Wireless) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(type));

        if (shared) {
            // replaces shared wifi setup from old kcm.cpp
            auto wifiSetting = connectionSettings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
            wifiSetting->setMode(NetworkManager::WirelessSetting::Adhoc);
            wifiSetting->setSsid(i18n("my_shared_connection").toUtf8());

            for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
                if (device->type() != NetworkManager::Device::Wifi)
                    continue;
                auto wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
                if (wifiDev && wifiDev->wirelessCapabilities().testFlag(NetworkManager::WirelessDevice::ApCap)) {
                    wifiSetting->setMode(NetworkManager::WirelessSetting::Ap);
                    wifiSetting->setMacAddress(NetworkManager::macAddressFromString(wifiDev->permanentHardwareAddress()));
                }
            }

            // auto ipv4Setting = connectionSettings->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
            // ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Shared);
            // connectionSettings->setAutoconnect(false);
        }

        connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        addConnection(connectionSettings);
    }
}

void KCMNetworkManagementQml::addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings)
{
    m_pendingNewSettings = connectionSettings;
    m_createdConnectionUuid = connectionSettings->uuid();

    m_currentConnectionPath.clear();
    m_wifiSecurity->setSecurityType(WifiSecuritySetting::None);

    Q_EMIT connectionLoaded(QString());
}

void KCMNetworkManagementQml::onConnectionAdded(const QString &connection)
{
    if (m_createdConnectionUuid.isEmpty())
        return;

    NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnection(connection);
    if (!newConnection)
        return;

    auto settings = newConnection->settings();
    if (settings && settings->uuid() == m_createdConnectionUuid) {
        m_currentConnectionPath = newConnection->path();
        loadConnectionSettings(settings);
        m_createdConnectionUuid.clear();
    }
}

void KCMNetworkManagementQml::onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath)
{
    Q_UNUSED(connectionName)

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);

    if (connection && needsSave()) {
        if (KMessageBox::questionTwoActions(nullptr,
                                            i18n("Do you want to save changes made to the "
                                                 "connection '%1'?",
                                                 connection->name()),
                                            i18nc("@title:window", "Save Changes"),
                                            KStandardGuiItem::save(),
                                            KStandardGuiItem::discard(),
                                            QString(),
                                            KMessageBox::Notify)
            == KMessageBox::ButtonCode::PrimaryAction) {
            save();
        }
    }

    onSelectedConnectionChanged(connectionPath);
}
void KCMNetworkManagementQml::onRequestDuplicateConnection(const QString &connectionPath)
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(connectionPath);
    if (!connection)
        return;

    auto connSettings = connection->settings();
    if (!connSettings)
        return;

    // only duplicate wireless for now
    if (connSettings->connectionType() != NetworkManager::ConnectionSettings::Wireless)
        return;

    NetworkManager::ConnectionSettings::Ptr newSettings(new NetworkManager::ConnectionSettings(connSettings->connectionType()));
    newSettings->fromMap(connSettings->toMap());
    newSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    newSettings->setId(connSettings->id() + i18nc("Suffix appended to duplicated connection name", " (Copy)"));

    addConnection(newSettings);
}
void KCMNetworkManagementQml::kcmChanged(bool changed)
{
    setNeedsSave(changed);
    Q_EMIT kcmChangedStateChanged(changed);
}

void KCMNetworkManagementQml::resetSelection()
{
    m_currentConnectionPath.clear();
    m_pendingNewSettings.reset();
    setNeedsSave(false);
    Q_EMIT kcmChangedStateChanged(false);
}
KCMNetworkManagementQml::ImportResult KCMNetworkManagementQml::ImportResult::pass()
{
    return {true, QString()};
}

KCMNetworkManagementQml::ImportResult KCMNetworkManagementQml::ImportResult::fail(const QString &message)
{
    return {false, message};
}
#include "kcm.moc"
