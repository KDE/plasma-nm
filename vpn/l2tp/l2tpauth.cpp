/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "l2tpauth.h"
#include "ui_l2tpauth.h"
#include "nm-l2tp-service.h"

#include <QString>

class L2tpAuthDialogPrivate
{
public:
    Ui_L2tpAuth ui;
    NetworkManager::VpnSetting::Ptr setting;
};

L2tpAuthDialog::L2tpAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent)
    , d_ptr(new L2tpAuthDialogPrivate)
{
    Q_D(L2tpAuthDialog);
    d->ui.setupUi(this);
    d->setting = setting;

    KAcceleratorManager::manage(this);

    readSecrets();
}

L2tpAuthDialog::~L2tpAuthDialog()
{
    delete d_ptr;
}

void L2tpAuthDialog::readSecrets()
{
    Q_D(L2tpAuthDialog);
    const NMStringMap data = d->setting->data();
    const NMStringMap secrets = d->setting->secrets();

    QString user = data.value(NM_L2TP_KEY_USER);
    if (!user.isEmpty()) {
        d->ui.leUserName->setText(user);
    }
    bool haveUserPassword = true;
    if (!((NetworkManager::Setting::SecretFlags)data.value(NM_L2TP_KEY_PASSWORD"-flags").toInt()).testFlag(NetworkManager::Setting::NotRequired)) {
        d->ui.leUserPassword->setText(secrets.value(QLatin1String(NM_L2TP_KEY_PASSWORD)));
    } else {
        d->ui.userNameLabel->setVisible(false);
        d->ui.leUserName->setVisible(false);
        d->ui.userPasswordLabel->setVisible(false);
        d->ui.leUserPassword->setVisible(false);
        haveUserPassword = false;
    }

    if (haveUserPassword && d->ui.leUserPassword->text().isEmpty()) {
        d->ui.leUserPassword->setFocus(Qt::OtherFocusReason);
    }
}

QVariantMap L2tpAuthDialog::setting(bool agentOwned) const
{
    Q_D(const L2tpAuthDialog);
    Q_UNUSED(agentOwned)

    NMStringMap secrets;
    QVariantMap result;

    if (!d->ui.leUserPassword->text().isEmpty()) {
        secrets.insert(NM_L2TP_KEY_PASSWORD, d->ui.leUserPassword->text());
    }

    result.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));

    return result;
}
