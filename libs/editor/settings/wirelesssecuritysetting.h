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

#ifndef PLASMA_NM_WIRELESS_SECURITY_SETTING_H
#define PLASMA_NM_WIRELESS_SECURITY_SETTING_H

#include "setting.h"

class QObject;

class WirelessSecuritySettingPrivate;

#include <NetworkManagerQt/WirelessSecuritySetting>

class WirelessSecuritySetting : public Setting
{
    Q_OBJECT
public:
    explicit WirelessSecuritySetting(const NetworkManager::Setting::Ptr &setting = NetworkManager::WirelessSecuritySetting::Ptr(), QObject *parent = nullptr);
    virtual ~WirelessSecuritySetting();

    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

Q_SIGNALS:
    void settingChanged();
    void validityChanged(bool valid);

protected:
    WirelessSecuritySettingPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(WirelessSecuritySetting)

};

#endif // PLASMA_NM_WIRELESS_SECURITY_SETTING_H

