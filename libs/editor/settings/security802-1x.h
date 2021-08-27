/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_SECURITY8021X_H
#define PLASMA_NM_SECURITY8021X_H

#include <QRegExpValidator>
#include <QWidget>

#include <NetworkManagerQt/Security8021xSetting>

#include "settingwidget.h"

namespace Ui
{
class Security8021x;
}

class Q_DECL_EXPORT Security8021x : public SettingWidget
{
    Q_OBJECT
public:
    explicit Security8021x(const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(),
                           bool wifiMode = true,
                           QWidget *parent = nullptr,
                           Qt::WindowFlags f = {});
    ~Security8021x() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

private Q_SLOTS:
    void altSubjectMatchesButtonClicked();
    void connectToServersButtonClicked();
    void currentAuthChanged(int index);

private:
    NetworkManager::Security8021xSetting::Ptr m_setting;
    Ui::Security8021x *m_ui;
    QRegExpValidator *altSubjectValidator;
    QRegExpValidator *serversValidator;
};

#endif // SECURITY8021X_H
