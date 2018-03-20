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

#ifndef PLASMA_NM_IPV6_SETTING_H
#define PLASMA_NM_IPV6_SETTING_H

#include "setting.h"

class QObject;

class Ipv6SettingPrivate;

#include <NetworkManagerQt/Ipv6Setting>

class Ipv6Setting : public Setting
{
    Q_OBJECT
public:
    explicit Ipv6Setting(const NetworkManager::Setting::Ptr &setting = NetworkManager::Ipv6Setting::Ptr(), QObject *parent = nullptr);
    virtual ~Ipv6Setting();

//     virtual void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
//     virtual void loadSecrets(const NetworkManager::Setting::Ptr &setting) = 0;

    QVariantMap setting() const override;

    bool isValid() const override;

Q_SIGNALS:
    void settingChanged();
    void validityChanged(bool valid);

protected:
    Ipv6SettingPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Ipv6Setting)

};

#endif // PLASMA_NM_IPV6_SETTING_H

