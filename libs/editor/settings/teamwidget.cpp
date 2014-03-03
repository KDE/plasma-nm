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
#include "connectiondetaileditor.h"

#include <QDebug>
#include <QDesktopServices>

#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/ConnectionSettings>

#include <KLocalizedString>
#include <KMessageBox>
#include <KFileDialog>

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
    action = new QAction(i18n("Wireless"), this);
    action->setData(NetworkManager::ConnectionSettings::Wireless);
    m_menu->addAction(action);
    action = new QAction(i18n("VLAN"), this);
    action->setData(NetworkManager::ConnectionSettings::Vlan);
    m_menu->addAction(action);
    m_ui->btnAdd->setMenu(m_menu);
    connect(m_menu, SIGNAL(triggered(QAction*)), SLOT(addTeam(QAction*)));
    connect(m_ui->btnEdit, SIGNAL(clicked()), SLOT(editTeam()));
    connect(m_ui->btnDelete, SIGNAL(clicked()), SLOT(deleteTeam()));

    connect(m_ui->btnImport, SIGNAL(clicked()), SLOT(importConfig()));

    // teams
    populateTeams();
    connect(m_ui->teams, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(currentTeamChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(m_ui->teams, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editTeam()));

    connect(m_ui->ifaceName, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));

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

QVariantMap TeamWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned)

    NetworkManager::TeamSetting setting;
    setting.setInterfaceName(m_ui->ifaceName->text());
    setting.setConfig(m_ui->config->toPlainText());

    return setting.toMap();
}

void TeamWidget::addTeam(QAction *action)
{
    qDebug() << "Adding teamed connection:" << action->data();
    qDebug() << "Master UUID:" << m_uuid;
    qDebug() << "Slave type:" << type();

    QPointer<ConnectionDetailEditor> teamEditor = new ConnectionDetailEditor(NetworkManager::ConnectionSettings::ConnectionType(action->data().toInt()),
                                                                               this, m_uuid, type());
    if (teamEditor->exec() == QDialog::Accepted) {
        qDebug() << "Saving slave connection";
        connect(NetworkManager::settingsNotifier(), SIGNAL(connectionAddComplete(QString,bool,QString)),
                this, SLOT(teamAddComplete(QString,bool,QString)));
    }

    if (teamEditor) {
        teamEditor->deleteLater();
    }
}

void TeamWidget::currentTeamChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    m_ui->btnEdit->setEnabled(current);
    m_ui->btnDelete->setEnabled(current);
}

void TeamWidget::teamAddComplete(const QString &uuid, bool success, const QString &msg)
{
    qDebug() << Q_FUNC_INFO << uuid << success << msg;

    // find the slave connection with matching UUID
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);
    if (connection && connection->settings()->master() == m_uuid && success) {
        const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
        QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->teams);
        slaveItem->setData(Qt::UserRole, uuid);
        slotWidgetChanged();
    } else {
        qWarning() << "Teamed connection not added:" << msg;
    }

    disconnect(NetworkManager::settingsNotifier(), SIGNAL(connectionAddComplete(QString,bool,QString)),
               this, SLOT(teamAddComplete(QString,bool,QString)));
}

void TeamWidget::editTeam()
{
    QListWidgetItem * currentItem = m_ui->teams->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);

    if (connection) {
        qDebug() << "Editing teamed connection" << currentItem->text() << uuid;
        QPointer<ConnectionDetailEditor> teamEditor = new ConnectionDetailEditor(connection->settings(), this);
        if (teamEditor->exec() == QDialog::Accepted) {
            connect(connection.data(), SIGNAL(updated()), this, SLOT(populateTeams()));
        }

        if (teamEditor) {
            teamEditor->deleteLater();
        }
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
        qDebug() << "About to delete teamed connection" << currentItem->text() << uuid;
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

    foreach (const NetworkManager::Connection::Ptr &connection, NetworkManager::listConnections()) {
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
    const QString filename = KFileDialog::getOpenFileName(QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                          QLatin1String("text/plain"), this, i18n("Select file to import"));
    if (!filename.isEmpty()) {
        //qDebug() << "Importing" << filename;
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
