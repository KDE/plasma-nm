/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kcm.h"

#include "configuration.h"
#include "connectioneditordialog.h"
#include "mobileconnectionwizard.h"
#include "plasma_nm_kcm.h"
#include "settings/wireguardinterfacewidget.h"
#include "uiutils.h"
#include "vpnuiplugin.h"

// KDE
#include <KMessageBox>
#include <KPluginFactory>
#include <KPluginMetaData>
#include <KSharedConfig>
#include <kdeclarative/kdeclarative.h>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Ipv6Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WireguardSetting>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

// Qt
#include <QFileDialog>
#include <QMenu>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>
#include <QQuickWidget>
#include <QTimer>
#include <QVBoxLayout>

K_PLUGIN_CLASS_WITH_JSON(KCMNetworkmanagement, "kcm_networkmanagement.json")

KCMNetworkmanagement::KCMNetworkmanagement(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
    , m_handler(new Handler(this))
    , m_ui(new Ui::KCMForm)
{
    auto mainWidget = new QWidget(this);
    m_ui->setupUi(mainWidget);

    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_ui->connectionView->engine());
    kdeclarative.setupEngine(m_ui->connectionView->engine());

    // Check if we can use AP mode to identify security type
    bool useApMode = false;
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
                    useApMode = true;
                }

                // We prefer inactive wireless card with AP capabilities
                if (foundInactive && useApMode) {
                    break;
                }
            }
        }
    }

    m_ui->connectionView->setMinimumWidth(300);
    m_ui->connectionView->rootContext()->setContextProperty("alternateBaseColor", mainWidget->palette().color(QPalette::Active, QPalette::AlternateBase));
    m_ui->connectionView->rootContext()->setContextProperty("backgroundColor", mainWidget->palette().color(QPalette::Active, QPalette::Window));
    m_ui->connectionView->rootContext()->setContextProperty("baseColor", mainWidget->palette().color(QPalette::Active, QPalette::Base));
    m_ui->connectionView->rootContext()->setContextProperty("highlightColor", mainWidget->palette().color(QPalette::Active, QPalette::Highlight));
    m_ui->connectionView->rootContext()->setContextProperty("textColor", mainWidget->palette().color(QPalette::Active, QPalette::Text));
    m_ui->connectionView->rootContext()->setContextProperty("connectionModified", false);
    m_ui->connectionView->rootContext()->setContextProperty("useApMode", useApMode);
    m_ui->connectionView->setClearColor(Qt::transparent);
    m_ui->connectionView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_ui->connectionView->setSource(
        QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kcm_networkmanagement/qml/main.qml"))));

    QObject *rootItem = m_ui->connectionView->rootObject();
    connect(rootItem, SIGNAL(selectedConnectionChanged(QString)), this, SLOT(onSelectedConnectionChanged(QString)));
    connect(rootItem, SIGNAL(requestCreateConnection(int, QString, QString, bool)), this, SLOT(onRequestCreateConnection(int, QString, QString, bool)));
    connect(rootItem, SIGNAL(requestExportConnection(QString)), this, SLOT(onRequestExportConnection(QString)));
    connect(rootItem, SIGNAL(requestToChangeConnection(QString, QString)), this, SLOT(onRequestToChangeConnection(QString, QString)));

    auto l = new QVBoxLayout(this);
    l->addWidget(mainWidget);

    setButtons(Button::Apply);

    NetworkManager::Connection::Ptr selectedConnection;

    // Look in the arguments for a connection ID to preselect
    static const QLatin1String uuidArgumentMarker{"Uuid="};
    for (QVariant arg : args) {
        if (arg.canConvert(QMetaType::QString)) {
            QString uuid = arg.toString();
            if (uuid.startsWith(uuidArgumentMarker)) {
                uuid = uuid.replace(uuidArgumentMarker, QString());
                selectedConnection = NetworkManager::findConnectionByUuid(uuid);
                qDebug(PLASMA_NM_KCM_LOG) << "Selecting user connection:" << uuid;
                break;
            }
        }
    }

    // Pre-select the currently active primary connection
    if (!selectedConnection || !selectedConnection->isValid()) {
        NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::primaryConnection();
        if (activeConnection && activeConnection->isValid()) {
            selectedConnection = activeConnection->connection();
            qDebug(PLASMA_NM_KCM_LOG) << "Selecting active connection:" << selectedConnection->uuid();
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
                qDebug(PLASMA_NM_KCM_LOG) << "Selecting first connection:" << connection->uuid();
                break;
            }
        }
    }

    if (selectedConnection && selectedConnection->isValid()) {
        const NetworkManager::ConnectionSettings::Ptr settings = selectedConnection->settings();
        if (UiUtils::isConnectionTypeSupported(settings->connectionType())) {
            QMetaObject::invokeMethod(rootItem, "selectConnection", Q_ARG(QVariant, settings->id()), Q_ARG(QVariant, selectedConnection->path()));
        }
    } else {
        qDebug(PLASMA_NM_KCM_LOG) << "Cannot preselect a connection";
    }

    connect(NetworkManager::settingsNotifier(),
            &NetworkManager::SettingsNotifier::connectionAdded,
            this,
            &KCMNetworkmanagement::onConnectionAdded,
            Qt::UniqueConnection);

    // Initialize first scan and then scan every 15 seconds
    m_handler->requestScan();

    m_timer = new QTimer(this);
    m_timer->setInterval(15000);
    connect(m_timer, &QTimer::timeout, [this]() {
        m_handler->requestScan();
    });
    m_timer->start();
}

