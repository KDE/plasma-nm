/*
    SPDX-FileCopyrightText: 2010 Andrey Borzenkov <arvidjaar@gmail.com>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "vpncauth.h"
#include "nm-vpnc-service.h"

#include <QString>

VpncAuthDialog::VpncAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    ui.setupUi(this);
    m_setting = setting;

    readSecrets();

    KAcceleratorManager::manage(this);
}

void VpncAuthDialog::readSecrets()
{
    const NMStringMap data = m_setting->data();
    const NMStringMap secrets = m_setting->secrets();

    //   username
    const QString user = data.value(NM_VPNC_KEY_XAUTH_USER);
    if (!user.isEmpty()) {
        ui.leUserName->setText(user);
    }
    //   group name
    const QString group = data.value(NM_VPNC_KEY_ID);
    if (!group.isEmpty()) {
        ui.leGroupName->setText(group);
    }

    bool haveUserPassword = true;
    if (!((NetworkManager::Setting::SecretFlags)data.value(NM_VPNC_KEY_XAUTH_PASSWORD "-flags").toInt()).testFlag(NetworkManager::Setting::NotRequired)) {
        ui.leUserPassword->setText(secrets.value(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD)));
    } else {
        ui.userNameLabel->setVisible(false);
        ui.leUserName->setVisible(false);
        ui.userPasswordLabel->setVisible(false);
        ui.leUserPassword->setVisible(false);
        haveUserPassword = false;
    }

    if (!((NetworkManager::Setting::SecretFlags)data.value(NM_VPNC_KEY_SECRET "-flags").toInt()).testFlag(NetworkManager::Setting::NotRequired)) {
        ui.leGroupPassword->setText(secrets.value(QLatin1String(NM_VPNC_KEY_SECRET)));
    } else {
        ui.groupNameLabel->setVisible(false);
        ui.leGroupName->setVisible(false);
        ui.groupPasswordLabel->setVisible(false);
        ui.leGroupPassword->setVisible(false);
    }

    if (haveUserPassword && ui.leUserPassword->text().isEmpty()) {
        ui.leUserPassword->setFocus(Qt::OtherFocusReason);
    } else if (ui.leGroupPassword->text().isEmpty()) {
        ui.leGroupPassword->setFocus(Qt::OtherFocusReason);
    }
}

QVariantMap VpncAuthDialog::setting() const
{
    NMStringMap secrets;
    QVariantMap result;

    //   user password
    if (!ui.leUserPassword->text().isEmpty()) {
        secrets.insert(NM_VPNC_KEY_XAUTH_PASSWORD, ui.leUserPassword->text());
    }
    //   group password
    if (!ui.leGroupPassword->text().isEmpty()) {
        secrets.insert(NM_VPNC_KEY_SECRET, ui.leGroupPassword->text());
    }

    result.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));

    return result;
}
