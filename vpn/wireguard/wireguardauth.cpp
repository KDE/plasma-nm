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

#include "wireguardauth.h"
#include "ui_wireguardauth.h"

#include <QString>

class WireGuardAuthWidgetPrivate
{
public:
    Ui_WireGuardAuth ui;
    NetworkManager::VpnSetting::Ptr setting;
};

WireGuardAuthWidget::WireGuardAuthWidget(const NetworkManager::VpnSetting::Ptr &setting,
                                         QWidget *parent)
    : SettingWidget(setting, parent)
    , d_ptr(new WireGuardAuthWidgetPrivate)
{
    Q_D(WireGuardAuthWidget);
    d->ui.setupUi(this);
    d->setting = setting;

    KAcceleratorManager::manage(this);
}

WireGuardAuthWidget::~WireGuardAuthWidget()
{
    delete d_ptr;
}

QVariantMap WireGuardAuthWidget::setting() const
{
    Q_D(const WireGuardAuthWidget);

    QVariantMap result;
    return result;
}
