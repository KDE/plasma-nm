/*
Copyright 2010 Andrey Borzenkov <arvidjaar@gmail.com>
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

#include <QString>

#include "vpncauth.h"
#include "ui_vpncauth.h"
#include "nm-vpnc-service.h"

class VpncAuthDialogPrivate
{
public:
    Ui_VpncAuth ui;
    NetworkManager::Settings::VpnSetting * setting;
};

VpncAuthDialog::VpncAuthDialog(NetworkManager::Settings::VpnSetting *setting, QWidget * parent)
    : SettingWidget(setting, parent), d_ptr(new VpncAuthDialogPrivate)
{
    Q_D(VpncAuthDialog);
    d->ui.setupUi(this);
    d->setting = setting;
    connect(d->ui.cbShowPasswords, SIGNAL(toggled(bool)), this, SLOT(showPasswordsChanged(bool)));

    readSecrets();
}

VpncAuthDialog::~VpncAuthDialog()
{
    delete d_ptr;
}

void VpncAuthDialog::readSecrets()
{
    Q_D(VpncAuthDialog);
    QStringMap data = d->setting->data();
    QStringMap secrets = d->setting->secrets();

    //   username
    QString user = data.value(NM_VPNC_KEY_XAUTH_USER);
    if (!user.isEmpty()) {
        d->ui.leUserName->setText(user);
    }
    //   group name
    QString group = data.value(NM_VPNC_KEY_ID);
    if (!group.isEmpty()) {
        d->ui.leGroupName->setText(group);
    }

    bool haveUserPassword = true;
    if (!((NetworkManager::Settings::Setting::SecretFlags)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt()).testFlag(NetworkManager::Settings::Setting::NotRequired)) {
        d->ui.leUserPassword->setText(secrets.value(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD)));
    } else {
        d->ui.userNameLabel->setVisible(false);
        d->ui.leUserName->setVisible(false);
        d->ui.userPasswordLabel->setVisible(false);
        d->ui.leUserPassword->setVisible(false);
        haveUserPassword = false;
    }

    if (!((NetworkManager::Settings::Setting::SecretFlags)data.value(NM_VPNC_KEY_SECRET"-flags").toInt()).testFlag(NetworkManager::Settings::Setting::NotRequired)) {
        d->ui.leGroupPassword->setText(secrets.value(QLatin1String(NM_VPNC_KEY_SECRET)));
    } else {
        d->ui.groupNameLabel->setVisible(false);
        d->ui.leGroupName->setVisible(false);
        d->ui.groupPasswordLabel->setVisible(false);
        d->ui.leGroupPassword->setVisible(false);
    }

    if (haveUserPassword && d->ui.leUserPassword->text().isEmpty())
        d->ui.leUserPassword->setFocus(Qt::OtherFocusReason);
    else if (d->ui.leGroupPassword->text().isEmpty())
        d->ui.leGroupPassword->setFocus(Qt::OtherFocusReason);
}

QVariantMap VpncAuthDialog::setting() const
{
    Q_D(const VpncAuthDialog);

    QVariantMap result;

    //   user password
    if (!d->ui.leUserPassword->text().isEmpty()) {
        result.insert(NM_VPNC_KEY_XAUTH_PASSWORD, d->ui.leUserPassword->text());
    }
    //   group password
    if (!d->ui.leGroupPassword->text().isEmpty()) {
        result.insert(NM_VPNC_KEY_SECRET, d->ui.leGroupPassword->text());
    }

    return result;
}

void VpncAuthDialog::showPasswordsChanged(bool show)
{
    Q_D(VpncAuthDialog);
    d->ui.leUserPassword->setPasswordMode(!show);
    d->ui.leGroupPassword->setPasswordMode(!show);
}
