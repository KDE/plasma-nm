/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef SSH_WIDGET_H
#define SSH_WIDGET_H

#include "passwordfield.h"
#include "settingwidget.h"
#include "ui_sshadvanced.h"
#include "ui_sshwidget.h"

#include <NetworkManagerQt/VpnSetting>

class SshSettingWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit SshSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;
    bool isValid() const override;

private Q_SLOTS:
    void authTypeChanged(int index);
    void doAdvancedDialog();
    void passwordTypeChanged(int index);

private:
    void fillOnePasswordCombo(PasswordField *, NetworkManager::Setting::SecretFlags);
    void handleOnePasswordType(const PasswordField *, const QString &, NMStringMap &) const;

    Ui_SshWidget ui;
    Ui_SshAdvanced advUi;
    NetworkManager::VpnSetting::Ptr m_setting;
    QDialog *advancedDlg = nullptr;
    QWidget *advancedWid = nullptr;
};

#endif // SSH_WIDGET_H
