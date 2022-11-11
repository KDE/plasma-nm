/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_FORTISSLVPN_AUTH_H
#define PLASMA_NM_FORTISSLVPN_AUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"
#include "ui_fortisslvpnauth.h"

class FortisslvpnAuthDialog : public SettingWidget
{
    Q_OBJECT
public:
    explicit FortisslvpnAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);
    QVariantMap setting() const override;

private:
    Ui_FortisslvpnAuth ui;
    NetworkManager::VpnSetting::Ptr m_setting;
};

#endif // PLASMA_NM_FORTISSLVPN_AUTH_H
