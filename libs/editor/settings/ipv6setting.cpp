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
#include "ipv6setting.h"

#include <NetworkManagerQt/Utils>

class Ipv6SettingPrivate : public SettingPrivate
{
public:
    explicit Ipv6SettingPrivate(const NetworkManager::Setting::Ptr &setting, NetworkManager::Setting::SettingType type)
        : SettingPrivate(setting, type)
        {
            wirelessSecuritySetting = setting.dynamicCast<NetworkManager::Ipv6Setting>();
        }
    NetworkManager::Ipv6Setting::Ptr wirelessSecuritySetting;
};

Ipv6Setting::Ipv6Setting(const NetworkManager::Setting::Ptr &setting, QObject *parent)
    : Setting(setting, parent)
    , d_ptr(new Ipv6SettingPrivate(setting, setting->type()))
{
}

Ipv6Setting::~Ipv6Setting()
{
    delete d_ptr;
}

// void Ipv6Setting::loadConfig(const NetworkManager::Setting::Ptr &setting)
// {
// }

QVariantMap Ipv6Setting::setting() const
{
    Q_D(const Ipv6Setting);

    return d->wirelessSecuritySetting->toMap();
}

bool Ipv6Setting::isValid() const
{
    Q_D(const Ipv6Setting);

    return true;
}


