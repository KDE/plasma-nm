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

#ifndef WIRED_SECURITY
#define WIRED_SECURITY

#include <QtGui/QWidget>

#include <QtNetworkManager/settings/802-1x.h>

#include "settingwidget.h"
#include "security802-1x.h"

namespace Ui
{
class WiredSecurity;
}

class WiredSecurity : public SettingWidget
{
    Q_OBJECT
public:
    WiredSecurity(NetworkManager::Settings::Security8021xSetting * setting8021x = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~WiredSecurity();
    void loadConfig(NetworkManager::Settings::Setting * setting);
    QVariantMap setting() const;

private:
    Ui::WiredSecurity * m_ui;
    Security8021x * m_8021xWidget;
    NetworkManager::Settings::Security8021xSetting * m_8021xSetting;
};

#endif // WIRED_SECURITY
