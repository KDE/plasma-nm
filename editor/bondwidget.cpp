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

#include <NetworkManagerQt/generic-types.h>

#include <KLocalizedString>

BondWidget::BondWidget(const NetworkManager::Settings::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::BondWidget)
{
    m_ui->setupUi(this);

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

    // TODO
}

QVariantMap BondWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned);

    NetworkManager::Settings::BondSetting setting;
    setting.setInterfaceName(m_ui->ifaceName->text());

    NMStringMap options;

    // TODO

    setting.setOptions(options);
    return setting.toMap();
}
