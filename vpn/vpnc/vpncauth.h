/*
    Copyright 2008 Will Stephenson <wstephenson@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_VPNC_AUTH_H
#define PLASMA_NM_VPNC_AUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

class VpncAuthDialogPrivate;

class VpncAuthDialog : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(VpncAuthDialog)
public:
    explicit VpncAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);
    ~VpncAuthDialog() override;

    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    VpncAuthDialogPrivate *const d_ptr;
};

#endif // PLASMA_NM_VPNC_AUTH_H
