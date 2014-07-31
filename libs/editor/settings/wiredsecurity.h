/*
    Copyright (c) 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_WIRED_SECURITY_H
#define PLASMA_NM_WIRED_SECURITY_H

#include <QtWidgets/QWidget>

#include <NetworkManagerQt/Security8021xSetting>

#include "settingwidget.h"
#include "security802-1x.h"

namespace Ui
{
class WiredSecurity;
}

class Q_DECL_EXPORT WiredSecurity : public SettingWidget
{
    Q_OBJECT
public:
    explicit WiredSecurity(const NetworkManager::Security8021xSetting::Ptr &setting8021x = NetworkManager::Security8021xSetting::Ptr(), QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~WiredSecurity();
    void loadConfig(const NetworkManager::Setting::Ptr &setting);
    QVariantMap setting(bool agentOwned = false) const;

    bool enabled8021x() const;

private:
    Ui::WiredSecurity * m_ui;
    Security8021x * m_8021xWidget;
    NetworkManager::Security8021xSetting::Ptr m_8021xSetting;
};

#endif // PLASMA_NM_WIRED_SECURITY_H
