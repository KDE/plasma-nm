/*
    SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_FORTISSLVPN_WIDGET_H
#define PLASMA_NM_FORTISSLVPN_WIDGET_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"
#include "ui_fortisslvpn.h"
#include "ui_fortisslvpnadvanced.h"

class FortisslvpnWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit FortisslvpnWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr, Qt::WindowFlags f = {});

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;
    QVariantMap setting() const override;
    bool isValid() const override;

private Q_SLOTS:
    void showAdvanced();

private:
    Ui::FortisslvpnWidget ui;
    Ui::FortisslvpnAdvancedWidget advUi;
    NetworkManager::VpnSetting::Ptr m_setting;
    QDialog *advancedDlg = nullptr;
    QWidget *advancedWid = nullptr;
};

#endif // PLASMA_NM_FORTISSLVPN_WIDGET_H
