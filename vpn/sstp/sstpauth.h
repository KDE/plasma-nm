/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef SSTP_AUTH_H
#define SSTP_AUTH_H

#include "settingwidget.h"
#include "ui_sstpauth.h"

#include <NetworkManagerQt/VpnSetting>

class SstpAuthWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit SstpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);

    QVariantMap setting() const override;

private:
    NetworkManager::VpnSetting::Ptr m_setting;
    Ui_SstpAuth ui;
};

#endif // SSTP_AUTH_H
