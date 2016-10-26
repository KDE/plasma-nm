/*
    Copyright 2014 Lukas Tinkl <ltinkl@redhat.com>

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

#include "teamwidget.h"
#include "ui_team.h"
#include "connectioneditordialog.h"
#include "debug.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QDBusPendingReply>

#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/ConnectionSettings>

#include <KLocalizedString>
#include <KMessageBox>

TeamWidget::TeamWidget(const QString & masterUuid, const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_uuid(masterUuid),
    m_ui(new Ui::TeamWidget)
{
    m_ui->setupUi(this);

    // Action buttons and menu
    m_menu = new QMenu(this);
    QAction * action = new QAction(i18n("Ethernet"), this);
    action->setData(NetworkManager::ConnectionSettings::Wired);
    m_menu->addAction(action);
    action = new QAction(i18n("Infiniband"), this);
    action->setData(NetworkManager::ConnectionSettings::Infiniband);
    m_menu->addAction(action);
    action = new QAction(i18n("Wi-Fi"), this);
    action->setData(NetworkManager::ConnectionSettings::Wireless);
    m_menu->addAction(action);
    action = new QAction(i18n("VLAN"), this);
    action->setData(NetworkManager::ConnectionSettings::Vlan);
    m_menu->addAction(action);
    m_ui->btnAdd->setMenu(m_menu);
    connect(m_menu, &QMenu::triggered, this, &TeamWidget::addTeam);
    connect(m_ui->btnEdit, &QPushButton::clicked, this, &TeamWidget::editTeam);
    connect(m_ui->btnDelete, &QPushButton::clicked, this, &TeamWidget::deleteTeam);

    connect(m_ui->btnImport, &QPushButton::clicked, this, &TeamWidget::importConfig);

    // teams
    populateTeams();
    connect(m_ui->teams, &QListWidget::currentItemChanged, this, &TeamWidget::currentTeamChanged);
    connect(m_ui->teams, &QListWidget::itemDoubleClicked, this, &TeamWidget::editTeam);

    connect(m_ui->ifaceName, &KLineEdit::textChanged, this, &TeamWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);
    KAcceleratorManager::manage(m_menu);

    if (setting)
        loadConfig(setting);
}

TeamWidget::~TeamWidget()
{
    delete m_ui;
}

void TeamWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::TeamSetting::Ptr teamSetting = setting.staticCast<NetworkManager::TeamSetting>();

    m_ui->ifaceName->setText(teamSetting->interfaceName());
    m_ui->config->setPlainText(teamSetting->config());
}

QVariantMap TeamWidget::setting() const
{
    NetworkManager::TeamSetting setting;
    setting.setInterfaceName(m_ui->ifaceName->text());
    setting.setConfig(m_ui->config->toPlainText());

    return setting.toMap();
}

void TeamWidget::addTeam(QAction *action)
{
    qCDebug(PLASMA_NM) << "Adding teamed connection:" << action->data();
    qCDebug(PLASMA_NM) << "Master UUID:" << m_uuid;
    qCDebug(PLASMA_NM) << "Slave type:" << type();

    NetworkManager::ConnectionSettings::ConnectionType connectionType = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(action->data().toInt());
    NetworkManager::ConnectionSettings::Ptr connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(connectionType));
    connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    connectionSettings->setMaster(m_uuid);
    connectionSettings->setSlaveType(type());
    connectionSettings->setAutoconnect(false);

    QPointer<ConnectionEditorDialog> teamEditor = new ConnectionEditorDialog(connectionSettings);
    connect(teamEditor.data(), &ConnectionEditorDialog::accepted,
            [teamEditor, this] () {
                qCDebug(PLASMA_NM) << "Saving slave connection";
                // qCDebug(PLASMA_NM) << teamEditor->setting();
                QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(teamEditor->setting());
                QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
                connect(watcher, &QDBusPendingCallWatcher::finished, this, &TeamWidget::teamAddComplete);
            });
    connect(teamEditor.data(), &ConnectionEditorDialog::finished,
            [teamEditor] () {
                if (teamEditor) {
                    teamEditor->deleteLater();
                }
            });
    teamEditor->setModal(true);
    teamEditor->show();
}

void TeamWidget::currentTeamChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    m_ui->btnEdit->setEnabled(current);
    m_ui->btnDelete->setEnabled(current);
}

void TeamWidget::teamAddComplete(QDBusPendingCallWatcher * watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;

    if (reply.isValid()) {
        // find the slave connection with matching UUID
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(reply.value().path());
        if (connection && connection->settings()->master() == m_uuid) {
            const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
            QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->teams);
            slaveItem->setData(Qt::UserRole, connection->uuid());
            slotWidgetChanged();
        }
    } else {
        qCWarning(PLASMA_NM) << "Teamed connection not added:" << reply.error().message();
    }
}

void TeamWidget::editTeam()
{
    QListWidgetItem * currentItem = m_ui->teams->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);

    if (connection) {
        qCDebug(PLASMA_NM) << "Editing teamed connection" << currentItem->text() << uuid;
        QPointer<ConnectionEditorDialog> teamEditor = new ConnectionEditorDialog(connection->settings());
        connect(teamEditor.data(), &ConnectionEditorDialog::accepted,
                [connection, teamEditor, this] () {
                    connection->update(teamEditor->setting());
                    connect(connection.data(), &NetworkManager::Connection::updated, this, &TeamWidget::populateTeams);
                });
        connect(teamEditor.data(), &ConnectionEditorDialog::finished,
                [teamEditor] () {
                    if (teamEditor) {
                        teamEditor->deleteLater();
                    }
                });
        teamEditor->setModal(true);
        teamEditor->show();
    }
}

void TeamWidget::deleteTeam()
{
    QListWidgetItem * currentItem = m_ui->teams->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);

    if (connection) {
        qCDebug(PLASMA_NM) << "About to delete teamed connection" << currentItem->text() << uuid;
        if (KMessageBox::questionYesNo(this, i18n("Do you want to remove the connection '%1'?", connection->name()), i18n("Remove Connection"), KStandardGuiItem::remove(),
                                       KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
                == KMessageBox::Yes) {
            connection->remove();
            delete currentItem;
            slotWidgetChanged();
        }
    }
}

void TeamWidget::populateTeams()
{
    m_ui->teams->clear();

    Q_FOREACH (const NetworkManager::Connection::Ptr &connection, NetworkManager::listConnections()) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == m_uuid && settings->slaveType() == type()) {
            const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
            QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->teams);
            slaveItem->setData(Qt::UserRole, connection->uuid());
        }
    }
}

void TeamWidget::importConfig()
{
    const QString filename = QFileDialog::getOpenFileName(this, i18n("Select file to import"), QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                          "text/plain");
    if (!filename.isEmpty()) {
        //qCDebug(PLASMA_NM) << "Importing" << filename;
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            m_ui->config->setPlainText(stream.readAll());
            file.close();
        }
    }
}

bool TeamWidget::isValid() const
{
    return !m_ui->ifaceName->text().isEmpty() && m_ui->teams->count() > 0;
}
