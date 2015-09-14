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

#include <KAcceleratorManager>

Security8021x::Security8021x(const NetworkManager::Security8021xSetting::Ptr &setting, bool wifiMode, QWidget *parent) :
    QWidget(parent),
    m_setting(setting),
    m_ui(new Ui::Security8021x)
{
    m_ui->setupUi(this);

    m_ui->auth->setItemData(0, NetworkManager::Security8021xSetting::EapMethodMd5);
    m_ui->auth->setItemData(1, NetworkManager::Security8021xSetting::EapMethodTls);
    m_ui->auth->setItemData(2, NetworkManager::Security8021xSetting::EapMethodLeap);
    m_ui->auth->setItemData(3, NetworkManager::Security8021xSetting::EapMethodFast);
    m_ui->auth->setItemData(4, NetworkManager::Security8021xSetting::EapMethodTtls);
    m_ui->auth->setItemData(5, NetworkManager::Security8021xSetting::EapMethodPeap);

    if (wifiMode) {
        m_ui->auth->removeItem(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodMd5)); // MD 5
        m_ui->stackedWidget->removeWidget(m_ui->md5Page);
    } else {
        m_ui->auth->removeItem(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodLeap)); // LEAP
        m_ui->stackedWidget->removeWidget(m_ui->leapPage);
    }

    KAcceleratorManager::manage(this);
    connect(m_ui->stackedWidget, &QStackedWidget::currentChanged, this, &Security8021x::currentAuthChanged);

    if (m_setting) {
        loadConfig();
    }
}

Security8021x::~Security8021x()
{
    delete m_ui;
}

void Security8021x::loadSecrets(const NetworkManager::Security8021xSetting::Ptr &setting)
{
    const QString password = setting->password();
    const QList<NetworkManager::Security8021xSetting::EapMethod> eapMethods = m_setting->eapMethods();

    if (!password.isEmpty()) {
        if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodMd5)) {
            m_ui->md5Password->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodLeap)) {
            m_ui->leapPassword->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodFast)) {
            m_ui->fastPassword->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTtls)) {
            m_ui->ttlsPassword->setText(setting->password());
        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPeap)) {
            m_ui->peapPassword->setText(setting->password());
        }
    }

    if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTls)) {
        const QString privateKeyPassword = setting->privateKeyPassword();
        if (!privateKeyPassword.isEmpty()) {
            m_ui->tlsPrivateKeyPassword->setText(setting->privateKeyPassword());
        }
    }
}

void Security8021x::loadConfig()
{
    const QList<NetworkManager::Security8021xSetting::EapMethod> eapMethods = m_setting->eapMethods();
    const NetworkManager::Security8021xSetting::AuthMethod phase2AuthMethod = m_setting->phase2AuthMethod();
    const bool notSavedPassword = m_setting->passwordFlags() & NetworkManager::Setting::NotSaved;

    if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodMd5)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodMd5));
        m_ui->md5UserName->setText(m_setting->identity());
        m_ui->cbAskMd5Password->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTls)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodTls));
        m_ui->tlsIdentity->setText(m_setting->identity());
        m_ui->tlsUserCert->setUrl(QUrl::fromLocalFile(m_setting->clientCertificate()));
        m_ui->tlsCACert->setUrl(QUrl::fromLocalFile(m_setting->caCertificate()));
        m_ui->tlsPrivateKey->setUrl(QUrl::fromLocalFile(m_setting->privateKey()));
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodLeap)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodLeap));
        m_ui->leapUsername->setText(m_setting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodFast)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodFast));
        m_ui->fastAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->fastAllowPacProvisioning->setChecked((int)m_setting->phase1FastProvisioning() > 0);
        m_ui->pacMethod->setCurrentIndex(m_setting->phase1FastProvisioning() - 1);
        m_ui->pacFile->setUrl(QUrl::fromLocalFile(m_setting->pacFile()));
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodGtc) {
            m_ui->fastInnerAuth->setCurrentIndex(0);
        } else {
            m_ui->fastInnerAuth->setCurrentIndex(1);
        }
        m_ui->fastUsername->setText(m_setting->identity());
        m_ui->cbAskFastPassword->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTtls)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodTtls));
        m_ui->ttlsAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->ttlsCACert->setUrl(QUrl::fromLocalFile(m_setting->caCertificate()));
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodPap) {
            m_ui->ttlsInnerAuth->setCurrentIndex(0);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschap) {
            m_ui->ttlsInnerAuth->setCurrentIndex(1);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschapv2) {
            m_ui->ttlsInnerAuth->setCurrentIndex(2);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodChap) {
            m_ui->ttlsInnerAuth->setCurrentIndex(3);
        }
        m_ui->ttlsUsername->setText(m_setting->identity());
        m_ui->cbAskTtlsPassword->setChecked(notSavedPassword);
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPeap)) {
        m_ui->auth->setCurrentIndex(m_ui->auth->findData(NetworkManager::Security8021xSetting::EapMethodPeap));
        m_ui->peapAnonIdentity->setText(m_setting->anonymousIdentity());
        m_ui->peapCACert->setUrl(QUrl::fromLocalFile(m_setting->caCertificate()));
        m_ui->peapVersion->setCurrentIndex(m_setting->phase1PeapVersion() + 1);
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschapv2) {
            m_ui->peapInnerAuth->setCurrentIndex(0);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMd5) {
            m_ui->peapInnerAuth->setCurrentIndex(1);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodGtc) {
            m_ui->peapInnerAuth->setCurrentIndex(2);
        }
        m_ui->peapUsername->setText(m_setting->identity());
        m_ui->cbAskPeapPassword->setChecked(notSavedPassword);
    }

    loadSecrets(m_setting);
}

