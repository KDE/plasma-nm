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
#include "connectioneditordialog.h"

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
#include <QMenuBar>

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
#include <KSharedConfig>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/GsmSetting>
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
    m_editor->connectionsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_editor->connectionsWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    m_editor->messageWidget->hide();
    m_editor->messageWidget->setCloseButtonVisible(false);
    m_editor->messageWidget->setWordWrap(true);

    m_editor->ktreewidgetsearchline->lineEdit()->setPlaceholderText(i18n("Type here to search connections..."));

    initializeConnections();
    initializeMenu();

    KSharedConfigPtr config = KSharedConfig::openConfig();
    KConfigGroup grp(config, "ConnectionsWidget");
    m_editor->connectionsWidget->header()->restoreState(grp.readEntry<QByteArray>("state", QByteArray()));

    KConfigGroup grp2(config, "General");
    if (grp2.isValid()) {
        if (grp2.readEntry("FirstStart", true)) {
            importSecretsFromPlainTextFiles();
        }
        grp2.writeEntry("FirstStart", false);
    }

    connect(m_editor->connectionsWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ConnectionEditor::slotSelectionChanged);
    connect(m_editor->connectionsWidget, &QTreeView::doubleClicked, this, &ConnectionEditor::slotItemDoubleClicked);
    connect(m_editor->connectionsWidget, &QTreeView::customContextMenuRequested, this, &ConnectionEditor::slotContextMenuRequested);
    connect(m_menu->menu(), &QMenu::triggered, this, static_cast<void (ConnectionEditor::*)(QAction*)>(&ConnectionEditor::addConnection));
    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionAdded, this, &ConnectionEditor::connectionAdded);

    QLoggingCategory::setFilterRules(QStringLiteral("plasma-nm.debug = false"));
    QLoggingCategory::setFilterRules(QStringLiteral("plasma-nm.warning = true"));
}

ConnectionEditor::~ConnectionEditor()
{
    KConfigGroup grp(KSharedConfig::openConfig(), "ConnectionsWidget");
    grp.writeEntry("state", m_editor->connectionsWidget->header()->saveState());
    delete m_editor;
}

void ConnectionEditor::activateAndRaise()
{
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
}

