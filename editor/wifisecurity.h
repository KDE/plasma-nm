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

#ifndef WIFI_SECURITY
#define WIFI_SECURITY

#include <QtGui/QWidget>

#include <QtNetworkManager/settings/802-11-wireless-security.h>
#include <QtNetworkManager/settings/802-1x.h>

#include "settingwidget.h"
#include "security802-1x.h"

namespace Ui
{
class WifiSecurity;
}

class WifiSecurity : public SettingWidget
{
    Q_OBJECT
public:
    WifiSecurity(NetworkManager::Settings::Setting* setting = 0, NetworkManager::Settings::Security8021xSetting * setting8021x = 0,
                 QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~WifiSecurity();
    void loadConfig(NetworkManager::Settings::Setting * setting);
    QVariantMap setting() const;
    QVariantMap setting8021x() const;
private slots:
    void slotShowWepKeyPasswordChecked(bool checked);
    void slotShowLeapPasswordChecked(bool checked);
    void slotShowPskPasswordChecked(bool checked);
    void setWepKey(int keyIndex);

private:
    Ui::WifiSecurity * m_ui;
    Security8021x * m_8021xWidget;
    Security8021x * m_WPA2Widget;
    NetworkManager::Settings::WirelessSecuritySetting * m_wifiSecurity;
};

#endif // WIFI_SECURITY
