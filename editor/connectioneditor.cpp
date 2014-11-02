/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

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


#include "connectioneditor.h"
#include "connectiondetaileditor.h"

#include "debug.h"
#include "editoridentitymodel.h"
#include "editorproxymodel.h"
#include "networkmodel.h"
#include "mobileconnectionwizard.h"
#include "uiutils.h"
#include "ui_connectioneditor.h"
#include "vpnuiplugin.h"

#include <networkmodelitem.h>

#include <QFileDialog>
#include <QStandardPaths>
#include <QUrl>
#include <QAction>
#include <QIcon>

#include <KAcceleratorManager>
#include <KActionCollection>
#include <KConfig>
#include <KConfigGroup>
#include <KFilterProxySearchLine>
#include <KLocalizedString>
#include <KMessageBox>
#include <KService>
#include <KServiceTypeTrader>
#include <KStandardAction>
#include <KXMLGUIFactory>
#include <KWallet/Wallet>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/VpnSetting>

ConnectionEditor::ConnectionEditor(QWidget* parent, Qt::WindowFlags flags)
    : KXmlGuiWindow(parent, flags)
    , m_editor(new Ui::ConnectionEditor)
    , m_handler(new Handler(this))
{
    QWidget * tmp = new QWidget(this);
    m_editor->setupUi(tmp);
    setCentralWidget(tmp);

    m_editor->connectionsWidget->setSortingEnabled(false);
    m_editor->connectionsWidget->sortByColumn(0, Qt::AscendingOrder);
    m_editor->connectionsWidget->setSortingEnabled(true);
    m_editor->connectionsWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_editor->messageWidget->hide();
    m_editor->messageWidget->setCloseButtonVisible(false);
    m_editor->messageWidget->setWordWrap(true);

    m_editor->ktreewidgetsearchline->lineEdit()->setPlaceholderText(i18n("Type here to search connections..."));

    initializeConnections();
    initializeMenu();

    connect(m_editor->connectionsWidget, SIGNAL(pressed(QModelIndex)),
            SLOT(slotItemClicked(QModelIndex)));
    connect(m_editor->connectionsWidget, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(slotItemDoubleClicked(QModelIndex)));
    connect(m_editor->connectionsWidget, SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(slotContextMenuRequested(QPoint)));
    connect(m_menu->menu(), SIGNAL(triggered(QAction*)),
            SLOT(addConnection(QAction*)));
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionAdded(QString)),
            SLOT(connectionAdded(QString)));

    KConfig config("kde5-nm-connection-editor");
    KConfigGroup generalGroup = config.group("General");

    if (generalGroup.isValid()) {
        if (generalGroup.readEntry("FirstStart", true)) {
            importSecretsFromPlainTextFiles();
        }

        generalGroup.writeEntry("FirstStart", false);
    }

    QLoggingCategory::setFilterRules(QStringLiteral("plasma-nm.debug = false"));
    QLoggingCategory::setFilterRules(QStringLiteral("plasma-nm.warning = true"));
}

ConnectionEditor::~ConnectionEditor()
{
    delete m_editor;
}