void ConnectionEditor::initializeMenu()
{
    m_menu = new KActionMenu(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add"), this);
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

    m_menu->menu()->addSection(i18n("VPN"));

    KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");

    std::sort(services.begin(), services.end(), [] (const KService::Ptr &left, const KService::Ptr &right)
    {
        return QString::localeAwareCompare(left->name(), right->name()) <= 0;
    });

    Q_FOREACH (const KService::Ptr & service, services) {
        qCDebug(PLASMA_NM) << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String).toString();

        const QString vpnSubType = service->property("X-NetworkManager-Services-Subtype", QVariant::String).toString();
        action = new QAction(service->name(), this);
        action->setData(NetworkManager::ConnectionSettings::Vpn);
        action->setProperty("type", service->property("X-NetworkManager-Services", QVariant::String));
        if (!vpnSubType.isEmpty()) {
            action->setProperty("subtype", vpnSubType);
        }
        m_menu->addAction(action);
    }

    m_menu->menu()->addSeparator();

    action = new QAction(i18n("Import VPN..."), this);
    action->setData(NetworkManager::ConnectionSettings::Vpn);
    action->setProperty("type", "imported");
    m_menu->addAction(action);

    actionCollection()->addAction(QStringLiteral("add_connection"), m_menu);

    QAction * kAction = new QAction(QIcon::fromTheme(QStringLiteral("network-connect")), i18n("Connect"), this);
    kAction->setEnabled(false);
    connect(kAction, &QAction::triggered, this, &ConnectionEditor::connectConnection);
    actionCollection()->addAction(QStringLiteral("connect_connection"), kAction);

    kAction = new QAction(QIcon::fromTheme(QStringLiteral("network-disconnect")), i18n("Disconnect"), this);
    kAction->setEnabled(false);
    connect(kAction, &QAction::triggered, this, &ConnectionEditor::disconnectConnection);
    actionCollection()->addAction(QStringLiteral("disconnect_connection"), kAction);

    kAction = new QAction(QIcon::fromTheme(QStringLiteral("configure")), i18n("Edit..."), this);
    kAction->setEnabled(false);
    connect(kAction, &QAction::triggered, this, &ConnectionEditor::editConnection);
    actionCollection()->addAction(QStringLiteral("edit_connection"), kAction);

    kAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Delete"), this);
    kAction->setEnabled(false);
    connect(kAction, &QAction::triggered, this, &ConnectionEditor::removeConnection);
    actionCollection()->addAction(QStringLiteral("delete_connection"), kAction);
    actionCollection()->setDefaultShortcut(kAction, QKeySequence::Delete);

    kAction = new QAction(QIcon::fromTheme(QStringLiteral("document-import")), i18n("Import VPN..."), this);
    actionCollection()->addAction(QStringLiteral("import_vpn"), kAction);
    connect(kAction, &QAction::triggered, this, &ConnectionEditor::importVpn);

    kAction = new QAction(QIcon::fromTheme(QStringLiteral("document-export")), i18n("Export VPN..."), this);
    actionCollection()->addAction(QStringLiteral("export_vpn"), kAction);
    kAction->setEnabled(false);
    connect(kAction, &QAction::triggered, this, &ConnectionEditor::exportVpn);

    KStandardAction::showMenubar(menuBar(), SLOT(setVisible(bool)), actionCollection());
    KStandardAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());
    KStandardAction::quit(this, SLOT(close()), actionCollection());
    setupGUI(QSize(480, 480));

    setAutoSaveSettings();

    KAcceleratorManager::manage(this);
}

