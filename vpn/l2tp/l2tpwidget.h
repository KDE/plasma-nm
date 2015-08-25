/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_L2TP_WIDGET_H
#define PLASMA_NM_L2TP_WIDGET_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

namespace Ui
{
class L2tpWidget;
}

class L2tpWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit L2tpWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~L2tpWidget();

    void loadConfig(const NetworkManager::Setting::Ptr &setting) Q_DECL_OVERRIDE;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) Q_DECL_OVERRIDE;

    QVariantMap setting(bool agentOwned = false) const;

    virtual bool isValid() const;

private Q_SLOTS:
    void userPasswordTypeChanged(int index);
    void showAdvanced();
    void showPpp();

private:
    Ui::L2tpWidget * m_ui;
    NetworkManager::VpnSetting::Ptr m_setting;
    NetworkManager::VpnSetting::Ptr m_tmpAdvancedSetting;
    NetworkManager::VpnSetting::Ptr m_tmpPppSetting;
};

#endif // PLASMA_NM_L2TP_WIDGET_H