void ConnectionEditor::initializeMenu()
{
    m_menu = new KActionMenu(QIcon::fromTheme("list-add"), i18n("Add"), this);
    m_menu->menu()->setSeparatorsCollapsible(false);
    m_menu->setDelayed(false);

    m_menu->menu()->addSection(i18n("Hardware"));

    // TODO Adsl
    QAction * action = new QAction(i18n("DSL"), this);
    action->setData(NetworkManager::ConnectionSettings::Pppoe);
    m_menu->addAction(action);
    action = new QAction(i18n("InfiniBand"), this);
    action->setData(NetworkManager::ConnectionSettings::Infiniband);
    m_menu->addAction(action);
#if WITH_MODEMMANAGER_SUPPORT
    action = new QAction(i18n("Mobile Broadband..."), this);
    action->setData(NetworkManager::ConnectionSettings::Gsm);
    m_menu->addAction(action);
#endif
    action = new QAction(i18n("Wired Ethernet"), this);
    action->setData(NetworkManager::ConnectionSettings::Wired);
    action->setProperty("shared", false);
    m_menu->addAction(action);
    action = new QAction(i18n("Wired Ethernet (shared)"), this);
    action->setData(NetworkManager::ConnectionSettings::Wired);
    action->setProperty("shared", true);
    m_menu->addAction(action);
    action = new QAction(i18n("Wi-Fi"), this);
    action->setData(NetworkManager::ConnectionSettings::Wireless);
    action->setProperty("shared", false);
    m_menu->addAction(action);
    action = new QAction(i18n("Wi-Fi (shared)"), this);
    action->setData(NetworkManager::ConnectionSettings::Wireless);
    action->setProperty("shared", true);
    m_menu->addAction(action);
    action = new QAction(i18n("WiMAX"), this);
    action->setData(NetworkManager::ConnectionSettings::Wimax);
    m_menu->addAction(action);

    m_menu->menu()->addSection(i18nc("Virtual hardware devices, eg Bridge, Bond", "Virtual"));

    action = new QAction(i18n("Bond"), this);
    action->setData(NetworkManager::ConnectionSettings::Bond);
    m_menu->addAction(action);
    action = new QAction(i18n("Bridge"), this);
    action->setData(NetworkManager::ConnectionSettings::Bridge);
    m_menu->addAction(action);
    action = new QAction(i18n("VLAN"), this);
    action->setData(NetworkManager::ConnectionSettings::Vlan);
    m_menu->addAction(action);
#if NM_CHECK_VERSION(0, 9, 10)
    action = new QAction(i18n("Team"), this);
    action->setData(NetworkManager::ConnectionSettings::Team);
    m_menu->addAction(action);
#endif

    m_menu->menu()->addSection(i18n("VPN"));

    const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");
    foreach (const KService::Ptr & service, services) {
        qCDebug(PLASMA_NM) << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String).toString();

        action = new QAction(service->name(), this);
        action->setData(NetworkManager::ConnectionSettings::Vpn);
        action->setProperty("type", service->property("X-NetworkManager-Services", QVariant::String));
        m_menu->addAction(action);
    }

    actionCollection()->addAction("add_connection", m_menu);

    QAction * kAction = new QAction(QIcon::fromTheme("configure"), i18n("Edit..."), this);
    kAction->setEnabled(false);
    connect(kAction, SIGNAL(triggered()), SLOT(editConnection()));
    actionCollection()->addAction("edit_connection", kAction);

    kAction = new QAction(QIcon::fromTheme("edit-delete"), i18n("Delete"), this);
    kAction->setEnabled(false);
    connect(kAction, SIGNAL(triggered()), SLOT(removeConnection()));
    actionCollection()->addAction("delete_connection", kAction);
    actionCollection()->setDefaultShortcut(kAction, QKeySequence::Delete);

    kAction = new QAction(QIcon::fromTheme("document-import"), i18n("Import VPN..."), this);
    actionCollection()->addAction("import_vpn", kAction);
    connect(kAction, SIGNAL(triggered()), SLOT(importVpn()));

    kAction = new QAction(QIcon::fromTheme("document-export"), i18n("Export VPN..."), this);
    actionCollection()->addAction("export_vpn", kAction);
    kAction->setEnabled(false);
    connect(kAction, SIGNAL(triggered()), SLOT(exportVpn()));

    KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());
    KStandardAction::quit(this, SLOT(close()), actionCollection());

    setupGUI(QSize(480, 480));

    setAutoSaveSettings();

    KAcceleratorManager::manage(this);
}

