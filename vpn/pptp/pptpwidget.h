/*
    SPDX-FileCopyrightText: 2009 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PPTPWIDGET_H
#define PPTPWIDGET_H

#include "passwordfield.h"
#include "settingwidget.h"
#include "ui_pptpadvanced.h"
#include "ui_pptpprop.h"

#include <NetworkManagerQt/VpnSetting>

class PptpSettingWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit PptpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;
    bool isValid() const override;

private Q_SLOTS:
    void doAdvancedDialog();

private:
    void fillOnePasswordCombo(PasswordField *, NetworkManager::Setting::SecretFlags);
    void handleOnePasswordType(const PasswordField *, const QString &, NMStringMap &) const;

    Ui_PptpProp ui;
    Ui_PptpAdvanced advUi;
    NetworkManager::VpnSetting::Ptr m_setting;
    QDialog *advancedDlg;
    QWidget *advancedWid;
};

#endif // PPTPWIDGET_H
