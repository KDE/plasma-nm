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

#include "security802-1x.h"
#include "ui_802-1x.h"

Security8021x::Security8021x(NetworkManager::Settings::Security8021xSetting *setting, bool wifiMode, QWidget *parent) :
    QWidget(parent),
    m_setting(setting),
    m_ui(new Ui::Security8021x)
{
    m_ui->setupUi(this);

    m_ui->auth->setItemData(0, NetworkManager::Settings::Security8021xSetting::EapMethodMd5);
    m_ui->auth->setItemData(1, NetworkManager::Settings::Security8021xSetting::EapMethodTls);
    m_ui->auth->setItemData(2, NetworkManager::Settings::Security8021xSetting::EapMethodLeap);
    m_ui->auth->setItemData(3, NetworkManager::Settings::Security8021xSetting::EapMethodFast);
    m_ui->auth->setItemData(4, NetworkManager::Settings::Security8021xSetting::EapMethodTtls);
    m_ui->auth->setItemData(5, NetworkManager::Settings::Security8021xSetting::EapMethodPeap);

    connect(m_ui->cbShowMd5Password, SIGNAL(toggled(bool)), SLOT(setShowMD5Password(bool)));
    connect(m_ui->cbShowTlsPassword, SIGNAL(toggled(bool)), SLOT(setShowTlsPrivateKeyPassword(bool)));
    connect(m_ui->cbShowLeapPassword, SIGNAL(toggled(bool)), SLOT(setShowLeapPassword(bool)));
    connect(m_ui->cbShowFastPassword, SIGNAL(toggled(bool)), SLOT(setShowFastPassword(bool)));
    connect(m_ui->cbShowTtlsPassword, SIGNAL(toggled(bool)), SLOT(setShowTtlsPassword(bool)));
    connect(m_ui->cbShowPeapPassword, SIGNAL(toggled(bool)), SLOT(setShowPeapPassword(bool)));

    connect(m_ui->cbAskMd5Password, SIGNAL(toggled(bool)), m_ui->cbShowMd5Password, SLOT(setDisabled(bool)));
    connect(m_ui->cbAskFastPassword, SIGNAL(toggled(bool)), m_ui->cbShowFastPassword, SLOT(setDisabled(bool)));
    connect(m_ui->cbAskPeapPassword, SIGNAL(toggled(bool)), m_ui->cbShowPeapPassword, SLOT(setDisabled(bool)));
    connect(m_ui->cbAskTtlsPassword, SIGNAL(toggled(bool)), m_ui->cbShowTtlsPassword, SLOT(setDisabled(bool)));

    if (wifiMode) {
        m_ui->auth->removeItem(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodMd5)); // MD 5
        m_ui->stackedWidget->removeWidget(m_ui->md5Page);
    }
    else {
        m_ui->auth->removeItem(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodLeap)); // LEAP
        m_ui->stackedWidget->removeWidget(m_ui->leapPage);
    }

    if (m_setting)
        loadConfig();
}

Security8021x::~Security8021x()
{
}

