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

#include <QDebug>

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

    connect(m_ui->securityCombo, SIGNAL(currentIndexChanged(int)), SLOT(securityChanged(int)));

    connect(m_ui->cbShowWepKey, SIGNAL(toggled(bool)), SLOT(slotShowWepKeyPasswordChecked(bool)));
    connect(m_ui->cbShowLeapPassword, SIGNAL(toggled(bool)), SLOT(slotShowLeapPasswordChecked(bool)));
    connect(m_ui->cbShowPsk, SIGNAL(toggled(bool)), SLOT(slotShowPskPasswordChecked(bool)));

    connect(m_ui->wepIndex, SIGNAL(currentIndexChanged(int)), SLOT(setWepKey(int)));

    connect(m_ui->wepKey, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->leapUsername, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->leapPassword, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->psk, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));
    connect(m_ui->wepIndex, SIGNAL(currentIndexChanged(int)), SLOT(slotWidgetChanged()));
    connect(m_ui->securityCombo, SIGNAL(currentIndexChanged(int)), SLOT(slotWidgetChanged()));

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
    if (m_ui->securityCombo->currentIndex() == 3 || m_ui->securityCombo->currentIndex() == 5) {
        return true;
    }

    return false;
}

bool WifiSecurity::isValid() const
{
    const int securityIndex = m_ui->securityCombo->currentIndex();

    if (securityIndex == 1) { // WEP Hex
        return NetworkManager::Utils::wepKeyIsValid(m_ui->wepKey->text(), NetworkManager::WirelessSecuritySetting::Hex);
    } else if (securityIndex == 2) { // WEP Passphrase
        return NetworkManager::Utils::wepKeyIsValid(m_ui->wepKey->text(), NetworkManager::WirelessSecuritySetting::Passphrase);
    }else if (securityIndex == 3) { // LEAP
        return !m_ui->leapUsername->text().isEmpty() && !m_ui->leapPassword->text().isEmpty();
    } else if (securityIndex == 5) { // WPA
        return NetworkManager::Utils::wpaPskIsValid(m_ui->psk->text());
    }

    return true;
}

void WifiSecurity::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity = setting.staticCast<NetworkManager::WirelessSecuritySetting>();

    const NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt = wifiSecurity->keyMgmt();
    const NetworkManager::WirelessSecuritySetting::AuthAlg authAlg = wifiSecurity->authAlg();

    if (keyMgmt == NetworkManager::WirelessSecuritySetting::Unknown) {
        m_ui->securityCombo->setCurrentIndex(0); // None

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Wep) {
        if (wifiSecurity->wepKeyType() == NetworkManager::WirelessSecuritySetting::Hex ||
            wifiSecurity->wepKeyType() == NetworkManager::WirelessSecuritySetting::NotSpecified) {
            m_ui->securityCombo->setCurrentIndex(1);  // WEP Hex
        } else {
            m_ui->securityCombo->setCurrentIndex(2);
        }
        const int keyIndex = static_cast<int>(wifiSecurity->wepTxKeyindex());
        setWepKey(keyIndex);
        m_ui->wepIndex->setCurrentIndex(keyIndex);

        if (wifiSecurity->authAlg() == NetworkManager::WirelessSecuritySetting::Open) {
            m_ui->wepAuth->setCurrentIndex(0);
        }
        else {
            m_ui->wepAuth->setCurrentIndex(1);
        }

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x
               && authAlg == NetworkManager::WirelessSecuritySetting::Leap) {
        m_ui->securityCombo->setCurrentIndex(2);  // LEAP
        m_ui->leapUsername->setText(wifiSecurity->leapUsername());
        m_ui->leapPassword->setText(wifiSecurity->leapPassword());

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x) {
        m_ui->securityCombo->setCurrentIndex(3);  // Dynamic WEP
        // done in the widget

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaPsk) {
        m_ui->securityCombo->setCurrentIndex(4);  // WPA
        m_ui->psk->setText(wifiSecurity->psk());

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEap) {
        m_ui->securityCombo->setCurrentIndex(5);  // WPA2 Enterprise
        // done in the widget
    }
}

QVariantMap WifiSecurity::setting(bool agentOwned) const
{
    NetworkManager::WirelessSecuritySetting wifiSecurity;

    const int securityIndex = m_ui->securityCombo->currentIndex();
    if (securityIndex == 0) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Unknown);
    } else if (securityIndex == 1 || securityIndex == 2)  { // WEP
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Wep);
        if (securityIndex == 1) {
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
    } else if (securityIndex == 2) { // LEAP
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
        wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Leap);
        wifiSecurity.setLeapUsername(m_ui->leapUsername->text());
        wifiSecurity.setLeapPassword(m_ui->leapPassword->text());
        if (agentOwned) {
            wifiSecurity.setLeapPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (securityIndex == 3) {  // Dynamic WEP
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
    } else if (securityIndex == 4) { // WPA
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
        wifiSecurity.setPsk(m_ui->psk->text());
        if (agentOwned) {
            wifiSecurity.setPskFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (securityIndex == 5) {  // WPA2 Enterprise
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
    }

    return wifiSecurity.toMap();
}

QVariantMap WifiSecurity::setting8021x(bool agentOwned) const
{
    if (m_ui->securityCombo->currentIndex() == 3) // Dynamic WEP
        return m_8021xWidget->setting(agentOwned);
    else if (m_ui->securityCombo->currentIndex() == 5) // WPA2 Enterprise
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
