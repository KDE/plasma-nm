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

#include <NetworkManagerQt/Utils>

WifiSecurity::WifiSecurity(const NetworkManager::Setting::Ptr &setting, const NetworkManager::Security8021xSetting::Ptr &setting8021x,
                           QWidget* parent, Qt::WindowFlags f):
    SettingWidget(setting, parent, f),
    m_ui(new Ui::WifiSecurity)
{
    m_wifiSecurity = setting.staticCast<NetworkManager::WirelessSecuritySetting>();

    m_ui->setupUi(this);

    m_8021xWidget = new Security8021x(setting8021x, true, this);  // Dynamic WEP
    m_WPA2Widget = new Security8021x(setting8021x, true, this);   // WPA(2) Enterprise
    m_ui->stackedWidget->insertWidget(3, m_8021xWidget);
    m_ui->stackedWidget->insertWidget(5, m_WPA2Widget);

    connect(m_ui->securityCombo, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &WifiSecurity::securityChanged);
    connect(m_ui->wepIndex, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &WifiSecurity::setWepKey);

    connect(m_ui->wepKey, &KLineEdit::textChanged, this, &WifiSecurity::slotWidgetChanged);
    connect(m_ui->leapUsername, &KLineEdit::textChanged, this, &WifiSecurity::slotWidgetChanged);
    connect(m_ui->leapPassword, &KLineEdit::textChanged, this, &WifiSecurity::slotWidgetChanged);
    connect(m_ui->psk, &KLineEdit::textChanged, this, &WifiSecurity::slotWidgetChanged);
    connect(m_ui->wepIndex, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &WifiSecurity::slotWidgetChanged);
    connect(m_ui->securityCombo, static_cast<void (KComboBox::*)(int)>(&KComboBox::currentIndexChanged), this, &WifiSecurity::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (setting)
        loadConfig(setting);
}

WifiSecurity::~WifiSecurity()
{
    delete m_ui;
}

bool WifiSecurity::enabled() const
{
    return m_ui->securityCombo->currentIndex() > 0;
}

bool WifiSecurity::enabled8021x() const
{
    if (m_ui->securityCombo->currentIndex() == 4 || m_ui->securityCombo->currentIndex() == 6) {
        return true;
    }

    return false;
}

bool WifiSecurity::isValid() const
{
    const int securityIndex = m_ui->securityCombo->currentIndex();

    if (securityIndex == WepHex) { // WEP Hex
        return NetworkManager::wepKeyIsValid(m_ui->wepKey->text(), NetworkManager::WirelessSecuritySetting::Hex);
    } else if (securityIndex == WepPassphrase) { // WEP Passphrase
        return NetworkManager::wepKeyIsValid(m_ui->wepKey->text(), NetworkManager::WirelessSecuritySetting::Passphrase);
    }else if (securityIndex == Leap) { // LEAP
        return !m_ui->leapUsername->text().isEmpty() && !m_ui->leapPassword->text().isEmpty();
    } else if (securityIndex == WpaPsk) { // WPA
        return NetworkManager::wpaPskIsValid(m_ui->psk->text());
    }

    return true;
}

void WifiSecurity::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity = setting.staticCast<NetworkManager::WirelessSecuritySetting>();

    const NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt = wifiSecurity->keyMgmt();
    const NetworkManager::WirelessSecuritySetting::AuthAlg authAlg = wifiSecurity->authAlg();

    if (keyMgmt == NetworkManager::WirelessSecuritySetting::Unknown) {
        m_ui->securityCombo->setCurrentIndex(None); // None

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Wep) {
        if (wifiSecurity->wepKeyType() == NetworkManager::WirelessSecuritySetting::Hex ||
            wifiSecurity->wepKeyType() == NetworkManager::WirelessSecuritySetting::NotSpecified) {
            m_ui->securityCombo->setCurrentIndex(WepHex);  // WEP Hex
        } else {
            m_ui->securityCombo->setCurrentIndex(WepPassphrase);
        }
        const int keyIndex = static_cast<int>(wifiSecurity->wepTxKeyindex());
        m_ui->wepIndex->setCurrentIndex(keyIndex);

        if (wifiSecurity->authAlg() == NetworkManager::WirelessSecuritySetting::Open) {
            m_ui->wepAuth->setCurrentIndex(0);
        }
        else {
            m_ui->wepAuth->setCurrentIndex(1);
        }

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x
               && authAlg == NetworkManager::WirelessSecuritySetting::Leap) {
        m_ui->securityCombo->setCurrentIndex(Leap);  // LEAP
        m_ui->leapUsername->setText(wifiSecurity->leapUsername());
        m_ui->leapPassword->setText(wifiSecurity->leapPassword());

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x) {
        m_ui->securityCombo->setCurrentIndex(DynamicWep);  // Dynamic WEP
        // done in the widget

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaPsk) {
        m_ui->securityCombo->setCurrentIndex(WpaPsk);  // WPA

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEap) {
        m_ui->securityCombo->setCurrentIndex(WpaEap);  // WPA2 Enterprise
        // done in the widget
    }

    if (keyMgmt != NetworkManager::WirelessSecuritySetting::Ieee8021x &&
        keyMgmt != NetworkManager::WirelessSecuritySetting::WpaEap) {
        loadSecrets(setting);
    }
}