QVariantMap Security8021x::setting(bool agentOwned) const
{
    NetworkManager::Security8021xSetting setting;

    NetworkManager::Security8021xSetting::EapMethod method =
            static_cast<NetworkManager::Security8021xSetting::EapMethod>(m_ui->auth->itemData(m_ui->auth->currentIndex()).toInt());

    setting.setEapMethods(QList<NetworkManager::Security8021xSetting::EapMethod>() << method);

    if (method == NetworkManager::Security8021xSetting::EapMethodMd5) {
        if (!m_ui->md5UserName->text().isEmpty()) {
            setting.setIdentity(m_ui->md5UserName->text());
        }

        if (m_ui->cbAskMd5Password->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->md5Password->text().isEmpty()) {
            setting.setPassword(m_ui->md5Password->text());
        }

        if (agentOwned && !m_ui->cbAskMd5Password->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTls) {
        if (!m_ui->tlsIdentity->text().isEmpty()) {
            setting.setIdentity(m_ui->tlsIdentity->text());
        }

        if (m_ui->tlsUserCert->url().isValid()) {
            setting.setClientCertificate(m_ui->tlsUserCert->url().toString().toUtf8().append('\0'));
        }

        if (m_ui->tlsCACert->url().isValid()) {
            setting.setCaCertificate(m_ui->tlsCACert->url().toString().toUtf8().append('\0'));
        }

        if (m_ui->tlsPrivateKey->url().isValid()) {
            setting.setPrivateKey(m_ui->tlsPrivateKey->url().toString().toUtf8().append('\0'));
        }

        if (!m_ui->tlsPrivateKeyPassword->text().isEmpty()) {
            setting.setPrivateKeyPassword(m_ui->tlsPrivateKeyPassword->text());
        }

        if (agentOwned) {
            setting.setPrivateKeyPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodLeap) {
        if (!m_ui->leapUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->leapUsername->text());
        }

        if (!m_ui->leapPassword->text().isEmpty()) {
            setting.setPassword(m_ui->leapPassword->text());
        }

        if (agentOwned) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodFast) {
        if (!m_ui->fastAnonIdentity->text().isEmpty()) {
            setting.setAnonymousIdentity(m_ui->fastAnonIdentity->text());
        }

        if (!m_ui->fastAllowPacProvisioning->isChecked()) {
            setting.setPhase1FastProvisioning(NetworkManager::Security8021xSetting::FastProvisioningDisabled);
        } else {
            setting.setPhase1FastProvisioning(static_cast<NetworkManager::Security8021xSetting::FastProvisioning>(m_ui->pacMethod->currentIndex() + 1));
        }

        if (m_ui->pacFile->url().isValid()) {
            setting.setPacFile(QFile::encodeName(m_ui->pacFile->url().toLocalFile()));
        }

        if (m_ui->fastInnerAuth->currentIndex() == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
        } else {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        }

        if (!m_ui->fastUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->fastUsername->text());
        }

        if (m_ui->cbAskFastPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->fastPassword->text().isEmpty()) {
            setting.setPassword(m_ui->fastPassword->text());
        }

        if (agentOwned && !m_ui->cbAskFastPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTtls) {
        if (!m_ui->ttlsAnonIdentity->text().isEmpty()) {
            setting.setAnonymousIdentity(m_ui->ttlsAnonIdentity->text());
        }

        if (m_ui->ttlsCACert->url().isValid()) {
            setting.setCaCertificate(m_ui->ttlsCACert->url().toString().toUtf8().append('\0'));
        }

        const int innerAuth = m_ui->ttlsInnerAuth->currentIndex();
        if (innerAuth == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodPap);
        } else if (innerAuth == 1) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschap);
        } else if (innerAuth == 2) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        } else if (innerAuth == 3) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodChap);
        }

        if (!m_ui->ttlsUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->ttlsUsername->text());
        }

        if (m_ui->cbAskTtlsPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->ttlsPassword->text().isEmpty()) {
            setting.setPassword(m_ui->ttlsPassword->text());
        }

        if (agentOwned && !m_ui->cbAskTtlsPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodPeap) {
        if (!m_ui->peapAnonIdentity->text().isEmpty()) {
            setting.setAnonymousIdentity(m_ui->peapAnonIdentity->text());
        }

        if (m_ui->peapCACert->url().isValid()) {
            setting.setCaCertificate(m_ui->peapCACert->url().toString().toUtf8().append('\0'));
        }

        setting.setPhase1PeapVersion(static_cast<NetworkManager::Security8021xSetting::PeapVersion>(m_ui->peapVersion->currentIndex() - 1));
        const int innerAuth = m_ui->peapInnerAuth->currentIndex();
        if (innerAuth == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        } else if (innerAuth == 1) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMd5);
        } else if (innerAuth == 2) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
        }

        if (m_ui->cbAskPeapPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        } else if (!m_ui->peapPassword->text().isEmpty()) {
            setting.setPassword(m_ui->peapPassword->text());
        }

        if (!m_ui->peapUsername->text().isEmpty()) {
            setting.setIdentity(m_ui->peapUsername->text());
        }

        if (agentOwned && !m_ui->cbAskPeapPassword->isChecked()) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        }
    }

    return setting.toMap();
}

void Security8021x::currentAuthChanged(int index)
{
    Q_UNUSED(index);
    KAcceleratorManager::manage(m_ui->stackedWidget->currentWidget());
}

