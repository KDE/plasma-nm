/*
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

#include "bridgewidget.h"
#include "ui_bridge.h"
#include "connectiondetaileditor.h"

#include <QDebug>

#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/settings/Connection>

#include <KLocalizedString>
#include <KMessageBox>

BridgeWidget::BridgeWidget(const QString & masterUuid, const NetworkManager::Settings::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_uuid(masterUuid),
    m_ui(new Ui::BridgeWidget)
{
    m_ui->setupUi(this);

    // Action buttons and menu
    m_menu = new QMenu(this);
    QAction * action = new QAction(i18n("Ethernet"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Wired);
    m_menu->addAction(action);
    action = new QAction(i18n("InfiniBand"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Infiniband);
    m_menu->addAction(action);
    action = new QAction(i18n("Wireless"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Wireless);
    m_menu->addAction(action);
    m_ui->btnAdd->setMenu(m_menu);
    connect(m_menu, SIGNAL(triggered(QAction*)), SLOT(addBridge(QAction*)));
    connect(m_ui->btnEdit, SIGNAL(clicked()), SLOT(editBridge()));
    connect(m_ui->btnDelete, SIGNAL(clicked()), SLOT(deleteBridge()));

    // bridges
    populateBridges();
    connect(m_ui->bridges, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(currentBridgeChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(m_ui->bridges, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editBridge()));

    if (setting)
        loadConfig(setting);
}

BridgeWidget::~BridgeWidget()
{
}

void BridgeWidget::loadConfig(const NetworkManager::Settings::Setting::Ptr &setting)
{
    NetworkManager::Settings::BridgeSetting bridgeSetting = setting.staticCast<NetworkManager::Settings::BridgeSetting>();

    m_ui->ifaceName->setText(bridgeSetting.interfaceName());
    m_ui->agingTime->setValue(bridgeSetting.agingTime());

    const bool stp = bridgeSetting.stp();
    m_ui->stpGroup->setChecked(stp);
    if (stp) {
        m_ui->priority->setValue(bridgeSetting.priority());
        m_ui->forwardDelay->setValue(bridgeSetting.forwardDelay());
        m_ui->helloTime->setValue(bridgeSetting.helloTime());
        m_ui->maxAge->setValue(bridgeSetting.maxAge());
    }
}

QVariantMap BridgeWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned)

    NetworkManager::Settings::BridgeSetting setting;
    setting.setInterfaceName(m_ui->ifaceName->text());
    setting.setAgingTime(m_ui->agingTime->value());

    const bool stp = m_ui->stpGroup->isChecked();
    setting.setStp(stp);
    if (stp) {
        setting.setPriority(m_ui->priority->value());
        setting.setForwardDelay(m_ui->forwardDelay->value());
        setting.setHelloTime(m_ui->helloTime->value());
        setting.setMaxAge(m_ui->maxAge->value());
    }

    return setting.toMap();
}

void BridgeWidget::addBridge(QAction *action)
{
    qDebug() << "Adding bridged connection:" << action->data();
    qDebug() << "Master UUID:" << m_uuid;
    qDebug() << "Slave type:" << type();

    ConnectionDetailEditor * bridgeEditor = new ConnectionDetailEditor(NetworkManager::Settings::ConnectionSettings::ConnectionType(action->data().toInt()),
                                                                       this, QString(), m_uuid, type());
    if (bridgeEditor->exec() == QDialog::Accepted) {
        qDebug() << "Saving slave connection";
        connect(NetworkManager::Settings::notifier(), SIGNAL(connectionAddComplete(QString,bool,QString)),
                this, SLOT(bridgeAddComplete(QString,bool,QString)));
    }
}

void BridgeWidget::currentBridgeChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    m_ui->btnEdit->setEnabled(current);
    m_ui->btnDelete->setEnabled(current);
}

void BridgeWidget::bridgeAddComplete(const QString &uuid, bool success, const QString &msg)
{
    qDebug() << Q_FUNC_INFO << uuid << success << msg;

    // find the slave connection with matching UUID
    NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnectionByUuid(uuid);
    if (connection && connection->settings()->master() == m_uuid && success) {
        const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
        QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->bridges);
        slaveItem->setData(Qt::UserRole, uuid);
    } else {
        qWarning() << "Bridged connection not added:" << msg;
    }

    disconnect(NetworkManager::Settings::notifier(), SIGNAL(connectionAddComplete(QString,bool,QString)),
               this, SLOT(bridgeAddComplete(QString,bool,QString)));
}

void BridgeWidget::editBridge()
{
    QListWidgetItem * currentItem = m_ui->bridges->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnectionByUuid(uuid);

    if (connection) {
        qDebug() << "Editing bridged connection" << currentItem->text() << uuid;
        ConnectionDetailEditor * bridgeEditor = new ConnectionDetailEditor(connection->settings(), this);
        if (bridgeEditor->exec() == QDialog::Accepted) {
            connect(connection.data(), SIGNAL(updated()), this, SLOT(populateBridges()));
        }
    }
}

void BridgeWidget::deleteBridge()
{
    QListWidgetItem * currentItem = m_ui->bridges->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnectionByUuid(uuid);

    if (connection) {
        qDebug() << "About to delete bridged connection" << currentItem->text() << uuid;
        if (KMessageBox::questionYesNo(this, i18n("Do you want to remove the connection '%1'?", connection->name()), i18n("Remove Connection"), KStandardGuiItem::remove(),
                                       KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
                == KMessageBox::Yes) {
            connection->remove();
            delete currentItem;
        }
    }
}

void BridgeWidget::populateBridges()
{
    m_ui->bridges->clear();

    foreach (const NetworkManager::Settings::Connection::Ptr &connection, NetworkManager::Settings::listConnections()) {
        NetworkManager::Settings::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == m_uuid && settings->slaveType() == type()) {
            const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
            QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->bridges);
            slaveItem->setData(Qt::UserRole, connection->uuid());
        }
    }
}
