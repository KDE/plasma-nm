/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef SSH_AUTH_H
#define SSH_AUTH_H

#include "settingwidget.h"
#include "ui_sshauth.h"

#include <NetworkManagerQt/VpnSetting>

class SshAuthWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit SshAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);

    QVariantMap setting() const override;

private:
    NetworkManager::VpnSetting::Ptr m_setting;
    Ui_SshAuth ui;
};

#endif // SSH_AUTH_H
