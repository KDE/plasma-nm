/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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
#include "ui_connectioneditor.h"
#include "connectionitem.h"
#include "connectiontypeitem.h"
#include "connectiondetaileditor.h"
#include "mobileconnectionwizard.h"

#include <QtGui/QTreeWidgetItem>

#include <KActionCollection>
#include <KLocale>
#include <KMessageBox>
#include <KService>
#include <KServiceTypeTrader>
#include <KStandardAction>
#include <KAction>
#include <KAboutApplicationDialog>
#include <KAboutData>
#include <KAcceleratorManager>
#include <KConfig>
#include <KConfigGroup>
#include <KWallet/Wallet>

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ActiveConnection>

using namespace NetworkManager;

ConnectionEditor::ConnectionEditor(QWidget* parent, Qt::WindowFlags flags):
    KXmlGuiWindow(parent, flags),
    m_editor(new Ui::ConnectionEditor)
{
    QWidget * tmp = new QWidget(this);
    m_editor->setupUi(tmp);
    setCentralWidget(tmp);

    m_menu = new QMenu(this);
    m_menu->setSeparatorsCollapsible(false);

    QAction * action = m_menu->addSeparator();
    action->setText(i18n("Hardware"));

    // TODO Adsl
    action = new QAction(i18n("DSL"), this);
    action->setData(NetworkManager::ConnectionSettings::Pppoe);
    m_menu->addAction(action);
    action = new QAction(i18n("InfiniBand"), this);
    action->setData(NetworkManager::ConnectionSettings::Infiniband);
    m_menu->addAction(action);
    action = new QAction(i18n("Mobile Broadband..."), this);
    action->setData(NetworkManager::ConnectionSettings::Gsm);
    m_menu->addAction(action);
    action = new QAction(i18n("Wired"), this);
    action->setData(NetworkManager::ConnectionSettings::Wired);
    action->setProperty("shared", false);
    m_menu->addAction(action);
    action = new QAction(i18n("Wired (shared)"), this);
    action->setData(NetworkManager::ConnectionSettings::Wired);
    action->setProperty("shared", true);
    m_menu->addAction(action);
    action = new QAction(i18n("Wireless"), this);
    action->setData(NetworkManager::ConnectionSettings::Wireless);
    action->setProperty("shared", false);
    m_menu->addAction(action);
    action = new QAction(i18n("Wireless (shared)"), this);
    action->setData(NetworkManager::ConnectionSettings::Wireless);
    action->setProperty("shared", true);
    m_menu->addAction(action);
    action = new QAction(i18n("WiMAX"), this);
    action->setData(NetworkManager::ConnectionSettings::Wimax);
    m_menu->addAction(action);

    action = m_menu->addSeparator();
    action->setText(i18n("Virtual"));

    action = new QAction(i18n("Bond"), this);
    action->setData(NetworkManager::ConnectionSettings::Bond);
    m_menu->addAction(action);
    action = new QAction(i18n("Bridge"), this);
    action->setData(NetworkManager::ConnectionSettings::Bridge);
    m_menu->addAction(action);
    action = new QAction(i18n("VLAN"), this);
    action->setData(NetworkManager::ConnectionSettings::Vlan);
    m_menu->addAction(action);

    action = m_menu->addSeparator();
    action->setText(i18n("VPN"));

    const KService::List services = KServiceTypeTrader::self()->query("PlasmaNM/VpnUiPlugin");
    foreach (KService::Ptr service, services) {
        qDebug() << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String);

        action = new QAction(service->name(), this);
        action->setData(NetworkManager::ConnectionSettings::Vpn);
        action->setProperty("type", service->property("X-NetworkManager-Services", QVariant::String));
        m_menu->addAction(action);
    }

    m_editor->addButton->setMenu(m_menu);

    m_importMenu = new QMenu(this);
    m_importMenu->setSeparatorsCollapsible(false);

    action = m_importMenu->addSeparator();
    action->setText(i18n("Secrets"));

    KAction * kAction = new KAction(i18n("From file..."), this);
    kAction->setData(SecretsFromFile);
    kAction->setDisabled(true);
    m_importMenu->addAction(kAction);
    actionCollection()->addAction("import_from_file", kAction);
    kAction = new KAction(i18n("From old applet"), this);
    kAction->setData(SecretsFromApplet);
    m_importMenu->addAction(kAction);
    actionCollection()->addAction("import_from_applet", kAction);

    action = m_importMenu->addSeparator();
    action->setText(i18n("VPN Plugin"));

    kAction = new KAction(i18n("From file..."), this);
    kAction->setData(VpnFromFile);
    kAction->setDisabled(true);
    m_importMenu->addAction(kAction);
    actionCollection()->addAction("import_vpn", kAction);

    m_editor->importButton->setMenu(m_importMenu);

    m_editor->connectionsWidget->setSortingEnabled(false);
    initializeConnections();
    m_editor->connectionsWidget->setSortingEnabled(true);

    connect(m_editor->connectionsWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(m_menu, SIGNAL(triggered(QAction*)),
            SLOT(addConnection(QAction*)));
    connect(m_editor->editButton, SIGNAL(clicked()),
            SLOT(editConnection()));
    connect(m_editor->deleteButton, SIGNAL(clicked()),
            SLOT(removeConnection()));
    connect(m_editor->connectionsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            SLOT(editConnection()));
    connect(m_importMenu, SIGNAL(triggered(QAction*)),
            SLOT(import(QAction*)));
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionAdded(QString)),
            SLOT(connectionAdded(QString)));
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionRemoved(QString)),
            SLOT(connectionRemoved(QString)));

    m_editor->messageWiget->animatedHide();
    m_editor->messageWiget->setCloseButtonVisible(false);
    m_editor->messageWiget->setWordWrap(true);

    KStandardAction::quit(this, SLOT(close()), actionCollection());

    createGUI();

    setAutoSaveSettings();

    KAcceleratorManager::manage(this);
    KAcceleratorManager::manage(m_menu);
    KAcceleratorManager::manage(m_importMenu);

    KConfig config("kde-nm-connection-editor");
    KConfigGroup generalGroup = config.group("General");

    if (generalGroup.isValid()) {
        if (generalGroup.readEntry("FirstStart", true)) {
            if (KWallet::Wallet::isEnabled()) {
                KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);

                if (wallet && wallet->isOpen() && wallet->hasFolder("Network Management")) {
                    if (KMessageBox::questionYesNo(this, i18n("Do you want to import secrets from the old networkmanagement applet?"), i18n("Import secrets"), KStandardGuiItem::add(),
                                   KStandardGuiItem::no()) == KMessageBox::Yes) {
                        importFromApplet();
                    }
                }

                if (wallet) {
                    delete wallet;
                }
            }
            generalGroup.writeEntry("FirstStart", false);
        }
    }
}