void Security8021x::loadConfig()
{
    const QList<NetworkManager::Settings::Security8021xSetting::EapMethod> eapMethods = m_setting->eapMethods();
    const NetworkManager::Settings::Security8021xSetting::AuthMethod phase2AuthMethod = m_setting->phase2AuthMethod();
    const bool notSavedPassword = m_setting->passwordFlags() & NetworkManager::Settings::Setting::NotSaved;

    if (eapMethods.contains(NetworkManager::Settings::Security8021xSetting::EapMethodMd5)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodMd5));
        m_ui->md5UserName->setText(m_setting->identity());
        m_ui->md5Password->setText(m_setting->password());
        m_ui->cbAskMd5Password->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Settings::Security8021xSetting::EapMethodTls)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodTls));
        m_ui->tlsIdentity->setText(m_setting->identity());
        m_ui->tlsUserCert->setText(m_setting->clientCertificate()); // FIXME check the blob vs. path case
        m_ui->tlsCACert->setText(m_setting->caCertificate()); // FIXME check the blob vs. path case
        m_ui->tlsPrivateKey->setText(m_setting->privateKey()); // FIXME check the blob vs. path case
        m_ui->tlsPrivateKeyPassword->setText(m_setting->privateKeyPassword());
    } else if (eapMethods.contains(NetworkManager::Settings::Security8021xSetting::EapMethodLeap)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodLeap));
        m_ui->leapUsername->setText(m_setting->identity());
        m_ui->leapPassword->setText(m_setting->password());
    } else if (eapMethods.contains(NetworkManager::Settings::Security8021xSetting::EapMethodFast)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodFast));
        m_ui->fastAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->fastAllowPacProvisioning->setChecked((int)m_setting->phase1FastProvisioning() > 0);
        m_ui->pacMethod->setCurrentIndex(m_setting->phase1FastProvisioning() - 1);
        m_ui->pacFile->setText(m_setting->pacFile()); // TODO check the file scheme used
        if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodGtc)
            m_ui->fastInnerAuth->setCurrentIndex(0);
        else
            m_ui->fastInnerAuth->setCurrentIndex(1);
        m_ui->fastUsername->setText(m_setting->identity());
        m_ui->fastPassword->setText(m_setting->password());
        m_ui->cbAskFastPassword->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Settings::Security8021xSetting::EapMethodTtls)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodTtls));
        m_ui->ttlsAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->ttlsCACert->setText(m_setting->caCertificate());  // FIXME check the blob vs. path case
        if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodPap)
            m_ui->ttlsInnerAuth->setCurrentIndex(0);
        else if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodMschap)
            m_ui->ttlsInnerAuth->setCurrentIndex(1);
        else if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodMschapv2)
            m_ui->ttlsInnerAuth->setCurrentIndex(2);
        else if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodChap)
            m_ui->ttlsInnerAuth->setCurrentIndex(3);
        m_ui->ttlsUsername->setText(m_setting->identity());
        m_ui->ttlsPassword->setText(m_setting->password());
        m_ui->cbAskTtlsPassword->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Settings::Security8021xSetting::EapMethodPeap)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Settings::Security8021xSetting::EapMethodPeap));
        m_ui->peapAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->peapCACert->setText(m_setting->caCertificate()); // FIXME check the blob vs. path case
        m_ui->peapVersion->setCurrentIndex(m_setting->phase1PeapVersion() + 1);
        if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodMschapv2)
            m_ui->peapInnerAuth->setCurrentIndex(0);
        else if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodMd5)
            m_ui->peapInnerAuth->setCurrentIndex(1);
        else if (phase2AuthMethod == NetworkManager::Settings::Security8021xSetting::AuthMethodGtc)
            m_ui->peapInnerAuth->setCurrentIndex(2);
        m_ui->peapUsername->setText(m_setting->identity());
        m_ui->peapPassword->setText(m_setting->password());
        m_ui->cbAskPeapPassword->setChecked(notSavedPassword);
    }
}

