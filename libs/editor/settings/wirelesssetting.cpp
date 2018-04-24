/*
    Copyright 2018 Jan Grulich <jgrulich@redhat.com>

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
#include "wirelesssetting.h"

#include <NetworkManagerQt/Utils>

class WirelessSettingPrivate : public SettingPrivate
{
public:
    explicit WirelessSettingPrivate(const NetworkManager::Setting::Ptr &setting, NetworkManager::Setting::SettingType type)
        : SettingPrivate(setting, type)
        {
            wirelessSetting = setting.dynamicCast<NetworkManager::WirelessSetting>();
        }
    NetworkManager::WirelessSetting::Ptr wirelessSetting;
};

WirelessSetting::WirelessSetting(const NetworkManager::Setting::Ptr &setting, QObject *parent)
    : Setting(setting, parent)
    , d_ptr(new WirelessSettingPrivate(setting, setting->type()))
{
}

WirelessSetting::~WirelessSetting()
{
    delete d_ptr;
}

// void WirelessSetting::loadConfig(const NetworkManager::Setting::Ptr &setting)
// {
// }
//
// QVariantMap WirelessSetting::setting() const
// {
//     return QVariantMap();
// }

QString WirelessSetting::ssid() const
{
    Q_D(const WirelessSetting);

    return QString::fromUtf8(d->wirelessSetting->ssid());
}

void WirelessSetting::setSsid(const QString &ssid)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setSsid(ssid.toUtf8());
}

uint WirelessSetting::mode() const
{
    Q_D(const WirelessSetting);

    return d->wirelessSetting->mode();
}

void WirelessSetting::setMode(uint mode)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setMode((NetworkManager::WirelessSetting::NetworkMode) mode);
}

QString WirelessSetting::bssid() const
{
    Q_D(const WirelessSetting);

    return NetworkManager::macAddressAsString(d->wirelessSetting->bssid());
}

void WirelessSetting::setBssid(const QString &bssid)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setBssid(bssid.toUtf8());
}

uint WirelessSetting::band() const
{
    Q_D(const WirelessSetting);

    return d->wirelessSetting->band();
}

void WirelessSetting::setBand(uint band)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setBand((NetworkManager::WirelessSetting::FrequencyBand) band);
}

uint WirelessSetting::channel() const
{
    Q_D(const WirelessSetting);

    return d->wirelessSetting->channel();
}

void WirelessSetting::setChannel(uint channel)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setChannel(channel);
}

QString WirelessSetting::macAddress() const
{
    Q_D(const WirelessSetting);

    return NetworkManager::macAddressAsString(d->wirelessSetting->macAddress());
}

void WirelessSetting::setMacAddress(const QString &macAddress)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setMacAddress(NetworkManager::macAddressFromString(macAddress));
}

QString WirelessSetting::assignedMacAddress() const
{
    Q_D(const WirelessSetting);

    return d->wirelessSetting->assignedMacAddress();
}

void WirelessSetting::setAssignedMacAddress(const QString &assignedMacAddress)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setAssignedMacAddress(assignedMacAddress);
}

uint WirelessSetting::mtu() const
{
    Q_D(const WirelessSetting);

    return d->wirelessSetting->mtu();
}

void WirelessSetting::setMtu(uint mtu)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setMtu(mtu);
}

bool WirelessSetting::hidden() const
{
    Q_D(const WirelessSetting);

    return d->wirelessSetting->hidden();
}

void WirelessSetting::setHidden(bool hidden)
{
    Q_D(WirelessSetting);

    d->wirelessSetting->setHidden(hidden);
}

bool WirelessSetting::isValid() const
{
    return true;
}
