/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef OPENCONNECTWIDGET_H
#define OPENCONNECTWIDGET_H

#include "settingwidget.h"

#include "ui_openconnectprop.h"
#include "ui_openconnecttoken.h"

#include <NetworkManagerQt/VpnSetting>

class OpenconnectSettingWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit OpenconnectSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);
    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;
    QVariantMap setting() const override;
    bool isValid() const override;
    bool initTokenGroup();

private Q_SLOTS:
    void showTokens();
    void handleTokenSecret(int index);
    void saveTokens();
    void restoreTokens();

private:
    Ui_OpenconnectProp ui;
    Ui::OpenConnectToken tokenUi;
    NetworkManager::VpnSetting::Ptr m_setting;
    QDialog *tokenDlg;

    using Token = struct {
        int tokenIndex;
        QString tokenSecret;
    };

    Token token;
};

#endif // OPENCONNECTWIDGET_H
