/*
    Copyright (c) 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "wificonnectionwidget.h"
#include "ui_wificonnectionwidget.h"

#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WirelessSetting>

#include "uiutils.h"

WifiConnectionWidget::WifiConnectionWidget(const NetworkManager::Setting::Ptr &setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::WifiConnectionWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->btnRandomMacAddr, SIGNAL(clicked()), SLOT(generateRandomClonedMac()));
    connect(m_ui->SSIDCombo, SIGNAL(currentIndexChanged(int)), SLOT(ssidChanged()));
    connect(m_ui->SSIDCombo, SIGNAL(editTextChanged(QString)), SLOT(ssidChanged()));

    if (setting)
        loadConfig(setting);
}

WifiConnectionWidget::~WifiConnectionWidget()
{
}

void WifiConnectionWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::WirelessSetting::Ptr wifiSetting = setting.staticCast<NetworkManager::WirelessSetting>();

    m_ui->SSIDCombo->init(wifiSetting->ssid());

    if (wifiSetting->mode() != NetworkManager::WirelessSetting::Infrastructure) {
        m_ui->modeComboBox->setCurrentIndex(wifiSetting->mode());
    }

    m_ui->BSSIDCombo->init(NetworkManager::Utils::macAddressAsString(wifiSetting->bssid()), wifiSetting->ssid());

    m_ui->macAddress->init(NetworkManager::Device::Wifi, NetworkManager::Utils::macAddressAsString(wifiSetting->macAddress()));

    if (!wifiSetting->clonedMacAddress().isEmpty()) {
        m_ui->clonedMacAddress->setText(NetworkManager::Utils::macAddressAsString(wifiSetting->clonedMacAddress()));
    }

    if (wifiSetting->mtu()) {
        m_ui->mtu->setValue(wifiSetting->mtu());
    }

    if (wifiSetting->hidden()) {
        m_ui->hiddenNetwork->setChecked(true);
    }
}

QVariantMap WifiConnectionWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned);

    NetworkManager::WirelessSetting wifiSetting;

    wifiSetting.setSsid(m_ui->SSIDCombo->ssid().toUtf8());

    if (m_ui->modeComboBox->currentIndex() != 0) {
        wifiSetting.setMode(static_cast<NetworkManager::WirelessSetting::NetworkMode>(m_ui->modeComboBox->currentIndex()));
    }

    wifiSetting.setBssid(NetworkManager::Utils::macAddressFromString(m_ui->BSSIDCombo->bssid()));

    wifiSetting.setMacAddress(NetworkManager::Utils::macAddressFromString(m_ui->macAddress->hwAddress()));

    if (!m_ui->clonedMacAddress->text().isEmpty() && m_ui->clonedMacAddress->text() != ":::::") {
        wifiSetting.setClonedMacAddress(NetworkManager::Utils::macAddressFromString(m_ui->clonedMacAddress->text()));
    }

    if (m_ui->mtu->value()) {
        wifiSetting.setMtu(m_ui->mtu->value());
    }

    wifiSetting.setHidden(m_ui->hiddenNetwork->isChecked());

    return wifiSetting.toMap();
}

void WifiConnectionWidget::generateRandomClonedMac()
{
    QByteArray mac;
    mac.resize(6);
    for (int i = 0; i < 6; i++) {
        int random = qrand() % 255;
        mac[i] = random;
    }
    m_ui->clonedMacAddress->setText(NetworkManager::Utils::macAddressAsString(mac));
}

void WifiConnectionWidget::ssidChanged()
{
    m_ui->BSSIDCombo->init(m_ui->BSSIDCombo->bssid(), m_ui->SSIDCombo->ssid());
    slotWidgetChanged();
}

bool WifiConnectionWidget::isValid() const
{
    return !m_ui->SSIDCombo->currentText().isEmpty();
}
