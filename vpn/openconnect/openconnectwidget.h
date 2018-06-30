/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef OPENCONNECTWIDGET_H
#define OPENCONNECTWIDGET_H

#include "settingwidget.h"

#include <NetworkManagerQt/VpnSetting>

class OpenconnectSettingWidgetPrivate;

class OpenconnectSettingWidget : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OpenconnectSettingWidget)
public:
    explicit OpenconnectSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = 0);
    ~OpenconnectSettingWidget() override;
    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    QVariantMap setting() const override;
    bool isValid() const override;

private:
    OpenconnectSettingWidgetPrivate *const d_ptr;
};

#endif // OPENCONNECTWIDGET_H
