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

#include "bondwidget.h"
#include "ui_bond.h"
#include "connectiondetaileditor.h"
#include "debug.h"

#include <QDBusPendingReply>

#include <NetworkManagerQt/GenericTypes>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/ConnectionSettings>

#include <KLocalizedString>
#include <KMessageBox>

#define NM_SETTING_BOND_OPTION_MII_MONITOR "mii"
#define NM_SETTING_BOND_OPTION_ARP_MONITOR "arp"

BondWidget::BondWidget(const QString & masterUuid, const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_uuid(masterUuid),
    m_ui(new Ui::BondWidget)
{
    m_ui->setupUi(this);

    // Action buttons and menu
    m_menu = new QMenu(this);
    QAction * action = new QAction(i18n("Ethernet"), this);
    action->setData(NetworkManager::ConnectionSettings::Wired);
    m_menu->addAction(action);
    action = new QAction(i18n("InfiniBand"), this);
    action->setData(NetworkManager::ConnectionSettings::Infiniband);
    m_menu->addAction(action);
    m_ui->btnAdd->setMenu(m_menu);
    connect(m_menu, SIGNAL(triggered(QAction*)), SLOT(addBond(QAction*)));
    connect(m_ui->btnEdit, SIGNAL(clicked()), SLOT(editBond()));
    connect(m_ui->btnDelete, SIGNAL(clicked()), SLOT(deleteBond()));

    // mode
    m_ui->mode->addItem(i18nc("bond mode", "Round-robin"), QLatin1String("balance-rr"));
    m_ui->mode->addItem(i18nc("bond mode", "Active backup"), QLatin1String("active-backup"));
    m_ui->mode->addItem(i18nc("bond mode", "Broadcast"), QLatin1String("broadcast"));
    m_ui->mode->addItem(i18nc("bond mode", "802.3ad"), QLatin1String("802.3ad"));
    m_ui->mode->addItem(i18nc("bond mode", "Adaptive transmit load balancing"), QLatin1String("balance-tlb"));
    m_ui->mode->addItem(i18nc("bond mode", "Adaptive load balancing"), QLatin1String("balance-alb"));

    // link monitor
    m_ui->linkMonitoring->addItem(i18nc("bond link monitoring", "MII (recommended)"), NM_SETTING_BOND_OPTION_MII_MONITOR);
    m_ui->linkMonitoring->addItem(i18nc("bond link monitoring", "ARP"), NM_SETTING_BOND_OPTION_ARP_MONITOR);

    // bonds
    populateBonds();
    connect(m_ui->bonds, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(currentBondChanged(QListWidgetItem*,QListWidgetItem*)));
    connect(m_ui->bonds, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(editBond()));

    connect(m_ui->ifaceName, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->arpTargets, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->linkMonitoring, SIGNAL(currentIndexChanged(int)), SLOT(slotWidgetChanged()));

    KAcceleratorManager::manage(this);
    KAcceleratorManager::manage(m_menu);

    if (setting)
        loadConfig(setting);
}

BondWidget::~BondWidget()
{
    delete m_ui;
}

void BondWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::BondSetting::Ptr bondSetting = setting.staticCast<NetworkManager::BondSetting>();

    m_ui->ifaceName->setText(bondSetting->interfaceName());

    const NMStringMap options = bondSetting->options();

    // mode
    int modeIndex = m_ui->mode->findData(options.value(NM_SETTING_BOND_OPTION_MODE));
    if (modeIndex == -1)
        modeIndex = 0;
    m_ui->mode->setCurrentIndex(modeIndex);

    const QString arpTargets = options.value(NM_SETTING_BOND_OPTION_ARP_IP_TARGET);
    if (!arpTargets.isEmpty()) { // ARP
        m_ui->linkMonitoring->setCurrentIndex(m_ui->linkMonitoring->findData(NM_SETTING_BOND_OPTION_ARP_MONITOR));

        bool ok = false;
        const int arpMonFreq = options.value(NM_SETTING_BOND_OPTION_ARP_INTERVAL).toInt(&ok);
        if (ok && arpMonFreq > 0)
            m_ui->monitorFreq->setValue(arpMonFreq);

        m_ui->arpTargets->setText(arpTargets);
    } else { // MII
        m_ui->linkMonitoring->setCurrentIndex(m_ui->linkMonitoring->findData(NM_SETTING_BOND_OPTION_MII_MONITOR));

        bool ok = false;
        const int miiMonFreq = options.value(NM_SETTING_BOND_OPTION_MIIMON).toInt(&ok);
        if (ok && miiMonFreq > 0)
            m_ui->monitorFreq->setValue(miiMonFreq);

        ok = false;
        const int upDelay = options.value(NM_SETTING_BOND_OPTION_UPDELAY).toInt(&ok);
        if (ok && upDelay > 0)
            m_ui->upDelay->setValue(upDelay);

        ok = false;
        const int downDelay = options.value(NM_SETTING_BOND_OPTION_DOWNDELAY).toInt(&ok);
        if (ok && downDelay > 0)
            m_ui->upDelay->setValue(downDelay);
    }
}