KCMNetworkmanagement::~KCMNetworkmanagement()
{
    delete m_handler;
    delete m_tabWidget;
    delete m_ui;
}

void KCMNetworkmanagement::defaults()
{
    KCModule::defaults();
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
        if (m_tabWidget) {
            m_tabWidget->setConnection(connectionSettings);
        }
    }

    KCModule::load();
}

void KCMNetworkmanagement::save()
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);

    if (connection) {
        m_handler->updateConnection(connection, m_tabWidget->setting());
    }

    kcmChanged(false);

    KCModule::save();
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
            QObject *rootItem = m_ui->connectionView->rootObject();
            loadConnectionSettings(connectionSettings);
            QMetaObject::invokeMethod(rootItem, "selectConnection", Q_ARG(QVariant, connectionSettings->id()), Q_ARG(QVariant, newConnection->path()));
            m_createdConnectionUuid.clear();
        }
    }
}

void KCMNetworkmanagement::onRequestCreateConnection(int connectionType, const QString &vpnType, const QString &specificType, bool shared)
{
    auto type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(connectionType);

    if (type == NetworkManager::ConnectionSettings::Vpn && vpnType == "imported") {
        importVpn();
    } else if (type == NetworkManager::ConnectionSettings::Gsm) { // launch the mobile broadband wizard, both gsm/cdma
#if WITH_MODEMMANAGER_SUPPORT
        QPointer<MobileConnectionWizard> wizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Unknown, this);
        connect(wizard.data(), &MobileConnectionWizard::accepted, [wizard, this]() {
            if (wizard->getError() == MobileProviders::Success) {
                qCDebug(PLASMA_NM_KCM_LOG) << "Mobile broadband wizard finished:" << wizard->type() << wizard->args();

                if (wizard->args().count() == 2) {
                    auto tmp = qdbus_cast<QVariantMap>(wizard->args().value(1));

                    NetworkManager::ConnectionSettings::Ptr connectionSettings;
                    connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(wizard->type()));
                    connectionSettings->setId(wizard->args().value(0).toString());
                    if (wizard->type() == NetworkManager::ConnectionSettings::Gsm) {
                        NetworkManager::GsmSetting::Ptr gsmSetting =
                            connectionSettings->setting(NetworkManager::Setting::Gsm).staticCast<NetworkManager::GsmSetting>();
                        gsmSetting->fromMap(tmp);
                        gsmSetting->setPasswordFlags(NetworkManager::Setting::NotRequired);
                        gsmSetting->setPinFlags(NetworkManager::Setting::NotRequired);
                    } else if (wizard->type() == NetworkManager::ConnectionSettings::Cdma) {
                        connectionSettings->setting(NetworkManager::Setting::Cdma)->fromMap(tmp);
                    } else {
                        qCWarning(PLASMA_NM_KCM_LOG) << Q_FUNC_INFO << "Unhandled setting type";
                    }
                    // Generate new UUID
                    connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
                    addConnection(connectionSettings);
                } else {
                    qCWarning(PLASMA_NM_KCM_LOG) << Q_FUNC_INFO << "Unexpected number of args to parse";
                }
            }
        });
        connect(wizard.data(), &MobileConnectionWizard::finished, [wizard]() {
            if (wizard) {
                wizard->deleteLater();
            }
        });
        wizard->setModal(true);
        wizard->show();
