/*
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_SSTP_H
#define PLASMA_NM_SSTP_H

#include "vpnuiplugin.h"

class Q_DECL_EXPORT SstpUiPlugin : public VpnUiPlugin
{
Q_OBJECT
public:
    explicit SstpUiPlugin(QObject *parent = 0, const QVariantList& = QVariantList());
    ~SstpUiPlugin() override;
    SettingWidget * widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = 0) override;
    SettingWidget * askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = 0) override;
    QString suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const override;
    QString supportedFileExtensions() const override;

    NMVariantMapMap importConnectionSettings(const QString &fileName) override;
    bool exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName) override;
};

#endif //  PLASMA_NM_SSTP_H