QVariantMap BondWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned)

    NetworkManager::BondSetting setting;
    setting.setInterfaceName(m_ui->ifaceName->text());

    NMStringMap options;
    options.insert(NM_SETTING_BOND_OPTION_MODE, m_ui->mode->itemData(m_ui->mode->currentIndex()).toString());

    if (m_ui->linkMonitoring->itemData(m_ui->linkMonitoring->currentIndex()).toString() == NM_SETTING_BOND_OPTION_MII_MONITOR) { // MII
        options.insert(NM_SETTING_BOND_OPTION_MIIMON, QString::number(m_ui->monitorFreq->value()));
        const int upDelay = m_ui->upDelay->value();
        if (upDelay)
            options.insert(NM_SETTING_BOND_OPTION_UPDELAY, QString::number(upDelay));
        const int downDelay = m_ui->downDelay->value();
        if (downDelay)
            options.insert(NM_SETTING_BOND_OPTION_DOWNDELAY, QString::number(downDelay));
    } else { // ARP
        options.insert(NM_SETTING_BOND_OPTION_ARP_INTERVAL, QString::number(m_ui->monitorFreq->value()));
        const QString arpTargets = m_ui->arpTargets->text();
        if (!arpTargets.isEmpty())
            options.insert(NM_SETTING_BOND_OPTION_ARP_IP_TARGET, arpTargets);
    }

    setting.setOptions(options);
    return setting.toMap();
}

void BondWidget::addBond(QAction *action)
{
    qCDebug(PLASMA_NM) << "Adding bonded connection:" << action->data();
    qCDebug(PLASMA_NM) << "Master UUID:" << m_uuid;
    qCDebug(PLASMA_NM) << "Slave type:" << type();

    NetworkManager::ConnectionSettings::ConnectionType connectionType = static_cast<NetworkManager::ConnectionSettings::ConnectionType>(action->data().toInt());
    NetworkManager::ConnectionSettings::Ptr connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(connectionType));
    connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    connectionSettings->setMaster(m_uuid);
    connectionSettings->setSlaveType(type());
    connectionSettings->setAutoconnect(false);

    QPointer<ConnectionDetailEditor> bondEditor = new ConnectionDetailEditor(connectionSettings, true);

    if (bondEditor->exec() == QDialog::Accepted) {
        qCDebug(PLASMA_NM) << "Saving slave connection";
        // qCDebug(PLASMA_NM) << bondEditor->setting();
        QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(bondEditor->setting());
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &BondWidget::bondAddComplete);
    }

    if (bondEditor) {
        bondEditor->deleteLater();
    }
}

void BondWidget::currentBondChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    m_ui->btnEdit->setEnabled(current);
    m_ui->btnDelete->setEnabled(current);
}

void BondWidget::bondAddComplete(QDBusPendingCallWatcher * watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;

    if (reply.isValid()) {
        // find the slave connection with matching UUID
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(reply.value().path());
        if (connection && connection->settings()->master() == m_uuid) {
            const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
            QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->bonds);
            slaveItem->setData(Qt::UserRole, connection->uuid());
            slotWidgetChanged();
        }
    } else {
        qCWarning(PLASMA_NM) << "Bonded connection not added:" << reply.error().message();
    }
}

void BondWidget::editBond()
{
    QListWidgetItem * currentItem = m_ui->bonds->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);

    if (connection) {
        // qCDebug(PLASMA_NM) << "Editing bonded connection" << currentItem->text() << uuid;
        QPointer<ConnectionDetailEditor> bondEditor = new ConnectionDetailEditor(connection->settings(), this);
        if (bondEditor->exec() == QDialog::Accepted) {
            connection->update(bondEditor->setting());
            connect(connection.data(), SIGNAL(updated()), this, SLOT(populateBonds()));
        }

        if (bondEditor) {
            bondEditor->deleteLater();
        }
    }
}

void BondWidget::deleteBond()
{
    QListWidgetItem * currentItem = m_ui->bonds->currentItem();
    if (!currentItem)
        return;

    const QString uuid = currentItem->data(Qt::UserRole).toString();
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);

    if (connection) {
        // qCDebug(PLASMA_NM) << "About to delete bonded connection" << currentItem->text() << uuid;
        if (KMessageBox::questionYesNo(this, i18n("Do you want to remove the connection '%1'?", connection->name()), i18n("Remove Connection"),
                                       KStandardGuiItem::remove(), KStandardGuiItem::no(), QString(), KMessageBox::Dangerous)
                == KMessageBox::Yes) {
            connection->remove();
            delete currentItem;
            slotWidgetChanged();
        }
    }
}

void BondWidget::populateBonds()
{
    m_ui->bonds->clear();

    Q_FOREACH (const NetworkManager::Connection::Ptr &connection, NetworkManager::listConnections()) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == m_uuid && settings->slaveType() == type()) {
            const QString label = QString("%1 (%2)").arg(connection->name()).arg(connection->settings()->typeAsString(connection->settings()->connectionType()));
            QListWidgetItem * slaveItem = new QListWidgetItem(label, m_ui->bonds);
            slaveItem->setData(Qt::UserRole, connection->uuid());
        }
    }
}

bool BondWidget::isValid() const
{
    if (m_ui->linkMonitoring->itemData(m_ui->linkMonitoring->currentIndex()).toString() == NM_SETTING_BOND_OPTION_ARP_MONITOR) {
        const QStringList ipAddresses = m_ui->arpTargets->text().split(',');
        if (ipAddresses.isEmpty()) {
            return false;
        }

        Q_FOREACH (const QString & ip, ipAddresses) {
            QHostAddress ipAddress(ip);
            if (ipAddress.isNull()) {
                return false;
            }
        }
    }

    return !m_ui->ifaceName->text().isEmpty() && m_ui->bonds->count() > 0;
}
