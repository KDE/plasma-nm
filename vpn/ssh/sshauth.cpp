/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "sshauth.h"
#include "ui_sshauth.h"

#include "nm-ssh-service.h"

SshAuthWidget::SshAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    m_setting = setting;
    ui.setupUi(this);

    KAcceleratorManager::manage(this);
}

QVariantMap SshAuthWidget::setting() const
{
    NMStringMap secrets;
    QVariantMap secretData;

    if (!ui.le_password->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_SSH_KEY_PASSWORD), ui.le_password->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
