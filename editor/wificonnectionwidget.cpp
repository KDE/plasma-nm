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

#include <QtNetworkManager/settings/802-11-wireless.h>

#include "wificonnectionwidget.h"
#include "ui_wificonnectionwidget.h"

WifiConnectionWidget::WifiConnectionWidget(NetworkManager::Settings::Setting * setting, QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::WifiConnectionWidget)
{
    m_ui->setupUi(this);

    if (setting)
        loadConfig(setting);
}

WifiConnectionWidget::~WifiConnectionWidget()
{
}

void WifiConnectionWidget::loadConfig(NetworkManager::Settings::Setting * setting)
{
    NetworkManager::Settings::WirelessSetting * wifiSetting = static_cast<NetworkManager::Settings::WirelessSetting*>(setting);

    if (!wifiSetting->ssid().isEmpty()) {
        m_ui->SSIDLineEdit->setText(QString::fromUtf8(wifiSetting->ssid()));
    }

    if (wifiSetting->mode() != NetworkManager::Settings::WirelessSetting::Infrastructure) {
        m_ui->modeComboBox->setCurrentIndex(wifiSetting->mode());
    }

    if (!wifiSetting->bssid().isEmpty()) {
        m_ui->BSSIDLineEdit->setText(QString(wifiSetting->bssid()));
    }

    if (!wifiSetting->macAddress().isEmpty()) {
        m_ui->macAddress->setText(QString(wifiSetting->macAddress()));
    }

    if (!wifiSetting->clonedMacAddress().isEmpty()) {
        m_ui->clonedMacAddress->setText(QString(wifiSetting->clonedMacAddress()));
    }

    if (wifiSetting->mtu()) {
        m_ui->mtu->setValue(wifiSetting->mtu());
    }
}

QVariantMap WifiConnectionWidget::setting() const
{
    NetworkManager::Settings::WirelessSetting wifiSetting;

    if (!m_ui->SSIDLineEdit->text().isEmpty()) {
        wifiSetting.setSsid(m_ui->SSIDLineEdit->text().toUtf8());
    }

    if (m_ui->modeComboBox->currentIndex() != 0) {
        wifiSetting.setMode(static_cast<NetworkManager::Settings::WirelessSetting::NetworkMode>(m_ui->modeComboBox->currentIndex()));
    }

    if (!m_ui->BSSIDLineEdit->text().isEmpty() && m_ui->BSSIDLineEdit->text() != ":::::") {
        wifiSetting.setBssid(m_ui->BSSIDLineEdit->text().toLatin1());
    }

    if (!m_ui->macAddress->text().isEmpty() && m_ui->macAddress->text() != ":::::") {
        wifiSetting.setMacAddress(m_ui->macAddress->text().toLatin1());
    }

    if (!m_ui->clonedMacAddress->text().isEmpty() && m_ui->clonedMacAddress->text() != ":::::") {
        wifiSetting.setClonedMacAddress(m_ui->clonedMacAddress->text().toLatin1());
    }

    if (m_ui->mtu->value()) {
        wifiSetting.setMtu(m_ui->mtu->value());
    }

    return wifiSetting.toMap();
}
