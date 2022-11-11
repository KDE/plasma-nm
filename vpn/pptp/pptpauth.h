/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PPTPAUTH_H
#define PPTPAUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"
#include "ui_pptpauth.h"

class PptpAuthWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit PptpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);

    QVariantMap setting() const override;

private:
    NetworkManager::VpnSetting::Ptr m_setting;
    Ui_PptpAuthenticationWidget ui;
};

#endif // PPTPAUTH_H