void WifiSecurity::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    const NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt = m_wifiSecurity->keyMgmt();
    const NetworkManager::WirelessSecuritySetting::AuthAlg authAlg = m_wifiSecurity->authAlg();

    if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x ||
        keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEap) {
        NetworkManager::Security8021xSetting::Ptr security8021xSetting = setting.staticCast<NetworkManager::Security8021xSetting>();
        if (security8021xSetting) {
            if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x) {
                m_8021xWidget->loadSecrets(security8021xSetting);
            } else {
                m_WPA2Widget->loadSecrets(security8021xSetting);
            }
        }
    } else {
        NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity = setting.staticCast<NetworkManager::WirelessSecuritySetting>();
        if (wifiSecurity) {
            if (keyMgmt == NetworkManager::WirelessSecuritySetting::Wep) {
                m_wifiSecurity->secretsFromMap(wifiSecurity->secretsToMap());
                const int keyIndex = static_cast<int>(m_wifiSecurity->wepTxKeyindex());
                setWepKey(keyIndex);
            } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x
                    && authAlg == NetworkManager::WirelessSecuritySetting::Leap) {
                const QString leapPassword = wifiSecurity->leapPassword();
                if (!leapPassword.isEmpty()) {
                    m_ui->leapPassword->setText(leapPassword);
                }
            } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaPsk) {
                const QString psk = wifiSecurity->psk();
                if (!psk.isEmpty()) {
                    m_ui->psk->setText(psk);
                }
            }
        }
    }
}

QVariantMap WifiSecurity::setting(bool agentOwned) const
{
    NetworkManager::WirelessSecuritySetting wifiSecurity;

    const int securityIndex = m_ui->securityCombo->currentIndex();
    if (securityIndex == None) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Unknown);
    } else if (securityIndex == WepHex || securityIndex == WepPassphrase)  { // WEP
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Wep);
        if (securityIndex == WepHex) {
            wifiSecurity.setWepKeyType(NetworkManager::WirelessSecuritySetting::Hex);
        } else {
            wifiSecurity.setWepKeyType(NetworkManager::WirelessSecuritySetting::Passphrase);
        }
        const int keyIndex = m_ui->wepIndex->currentIndex();
        const QString wepKey = m_ui->wepKey->text();
        wifiSecurity.setWepTxKeyindex(keyIndex);
        if (keyIndex == 0)
            wifiSecurity.setWepKey0(wepKey);
        else if (keyIndex == 1)
            wifiSecurity.setWepKey1(wepKey);
        else if (keyIndex == 2)
            wifiSecurity.setWepKey2(wepKey);
        else if (keyIndex == 3)
            wifiSecurity.setWepKey3(wepKey);

        if (agentOwned) {
            wifiSecurity.setWepKeyFlags(NetworkManager::Setting::AgentOwned);
        }
        if (m_ui->wepAuth->currentIndex() == 0)
            wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Open);
        else
            wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Shared);
    } else if (securityIndex == Leap) { // LEAP
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
        wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Leap);
        wifiSecurity.setLeapUsername(m_ui->leapUsername->text());
        wifiSecurity.setLeapPassword(m_ui->leapPassword->text());
        if (agentOwned) {
            wifiSecurity.setLeapPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (securityIndex == DynamicWep) {  // Dynamic WEP
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
    } else if (securityIndex == WpaPsk) { // WPA
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
        wifiSecurity.setPsk(m_ui->psk->text());
        if (agentOwned) {
            wifiSecurity.setPskFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (securityIndex == WpaEap) {  // WPA2 Enterprise
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
    }

    return wifiSecurity.toMap();
}

QVariantMap WifiSecurity::setting8021x(bool agentOwned) const
{
    if (m_ui->securityCombo->currentIndex() == DynamicWep) // Dynamic WEP
        return m_8021xWidget->setting(agentOwned);
    else if (m_ui->securityCombo->currentIndex() == WpaEap) // WPA2 Enterprise
        return m_WPA2Widget->setting(agentOwned);

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

void WifiSecurity::securityChanged(int index)
{
    if (index == 0) {
        m_ui->stackedWidget->setCurrentIndex(0);
    } else if (index == 1 || index == 2) {
        m_ui->stackedWidget->setCurrentIndex(1);
    } else {
        m_ui->stackedWidget->setCurrentIndex(index-1);
    }

    KAcceleratorManager::manage(m_ui->stackedWidget->currentWidget());
}