void ConnectionEditor::addConnection(QAction* action)
{
    NetworkManager::ConnectionSettings::ConnectionType type = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(action->data().toUInt());
    const QString vpnType = action->property("type").toString();
    const QString vpnSubType = action->property("subtype").toString();

    // qCDebug(PLASMA_NM) << "Adding new connection of type " << type << " and subtype " << vpnSubType;
    if (type == NetworkManager::ConnectionSettings::Vpn && vpnType == "imported") {
        importVpn();
    } else if (type == NetworkManager::ConnectionSettings::Gsm) { // launch the mobile broadband wizard, both gsm/cdma
#if WITH_MODEMMANAGER_SUPPORT
        QPointer<MobileConnectionWizard> wizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Unknown, this);
        connect(wizard.data(), &MobileConnectionWizard::accepted,
                [wizard, this] () {
                    if (wizard->getError() == MobileProviders::Success) {
                        qCDebug(PLASMA_NM) << "Mobile broadband wizard finished:" << wizard->type() << wizard->args();

                        if (wizard->args().count() == 2) {
                            QVariantMap tmp = qdbus_cast<QVariantMap>(wizard->args().value(1));

#if 0                       // network IDs are not used yet and seem to break the setting
                            if (args.count() == 3) { // gsm specific
                            QStringList networkIds = args.value(1).toStringList();
                            if (!networkIds.isEmpty())
                                tmp.insert("network-id", networkIds.first());
                            }
#endif
                            NetworkManager::ConnectionSettings::Ptr connectionSettings;
                            connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(wizard->type()));
                            connectionSettings->setId(wizard->args().value(0).toString());
                            if (wizard->type() == NetworkManager::ConnectionSettings::Gsm) {
                                NetworkManager::GsmSetting::Ptr gsmSetting = connectionSettings->setting(NetworkManager::Setting::Gsm).staticCast<NetworkManager::GsmSetting>();
                                gsmSetting->fromMap(tmp);
                                gsmSetting->setPasswordFlags(NetworkManager::Setting::NotRequired);
                                gsmSetting->setPinFlags(NetworkManager::Setting::NotRequired);
                            } else if (wizard->type() == NetworkManager::ConnectionSettings::Cdma) {
                                connectionSettings->setting(NetworkManager::Setting::Cdma)->fromMap(tmp);
                            } else {
                                qCWarning(PLASMA_NM) << Q_FUNC_INFO << "Unhandled setting type";
                            }
                            // Generate new UUID
                            connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
                            addConnection(connectionSettings);
                        } else {
                            qCWarning(PLASMA_NM) << Q_FUNC_INFO << "Unexpected number of args to parse";
                        }
                    }
                });
        connect(wizard.data(), &MobileConnectionWizard::finished,
                [wizard] () {
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
                data.insert(QLatin1String("protocol"), vpnSubType);
                vpnSetting->setData(data);
            }
            // qCDebug(PLASMA_NM) << "VPN type: " << vpnType;
        }

        if (type == NetworkManager::ConnectionSettings::Wired || type == NetworkManager::ConnectionSettings::Wireless) {
            bool shared = action->property("shared").toBool();
            if (shared) {
                if (type == NetworkManager::ConnectionSettings::Wireless) {
                    NetworkManager::WirelessSetting::Ptr wifiSetting = connectionSettings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                    wifiSetting->setMode(NetworkManager::WirelessSetting::Adhoc);
                    wifiSetting->setSsid(i18n("my_shared_connection").toUtf8());

                    Q_FOREACH (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
                        if (device->type() == NetworkManager::Device::Wifi) {
                            NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
                            if (wifiDev) {
                                if (wifiDev->wirelessCapabilities().testFlag(NetworkManager::WirelessDevice::ApCap)) {
                                    wifiSetting->setMode(NetworkManager::WirelessSetting::Ap);
                                    wifiSetting->setMacAddress(NetworkManager::macAddressFromString(wifiDev->hardwareAddress()));
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

void ConnectionEditor::addConnection(const NetworkManager::ConnectionSettings::Ptr& connectionSettings)
{
    QPointer<ConnectionEditorDialog> editor = new ConnectionEditorDialog(connectionSettings);
    connect(editor.data(), &ConnectionEditorDialog::accepted,
            [connectionSettings, editor, this] () {
                qCDebug(PLASMA_NM) << "Adding new connection of type " << NetworkManager::ConnectionSettings::typeAsString(connectionSettings->connectionType());
                m_handler->addConnection(editor->setting());
            });
    connect(editor.data(), &ConnectionEditorDialog::finished,
            [editor] () {
                if (editor) {
                    editor->deleteLater();
                }
            });
    editor->setModal(true);
    editor->show();
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
    QTimer::singleShot(5000, m_editor->messageWidget, &KMessageWidget::animatedHide);
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

void ConnectionEditor::dataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector< int >& roles)
{
    Q_UNUSED(roles);
    QModelIndex currentIndex = m_editor->connectionsWidget->currentIndex();
    if (currentIndex.isValid()) {
        for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
            QModelIndex index = m_editor->connectionsWidget->model()->index(i, 0);
            if (index.isValid() && index == currentIndex) {
                // Re-check enabled/disabled actions
                slotSelectionChanged();
                break;
            }
        }
    }
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
    connect(filterModel, &EditorProxyModel::dataChanged, this, &ConnectionEditor::dataChanged);

    m_editor->connectionsWidget->setModel(filterModel);
    m_editor->ktreewidgetsearchline->setProxy(filterModel);
    m_editor->connectionsWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ConnectionEditor::removeConnection()
{
    QModelIndexList selected = m_editor->connectionsWidget->selectionModel()->selectedRows();

    QList<NetworkManager::Connection::Ptr> connections;
    QString connectionNames = QLatin1String("\n");
    bool removeConnections;

    Q_FOREACH( const QModelIndex& currentIndex, selected ) {

        if (!currentIndex.isValid() || currentIndex.parent().isValid()) {
            return;
        }

        NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(currentIndex.data(NetworkModel::UuidRole).toString());

        if (connection) {
            connections.append(connection);
            connectionNames.append(QStringLiteral("• %1\n").arg(connection->name()));
        }
    }

    connectionNames.chop(1);

    if (connections.size() == 1) {
        removeConnections = KMessageBox::questionYesNo(this, i18n("Do you want to remove the connection '%1'?", connectionNames), i18n("Remove Connection"),
                                   KStandardGuiItem::remove(), KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
        == KMessageBox::Yes;
    } else if (connections.size() > 1) {
        removeConnections = KMessageBox::questionYesNo(this, i18n("Do you want to remove the following connections: %1", connectionNames), i18n("Remove Connections"),
                                                       KStandardGuiItem::remove(), KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
        == KMessageBox::Yes;
    }

    Q_FOREACH( NetworkManager::Connection::Ptr connection, connections ) {

        if (removeConnections) {
            Q_FOREACH (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
                NetworkManager::ConnectionSettings::Ptr settings = con->settings();
                if (settings->master() == connection->uuid()) {
                    m_handler->removeConnection(con->path());
                }
            }
            m_handler->removeConnection(connection->path());
        }

    }
}

void ConnectionEditor::slotContextMenuRequested(const QPoint&)
{
    QMenu * menu = new QMenu(this);

    QModelIndex index = m_editor->connectionsWidget->currentIndex();
    const bool isActive = (NetworkManager::ActiveConnection::State)index.data(NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
    const bool isAvailable = (NetworkModelItem::ItemType)index.data(NetworkModel::ItemTypeRole).toUInt() == NetworkModelItem::AvailableConnection;

    if (isAvailable && !isActive) {
        menu->addAction(actionCollection()->action(QStringLiteral("connect_connection")));
    } else if (isAvailable && isActive) {
        menu->addAction(actionCollection()->action(QStringLiteral("disconnect_connection")));
    }
    menu->addAction(actionCollection()->action(QStringLiteral("edit_connection")));
    menu->addAction(actionCollection()->action(QStringLiteral("delete_connection")));
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->popup(QCursor::pos());
}

void ConnectionEditor::slotSelectionChanged()
{
    QModelIndexList selected = m_editor->connectionsWidget->selectionModel()->selectedRows();

    bool singleSelection = (selected.size() == 1);

    //qCDebug(PLASMA_NM) << "Clicked item" << index.data(NetworkModel::UuidRole).toString();

    if (selected.size() >= 1) {                       //connection
        QModelIndex index = selected.at(0);

        const bool isActive = (NetworkManager::ActiveConnection::State)index.data(NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activated;
        const bool isActivating = (NetworkManager::ActiveConnection::State)index.data(NetworkModel::ConnectionStateRole).toUInt() == NetworkManager::ActiveConnection::Activating;
        const bool isAvailable = (NetworkModelItem::ItemType)index.data(NetworkModel::ItemTypeRole).toUInt() == NetworkModelItem::AvailableConnection;

        actionCollection()->action(QStringLiteral("connect_connection"))->setEnabled(singleSelection && isAvailable && !isActive && !isActivating);
        actionCollection()->action(QStringLiteral("disconnect_connection"))->setEnabled(singleSelection && isAvailable && (isActive || isActivating));
        actionCollection()->action(QStringLiteral("edit_connection"))->setEnabled(singleSelection);
        actionCollection()->action(QStringLiteral("delete_connection"))->setEnabled(true);
        const bool isVpn = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(index.data(NetworkModel::TypeRole).toUInt()) ==
                        NetworkManager::ConnectionSettings::Vpn;
        actionCollection()->action(QStringLiteral("export_vpn"))->setEnabled(singleSelection && isVpn);
    } else { // category
        actionCollection()->action(QStringLiteral("connect_connection"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("disconnect_connection"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("edit_connection"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("delete_connection"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("export_vpn"))->setEnabled(false);
        actionCollection()->action(QStringLiteral("export_vpn"))->setEnabled(false);
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

    QPointer<ConnectionEditorDialog> editor = new ConnectionEditorDialog(connection->settings());
    connect(editor.data(), &ConnectionEditorDialog::accepted,
            [editor, connection, this] () {
                m_handler->updateConnection(connection, editor->setting());
            });
    connect(editor.data(), &ConnectionEditorDialog::finished,
            [editor] () {
                if (editor) {
                    editor->deleteLater();
                }
            });
    editor->setModal(true);
    editor->show();
}

void ConnectionEditor::importSecretsFromPlainTextFiles()
{
    const QString secretsDirectory = QStandardPaths::locate(QStandardPaths::DataLocation, QStringLiteral("networkmanagement/secrets/"), QStandardPaths::LocateDirectory);
    QDir dir(secretsDirectory);
    if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
        QMap<QString, QMap<QString, QString > > resultingMap;
        Q_FOREACH (const QString & file, dir.entryList(QDir::Files)) {
            KConfig config(secretsDirectory % file, KConfig::SimpleConfig);
            Q_FOREACH (const QString & groupName, config.groupList()) {
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
            Q_FOREACH (const QString & entry, map.keys()) {
                QString connectionUuid = entry.split(';').first();
                connectionUuid.remove('{').remove('}');
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
    Q_FOREACH (const KService::Ptr &service, services) {
        VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
        if (vpnPlugin) {
            extensions += vpnPlugin->supportedFileExtensions() % QStringLiteral(" ");
            delete vpnPlugin;
        }
    }

    const QString &filename = QFileDialog::getOpenFileName(this, i18n("Import VPN Connection"), QDir::homePath(), extensions.simplified());
    importVpnAtPath(filename);
}

void ConnectionEditor::importVpnAtPath(const QString &path)
{
    if (!path.isEmpty()) {
        const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");

        QFileInfo fi(path);
        const QString ext = QStringLiteral("*.") % fi.suffix();
        qCDebug(PLASMA_NM) << "Importing VPN connection " << path << "extension:" << ext;

        Q_FOREACH (const KService::Ptr &service, services) {
            VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
            if (vpnPlugin && vpnPlugin->supportedFileExtensions().contains(ext)) {
                qCDebug(PLASMA_NM) << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String).toString();

                NMVariantMapMap connection = vpnPlugin->importConnectionSettings(path);

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
                    QTimer::singleShot(5000, m_editor->messageWidget, &KMessageWidget::animatedHide);
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
            QTimer::singleShot(5000, m_editor->messageWidget, &KMessageWidget::animatedHide);
            return;
        }

        const QString url = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + vpnPlugin->suggestedFileName(connSettings);
        const QString filename = QFileDialog::getSaveFileName(this, i18n("Export VPN Connection"), url, vpnPlugin->supportedFileExtensions());
        if (!filename.isEmpty()) {
            if (!vpnPlugin->exportConnectionSettings(connSettings, filename)) {
                m_editor->messageWidget->animatedShow();
                m_editor->messageWidget->setMessageType(KMessageWidget::Error);
                m_editor->messageWidget->setText(i18n("Exporting VPN connection %1 failed\n%2", connection->name(), vpnPlugin->lastErrorMessage()));
                QTimer::singleShot(5000, m_editor->messageWidget, &KMessageWidget::animatedHide);
            } else {
                m_editor->messageWidget->animatedShow();
                m_editor->messageWidget->setMessageType(KMessageWidget::Positive);
                m_editor->messageWidget->setText(i18n("VPN connection %1 exported successfully", connection->name()));
                QTimer::singleShot(5000, m_editor->messageWidget, &KMessageWidget::animatedHide);
            }
        }
        delete vpnPlugin;
    } else {
        qCWarning(PLASMA_NM) << "Error getting VpnUiPlugin for export:" << error;
    }
}
