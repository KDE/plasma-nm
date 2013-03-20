/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#include <QtGui/QTreeWidgetItem>

#include <KLocale>

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/activeconnection.h>

using namespace NetworkManager;

ConnectionEditor::ConnectionEditor(QWidget* parent, Qt::WindowFlags flags):
    QMainWindow(parent, flags),
    m_editor(new Ui::ConnectionEditor)
{
    m_editor->setupUi(this);
    m_editor->addButton->setIcon(KIcon("list-add"));
    m_editor->editButton->setIcon(KIcon("configure"));
    m_editor->deleteButton->setIcon(KIcon("edit-delete"));

    m_menu = new QMenu(this);
    m_menu->setSeparatorsCollapsible(false);

    QAction * action = m_menu->addSeparator();
    action->setText(i18n("Hardware"));

    action = new QAction(i18n("DSL"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Adsl);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("InfiniBand"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Infiniband);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("Mobile Broadband"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Gsm);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("Wired"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Wired);
    m_menu->addAction(action);
    action = new QAction(i18n("Wireless"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Wireless);
    m_menu->addAction(action);
    action = new QAction(i18n("WiMAX"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Wimax);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);

    action = m_menu->addSeparator();
    action->setText(i18n("Virtual"));

    action = new QAction(i18n("Bond"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Bond);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("Bridge"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Bridge);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("VLAN"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vlan);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);

    action = m_menu->addSeparator();
    action->setText(i18n("VPN"));

    action = new QAction(i18n("Cisco AnyConnect Compatible VPN (openconnect)"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vpn);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("Cisco VPN (vpnc)"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vpn);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("IPSec based VPN"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vpn);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("Layer 2 Tunneling Protocol (l2tp)"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vpn);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("OpenVPN"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vpn);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);
    action = new QAction(i18n("Point-to-Point Tunneling Protocol VPN (PPTP)"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Vpn);
    // TODO: disabled for now
    action->setDisabled(true);
    m_menu->addAction(action);

    m_editor->addButton->setMenu(m_menu);

    m_editor->connectionsWidget->setSortingEnabled(false);
    initializeConnections();
    m_editor->connectionsWidget->setSortingEnabled(true);

    connect(m_editor->connectionsWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            SLOT(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(m_menu, SIGNAL(triggered(QAction*)),
            SLOT(addConnection(QAction*)));
    connect(m_editor->editButton, SIGNAL(clicked()),
            SLOT(editConnection()));
    connect(m_editor->connectionsWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            SLOT(editConnection()));
}

ConnectionEditor::~ConnectionEditor()
{
}

void ConnectionEditor::initializeConnections()
{
    m_editor->connectionsWidget->clear();

    QStringList actives;

    foreach(NetworkManager::ActiveConnection * active, NetworkManager::activeConnections()) {
        actives << active->connection()->uuid();
    }

    foreach (Settings::Connection * con, Settings::listConnections()) {
        Settings::ConnectionSettings * settings = new Settings::ConnectionSettings();
        settings->fromMap(con->settings());

        const QString name = settings->id();
        const QString type = Settings::ConnectionSettings::typeAsString(settings->connectionType());

        // Can't continue if name or type are empty
        if (name.isEmpty() || type.isEmpty()) {
            continue;
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
        connectionItem->setData(1, ConnectionItem::ConnectionLastUsedRole, lastUsed);

        m_editor->connectionsWidget->resizeColumnToContents(0);

        delete settings;
    }
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
    Settings::ConnectionSettings::ConnectionType type = (Settings::ConnectionSettings::ConnectionType) action->data().toUInt();
    Settings::ConnectionSettings * newConnection = new Settings::ConnectionSettings(type);

    ConnectionDetailEditor * editor = new ConnectionDetailEditor(newConnection, this);
    if (editor->exec() == QDialog::Accepted) {
        //TODO
        newConnection->printSetting();
    }

    delete newConnection;
}

void ConnectionEditor::editConnection()
{
    QTreeWidgetItem * currentItem = m_editor->connectionsWidget->currentItem();

    if (currentItem->data(0, Qt::UserRole).toString() != "connection") {
        qDebug() << "clicked on the root item which is not editable";
        return;
    }

    Settings::Connection * connection = Settings::findConnectionByUuid(currentItem->data(0, ConnectionItem::ConnectionIdRole).toString());
    Settings::ConnectionSettings * connectionSetting = new Settings::ConnectionSettings();
    connectionSetting->fromMap(connection->settings());

    ConnectionDetailEditor * editor = new ConnectionDetailEditor(connectionSetting, this);
    if (editor->exec() == QDialog::Accepted) {
        //TODO
        connectionSetting->printSetting();
    }

    delete connectionSetting;
}
