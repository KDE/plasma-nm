/*
    Copyright 2019 Jan Grulich <jgrulich@redhat.com>

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

#include "setting_p.h"
#include "wiredsetting.h"

#include <NetworkManagerQt/Utils>

class WiredSettingPrivate : public SettingPrivate
{
public:
    explicit WiredSettingPrivate(const NetworkManager::Setting::Ptr &setting, NetworkManager::Setting::SettingType type)
        : SettingPrivate(setting, type)
        {
            wirelessSecuritySetting = setting.dynamicCast<NetworkManager::WiredSetting>();
        }
    NetworkManager::WiredSetting::Ptr wirelessSecuritySetting;
};

WiredSetting::WiredSetting(const NetworkManager::Setting::Ptr &setting, QObject *parent)
    : Setting(setting, parent)
    , d_ptr(new WiredSettingPrivate(setting, setting->type()))
{
}

WiredSetting::~WiredSetting()
{
    delete d_ptr;
}

// void WiredSetting::loadConfig(const NetworkManager::Setting::Ptr &setting)
// {
// }

QVariantMap WiredSetting::setting() const
{
    Q_D(const WiredSetting);

    return d->wirelessSecuritySetting->toMap();
}

bool WiredSetting::isValid() const
{
    Q_D(const WiredSetting);

    return true;
}

