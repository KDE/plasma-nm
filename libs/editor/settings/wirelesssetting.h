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

#ifndef PLASMA_NM_WIRELESS_SETTING_H
#define PLASMA_NM_WIRELESS_SETTING_H

#include "setting.h"

class QObject;

class WirelessSettingPrivate;

#include <NetworkManagerQt/WirelessSetting>

class WirelessSetting : public Setting
{
    Q_OBJECT
    Q_PROPERTY(QString ssid READ ssid WRITE setSsid)
    Q_PROPERTY(uint mode READ mode WRITE setMode)
    Q_PROPERTY(QString bssid READ bssid WRITE setBssid)
    Q_PROPERTY(uint band READ band WRITE setBand)
    Q_PROPERTY(uint channel READ channel WRITE setChannel)
    Q_PROPERTY(QString macAddress READ macAddress WRITE setMacAddress)
    Q_PROPERTY(QString assignedMacAddress READ assignedMacAddress WRITE setAssignedMacAddress)
    Q_PROPERTY(uint mtu READ mtu WRITE setMtu)
    Q_PROPERTY(bool hidden READ hidden WRITE setHidden)
public:
    explicit WirelessSetting(const NetworkManager::Setting::Ptr &setting = NetworkManager::WirelessSetting::Ptr(), QObject *parent = nullptr);
    virtual ~WirelessSetting();

//     virtual void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
//     virtual void loadSecrets(const NetworkManager::Setting::Ptr &setting) = 0;

    QVariantMap setting() const override;

    QString ssid() const;
    void setSsid(const QString &ssid);

    uint mode() const;
    void setMode(uint mode);

    QString bssid() const;
    void setBssid(const QString &bssid);

    uint band() const;
    void setBand(uint band);

    uint channel() const;
    void setChannel(uint channel);

    QString macAddress() const;
    void setMacAddress(const QString &macAddress);

    QString assignedMacAddress() const;
    void setAssignedMacAddress(const QString &assignedMacAddress);

    uint mtu() const;
    void setMtu(uint mtu);

    bool hidden() const;
    void setHidden(bool hidden);

    bool isValid() const override;

Q_SIGNALS:
    void settingChanged();
    void validityChanged(bool valid);

protected:
    WirelessSettingPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(WirelessSetting)

};

#endif // PLASMA_NM_WIRELESS_SETTING_H
