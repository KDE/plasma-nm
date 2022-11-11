/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef SSTP_WIDGET_H
#define SSTP_WIDGET_H

#include "passwordfield.h"
#include "settingwidget.h"
#include "ui_sstpadvanced.h"
#include "ui_sstpwidget.h"

#include <NetworkManagerQt/VpnSetting>

class SstpSettingWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit SstpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;
    bool isValid() const override;

private Q_SLOTS:
    void doAdvancedDialog();

private:
    void fillOnePasswordCombo(PasswordField *, NetworkManager::Setting::SecretFlags);
    void handleOnePasswordType(const PasswordField *, const QString &, NMStringMap &) const;

    Ui_SstpWidget ui;
    Ui_SstpAdvanced advUi;
    NetworkManager::VpnSetting::Ptr m_setting;
    QDialog *advancedDlg = nullptr;
    QWidget *advancedWid = nullptr;
};

#endif // SSTP_WIDGET_H
