/*
    Copyright 2016-2018 Jan Grulich <jgrulich@redhat.com>

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

#include "kcm.h"

#include "debug.h"
#include "settings/connectionsettings.h"
// #include "mobileconnectionwizard.h"
#include "uiutils.h"
// #include "vpnuiplugin.h"

// KDE
#include <KAboutData>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KService>
#include <KServiceTypeTrader>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessDevice>

// Qt
#include <QFileDialog>
#include <QTimer>
#include <QtQuick/QQuickItem>

Q_LOGGING_CATEGORY(KCM_NETWORKMANAGEMENT, "kcm_networkmanagement")

K_PLUGIN_FACTORY_WITH_JSON(KCMNetworkmanagementFactory, "kcm_networkmanagement.json", registerPlugin<KCMNetworkmanagement>();)

KCMNetworkmanagement::KCMNetworkmanagement(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args)
    , m_handler(new Handler(this))
{
    KAboutData* about = new KAboutData(QStringLiteral("kcm_networkmanagement"), i18n("Edit your Network Connections"),
                                       QStringLiteral("0.1"), QString(), KAboutLicense::LGPL);
    about->addAuthor(i18n("Jan Grulich"), QString(), QStringLiteral("jgrulich@redhat.com"));
    setAboutData(about);
    setButtons(Apply);

    m_connectionSettings = new ConnectionSettings(this);
    connect(m_connectionSettings, &ConnectionSettings::settingChanged,
            [this] () {
                if (m_connectionSettings->isInitialized() && m_connectionSettings->isValid()) {
                    setNeedsSave(true);
                }
            });
    connect(m_connectionSettings, &ConnectionSettings::validityChanged,
            [this] (bool valid) {
                if (m_connectionSettings->isInitialized()) {
                    setNeedsSave(true);
                }
            });

//     NetworkManager::Connection::Ptr selectedConnection;
//
//     // Look in the arguments for a connection ID to preselect
//     static const QLatin1Literal uuidArgumentMarker { "Uuid=" };
//     for (QVariant arg : args) {
//         if (arg.canConvert(QMetaType::QString)) {
//             QString uuid = arg.toString();
//             if (uuid.startsWith(uuidArgumentMarker)) {
//                 uuid = uuid.replace(uuidArgumentMarker, QString());
//                 selectedConnection = NetworkManager::findConnectionByUuid(uuid);
//                 qDebug() << "Selecting user connection:" << uuid;
//                 break;
//             }
//         }
//     }
//
//     // Pre-select the currently active primary connection
//     if (!selectedConnection || !selectedConnection->isValid()) {
//         NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::primaryConnection();
//         if (activeConnection && activeConnection->isValid()) {
//             selectedConnection = activeConnection->connection();
//             qDebug() << "Selecting active connection:" << selectedConnection->uuid();
//         }
//     }
//
//     // Select the very first connection as a fallback
//     if (!selectedConnection || !selectedConnection->isValid()) {
//         NetworkManager::Connection::List connectionList = NetworkManager::listConnections();
//         std::sort(connectionList.begin(), connectionList.end(), [] (const NetworkManager::Connection::Ptr &left, const NetworkManager::Connection::Ptr &right)
//         {
//             const QString leftName = left->settings()->id();
//             const UiUtils::SortedConnectionType leftType = UiUtils::connectionTypeToSortedType(left->settings()->connectionType());
//             const QDateTime leftDate = left->settings()->timestamp();
//
//             const QString rightName = right->settings()->id();
//             const UiUtils::SortedConnectionType rightType = UiUtils::connectionTypeToSortedType(right->settings()->connectionType());
//             const QDateTime rightDate = right->settings()->timestamp();
//
//             if (leftType < rightType) {
//                 return true;
//             } else if (leftType > rightType) {
//                 return false;
//             }
//
//             if (leftDate > rightDate) {
//                 return true;
//             } else if (leftDate < rightDate) {
//                 return false;
//             }
//
//             if (QString::localeAwareCompare(leftName, rightName) > 0) {
//                 return true;
//             } else {
//                 return false;
//             }
//         });
//
//         for (const NetworkManager::Connection::Ptr &connection : connectionList) {
//             const NetworkManager::ConnectionSettings::ConnectionType type = connection->settings()->connectionType();
//             if (UiUtils::isConnectionTypeSupported(type)) {
//                 selectedConnection = connection;
//                 qDebug() << "Selecting first connection:" << connection->uuid();
//                 break;
//             }
//         }
//     }
//
//     if (selectedConnection && selectedConnection->isValid()) {
//         const NetworkManager::ConnectionSettings::Ptr settings = selectedConnection->settings();
//         if (UiUtils::isConnectionTypeSupported(settings->connectionType())) {
//             QMetaObject::invokeMethod(rootItem, "selectConnection", Q_ARG(QVariant, settings->id()), Q_ARG(QVariant, selectedConnection->path()));
//         }
//     } else {
//         qDebug() << "Cannot preselect a connection";
//     }

    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionAdded, this, &KCMNetworkmanagement::onConnectionAdded, Qt::UniqueConnection);

    // Initialize first scan and then scan every 15 seconds
    m_handler->requestScan();

    m_timer = new QTimer(this);
    m_timer->setInterval(15000);
    connect(m_timer, &QTimer::timeout, [this] () {
        m_handler->requestScan();
        m_timer->start();
    });
    m_timer->start();
}

KCMNetworkmanagement::~KCMNetworkmanagement()
{
    delete m_handler;
}

void KCMNetworkmanagement::defaults()
{
    KQuickAddons::ConfigModule::defaults();
}

void KCMNetworkmanagement::load()
{
    // If there is no loaded connection do nothing
    if (m_currentConnectionPath.isEmpty()) {
        return;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        // Re-load the connection again to load stored values
        m_connectionSettings->loadConfig(connectionSettings->toMap());
    }

    KQuickAddons::ConfigModule::load();
}

void KCMNetworkmanagement::save()
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);

    if (connection) {
        m_handler->updateConnection(connection, m_connectionSettings->settingMap());
    }

    KQuickAddons::ConfigModule::save();
}

void KCMNetworkmanagement::onConnectionAdded(const QString &connection)
{
    if (m_createdConnectionUuid.isEmpty()) {
        return;
    }

    NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnection(connection);
    if (newConnection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = newConnection->settings();
        if (connectionSettings && connectionSettings->uuid() == m_createdConnectionUuid) {
            loadConnectionSettings(connectionSettings->toMap());
            QMetaObject::invokeMethod(mainUi(), "selectConnectionInView", Q_ARG(QVariant, connectionSettings->id()), Q_ARG(QVariant, newConnection->path()));
            m_createdConnectionUuid.clear();
        }
    }
}

void KCMNetworkmanagement::selectConnection(const QString &connectionPath)
{
    if (connectionPath.isEmpty()) {
        resetSelection();
        return;
    }

    m_currentConnectionPath = connectionPath;

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        loadConnectionSettings(connectionSettings->toMap());
    }
}

void KCMNetworkmanagement::requestCreateConnection(int connectionType, const QString &vpnType, const QString &specificType, bool shared)
{
    NetworkManager::ConnectionSettings::ConnectionType type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connectionType);

    if (type == NetworkManager::ConnectionSettings::Vpn && vpnType == "imported") {
        importVpn();
    } else if (type == NetworkManager::ConnectionSettings::Gsm) { // launch the mobile broadband wizard, both gsm/cdma
// #if WITH_MODEMMANAGER_SUPPORT
//         QPointer<MobileConnectionWizard> wizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Unknown, this);
//         connect(wizard.data(), &MobileConnectionWizard::accepted,
//                 [wizard, this] () {
//                     if (wizard->getError() == MobileProviders::Success) {
//                         qCDebug(KCM_NETWORKMANAGEMENT) << "Mobile broadband wizard finished:" << wizard->type() << wizard->args();
//
//                         if (wizard->args().count() == 2) {
//                             QVariantMap tmp = qdbus_cast<QVariantMap>(wizard->args().value(1));
//
//                             NetworkManager::ConnectionSettings::Ptr connectionSettings;
//                             connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(wizard->type()));
//                             connectionSettings->setId(wizard->args().value(0).toString());
//                             if (wizard->type() == NetworkManager::ConnectionSettings::Gsm) {
//                                 NetworkManager::GsmSetting::Ptr gsmSetting = connectionSettings->setting(NetworkManager::Setting::Gsm).staticCast<NetworkManager::GsmSetting>();
//                                 gsmSetting->fromMap(tmp);
//                                 gsmSetting->setPasswordFlags(NetworkManager::Setting::NotRequired);
//                                 gsmSetting->setPinFlags(NetworkManager::Setting::NotRequired);
//                             } else if (wizard->type() == NetworkManager::ConnectionSettings::Cdma) {
//                                 connectionSettings->setting(NetworkManager::Setting::Cdma)->fromMap(tmp);
//                             } else {
//                                 qCWarning(KCM_NETWORKMANAGEMENT) << Q_FUNC_INFO << "Unhandled setting type";
//                             }
//                             // Generate new UUID
//                             connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
//                             addConnection(connectionSettings);
//                         } else {
//                             qCWarning(KCM_NETWORKMANAGEMENT) << Q_FUNC_INFO << "Unexpected number of args to parse";
//                         }
//                     }
//                 });
//         connect(wizard.data(), &MobileConnectionWizard::finished,
//                 [wizard] () {
//                     if (wizard) {
//                         wizard->deleteLater();
//                     }
//                 });
//         wizard->setModal(true);
//         wizard->show();
// #endif
    } else {
        NetworkManager::ConnectionSettings::Ptr connectionSettings;
        connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(type));

        if (type == NetworkManager::ConnectionSettings::Vpn) {
            NetworkManager::VpnSetting::Ptr vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
            vpnSetting->setServiceType(vpnType);
            // Set VPN subtype in case of Openconnect to add support for juniper
            if (vpnType == QLatin1String("org.freedesktop.NetworkManager.openconnect")) {
                NMStringMap data = vpnSetting->data();
                data.insert(QLatin1String("protocol"), specificType);
                vpnSetting->setData(data);
            }
        }

        if (type == NetworkManager::ConnectionSettings::Wired || type == NetworkManager::ConnectionSettings::Wireless) {
            // Set auto-negotiate to true, NM sets it to false by default, but we used to have this before and also
            // I don't think it's wise to request users to specify speed and duplex as most of them don't know what is that
            // and what to set
            if (type == NetworkManager::ConnectionSettings::Wired) {
                NetworkManager::WiredSetting::Ptr wiredSetting = connectionSettings->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
                wiredSetting->setAutoNegotiate(true);
            }

            if (shared) {
                if (type == NetworkManager::ConnectionSettings::Wireless) {
                    NetworkManager::WirelessSetting::Ptr wifiSetting = connectionSettings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                    wifiSetting->setMode(NetworkManager::WirelessSetting::Adhoc);
                    wifiSetting->setSsid(i18n("my_shared_connection").toUtf8());

                    for (const NetworkManager::Device::Ptr & device : NetworkManager::networkInterfaces()) {
                        if (device->type() == NetworkManager::Device::Wifi) {
                            NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
                            if (wifiDev) {
                                if (wifiDev->wirelessCapabilities().testFlag(NetworkManager::WirelessDevice::ApCap)) {
                                    wifiSetting->setMode(NetworkManager::WirelessSetting::Ap);
                                    wifiSetting->setMacAddress(NetworkManager::macAddressFromString(wifiDev->permanentHardwareAddress()));
                                }
                            }
                        }
                    }
                }

                NetworkManager::Ipv4Setting::Ptr ipv4Setting = connectionSettings->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
                ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Shared);
                connectionSettings->setAutoconnect(false);
            }
        }
        // Generate new UUID
        connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        addConnection(connectionSettings);
    }
}

void KCMNetworkmanagement::requestExportConnection(const QString &connectionPath)
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(connectionPath);
    if (!connection) {
        return;
    }

    NetworkManager::ConnectionSettings::Ptr connSettings = connection->settings();

    if (connSettings->connectionType() != NetworkManager::ConnectionSettings::Vpn)
        return;

    NetworkManager::VpnSetting::Ptr vpnSetting = connSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();

    qCDebug(KCM_NETWORKMANAGEMENT) << "Exporting VPN connection" << connection->name() << "type:" << vpnSetting->serviceType();

    QString error;
//     VpnUiPlugin * vpnPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QStringLiteral("PlasmaNetworkManagement/VpnUiPlugin"),
//                                                                                        QStringLiteral("[X-NetworkManager-Services]=='%1'").arg(vpnSetting->serviceType()),
//                                                                                        this, QVariantList(), &error);

//     if (vpnPlugin) {
//         if (vpnPlugin->suggestedFileName(connSettings).isEmpty()) { // this VPN doesn't support export
//             qCWarning(KCM_NETWORKMANAGEMENT) << "This VPN doesn't support export";
//             return;
//         }
//
//         const QString url = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + vpnPlugin->suggestedFileName(connSettings);
//         const QString filename = QFileDialog::getSaveFileName(this, i18n("Export VPN Connection"), url, vpnPlugin->supportedFileExtensions());
//         if (!filename.isEmpty()) {
//             if (!vpnPlugin->exportConnectionSettings(connSettings, filename)) {
//                 // TODO display failure
//                 qCWarning(KCM_NETWORKMANAGEMENT) << "Failed to export VPN connection";
//             } else {
//                 // TODO display success
//             }
//         }
//         delete vpnPlugin;
//     } else {
//         qCWarning(KCM_NETWORKMANAGEMENT) << "Error getting VpnUiPlugin for export:" << error;
//     }
}

QObject * KCMNetworkmanagement::connectionSettings() const
{
   return m_connectionSettings;
}

void KCMNetworkmanagement::addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings)
{
//     QPointer<ConnectionEditorDialog> editor = new ConnectionEditorDialog(connectionSettings);
//     connect(editor.data(), &ConnectionEditorDialog::accepted,
//             [connectionSettings, editor, this] () {
//                 // We got confirmation so watch this connection and select it once it is created
//                 m_createdConnectionUuid = connectionSettings->uuid();
//                 m_handler->addConnection(editor->setting());
//             });
//     connect(editor.data(), &ConnectionEditorDialog::finished,
//             [editor] () {
//                 if (editor) {
//                     editor->deleteLater();
//                 }
//             });
//     editor->setModal(true);
//     editor->show();
}

void KCMNetworkmanagement::loadConnectionSettings(const NMVariantMapMap &connectionSettings)
{
    m_connectionSettings->loadConfig(connectionSettings);

    QMetaObject::invokeMethod(mainUi(), "loadConnectionSetting");

    setNeedsSave(false);
}

void KCMNetworkmanagement::importVpn()
{
    // get the list of supported extensions
//     const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");
//     QString extensions;
//     Q_FOREACH (const KService::Ptr &service, services) {
//         VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
//         if (vpnPlugin) {
//             extensions += vpnPlugin->supportedFileExtensions() % QStringLiteral(" ");
//             delete vpnPlugin;
//         }
//     }
//
//     const QString &filename = QFileDialog::getOpenFileName(this, i18n("Import VPN Connection"), QDir::homePath(), extensions.simplified());
//
//     if (!filename.isEmpty()) {
//         const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");
//
//         QFileInfo fi(filename);
//         const QString ext = QStringLiteral("*.") % fi.suffix();
//         qCDebug(KCM_NETWORKMANAGEMENT) << "Importing VPN connection " << filename << "extension:" << ext;
//
//         Q_FOREACH (const KService::Ptr &service, services) {
//             VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
//             if (vpnPlugin && vpnPlugin->supportedFileExtensions().contains(ext)) {
//                 qCDebug(KCM_NETWORKMANAGEMENT) << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String).toString();
//
//                 NMVariantMapMap connection = vpnPlugin->importConnectionSettings(filename);
//
//                 // qCDebug(KCM_NETWORKMANAGEMENT) << "Raw connection:" << connection;
//
//                 NetworkManager::ConnectionSettings connectionSettings;
//                 connectionSettings.fromMap(connection);
//                 connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
//
//                 // qCDebug(KCM_NETWORKMANAGEMENT) << "Converted connection:" << connectionSettings;
//
//                 m_handler->addConnection(connectionSettings.toMap());
//                 // qCDebug(KCM_NETWORKMANAGEMENT) << "Adding imported connection under id:" << conId;
//
//                 if (connection.isEmpty()) { // the "positive" part will arrive with connectionAdded
//                     // TODO display success
//                 } else {
//                     delete vpnPlugin;
//                     break; // stop iterating over the plugins if the import produced at least some output
//                 }
//
//                 delete vpnPlugin;
//             }
//         }
//     }
}

void KCMNetworkmanagement::resetSelection()
{
    // Reset selected connections
    m_currentConnectionPath.clear();
    QMetaObject::invokeMethod(mainUi(), "deselectConnectionsInView");
//     if (m_connectionSetting) {
//         delete m_connectionSetting;
//         m_connectionSetting = nullptr;
//     }
    setNeedsSave(false);
}

#include "kcm.moc"