#endif
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
                NetworkManager::WiredSetting::Ptr wiredSetting =
                    connectionSettings->setting(NetworkManager::Setting::Wired).dynamicCast<NetworkManager::WiredSetting>();
                wiredSetting->setAutoNegotiate(true);
            }

            if (shared) {
                if (type == NetworkManager::ConnectionSettings::Wireless) {
                    NetworkManager::WirelessSetting::Ptr wifiSetting =
                        connectionSettings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                    wifiSetting->setMode(NetworkManager::WirelessSetting::Adhoc);
                    wifiSetting->setSsid(i18n("my_shared_connection").toUtf8());

                    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
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

                NetworkManager::Ipv4Setting::Ptr ipv4Setting =
                    connectionSettings->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
                ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Shared);
                connectionSettings->setAutoconnect(false);
            }
        }
        if (type == NetworkManager::ConnectionSettings::WireGuard) {
            NetworkManager::WireGuardSetting::Ptr wireguardSetting =
                connectionSettings->setting(NetworkManager::Setting::WireGuard).dynamicCast<NetworkManager::WireGuardSetting>();
            NetworkManager::Ipv4Setting::Ptr ipv4Setting =
                connectionSettings->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
            NetworkManager::Ipv6Setting::Ptr ipv6Setting =
                connectionSettings->setting(NetworkManager::Setting::Ipv6).dynamicCast<NetworkManager::Ipv6Setting>();
            connectionSettings->setAutoconnect(false);
            ipv4Setting->setMethod(NetworkManager::Ipv4Setting::Disabled);
            ipv6Setting->setMethod(NetworkManager::Ipv6Setting::Ignored);
        }
        // Generate new UUID
        connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
        addConnection(connectionSettings);
    }

    // Automatically enable virtual connection management if one is created
    if (type == NetworkManager::ConnectionSettings::Vlan || type == NetworkManager::ConnectionSettings::Bridge
        || type == NetworkManager::ConnectionSettings::Bond || type == NetworkManager::ConnectionSettings::Team) {
        Configuration::self().setManageVirtualConnections(true);
    }
}

void KCMNetworkmanagement::onRequestExportConnection(const QString &connectionPath)
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(connectionPath);
    if (!connection) {
        return;
    }

    NetworkManager::ConnectionSettings::Ptr connSettings = connection->settings();

    if (connSettings->connectionType() != NetworkManager::ConnectionSettings::Vpn)
        return;

    NetworkManager::VpnSetting::Ptr vpnSetting = connSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();

    qCDebug(PLASMA_NM_KCM_LOG) << "Exporting VPN connection" << connection->name() << "type:" << vpnSetting->serviceType();

    const auto result = VpnUiPlugin::loadPluginForType(nullptr, vpnSetting->serviceType());

    if (result) {
        std::unique_ptr<VpnUiPlugin> vpnPlugin(result.plugin);
        if (vpnPlugin->suggestedFileName(connSettings).isEmpty()) { // this VPN doesn't support export
            qCWarning(PLASMA_NM_KCM_LOG) << "This VPN doesn't support export";
            return;
        }

        const QString url =
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + vpnPlugin->suggestedFileName(connSettings);
        const QString filename = QFileDialog::getSaveFileName(this, i18n("Export VPN Connection"), url, vpnPlugin->supportedFileExtensions());
        if (!filename.isEmpty()) {
            if (!vpnPlugin->exportConnectionSettings(connSettings, filename)) {
                // TODO display failure
                qCWarning(PLASMA_NM_KCM_LOG) << "Failed to export VPN connection";
            } else {
                // TODO display success
            }
        }
    } else {
        qCWarning(PLASMA_NM_KCM_LOG) << "Error getting VpnUiPlugin for export:" << result.errorText;
    }
}

void KCMNetworkmanagement::onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath)
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);

    if (connection) {
        if (KMessageBox::questionYesNo(this,
                                       i18n("Do you want to save changes made to the connection '%1'?", connection->name()),
                                       i18nc("@title:window", "Save Changes"),
                                       KStandardGuiItem::save(),
                                       KStandardGuiItem::discard(),
                                       QString(),
                                       KMessageBox::Notify)
            == KMessageBox::Yes) {
            save();
        }
    }

    QObject *rootItem = m_ui->connectionView->rootObject();
    QMetaObject::invokeMethod(rootItem, "selectConnection", Q_ARG(QVariant, connectionName), Q_ARG(QVariant, connectionPath));
}

void KCMNetworkmanagement::onSelectedConnectionChanged(const QString &connectionPath)
{
    if (connectionPath.isEmpty()) {
        resetSelection();
        return;
    }

    m_currentConnectionPath = connectionPath;

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_currentConnectionPath);
    if (connection) {
        NetworkManager::ConnectionSettings::Ptr connectionSettings = connection->settings();
        loadConnectionSettings(connectionSettings);
    }
}

void KCMNetworkmanagement::addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings)
{
    QPointer<ConnectionEditorDialog> editor = new ConnectionEditorDialog(connectionSettings);
    connect(editor.data(), &ConnectionEditorDialog::accepted, [connectionSettings, editor, this]() {
        // We got confirmation so watch this connection and select it once it is created
        m_createdConnectionUuid = connectionSettings->uuid();
        m_handler->addConnection(editor->setting());
    });
    connect(editor.data(), &ConnectionEditorDialog::finished, [editor]() {
        if (editor) {
            editor->deleteLater();
        }
    });
    editor->setModal(true);
    editor->show();
}

