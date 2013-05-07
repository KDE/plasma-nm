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

#include "infinibandwidget.h"
#include "ui_infiniband.h"
#include "uiutils.h"

#include <KLocalizedString>

#include <NetworkManagerQt/settings/infiniband.h>

InfinibandWidget::InfinibandWidget(const NetworkManager::Settings::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::InfinibandWidget)
{
    m_ui->setupUi(this);

    m_ui->transport->addItem(i18nc("infiniband transport mode", "Datagram"), NetworkManager::Settings::InfinibandSetting::Datagram);
    m_ui->transport->addItem(i18nc("infiniband transport mode", "Connected"), NetworkManager::Settings::InfinibandSetting::Connected);
    m_ui->transport->setCurrentIndex(0);

    if (setting)
        loadConfig(setting);
}

InfinibandWidget::~InfinibandWidget()
{
}

void InfinibandWidget::loadConfig(const NetworkManager::Settings::Setting::Ptr &setting)
{
    NetworkManager::Settings::InfinibandSetting::Ptr infinibandSetting = setting.staticCast<NetworkManager::Settings::InfinibandSetting>();

    if (infinibandSetting->transportMode() != NetworkManager::Settings::InfinibandSetting::Unknown)
        m_ui->transport->setCurrentIndex(m_ui->transport->findData(infinibandSetting->transportMode()));
    m_ui->macAddress->init(NetworkManager::Device::InfiniBand, UiUtils::macAddressAsString(infinibandSetting->macAddress()));
    if (infinibandSetting->mtu()) {
        m_ui->mtu->setValue(infinibandSetting->mtu());
    }
}

QVariantMap InfinibandWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned)

    NetworkManager::Settings::InfinibandSetting setting;
    setting.setTransportMode(static_cast<NetworkManager::Settings::InfinibandSetting::TransportMode>(m_ui->transport->currentIndex()));
    setting.setMacAddress(UiUtils::macAddressFromString(m_ui->macAddress->hwAddress()));
    if (m_ui->mtu->value()) {
        setting.setMtu(m_ui->mtu->value());
    }

    return setting.toMap();
}
