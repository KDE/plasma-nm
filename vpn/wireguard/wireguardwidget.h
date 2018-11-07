/*
    Copyright 2018 Bruce Anderson <banderson19com@san.rr.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WIREGUARDWIDGET_H
#define WIREGUARDWIDGET_H

#include "settingwidget.h"
#include "ui_wireguard.h"

#include <NetworkManagerQt/VpnSetting>

class WireGuardSettingWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit WireGuardSettingWidget(const NetworkManager::VpnSetting::Ptr &setting,
                                    QWidget *parent = 0);
    ~WireGuardSettingWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    virtual bool isValid() const override;

private Q_SLOTS:
    void showAdvanced();

private:
    class Private;
    Private *d;
    void setProperty(NMStringMap &data, const QLatin1String &key, const QString &value) const;
    void setBackground(QWidget *w, bool result) const;
    void checkAddressValid();
    void checkPrivateKeyValid();
    void checkPublicKeyValid();
    void checkDnsValid();
    void checkAllowedIpsValid();
    void checkEndpointValid();
};

#endif // WIREGUARDWIDGET_H
