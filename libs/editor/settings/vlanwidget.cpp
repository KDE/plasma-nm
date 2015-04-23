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

#include "vlanwidget.h"
#include "ui_vlan.h"
#include "uiutils.h"

#include <NetworkManagerQt/VlanSetting>
#include <NetworkManagerQt/Settings>

VlanWidget::VlanWidget(const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::VlanWidget)
{
    m_ui->setupUi(this);

    fillConnections();

    connect(m_ui->ifaceName, &KLineEdit::textChanged, this, &VlanWidget::slotWidgetChanged);
    connect(m_ui->parent, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &VlanWidget::slotWidgetChanged);
    connect(m_ui->parent->lineEdit(), &QLineEdit::textChanged, this, &VlanWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (setting)
        loadConfig(setting);
}

VlanWidget::~VlanWidget()
{
    delete m_ui;
}

void VlanWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VlanSetting::Ptr vlanSetting = setting.staticCast<NetworkManager::VlanSetting>();

    m_ui->parent->setCurrentIndex(m_ui->parent->findData(vlanSetting->parent()));

    m_ui->id->setValue(vlanSetting->id());
    m_ui->ifaceName->setText(vlanSetting->interfaceName());

    m_ui->reorderHeaders->setChecked(vlanSetting->flags().testFlag(NetworkManager::VlanSetting::ReorderHeaders));
    m_ui->gvrp->setChecked(vlanSetting->flags().testFlag(NetworkManager::VlanSetting::Gvrp));
    m_ui->looseBinding->setChecked(vlanSetting->flags().testFlag(NetworkManager::VlanSetting::LooseBinding));
}

QVariantMap VlanWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned);

    NetworkManager::VlanSetting setting;

    setting.setParent(m_ui->parent->itemData(m_ui->parent->currentIndex()).toString());
    setting.setId(m_ui->id->value());

    const QString ifaceName = m_ui->ifaceName->text();
    if (!ifaceName.isEmpty())
        setting.setInterfaceName(ifaceName);

    NetworkManager::VlanSetting::Flags flags;
    if (m_ui->reorderHeaders->isChecked())
        flags |= NetworkManager::VlanSetting::ReorderHeaders;
    if (m_ui->gvrp->isChecked())
        flags |= NetworkManager::VlanSetting::Gvrp;
    if (m_ui->looseBinding->isChecked())
        flags |= NetworkManager::VlanSetting::LooseBinding;
    if (flags)
        setting.setFlags(flags);

    return setting.toMap();
}

void VlanWidget::fillConnections()
{
    m_ui->parent->clear();

    Q_FOREACH (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
        if (!con->settings()->isSlave() && con->settings()->connectionType() == NetworkManager::ConnectionSettings::Wired)
            m_ui->parent->addItem(con->name(), con->uuid());
    }
}

bool VlanWidget::isValid() const
{
    return !m_ui->parent->currentText().isEmpty() || !m_ui->ifaceName->text().isEmpty();
}
