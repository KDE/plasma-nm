/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_OPENSWAN_AUTH_H
#define PLASMA_NM_OPENSWAN_AUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

class OpenswanAuthDialogPrivate;

class OpenswanAuthDialog : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OpenswanAuthDialog)
public:
    explicit OpenswanAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);
    ~OpenswanAuthDialog() override;
    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    OpenswanAuthDialogPrivate *const d_ptr;
};

#endif // PLASMA_NM_OPENSWAN_AUTH_H