ConnectionEditor::~ConnectionEditor()
{
    delete m_editor;
}

void ConnectionEditor::initializeConnections()
{
    m_editor->connectionsWidget->clear();

    foreach (const Connection::Ptr &con, NetworkManager::listConnections()) {
        if (con->settings()->isSlave())
            continue;
        insertConnection(con);
    }
}

void ConnectionEditor::insertConnection(const NetworkManager::Connection::Ptr &connection)
{
    connect(connection.data(), SIGNAL(updated()), SLOT(connectionUpdated()));

    ConnectionSettings::Ptr settings = connection->settings();

    const QString name = settings->id();
    QString type = ConnectionSettings::typeAsString(settings->connectionType());
    if (type == "gsm" || type == "cdma")
        type = "mobile"; // cdma+gsm meta category

    // Can't continue if name or type are empty
    if (name.isEmpty() || type.isEmpty()) {
        return;
    }

    QStringList actives;

    foreach(const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        if (active->state() == NetworkManager::ActiveConnection::Activated) {
            actives << active->connection()->uuid();
        }
    }

    const bool active = actives.contains(settings->uuid());
    const QString lastUsed = formatDateRelative(settings->timestamp());

    QStringList params;
    params << name;
    params << lastUsed;

    // Create a root item if this type doesn't exist
    if (!findTopLevelItem(type)) {
        qDebug() << "creating toplevel item" << type;
        (void) new ConnectionTypeItem(m_editor->connectionsWidget, type);
    }

    QTreeWidgetItem * item = findTopLevelItem(type);
    ConnectionItem * connectionItem = new ConnectionItem(item, params, active);
    connectionItem->setData(0, Qt::UserRole, "connection");
    connectionItem->setData(0, ConnectionItem::ConnectionIdRole, settings->uuid());
    connectionItem->setData(0, ConnectionItem::ConnectionPathRole, connection->path());
    connectionItem->setData(1, ConnectionItem::ConnectionLastUsedRole, settings->timestamp());

    m_editor->connectionsWidget->resizeColumnToContents(0);
}