void KCMNetworkmanagement::kcmChanged(bool kcmChanged)
{
    m_ui->connectionView->rootContext()->setContextProperty("connectionModified", kcmChanged);
    Q_EMIT changed(kcmChanged);
}

void KCMNetworkmanagement::loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connectionSettings)
{
    if (m_tabWidget) {
        m_tabWidget->setConnection(connectionSettings);
    } else {
        m_tabWidget = new ConnectionEditorTabWidget(connectionSettings);
        connect(m_tabWidget, &ConnectionEditorTabWidget::settingChanged, [this]() {
            if (m_tabWidget->isInitialized() && m_tabWidget->isValid()) {
                kcmChanged(true);
            }
        });
        connect(m_tabWidget, &ConnectionEditorTabWidget::validityChanged, [this](bool valid) {
            if (m_tabWidget->isInitialized() && m_tabWidget->isValid() != valid) {
                kcmChanged(valid);
            }
        });
        auto layout = new QVBoxLayout(m_ui->connectionConfiguration);
        layout->addWidget(m_tabWidget);
    }

    kcmChanged(false);
}

void KCMNetworkmanagement::importVpn()
{
    // get the list of supported extensions
    const QVector<KPluginMetaData> services = KPluginMetaData::findPlugins(QStringLiteral("plasma/network/vpn"));
    QString extensions;
    for (const KPluginMetaData &service : services) {
        const auto result = KPluginFactory::instantiatePlugin<VpnUiPlugin>(service);
        if (result) {
            extensions += result.plugin->supportedFileExtensions() % QStringLiteral(" ");
            delete result.plugin;
        }
    }

    const QString &filename = QFileDialog::getOpenFileName(this, i18n("Import VPN Connection"), QDir::homePath(), extensions.simplified());

    if (!filename.isEmpty()) {
        const QVector<KPluginMetaData> services = KPluginMetaData::findPlugins(QStringLiteral("plasma/network/vpn"));

        QFileInfo fi(filename);
        const QString ext = QStringLiteral("*.") % fi.suffix();
        qCDebug(PLASMA_NM_KCM_LOG) << "Importing VPN connection " << filename << "extension:" << ext;

        // Handle WireGuard separately because it is different than all the other VPNs
        if (WireGuardInterfaceWidget::supportedFileExtensions().contains(ext)) {
            NMVariantMapMap connection = WireGuardInterfaceWidget::importConnectionSettings(filename);
            NetworkManager::ConnectionSettings connectionSettings;
            connectionSettings.fromMap(connection);
            connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

            // qCDebug(PLASMA_NM_KCM_LOG) << "Converted connection:" << connectionSettings;

            m_handler->addConnection(connectionSettings.toMap());
            // qCDebug(PLASMA_NM_KCM_LOG) << "Adding imported connection under id:" << conId;

            if (!connection.isEmpty()) {
                return; // get out if the import produced at least some output
            }
        }
        for (const KPluginMetaData &service : services) {
            const auto result = KPluginFactory::instantiatePlugin<VpnUiPlugin>(service);

            if (!result) {
                continue;
            }

            std::unique_ptr<VpnUiPlugin> vpnPlugin(result.plugin);

            if (vpnPlugin->supportedFileExtensions().contains(ext)) {
                qCDebug(PLASMA_NM_KCM_LOG) << "Found VPN plugin" << service.name() << ", type:" << service.value("X-NetworkManager-Services");

                NMVariantMapMap connection = vpnPlugin->importConnectionSettings(filename);

                // qCDebug(PLASMA_NM_KCM_LOG) << "Raw connection:" << connection;

                NetworkManager::ConnectionSettings connectionSettings;
                connectionSettings.fromMap(connection);
                connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

                // qCDebug(PLASMA_NM_KCM_LOG) << "Converted connection:" << connectionSettings;

                m_handler->addConnection(connectionSettings.toMap());
                // qCDebug(PLASMA_NM_KCM_LOG) << "Adding imported connection under id:" << conId;

                if (connection.isEmpty()) { // the "positive" part will arrive with connectionAdded
                    // TODO display success
                } else {
                    break; // stop iterating over the plugins if the import produced at least some output
                }
            }
        }
    }
}

void KCMNetworkmanagement::resetSelection()
{
    // Reset selected connections
    m_currentConnectionPath.clear();
    QObject *rootItem = m_ui->connectionView->rootObject();
    QMetaObject::invokeMethod(rootItem, "deselectConnections");
    if (m_tabWidget) {
        delete m_ui->connectionConfiguration->layout();
        delete m_tabWidget;
        m_tabWidget = nullptr;
    }
    Q_EMIT changed(false);
}

#include "kcm.moc"
