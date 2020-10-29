/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

#include "fortisslvpnauth.h"
#include "ui_fortisslvpnauth.h"

#include "nm-fortisslvpn-service.h"

#include <QString>

class FortisslvpnAuthDialogPrivate
{
public:
    Ui_FortisslvpnAuth ui;
    NetworkManager::VpnSetting::Ptr setting;
};

FortisslvpnAuthDialog::FortisslvpnAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
    , d_ptr(new FortisslvpnAuthDialogPrivate)
{
    Q_D(FortisslvpnAuthDialog);

    d->ui.setupUi(this);
    d->setting = setting;

    const NMStringMap data = d->setting->data();

    const NetworkManager::Setting::SecretFlags otpFlag = static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_OTP"-flags").toInt());
    d->ui.otpFrame->setVisible(otpFlag == NetworkManager::Setting::NotSaved);

    const NetworkManager::Setting::SecretFlags passwordFlag = static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_PASSWORD"-flags").toInt());
    d->ui.passwordFrame->setVisible(passwordFlag == NetworkManager::Setting::NotSaved);

    const QString hint = data.value("hint");
    if (!hint.isEmpty()){
        const QString hintMsg = data.value("hint-msg");
        d->ui.otpLabel->setText(hint);
        d->ui.labelOtp->setText(hintMsg.section(":", -2));
        d->ui.otpFrame->setVisible(true);
    }

    KAcceleratorManager::manage(this);
}

FortisslvpnAuthDialog::~FortisslvpnAuthDialog()
{
    delete d_ptr;
}

QVariantMap FortisslvpnAuthDialog::setting() const
{
    Q_D(const FortisslvpnAuthDialog);

    const NMStringMap data = d->setting->data();
    NMStringMap secrets;
    QVariantMap secretData;

    if (!d->ui.password->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD), d->ui.password->text());
    }

    if (!data.value(NM_FORTISSLVPN_KEY_OTP"-flags").isEmpty()) {
        const NetworkManager::Setting::SecretFlags otpFlag = static_cast<NetworkManager::Setting::SecretFlags>(data.value(NM_FORTISSLVPN_KEY_OTP"-flags").toInt());
        if (otpFlag == NetworkManager::Setting::NotSaved && !d->ui.otp->text().isEmpty()) {
            secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP), d->ui.otp->text());
        }
    }

    if (!data.value(NM_FORTISSLVPN_KEY_2FA"-flags").isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_2FA), d->ui.otp->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