QVariantMap Security8021x::setting() const
{
    NetworkManager::Settings::Security8021xSetting::EapMethod method =
            static_cast<NetworkManager::Settings::Security8021xSetting::EapMethod>(m_ui->auth->itemData(m_ui->auth->currentIndex()).toInt());

    m_setting->setEapMethods(QList<NetworkManager::Settings::Security8021xSetting::EapMethod>() << method);

    if (method == NetworkManager::Settings::Security8021xSetting::EapMethodMd5) {
        if (!m_ui->md5UserName->text().isEmpty())
            m_setting->setIdentity(m_ui->md5UserName->text());
        if (m_ui->cbAskMd5Password->isChecked())
            m_setting->setPasswordFlags(NetworkManager::Settings::Setting::NotSaved);
        else if (!m_ui->md5Password->text().isEmpty())
            m_setting->setPassword(m_ui->md5Password->text());
    } else if (method == NetworkManager::Settings::Security8021xSetting::EapMethodTls) {
        if (!m_ui->tlsIdentity->text().isEmpty())
            m_setting->setIdentity(m_ui->tlsIdentity->text());
        if (!m_ui->tlsUserCert->url().isEmpty())
            m_setting->setClientCertificate(QFile::encodeName(m_ui->tlsUserCert->url().url()));
        if (!m_ui->tlsCACert->url().isEmpty())
            m_setting->setCaCertificate(QFile::encodeName(m_ui->tlsCACert->url().url()));
        if (!m_ui->tlsPrivateKey->url().isEmpty())
            m_setting->setPrivateKey(QFile::encodeName(m_ui->tlsPrivateKey->url().url()));
        if (!m_ui->tlsPrivateKeyPassword->text().isEmpty())
            m_setting->setPrivateKeyPassword(m_ui->tlsPrivateKeyPassword->text());
    } else if (method == NetworkManager::Settings::Security8021xSetting::EapMethodLeap) {
        if (!m_ui->leapUsername->text().isEmpty())
            m_setting->setIdentity(m_ui->leapUsername->text());
        if (!m_ui->leapPassword->text().isEmpty())
            m_setting->setPassword(m_ui->leapPassword->text());
    } else if (method == NetworkManager::Settings::Security8021xSetting::EapMethodFast) {
        if (!m_ui->fastAnonIdentity->text().isEmpty())
            m_setting->setAnonymousIdentity(m_ui->fastAnonIdentity->text());
        if (!m_ui->fastAllowPacProvisioning->isChecked()) {
            m_setting->setPhase1FastProvisioning(NetworkManager::Settings::Security8021xSetting::FastProvisioningDisabled);
        } else {
            m_setting->setPhase1FastProvisioning(static_cast<NetworkManager::Settings::Security8021xSetting::FastProvisioning>(m_ui->pacMethod->currentIndex() + 1));
        }
        if (!m_ui->pacFile->text().isEmpty())
            m_setting->setPacFile(QFile::encodeName(m_ui->pacFile->url().url()));
        if (m_ui->fastInnerAuth->currentIndex() == 0)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodGtc);
        else
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodMschapv2);
        if (!m_ui->fastUsername->text().isEmpty())
            m_setting->setIdentity(m_ui->fastUsername->text());
        if (m_ui->cbAskFastPassword->isChecked())
            m_setting->setPasswordFlags(NetworkManager::Settings::Setting::NotSaved);
        else if (!m_ui->fastPassword->text().isEmpty())
            m_setting->setPassword(m_ui->fastPassword->text());
    } else if (method == NetworkManager::Settings::Security8021xSetting::EapMethodTtls) {
        if (!m_ui->ttlsAnonIdentity->text().isEmpty())
            m_setting->setAnonymousIdentity(m_ui->ttlsAnonIdentity->text());
        if (!m_ui->ttlsCACert->text().isEmpty())
            m_setting->setCaCertificate(QFile::encodeName(m_ui->ttlsCACert->url().url()));
        const int innerAuth = m_ui->ttlsInnerAuth->currentIndex();
        if (innerAuth == 0)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodPap);
        else if (innerAuth == 1)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodMschap);
        else if (innerAuth == 2)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodMschapv2);
        else if (innerAuth == 3)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodChap);
        if (!m_ui->ttlsUsername->text().isEmpty())
            m_setting->setIdentity(m_ui->ttlsUsername->text());
        if (m_ui->cbAskTtlsPassword->isChecked())
            m_setting->setPasswordFlags(NetworkManager::Settings::Setting::NotSaved);
        else if (!m_ui->ttlsPassword->text().isEmpty())
            m_setting->setPassword(m_ui->ttlsPassword->text());
    } else if (method == NetworkManager::Settings::Security8021xSetting::EapMethodPeap) {
        if (!m_ui->peapAnonIdentity->text().isEmpty())
            m_setting->setAnonymousIdentity(m_ui->peapAnonIdentity->text());
        if (!m_ui->peapCACert->text().isEmpty())
            m_setting->setCaCertificate(QFile::encodeName(m_ui->peapCACert->url().url()));
        m_setting->setPhase1PeapVersion(static_cast<NetworkManager::Settings::Security8021xSetting::PeapVersion>(m_ui->peapVersion->currentIndex() - 1));
        const int innerAuth = m_ui->peapInnerAuth->currentIndex();
        if (innerAuth == 0)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodMschapv2);
        else if (innerAuth == 1)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodMd5);
        else if (innerAuth == 2)
            m_setting->setPhase2AuthMethod(NetworkManager::Settings::Security8021xSetting::AuthMethodGtc);
        if (m_ui->cbAskPeapPassword->isChecked())
            m_setting->setPasswordFlags(NetworkManager::Settings::Setting::NotSaved);
        else if (!m_ui->peapPassword->text().isEmpty())
            m_setting->setPassword(m_ui->peapPassword->text());
    }

    return m_setting->toMap();
}

void Security8021x::setShowMD5Password(bool on)
{
    m_ui->md5Password->setPasswordMode(!on);
}

void Security8021x::setShowTlsPrivateKeyPassword(bool on)
{
    m_ui->tlsPrivateKeyPassword->setPasswordMode(!on);
}

void Security8021x::setShowLeapPassword(bool on)
{
    m_ui->leapPassword->setPasswordMode(!on);
}

void Security8021x::setShowFastPassword(bool on)
{
    m_ui->fastPassword->setPasswordMode(!on);
}

void Security8021x::setShowTtlsPassword(bool on)
{
    m_ui->ttlsPassword->setPasswordMode(!on);
}

void Security8021x::setShowPeapPassword(bool on)
{
    m_ui->peapPassword->setPasswordMode(!on);
}
