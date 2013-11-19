/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
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

#ifndef STRONGSWANAUTH_H
#define STRONGSWANAUTH_H

#include "settingwidget.h"

#include <NetworkManagerQt/VpnSetting>

class StrongswanAuthWidgetPrivate;

class StrongswanAuthWidget : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(StrongswanAuthWidget)
public:
    explicit StrongswanAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    ~StrongswanAuthWidget();

    virtual void readSecrets();

    virtual QVariantMap setting(bool agentOwned = false) const;

public slots:
    void setVisible(bool);

private slots:
    void showPasswordsChanged(bool show);

private:
    StrongswanAuthWidgetPrivate * const d_ptr;
    void acceptDialog();
};

#endif // STRONGSWANAUTH_H