QString ConnectionEditor::formatDateRelative(const QDateTime & lastUsed) const
{
    QString lastUsedText;
    if (lastUsed.isValid()) {
        QDateTime now = QDateTime::currentDateTime();
        if (lastUsed.daysTo(now) == 0 ) {
            int secondsAgo = lastUsed.secsTo(now);
            if (secondsAgo < (60 * 60 )) {
                int minutesAgo = secondsAgo / 60;
                lastUsedText = i18ncp(
                        "Label for last used time for a network connection used in the last hour, as the number of minutes since usage",
                        "One minute ago",
                        "%1 minutes ago",
                        minutesAgo);
            } else {
                int hoursAgo = secondsAgo / (60 * 60);
                lastUsedText = i18ncp(
                        "Label for last used time for a network connection used in the last day, as the number of hours since usage",
                        "One hour ago",
                        "%1 hours ago",
                        hoursAgo);
            }
        } else if (lastUsed.daysTo(now) == 1) {
            lastUsedText = i18nc("Label for last used time for a network connection used the previous day", "Yesterday");
        } else {
            lastUsedText = KGlobal::locale()->formatDate(lastUsed.date(), KLocale::ShortDate);
        }
    } else {
        lastUsedText =  i18nc("Label for last used time for a "
                              "network connection that has never been used", "Never");
    }
    return lastUsedText;
}

QTreeWidgetItem* ConnectionEditor::findTopLevelItem(const QString& type)
{
    QTreeWidgetItemIterator it(m_editor->connectionsWidget);

    while (*it) {
        if ((*it)->data(0, Qt::UserRole).toString() == type) {
            qDebug() << "found:" << type;
            return (*it);
        }
        ++it;
    }

    qWarning() << "didn't find type" << type;
    return 0;
}

void ConnectionEditor::currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (!current) {
        return;
    }

    qDebug() << "Current item" << current->text(0) << "type:" << current->data(0, Qt::UserRole).toString();

    if (current->data(0, Qt::UserRole).toString() == "connection") {
        m_editor->editButton->setEnabled(true);
        m_editor->deleteButton->setEnabled(true);
    } else {
        m_editor->editButton->setDisabled(true);
        m_editor->deleteButton->setDisabled(true);
    }
}

void ConnectionEditor::addConnection(QAction* action)
{
    qDebug() << "ADDING new connection" << action->data().toUInt();
    const QString vpnType = action->property("type").toString();
    qDebug() << "VPN type:" << vpnType;

    ConnectionSettings::ConnectionType type = static_cast<ConnectionSettings::ConnectionType>(action->data().toUInt());

    if (type == NetworkManager::ConnectionSettings::Gsm) { // launch the mobile broadband wizard, both gsm/cdma
        QWeakPointer<MobileConnectionWizard> wizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Unknown, this);
        if (wizard.data()->exec() == QDialog::Accepted && wizard.data()->getError() == MobileProviders::Success) {
            qDebug() << "Mobile broadband wizard finished:" << wizard.data()->type() << wizard.data()->args();
            ConnectionDetailEditor * editor = new ConnectionDetailEditor(wizard.data()->type(), wizard.data()->args(), this);
            editor->exec();
        }
        if (wizard) {
            wizard.data()->deleteLater();
        }
    } else {
        bool shared = false;
        if (type == ConnectionSettings::Wired || type == ConnectionSettings::Wireless) {
            shared = action->property("shared").toBool();
        }

        QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(type, this, vpnType, shared);
        editor->exec();

        if (editor) {
            editor->deleteLater();
        }
    }
}

void ConnectionEditor::editConnection()
{
    QTreeWidgetItem * currentItem = m_editor->connectionsWidget->currentItem();

    if (currentItem->data(0, Qt::UserRole).toString() != "connection") {
        qDebug() << "clicked on the root item which is not editable";
        return;
    }

    Connection::Ptr connection = NetworkManager::findConnectionByUuid(currentItem->data(0, ConnectionItem::ConnectionIdRole).toString());

    if (!connection) {
        return;
    }

    QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(connection->settings(), this);
    editor->exec();

    if (editor) {
        editor->deleteLater();
    }
}

