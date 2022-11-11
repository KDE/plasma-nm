/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_VPNC_AUTH_H
#define PLASMA_NM_VPNC_AUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"
#include "ui_vpncauth.h"

class VpncAuthDialog : public SettingWidget
{
    Q_OBJECT
public:
    explicit VpncAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent = nullptr);

    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    Ui_VpncAuth ui;
    NetworkManager::VpnSetting::Ptr m_setting;
};

#endif // PLASMA_NM_VPNC_AUTH_H
