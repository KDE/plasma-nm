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

#include "vpncwidget.h"
#include "ui_vpnc.h"


VpncWidget::VpncWidget(NetworkManager::Settings::Setting * setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::VpncWidget)
{
    m_setting = static_cast<NetworkManager::Settings::VpnSetting *>(setting);
    m_ui->setupUi(this);

    if (setting)
        loadConfig(setting);
}

VpncWidget::~VpncWidget()
{
}

void VpncWidget::loadConfig(NetworkManager::Settings::Setting *setting)
{
    Q_UNUSED(setting);


}

QVariantMap VpncWidget::setting() const
{

    return m_setting->toMap();
}

void VpncWidget::userPasswordTypeChanged(int index)
{
    if (index == 0 || index == 2) {

    }
    else {

    }
}

void VpncWidget::groupPasswordTypeChanged(int index)
{
}
