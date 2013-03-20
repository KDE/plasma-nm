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

#include "wifisecurity.h"
#include "ui_wifisecurity.h"

WifiSecurity::WifiSecurity(NetworkManager::Settings::Setting * setting, NetworkManager::Settings::Security8021xSetting * setting8021x,
                           QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::WifiSecurity)
{
    m_wifiSecurity = static_cast<NetworkManager::Settings::WirelessSecuritySetting*>(setting);

    m_ui->setupUi(this);

    m_8021xWidget = new Security8021x(setting8021x, true, this);  // Dynamic WEP
    m_WPA2Widget = new Security8021x(setting8021x, true, this);   // WPA(2) Enterprise
    m_ui->stackedWidget->insertWidget(3, m_8021xWidget);
    m_ui->stackedWidget->insertWidget(5, m_WPA2Widget);

    connect(m_ui->cbShowWepKey, SIGNAL(toggled(bool)), SLOT(slotShowWepKeyPasswordChecked(bool)));
    connect(m_ui->cbShowLeapPassword, SIGNAL(toggled(bool)), SLOT(slotShowLeapPasswordChecked(bool)));
    connect(m_ui->cbShowPsk, SIGNAL(toggled(bool)), SLOT(slotShowPskPasswordChecked(bool)));

    connect(m_ui->wepIndex, SIGNAL(currentIndexChanged(int)), SLOT(setWepKey(int)));

    if (setting)
        loadConfig(setting);
}

WifiSecurity::~WifiSecurity()
{
}

void WifiSecurity::loadConfig(NetworkManager::Settings::Setting * setting)
{
    Q_UNUSED(setting);

    const NetworkManager::Settings::WirelessSecuritySetting::KeyMgmt keyMgmt = m_wifiSecurity->keyMgmt();
    const NetworkManager::Settings::WirelessSecuritySetting::AuthAlg authAlg = m_wifiSecurity->authAlg();

    if (keyMgmt == NetworkManager::Settings::WirelessSecuritySetting::Unknown) {
        m_ui->securityCombo->setCurrentIndex(0); // None

    } else if (keyMgmt == NetworkManager::Settings::WirelessSecuritySetting::Wep) {
        m_ui->securityCombo->setCurrentIndex(1);  // WEP
        const int keyIndex = static_cast<int>(m_wifiSecurity->wepTxKeyindex());
        setWepKey(keyIndex);
        m_ui->wepIndex->setCurrentIndex(keyIndex);

        if (m_wifiSecurity->authAlg() == NetworkManager::Settings::WirelessSecuritySetting::Open) {
            m_ui->wepAuth->setCurrentIndex(0);
        }
        else {
            m_ui->wepAuth->setCurrentIndex(1);
        }

    } else if (keyMgmt == NetworkManager::Settings::WirelessSecuritySetting::Ieee8021x
               && authAlg == NetworkManager::Settings::WirelessSecuritySetting::Leap) {
        m_ui->securityCombo->setCurrentIndex(2);  // LEAP
        m_ui->leapUsername->setText(m_wifiSecurity->leapUsername());
        m_ui->leapPassword->setText(m_wifiSecurity->leapPassword());

    } else if (keyMgmt == NetworkManager::Settings::WirelessSecuritySetting::Ieee8021x) {
        m_ui->securityCombo->setCurrentIndex(3);  // Dynamic WEP
        // done in the widget

    } else if (keyMgmt == NetworkManager::Settings::WirelessSecuritySetting::WpaPsk) {
        m_ui->securityCombo->setCurrentIndex(4);  // WPA
        m_ui->psk->setText(m_wifiSecurity->psk());

    } else if (keyMgmt == NetworkManager::Settings::WirelessSecuritySetting::WpaEap) {
        m_ui->securityCombo->setCurrentIndex(5);  // WPA2 Enterprise
        // done in the widget
    }
}

