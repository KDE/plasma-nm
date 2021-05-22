/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_OPENSWAN_WIDGET_H
#define PLASMA_NM_OPENSWAN_WIDGET_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

namespace Ui
{
class OpenswanWidget;
}

class OpenswanWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit OpenswanWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~OpenswanWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

private:
    Ui::OpenswanWidget *m_ui;
    NetworkManager::VpnSetting::Ptr m_setting;
};

#endif // PLASMA_NM_OPENSWAN_WIDGET_H
