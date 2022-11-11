/*
    SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fortisslvpnwidget.h"

#include "nm-fortisslvpn-service.h"

#include <NetworkManagerQt/Setting>

#include <QDialog>
#include <QDialogButtonBox>

FortisslvpnWidget::FortisslvpnWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent, Qt::WindowFlags f)
    : SettingWidget(setting, parent, f)
{
    m_setting = setting;

    ui.setupUi(this);

    ui.password->setPasswordOptionsEnabled(true);
    ui.password->setPasswordNotRequiredEnabled(true);

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(ui.gateway, &QLineEdit::textChanged, this, &FortisslvpnWidget::slotWidgetChanged);

    // Advanced configuration
    connect(ui.advancedButton, &QPushButton::clicked, this, &FortisslvpnWidget::showAdvanced);

    advancedDlg = new QDialog(this);
    advancedWid = new QWidget(this);
    advUi.setupUi(advancedWid);
    auto layout = new QVBoxLayout(advancedDlg);
    layout->addWidget(advancedWid);
    advancedDlg->setLayout(layout);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, advancedDlg);
    connect(buttons, &QDialogButtonBox::accepted, advancedDlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, advancedDlg, &QDialog::reject);
    layout->addWidget(buttons);
    KAcceleratorManager::manage(this);

    // Remove these from setting check:
    // Just popping up the advancedDlg changes nothing
    disconnect(ui.advancedButton, &QPushButton::clicked, this, &SettingWidget::settingChanged);
    // But the accept button does
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingWidget::settingChanged);

    if (setting && !setting->isNull()) {
        loadConfig(setting);
    }
}

void FortisslvpnWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    const NMStringMap data = m_setting->data();

    const QString gateway = data.value(NM_FORTISSLVPN_KEY_GATEWAY);
    if (!gateway.isEmpty()) {
        ui.gateway->setText(gateway);
    }

    const QString username = data.value(NM_FORTISSLVPN_KEY_USER);
    if (!username.isEmpty()) {
        ui.username->setText(username);
    }

    const NetworkManager::Setting::SecretFlags passwordFlag =
        static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_PASSWORD "-flags").toInt());
    if (passwordFlag == NetworkManager::Setting::None) {
        ui.password->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (passwordFlag == NetworkManager::Setting::AgentOwned) {
        ui.password->setPasswordOption(PasswordField::StoreForUser);
    } else if (passwordFlag == NetworkManager::Setting::NotSaved) {
        ui.password->setPasswordOption(PasswordField::AlwaysAsk);
    } else {
        ui.password->setPasswordOption(PasswordField::NotRequired);
    }

    const QString caCert = data.value(NM_FORTISSLVPN_KEY_CA);
    if (!caCert.isEmpty()) {
        ui.caCert->setText(caCert);
    }

    const QString userCert = data.value(NM_FORTISSLVPN_KEY_CERT);
    if (!userCert.isEmpty()) {
        ui.userCert->setText(userCert);
    }

    const QString userKey = data.value(NM_FORTISSLVPN_KEY_KEY);
    if (!userKey.isEmpty()) {
        ui.userKey->setText(userKey);
    }

    // From advanced dialog
    const QString trustedCert = data.value(NM_FORTISSLVPN_KEY_TRUSTED_CERT);
    if (!trustedCert.isEmpty()) {
        advUi.trustedCert->setText(trustedCert);
    }

    if (!data.value(NM_FORTISSLVPN_KEY_OTP "-flags").isEmpty()) {
        const NetworkManager::Setting::SecretFlags otpFlag =
            static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_OTP "-flags").toInt());
        if (otpFlag & NetworkManager::Setting::NotSaved) {
            advUi.otp->setChecked(true);
        }
    }

    if (!data.value(NM_FORTISSLVPN_KEY_2FA "-flags").isEmpty()) {
        const NetworkManager::Setting::SecretFlags tfaFlag =
            static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_2FA "-flags").toInt());
        if (tfaFlag & NetworkManager::Setting::AgentOwned) {
            advUi.tfa->setChecked(true);
        }
    }

    const QString realm = data.value(NM_FORTISSLVPN_KEY_REALM);
    if (!realm.isEmpty()) {
        advUi.realm->setText(realm);
    }

    loadSecrets(setting);
}

void FortisslvpnWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();

        const QString password = secrets.value(NM_FORTISSLVPN_KEY_PASSWORD);
        if (!password.isEmpty()) {
            ui.password->setText(password);
        }
    }
}

QVariantMap FortisslvpnWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_FORTISSLVPN));
    NMStringMap data;
    NMStringMap secrets;

    data.insert(NM_FORTISSLVPN_KEY_GATEWAY, ui.gateway->text());

    if (!ui.username->text().isEmpty()) {
        data.insert(NM_FORTISSLVPN_KEY_USER, ui.username->text());
    }

    if (!ui.password->text().isEmpty()) {
        secrets.insert(NM_FORTISSLVPN_KEY_PASSWORD, ui.password->text());
    }

    if (ui.password->passwordOption() == PasswordField::StoreForAllUsers) {
        data.insert(NM_FORTISSLVPN_KEY_PASSWORD "-flags", QString::number(NetworkManager::Setting::None));
    } else if (ui.password->passwordOption() == PasswordField::StoreForUser) {
        data.insert(NM_FORTISSLVPN_KEY_PASSWORD "-flags", QString::number(NetworkManager::Setting::AgentOwned));
    } else if (ui.password->passwordOption() == PasswordField::AlwaysAsk) {
        data.insert(NM_FORTISSLVPN_KEY_PASSWORD "-flags", QString::number(NetworkManager::Setting::NotSaved));
    } else {
        data.insert(NM_FORTISSLVPN_KEY_PASSWORD "-flags", QString::number(NetworkManager::Setting::NotRequired));
    }

    if (!ui.caCert->url().isEmpty()) {
        data.insert(NM_FORTISSLVPN_KEY_CA, ui.caCert->url().toLocalFile());
    }

    if (!ui.userCert->url().isEmpty()) {
        data.insert(NM_FORTISSLVPN_KEY_CERT, ui.userCert->url().toLocalFile());
    }

    if (!ui.userKey->url().isEmpty()) {
        data.insert(NM_FORTISSLVPN_KEY_KEY, ui.userKey->url().toLocalFile());
    }

    // From advanced
    if (!advUi.trustedCert->text().isEmpty()) {
        data.insert(NM_FORTISSLVPN_KEY_TRUSTED_CERT, advUi.trustedCert->text());
    }

    if (advUi.otp->isChecked()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"), QString::number(NetworkManager::Setting::NotSaved));
    } else {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"), QString::number(NetworkManager::Setting::None));
    }

    if (advUi.tfa->isChecked()) {
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_2FA "-flags"), QString::number(NetworkManager::Setting::AgentOwned));
        data.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"), QString::number(NetworkManager::Setting::None));
    }

    if (!advUi.realm->text().isEmpty()) {
        data.insert(NM_FORTISSLVPN_KEY_REALM, advUi.realm->text());
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

void FortisslvpnWidget::showAdvanced()
{
    advancedDlg->show();
}

bool FortisslvpnWidget::isValid() const
{
    return !ui.gateway->text().isEmpty();
}
