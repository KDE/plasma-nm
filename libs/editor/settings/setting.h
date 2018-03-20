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

#ifndef PLASMA_NM_SETTING_H
#define PLASMA_NM_SETTING_H

class QObject;

class SettingPrivate;

#include <NetworkManagerQt/Setting>

class Setting : public QObject
{
    Q_OBJECT
public:
    explicit Setting(const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(), QObject *parent = nullptr);
    virtual ~Setting();

    virtual void loadConfig(const NetworkManager::Setting::Ptr &setting) = 0;
    virtual void loadSecrets(const NetworkManager::Setting::Ptr &setting) = 0;

    virtual QVariantMap setting() const = 0;

    virtual NetworkManager::Setting::SettingType type() const;

    virtual bool isValid() const;

Q_SIGNALS:
    void changed();
    void validityChanged(bool valid);

protected:
    SettingPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(Setting)

};

#endif // PLASMA_NM_SETTING_H