void ConnectionEditor::removeConnection()
{
    QTreeWidgetItem * currentItem = m_editor->connectionsWidget->currentItem();

    if (currentItem->data(0, Qt::UserRole).toString() != "connection") {
        qDebug() << "clicked on the root item which is not removable";
        return;
    }

    Connection::Ptr connection = NetworkManager::findConnectionByUuid(currentItem->data(0, ConnectionItem::ConnectionIdRole).toString());

    if (!connection) {
        return;
    }

    if (KMessageBox::questionYesNo(this, i18n("Do you want to remove the connection '%1'?", connection->name()), i18n("Remove Connection"), KStandardGuiItem::remove(),
                                   KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
            == KMessageBox::Yes) {
        foreach (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
            NetworkManager::ConnectionSettings::Ptr settings = con->settings();
            if (settings->master() == connection->uuid()) {
                con->remove();
            }
        }
        connection->remove();
    }
}

void ConnectionEditor::import(QAction * action)
{
    ImportType type = static_cast<ImportType>(action->data().toUInt());

    if (type == SecretsFromFile) {
        // TODO
    } else if (type == SecretsFromApplet) {
        importFromApplet();
    } else if (type == VpnFromFile) {
        // TODO
    }
}

void ConnectionEditor::importFromApplet()
{
    if (KWallet::Wallet::isEnabled()) {
        KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);

        if (!wallet || !wallet->isOpen()) {
            return;
        }

        QMap<QString, QMap<QString, QString> > resultingMap;

        if (wallet->hasFolder("Network Management") && wallet->setFolder("Network Management")) {
            QMap<QString, QString> tmpMap;

            foreach (const QString & entry, wallet->entryList()) {
                wallet->readMap(entry, tmpMap);

                // Do not recover empty values
                QMap<QString, QString> map;
                foreach (const QString & key, tmpMap.keys()) {
                    if (!tmpMap.value(key).isEmpty()) {
                        // Fix VPN secrets
                        if (key == "VpnSecrets") {
                            QString value = tmpMap.value(key);
                            value.replace("%SEP%", "?SEP?");
                            map.insert("secrets", value);
                        } else {
                            map.insert(key, tmpMap.value(key));
                        }
                    }
                }

                if (!map.isEmpty()) {
                    // Fix UUID
                    QString correctEntry = entry;
                    correctEntry.replace('{',"").replace('}',"");
                    resultingMap.insert(correctEntry, map);
                }
            }
        } else {
            m_editor->messageWiget->setMessageType(KMessageWidget::Error);
            m_editor->messageWiget->setText(i18n("Could not find previous applet for recovering secrets"));
        }

        if (!wallet->hasFolder("plasma-nm")) {
            wallet->createFolder("plasma-nm");
        }

        if (wallet->hasFolder("plasma-nm") && wallet->setFolder("plasma-nm")) {
            int count = 0;
            foreach (const QString & entry, resultingMap.keys()) {
                QString connectionUuid = entry.split(';').first();
                connectionUuid.replace('{',"").replace('}',"");
                NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(connectionUuid);

                if (connection) {
                    wallet->writeMap(entry, resultingMap.value(entry));
                    ++count;
                }
            }

            m_editor->messageWiget->setMessageType(KMessageWidget::Positive);
            m_editor->messageWiget->setText(i18n("Imported %1 secrets").arg(count));
        } else {
            m_editor->messageWiget->setMessageType(KMessageWidget::Error);
            m_editor->messageWiget->setText(i18n("Could not open KWallet folder for storing secrets"));
        }

        if (wallet) {
            delete wallet;
        }
    } else {
        m_editor->messageWiget->setMessageType(KMessageWidget::Error);
        m_editor->messageWiget->setText(i18n("KWallet is not enabled"));
    }

    m_editor->messageWiget->animatedShow();
    QTimer::singleShot(5000, m_editor->messageWiget, SLOT(animatedHide()));
}

void ConnectionEditor::connectionAdded(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        return;
    }

    if (con->settings()->isSlave())
        return;

    m_editor->messageWiget->animatedShow();
    m_editor->messageWiget->setMessageType(KMessageWidget::Positive);
    m_editor->messageWiget->setText(i18n("Connection %1 has been added").arg(con->name()));
    QTimer::singleShot(5000, m_editor->messageWiget, SLOT(animatedHide()));

    insertConnection(con);
}

void ConnectionEditor::connectionRemoved(const QString& connection)
{
    QTreeWidgetItemIterator it(m_editor->connectionsWidget);

    while (*it) {
        if ((*it)->data(0, ConnectionItem::ConnectionPathRole).toString() == connection) {
            m_editor->messageWiget->animatedShow();
            m_editor->messageWiget->setMessageType(KMessageWidget::Information);
            m_editor->messageWiget->setText(i18n("Connection %1 has been removed").arg((*it)->text(0)));
            QTimer::singleShot(5000, m_editor->messageWiget, SLOT(animatedHide()));
            QTreeWidgetItem * parent = (*it)->parent();
            delete (*it);
            if (!parent->childCount()) {
                delete parent;
            }

            break;
        }
        ++it;
    }
}

void ConnectionEditor::connectionUpdated()
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(qobject_cast<NetworkManager::Connection*>(sender())->path());

    QTreeWidgetItemIterator it(m_editor->connectionsWidget);

    while (*it) {
        if ((*it)->data(0, ConnectionItem::ConnectionIdRole).toString() == connection->uuid()) {
            (*it)->setText(0, connection->name());
            m_editor->messageWiget->animatedShow();
            m_editor->messageWiget->setMessageType(KMessageWidget::Information);
            m_editor->messageWiget->setText(i18n("Connection %1 has been updated").arg(connection->name()));
            QTimer::singleShot(5000, m_editor->messageWiget, SLOT(animatedHide()));
            break;
        }
        ++it;
    }
}

void ConnectionEditor::aboutDialog()
{
    KAboutApplicationDialog * dlg = new KAboutApplicationDialog(KGlobal::mainComponent().aboutData(), this);
    dlg->show();
}
