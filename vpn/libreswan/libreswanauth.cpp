/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "libreswanauth.h"
#include "nm-libreswan-service.h"

#include <QString>

LibreswanAuthDialog::LibreswanAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    ui.setupUi(this);
    m_setting = setting;

    readSecrets();

    KAcceleratorManager::manage(this);
}

void LibreswanAuthDialog::readSecrets()
{
    const NMStringMap data = m_setting->data();
    const NMStringMap secrets = m_setting->secrets();

    const QString groupName = data.value(NM_LIBRESWAN_LEFTID);
    if (!groupName.isEmpty()) {
        ui.leGroupName->setText(groupName);
    }

    bool haveUserPassword = true;
    if (data.value(NM_LIBRESWAN_XAUTH_PASSWORD_INPUT_MODES) != NM_LIBRESWAN_PW_TYPE_UNUSED) {
        ui.leUserPassword->setText(secrets.value(NM_LIBRESWAN_XAUTH_PASSWORD));
    } else {
        ui.leUserPassword->setVisible(false);
        ui.userPasswordLabel->setVisible(false);
        haveUserPassword = false;
    }

    bool haveGroupPassword = true;
    if (data.value(NM_LIBRESWAN_PSK_INPUT_MODES) != NM_LIBRESWAN_PW_TYPE_UNUSED) {
        ui.leGroupPassword->setText(secrets.value(NM_LIBRESWAN_PSK_VALUE));
    } else {
        ui.leGroupPassword->setVisible(false);
        ui.groupPasswordLabel->setVisible(false);
        haveGroupPassword = false;
    }

    if (haveUserPassword && ui.leUserPassword->text().isEmpty()) {
        ui.leUserPassword->setFocus(Qt::OtherFocusReason);
    } else if (haveGroupPassword && ui.leGroupPassword->text().isEmpty()) {
        ui.leGroupPassword->setFocus(Qt::OtherFocusReason);
    }
}

QVariantMap LibreswanAuthDialog::setting() const
{
    NMStringMap secrets;
    QVariantMap result;

    if (!ui.leUserPassword->text().isEmpty()) {
        secrets.insert(NM_LIBRESWAN_XAUTH_PASSWORD, ui.leUserPassword->text());
    }

    if (!ui.leGroupPassword->text().isEmpty()) {
        secrets.insert(NM_LIBRESWAN_PSK_VALUE, ui.leGroupPassword->text());
    }

    result.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));

    return result;
}
