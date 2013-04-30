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

#include <QDebug>

#include <NetworkManagerQt/generic-types.h>
#include <NetworkManagerQt/connection.h>

#include <KLocalizedString>

#define NM_SETTING_BOND_OPTION_MII_MONITOR "mii"
#define NM_SETTING_BOND_OPTION_ARP_MONITOR "arp"

BondWidget::BondWidget(const QString & masterUuid, const NetworkManager::Settings::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_uuid(masterUuid),
    m_ui(new Ui::BondWidget)
{
    m_ui->setupUi(this);

    // Add context menu
    m_menu = new QMenu(this);
    QAction * action = new QAction(i18n("Ethernet"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Wired);
    m_menu->addAction(action);
    action = new QAction(i18n("InfiniBand"), this);
    action->setData(NetworkManager::Settings::ConnectionSettings::Infiniband);
    m_menu->addAction(action);
    m_ui->btnAdd->setMenu(m_menu);
    connect(m_menu, SIGNAL(triggered(QAction*)), SLOT(addBond(QAction*)));

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
    connect(m_ui->bonds, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(currentBondChanged(QListWidgetItem*,QListWidgetItem*)));

    if (setting)
        loadConfig(setting);
}

BondWidget::~BondWidget()
{
}

void BondWidget::loadConfig(const NetworkManager::Settings::Setting::Ptr &setting)
{
    NetworkManager::Settings::BondSetting bondSetting = setting.staticCast<NetworkManager::Settings::BondSetting>();

    m_ui->ifaceName->setText(bondSetting.interfaceName());

    NMStringMap options = bondSetting.options();

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

    NetworkManager::Settings::BondSetting setting;
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
    qDebug() << "Adding bonded connection:" << action->data();
    qDebug() << "Master UUID:" << m_uuid;
    qDebug() << "Slave type:" << type();

    ConnectionDetailEditor * bondEditor = new ConnectionDetailEditor(NetworkManager::Settings::ConnectionSettings::typeFromString(action->data().toString()),
                                                                     this, QString(), m_uuid, type());
    if (bondEditor->exec() == QDialog::Accepted) {
        qDebug() << "Saving slave connection:" << bondEditor->uuid();
    }
}

void BondWidget::currentBondChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)

    m_ui->btnEdit->setEnabled(current);
    m_ui->btnDelete->setEnabled(current);

    // TODO
}
