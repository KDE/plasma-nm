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

#include "pptpauth.h"
#include "ui_pptpauth.h"

#include "nm-pptp-service.h"

class PptpAuthWidgetPrivate
{
public:
    NetworkManager::VpnSetting::Ptr setting;
    Ui_PptpAuthenticationWidget ui;
};

PptpAuthWidget::PptpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
: SettingWidget(setting, parent), d_ptr(new PptpAuthWidgetPrivate)
{
    Q_D(PptpAuthWidget);
    d->setting = setting;
    d->ui.setupUi(this);
    connect(d->ui.chkShowPassword, SIGNAL(toggled(bool)), this, SLOT(showPasswordsToggled(bool)));
}

PptpAuthWidget::~PptpAuthWidget()
{
    delete d_ptr;
}

QVariantMap PptpAuthWidget::setting(bool agentOwned) const
{
    Q_D(const PptpAuthWidget);
    Q_UNUSED(agentOwned)

    NMStringMap secrets;
    QVariantMap secretData;

    if (!d->ui.lePassword->text().isEmpty()) {
        secrets.insert(QLatin1String(NM_PPTP_KEY_PASSWORD), d->ui.lePassword->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}

void PptpAuthWidget::showPasswordsToggled(bool toggled)
{
    Q_D(PptpAuthWidget);
    d->ui.lePassword->setPasswordMode(!toggled);
}
