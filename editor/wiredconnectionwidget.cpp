/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "wiredconnectionwidget.h"
#include "ui_wiredconnectionwidget.h"
#include "uiutils.h"

#include <NetworkManagerQt/settings/Ethernet>

WiredConnectionWidget::WiredConnectionWidget(const NetworkManager::Settings::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_widget(new Ui::WiredConnectionWidget)
{
    m_widget->setupUi(this);
    m_widget->speedLabel->setHidden(true);
    m_widget->speed->setHidden(true);
    m_widget->duplexLabel->setHidden(true);
    m_widget->duplex->setHidden(true);

    connect(m_widget->btnRandomMacAddr, SIGNAL(clicked()), SLOT(generateRandomClonedMac()));

    if (setting)
        loadConfig(setting);
}

WiredConnectionWidget::~WiredConnectionWidget()
{
}

void WiredConnectionWidget::loadConfig(const NetworkManager::Settings::Setting::Ptr &setting)
{
    NetworkManager::Settings::WiredSetting::Ptr wiredSetting = setting.staticCast<NetworkManager::Settings::WiredSetting>();

    m_widget->macAddress->init(NetworkManager::Device::Ethernet, UiUtils::macAddressAsString(wiredSetting->macAddress()));

    if (!wiredSetting->clonedMacAddress().isEmpty()) {
        m_widget->clonedMacAddress->setText(UiUtils::macAddressAsString(wiredSetting->clonedMacAddress()));
    }

    if (wiredSetting->mtu()) {
        m_widget->mtu->setValue(wiredSetting->mtu());
    }

    if (!wiredSetting->autoNegotiate()) {
        if (wiredSetting->speed()) {
            m_widget->speed->setValue(wiredSetting->speed());
        }

        if (wiredSetting->duplexType() == NetworkManager::Settings::WiredSetting::Full) {
            m_widget->duplex->setCurrentIndex(0);
        } else {
            m_widget->duplex->setCurrentIndex(1);
        }
    }
}

QVariantMap WiredConnectionWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned);

    NetworkManager::Settings::WiredSetting wiredSetting;

    wiredSetting.setMacAddress(UiUtils::macAddressFromString(m_widget->macAddress->hwAddress()));

    if (!m_widget->clonedMacAddress->text().isEmpty() && m_widget->clonedMacAddress->text() != ":::::") {
        wiredSetting.setClonedMacAddress(UiUtils::macAddressFromString(m_widget->clonedMacAddress->text()));
    }

    if (m_widget->mtu->value()) {
        wiredSetting.setMtu(m_widget->mtu->value());
    }

    if (m_widget->autonegotiate->isChecked()) {
        wiredSetting.setAutoNegotiate(true);
    } else {
        wiredSetting.setAutoNegotiate(false);

        if (m_widget->speed->value()) {
            wiredSetting.setSpeed(m_widget->speed->value());
        }

        if (m_widget->duplex->currentIndex() == 0) {
            wiredSetting.setDuplexType(NetworkManager::Settings::WiredSetting::Full);
        } else {
            wiredSetting.setDuplexType(NetworkManager::Settings::WiredSetting::Half);
        }
    }

    return wiredSetting.toMap();
}

void WiredConnectionWidget::generateRandomClonedMac()
{
    QByteArray mac;
    mac.resize(6);
    for (int i = 0; i < 6; i++) {
        int random = qrand() % 255;
        mac[i] = random;
    }
    m_widget->clonedMacAddress->setText(UiUtils::macAddressAsString(mac));
}
