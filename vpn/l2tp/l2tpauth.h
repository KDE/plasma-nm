/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_L2TP_AUTH_H
#define PLASMA_NM_L2TP_AUTH_H

#include <NetworkManagerQt/VpnSetting>

#include "settingwidget.h"

class L2tpAuthWidgetPrivate;

class L2tpAuthWidget : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(L2tpAuthWidget)
public:
    explicit L2tpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);
    ~L2tpAuthWidget() override;
    virtual void readSecrets();
    QVariantMap setting() const override;

private:
    L2tpAuthWidgetPrivate *const d_ptr;
};

#endif // PLASMA_NM_L2TP_AUTH_H
