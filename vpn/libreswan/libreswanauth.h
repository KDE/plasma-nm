/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_LIBRESWAN_AUTH_H
#define PLASMA_NM_LIBRESWAN_AUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"
#include "ui_libreswanauth.h"

class LibreswanAuthDialog : public SettingWidget
{
    Q_OBJECT
public:
    explicit LibreswanAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);
    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    Ui_LibreswanAuth ui;
    NetworkManager::VpnSetting::Ptr m_setting;
};

#endif // PLASMA_NM_LIBRESWAN_AUTH_H