void ConnectionEditor::addConnection(QAction* action)
{
    NetworkManager::ConnectionSettings::Ptr connectionSettings;
    NetworkManager::ConnectionSettings::ConnectionType type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(action->data().toUInt());
    const QString vpnType = action->property("type").toString();

    // qCDebug(PLASMA_NM) << "Adding new connection of type " << type;

    if (type == NetworkManager::ConnectionSettings::Gsm) { // launch the mobile broadband wizard, both gsm/cdma
#if WITH_MODEMMANAGER_SUPPORT
        QWeakPointer<MobileConnectionWizard> wizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Unknown, this);
        if (wizard.data()->exec() == QDialog::Accepted && wizard.data()->getError() == MobileProviders::Success) {
            qCDebug(PLASMA_NM) << "Mobile broadband wizard finished:" << wizard.data()->type() << wizard.data()->args();

            if (wizard.data()->args().count() == 2) {
                QVariantMap tmp = qdbus_cast<QVariantMap>(wizard.data()->args().value(1));

#if 0 // network IDs are not used yet and seem to break the setting
        if (args.count() == 3) { // gsm specific
            QStringList networkIds = args.value(1).toStringList();
            if (!networkIds.isEmpty())
                tmp.insert("network-id", networkIds.first());
        }
#endif
                connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(wizard.data()->type()));
                connectionSettings->setId(wizard.data()->args().value(0).toString());
                if (wizard.data()->type() == NetworkManager::ConnectionSettings::Gsm) {
                    connectionSettings->setting(NetworkManager::Setting::Gsm)->fromMap(tmp);
                } else if (wizard.data()->type() == NetworkManager::ConnectionSettings::Cdma) {
                    connectionSettings->setting(NetworkManager::Setting::Cdma)->fromMap(tmp);
                } else {
                    qCWarning(PLASMA_NM) << Q_FUNC_INFO << "Unhandled setting type";
                }
            } else {
                qCWarning(PLASMA_NM) << Q_FUNC_INFO << "Unexpected number of args to parse";
            }
        }

        if (wizard) {
            wizard.data()->deleteLater();
        }

