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
#include "wirelesssecuritysetting.h"

#include <NetworkManagerQt/Utils>

class WirelessSecuritySettingPrivate : public SettingPrivate
{
public:
    explicit WirelessSecuritySettingPrivate(const NetworkManager::Setting::Ptr &setting, NetworkManager::Setting::SettingType type)
        : SettingPrivate(setting, type)
        {
            wirelessSecuritySetting = setting.dynamicCast<NetworkManager::WirelessSecuritySetting>();
        }
    NetworkManager::WirelessSecuritySetting::Ptr wirelessSecuritySetting;
};

WirelessSecuritySetting::WirelessSecuritySetting(const NetworkManager::Setting::Ptr &setting, QObject *parent)
    : Setting(setting, parent)
    , d_ptr(new WirelessSecuritySettingPrivate(setting, setting->type()))
{
}

WirelessSecuritySetting::~WirelessSecuritySetting()
{
    delete d_ptr;
}

void WirelessSecuritySetting::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    Q_D(WirelessSecuritySetting);

    d->wirelessSecuritySetting->secretsFromMap(setting->toMap());
}

QVariantMap WirelessSecuritySetting::setting() const
{
    Q_D(const WirelessSecuritySetting);

    return d->wirelessSecuritySetting->toMap();
}

bool WirelessSecuritySetting::isValid() const
{
    Q_D(const WirelessSecuritySetting);

    return true;
}

