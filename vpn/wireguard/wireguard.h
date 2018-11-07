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

#ifndef PLASMANM_WIREGUARD_H
#define PLASMANM_WIREGUARD_H

#include "vpnuiplugin.h"

class Q_DECL_EXPORT WireGuardUiPlugin : public VpnUiPlugin
{
Q_OBJECT
public:
    explicit WireGuardUiPlugin(QObject *parent = nullptr, const QVariantList& = QVariantList());
    ~WireGuardUiPlugin() override;
    SettingWidget *widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr) override;
    SettingWidget *askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr) override;

    QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const override;
    QString supportedFileExtensions() const override;
    NMVariantMapMap importConnectionSettings(const QString &fileName) override;
    bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName) override;
};

#endif //  PLASMANM_WIREGUARD_H