QVariantMap WifiSecurity::setting() const
{
    QVariantMap tmp80211Map;

    const int securityIndex = m_ui->securityCombo->currentIndex();
    if (securityIndex == 0) {
        m_wifiSecurity->setKeyMgmt(NetworkManager::Settings::WirelessSecuritySetting::Unknown);
    }
    else if (securityIndex == 1)  { // WEP
        m_wifiSecurity->setKeyMgmt(NetworkManager::Settings::WirelessSecuritySetting::Wep);
        const int keyIndex = m_ui->wepIndex->currentIndex();
        const QString wepKey = m_ui->wepKey->text();
        m_wifiSecurity->setWepTxKeyindex(keyIndex);
        if (keyIndex == 0)
            m_wifiSecurity->setWepKey0(wepKey);
        else if (keyIndex == 1)
            m_wifiSecurity->setWepKey1(wepKey);
        else if (keyIndex == 2)
            m_wifiSecurity->setWepKey2(wepKey);
        else if (keyIndex == 3)
            m_wifiSecurity->setWepKey3(wepKey);

        if (m_ui->wepAuth->currentIndex() == 0)
            m_wifiSecurity->setAuthAlg(NetworkManager::Settings::WirelessSecuritySetting::Open);
        else
            m_wifiSecurity->setAuthAlg(NetworkManager::Settings::WirelessSecuritySetting::Shared);
    } else if (securityIndex == 2) { // LEAP
        m_wifiSecurity->setKeyMgmt(NetworkManager::Settings::WirelessSecuritySetting::Ieee8021x);
        m_wifiSecurity->setAuthAlg(NetworkManager::Settings::WirelessSecuritySetting::Leap);
        m_wifiSecurity->setLeapUsername(m_ui->leapUsername->text());
        m_wifiSecurity->setLeapPassword(m_ui->leapPassword->text());
    } else if (securityIndex == 3) {  // Dynamic WEP
        m_wifiSecurity->setKeyMgmt(NetworkManager::Settings::WirelessSecuritySetting::Ieee8021x);
        tmp80211Map = m_8021xWidget->setting();
    } else if (securityIndex == 4) { // WPA
        m_wifiSecurity->setKeyMgmt(NetworkManager::Settings::WirelessSecuritySetting::WpaPsk);
        m_wifiSecurity->setPsk(m_ui->psk->text());
    } else if (securityIndex == 5) {  // WPA2 Enterprise
        m_wifiSecurity->setKeyMgmt(NetworkManager::Settings::WirelessSecuritySetting::WpaEap);
        tmp80211Map = m_WPA2Widget->setting();
    }

    return m_wifiSecurity->toMap();
}

QVariantMap WifiSecurity::setting8021x() const
{
    if (m_ui->securityCombo->currentIndex() == 3) // Dynamic WEP
        return m_8021xWidget->setting();
    else if (m_ui->securityCombo->currentIndex() == 5) // WPA2 Enterprise
        return m_WPA2Widget->setting();

    return QVariantMap();
}

void WifiSecurity::setWepKey(int keyIndex)
{
    if (keyIndex == 0)
        m_ui->wepKey->setText(m_wifiSecurity->wepKey0());
    else if (keyIndex == 1)
        m_ui->wepKey->setText(m_wifiSecurity->wepKey1());
    else if (keyIndex == 2)
        m_ui->wepKey->setText(m_wifiSecurity->wepKey2());
    else if (keyIndex == 3)
        m_ui->wepKey->setText(m_wifiSecurity->wepKey3());
}

void WifiSecurity::slotShowWepKeyPasswordChecked(bool checked)
{
    m_ui->wepKey->setPasswordMode(!checked);
}

void WifiSecurity::slotShowLeapPasswordChecked(bool checked)
{
    m_ui->leapPassword->setPasswordMode(!checked);
}

void WifiSecurity::slotShowPskPasswordChecked(bool checked)
{
    m_ui->psk->setPasswordMode(!checked);
}
