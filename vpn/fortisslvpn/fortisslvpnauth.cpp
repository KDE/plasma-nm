/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fortisslvpnauth.h"

#include "nm-fortisslvpn-service.h"

#include <QString>

FortisslvpnAuthDialog::FortisslvpnAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    ui.setupUi(this);
    m_setting = setting;

    const NMStringMap data = m_setting->data();

    const NetworkManager::Setting::SecretFlags otpFlag = static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_OTP "-flags").toInt());
    ui.otpFrame->setVisible(otpFlag == NetworkManager::Setting::NotSaved);

    const NetworkManager::Setting::SecretFlags passwordFlag =
        static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_PASSWORD "-flags").toInt());
    ui.passwordFrame->setVisible(passwordFlag == NetworkManager::Setting::NotSaved);

    if (m_hints.count() == 2) {
        const QString hint = m_hints.at(0);
        const QString hintMsg = m_hints.at(1);
        ui.otpLabel->setText(hint);
        ui.labelOtp->setText(hintMsg.section(":", -2));
        ui.otpFrame->setVisible(true);
        ui.passwordFrame->setVisible(false);
    }

    KAcceleratorManager::manage(this);
}

QVariantMap FortisslvpnAuthDialog::setting() const
{
    const NMStringMap data = m_setting->data();
    NMStringMap secrets;
    QVariantMap secretData;

    if (!ui.password->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD), ui.password->text());
    }

    if (!data.value(NM_FORTISSLVPN_KEY_OTP "-flags").isEmpty()) {
        const NetworkManager::Setting::SecretFlags otpFlag =
            static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_OTP "-flags").toInt());
        if (otpFlag == NetworkManager::Setting::NotSaved && !ui.otp->text().isEmpty()) {
            secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP), ui.otp->text());
        }
    }

    if (!data.value(NM_FORTISSLVPN_KEY_2FA "-flags").isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_2FA), ui.otp->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
