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


#include "sstpauth.h"
#include "ui_sstpauth.h"

#include "nm-sstp-service.h"

#include <KAcceleratorManager>

class SstpAuthWidgetPrivate
{
public:
    NetworkManager::VpnSetting::Ptr setting;
    Ui_SstpAuth ui;
};

SstpAuthWidget::SstpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
    , d_ptr(new SstpAuthWidgetPrivate)
{
    Q_D(SstpAuthWidget);
    d->setting = setting;
    d->ui.setupUi(this);

    KAcceleratorManager::manage(this);
}

SstpAuthWidget::~SstpAuthWidget()
{
    delete d_ptr;
}

QVariantMap SstpAuthWidget::setting() const
{
    Q_D(const SstpAuthWidget);

    NMStringMap secrets;
    QVariantMap secretData;

    if (!d->ui.le_password->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_SSTP_KEY_PASSWORD), d->ui.le_password->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