#endif
    } else {
        connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(type));

        if (type == NetworkManager::ConnectionSettings::Vpn) {
            NetworkManager::VpnSetting::Ptr vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
            vpnSetting->setServiceType(vpnType);
            // qCDebug(PLASMA_NM) << "VPN type: " << vpnType;
        }

        if (type == NetworkManager::ConnectionSettings::Wired || type == NetworkManager::ConnectionSettings::Wireless) {
            bool shared = action->property("shared").toBool();
            if (shared) {
                if (type == NetworkManager::ConnectionSettings::Wireless) {
                    NetworkManager::WirelessSetting::Ptr wifiSetting = connectionSettings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                    wifiSetting->setMode(NetworkManager::WirelessSetting::Adhoc);
                    wifiSetting->setSsid(i18n("my_shared_connection").toUtf8());

                    foreach (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
                        if (device->type() == NetworkManager::Device::Wifi) {
                            NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
                            if (wifiDev) {
                                if (wifiDev->wirelessCapabilities().testFlag(NetworkManager::WirelessDevice::ApCap)) {
                                    wifiSetting->setMode(NetworkManager::WirelessSetting::Ap);
                                    wifiSetting->setMacAddress(NetworkManager::Utils::macAddressFromString(wifiDev->hardwareAddress()));
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
    }

    // In case that mobile broadband wizard has not been finished
    if (!connectionSettings) {
        return;
    }

    // Generate new UUID
    connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(connectionSettings, true);

    if (editor->exec() == QDialog::Accepted) {
        if (type == NetworkManager::ConnectionSettings::Vpn) {
            qCDebug(PLASMA_NM) << "Adding new connection of type " << vpnType;
        } else {
            qCDebug(PLASMA_NM) << "Adding new connection of type " << NetworkManager::ConnectionSettings::typeAsString(type);
        }
        m_handler->addConnection(editor->setting());
    }

    if (editor) {
        editor->deleteLater();
    }
}

void ConnectionEditor::connectionAdded(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        return;
    }

    if (con->settings()->isSlave())
        return;

    m_editor->messageWidget->animatedShow();
    m_editor->messageWidget->setMessageType(KMessageWidget::Positive);
    m_editor->messageWidget->setText(i18n("Connection %1 has been added", con->name()));
    QTimer::singleShot(5000, m_editor->messageWidget, SLOT(animatedHide()));
}

void ConnectionEditor::connectConnection()
{
    const QModelIndex currentIndex = m_editor->connectionsWidget->currentIndex();

    if (!currentIndex.isValid() || currentIndex.parent().isValid()) {
        return;
    }

    const QString connectionPath = currentIndex.data(NetworkModel::ConnectionPathRole).toString();
    const QString devicePath = currentIndex.data(NetworkModel::DevicePathRole).toString();
    const QString specificPath = currentIndex.data(NetworkModel::SpecificPathRole).toString();

    m_handler->activateConnection(connectionPath, devicePath, specificPath);
}

void ConnectionEditor::disconnectConnection()
{
    const QModelIndex currentIndex = m_editor->connectionsWidget->currentIndex();

    if (!currentIndex.isValid() || currentIndex.parent().isValid()) {
        return;
    }

    const QString connectionPath = currentIndex.data(NetworkModel::ConnectionPathRole).toString();
    const QString devicePath = currentIndex.data(NetworkModel::DevicePathRole).toString();
    m_handler->deactivateConnection(connectionPath, devicePath);
}

void ConnectionEditor::editConnection()
{
    const QModelIndex currentIndex = m_editor->connectionsWidget->currentIndex();

    if (!currentIndex.isValid() || currentIndex.parent().isValid()) {
        return;
    }

    slotItemDoubleClicked(currentIndex);
}

void ConnectionEditor::initializeConnections()
{
    EditorIdentityModel * model = new EditorIdentityModel(this);

    EditorProxyModel * filterModel = new EditorProxyModel(this);
    filterModel->setSourceModel(model);

    m_editor->connectionsWidget->setModel(filterModel);
    m_editor->ktreewidgetsearchline->setProxy(filterModel);

    m_editor->connectionsWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ConnectionEditor::removeConnection()
{
    const QModelIndex currentIndex = m_editor->connectionsWidget->currentIndex();

    if (!currentIndex.isValid() || currentIndex.parent().isValid()) {
        return;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(currentIndex.data(NetworkModel::UuidRole).toString());

    if (!connection) {
        return;
    }

    if (KMessageBox::questionYesNo(this, i18n("Do you want to remove the connection '%1'?", connection->name()), i18n("Remove Connection"), KStandardGuiItem::remove(),
                                   KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
            == KMessageBox::Yes) {
        foreach (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
            NetworkManager::ConnectionSettings::Ptr settings = con->settings();
            if (settings->master() == connection->uuid()) {
                m_handler->removeConnection(con->path());
            }
        }
        m_handler->removeConnection(connection->path());
    }
}

void ConnectionEditor::slotContextMenuRequested(const QPoint&)
{
    QMenu * menu = new QMenu(this);

    QModelIndex index = m_editor->connectionsWidget->currentIndex();
    const bool isActive = (NetworkManager::ActiveConnection::State)index.data(NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const bool isAvailable = (NetworkModelItem::ItemType)index.data(NetworkModel::ItemTypeRole).toUInt() == NetworkModelItem::AvailableConnection;

    if (isAvailable && !isActive) {
        menu->addAction(QIcon::fromTheme("user-online"), i18n("Connect"), this, SLOT(connectConnection()));
    } else if (isAvailable && isActive) {
        menu->addAction(QIcon::fromTheme("user-offline"), i18n("Disconnect"), this, SLOT(disconnectConnection()));
    }
    menu->addAction(actionCollection()->action("edit_connection"));
    menu->addAction(actionCollection()->action("delete_connection"));
    menu->exec(QCursor::pos());
}

void ConnectionEditor::slotItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    // qCDebug(PLASMA_NM) << "Clicked item" << index.data(NetworkModel::UuidRole).toString();

    if (index.parent().isValid()) { // category
        actionCollection()->action("edit_connection")->setEnabled(false);
        actionCollection()->action("delete_connection")->setEnabled(false);
        actionCollection()->action("export_vpn")->setEnabled(false);
        actionCollection()->action("export_vpn")->setEnabled(false);
    } else {                       //connection
        actionCollection()->action("edit_connection")->setEnabled(true);
        actionCollection()->action("delete_connection")->setEnabled(true);
        const bool isVpn = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(index.data(NetworkModel::TypeRole).toUInt()) ==
                           NetworkManager::ConnectionSettings::Vpn;
        actionCollection()->action("export_vpn")->setEnabled(isVpn);
    }
}

void ConnectionEditor::slotItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    // qCDebug(PLASMA_NM) << "Double clicked item" << index.data(NetworkModel::UuidRole).toString();

    if (index.parent().isValid()) { // category
        // qCDebug(PLASMA_NM) << "double clicked on the root item which is not editable";
        return;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(index.data(NetworkModel::UuidRole).toString());

    QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(connection->settings(), false);

    if (editor->exec() == QDialog::Accepted) {
        m_handler->updateConnection(connection, editor->setting());
    }

    if (editor) {
        editor->deleteLater();
    }
}

void ConnectionEditor::importSecretsFromPlainTextFiles()
{
    const QString secretsDirectory = QStandardPaths::locate(QStandardPaths::DataLocation, "networkmanagement/secrets/", QStandardPaths::LocateDirectory);
    QDir dir(secretsDirectory);
    if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
        QMap<QString, QMap<QString, QString > > resultingMap;
        foreach (const QString & file, dir.entryList(QDir::Files)) {
            KConfig config(secretsDirectory % file, KConfig::SimpleConfig);
            foreach (const QString & groupName, config.groupList()) {
                KConfigGroup group = config.group(groupName);
                QMap<QString, QString> map = group.entryMap();
                if (!map.isEmpty()) {
                    const QString entry = file % ';' % groupName;
                    resultingMap.insert(entry, map);
                }
            }
        }

        storeSecrets(resultingMap);
    }
}

void ConnectionEditor::storeSecrets(const QMap< QString, QMap< QString, QString > >& map)
{
    if (KWallet::Wallet::isEnabled()) {
        KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);

        if (!wallet || !wallet->isOpen()) {
            return;
        }

        const QString folderName(QStringLiteral("Network Management"));

        if (!wallet->hasFolder(folderName)) {
            wallet->createFolder(folderName);
        }

        if (wallet->hasFolder(folderName) && wallet->setFolder(folderName)) {
            int count = 0;
            foreach (const QString & entry, map.keys()) {
                QString connectionUuid = entry.split(';').first();
                connectionUuid.replace('{',"").replace('}',"");
                NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(connectionUuid);

                if (connection) {
                    wallet->writeMap(entry, map.value(entry));
                    ++count;
                }
            }
        }
    }
}

void ConnectionEditor::importVpn()
{
    // get the list of supported extensions
    const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");
    QString extensions;
    foreach (const KService::Ptr &service, services) {
        VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
        if (vpnPlugin) {
            extensions += vpnPlugin->supportedFileExtensions() % QStringLiteral(" ");
            delete vpnPlugin;
        }
    }

    const QString filename = QFileDialog::getOpenFileName(this, i18n("Import VPN Connection"), QDir::homePath(), extensions.simplified());
    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        const QString ext = QStringLiteral("*.") % fi.suffix();
        qCDebug(PLASMA_NM) << "Importing VPN connection " << filename << "extension:" << ext;

        foreach (const KService::Ptr &service, services) {
            VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
            if (vpnPlugin && vpnPlugin->supportedFileExtensions().contains(ext)) {
                qCDebug(PLASMA_NM) << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String).toString();

                NMVariantMapMap connection = vpnPlugin->importConnectionSettings(filename);

                // qCDebug(PLASMA_NM) << "Raw connection:" << connection;

                NetworkManager::ConnectionSettings connectionSettings;
                connectionSettings.fromMap(connection);
                connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

                // qCDebug(PLASMA_NM) << "Converted connection:" << connectionSettings;

                m_handler->addConnection(connectionSettings.toMap());
                // qCDebug(PLASMA_NM) << "Adding imported connection under id:" << conId;

                if (connection.isEmpty()) { // the "positive" part will arrive with connectionAdded
                    m_editor->messageWidget->animatedShow();
                    m_editor->messageWidget->setMessageType(KMessageWidget::Error);
                    m_editor->messageWidget->setText(i18n("Importing VPN connection %1 failed\n%2", fi.fileName(), vpnPlugin->lastErrorMessage()));
                    QTimer::singleShot(5000, m_editor->messageWidget, SLOT(animatedHide()));
                } else {
                    delete vpnPlugin;
                    break; // stop iterating over the plugins if the import produced at least some output
                }

                delete vpnPlugin;
            }
        }
    }
}

void ConnectionEditor::exportVpn()
{
    const QModelIndex currentIndex = m_editor->connectionsWidget->currentIndex();

    if (!currentIndex.isValid() || currentIndex.parent().isValid()) {
        return;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(currentIndex.data(NetworkModel::UuidRole).toString());
    if (!connection) {
        return;
    }

    NetworkManager::ConnectionSettings::Ptr connSettings = connection->settings();

    if (connSettings->connectionType() != NetworkManager::ConnectionSettings::Vpn)
        return;

    NetworkManager::VpnSetting::Ptr vpnSetting = connSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();

    qCDebug(PLASMA_NM) << "Exporting VPN connection" << connection->name() << "type:" << vpnSetting->serviceType();

    QString error;
    VpnUiPlugin * vpnPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QStringLiteral("PlasmaNetworkManagement/VpnUiPlugin"),
                                                                                       QStringLiteral("[X-NetworkManager-Services]=='%1'").arg(vpnSetting->serviceType()),
                                                                                       this, QVariantList(), &error);

    if (vpnPlugin) {
        if (vpnPlugin->suggestedFileName(connSettings).isEmpty()) { // this VPN doesn't support export
            m_editor->messageWidget->animatedShow();
            m_editor->messageWidget->setMessageType(KMessageWidget::Error);
            m_editor->messageWidget->setText(i18n("Export is not supported by this VPN type"));
            QTimer::singleShot(5000, m_editor->messageWidget, SLOT(animatedHide()));
            return;
        }

        const QString url = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + vpnPlugin->suggestedFileName(connSettings);
        const QString filename = QFileDialog::getSaveFileName(this, i18n("Export VPN Connection"), url, vpnPlugin->supportedFileExtensions());
        if (!filename.isEmpty()) {
            if (!vpnPlugin->exportConnectionSettings(connSettings, filename)) {
                m_editor->messageWidget->animatedShow();
                m_editor->messageWidget->setMessageType(KMessageWidget::Error);
                m_editor->messageWidget->setText(i18n("Exporting VPN connection %1 failed\n%2", connection->name(), vpnPlugin->lastErrorMessage()));
                QTimer::singleShot(5000, m_editor->messageWidget, SLOT(animatedHide()));
            } else {
                m_editor->messageWidget->animatedShow();
                m_editor->messageWidget->setMessageType(KMessageWidget::Positive);
                m_editor->messageWidget->setText(i18n("VPN connection %1 exported successfully", connection->name()));
                QTimer::singleShot(5000, m_editor->messageWidget, SLOT(animatedHide()));
            }
        }
        delete vpnPlugin;
    } else {
        qCWarning(PLASMA_NM) << "Error getting VpnUiPlugin for export:" << error;
    }
}
