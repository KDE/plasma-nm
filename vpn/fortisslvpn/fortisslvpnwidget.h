/*
    Copyright 2017 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PLASMA_NM_FORTISSLVPN_WIDGET_H
#define PLASMA_NM_FORTISSLVPN_WIDGET_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

class FortisslvpnWidgetPrivate;

class FortisslvpnWidget : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FortisslvpnWidget)
public:
    explicit FortisslvpnWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~FortisslvpnWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;
    QVariantMap setting() const override;
    bool isValid() const override;

private Q_SLOTS:
    void showAdvanced();

private:
    FortisslvpnWidgetPrivate *const d_ptr;
};

#endif // PLASMA_NM_FORTISSLVPN_WIDGET_H
