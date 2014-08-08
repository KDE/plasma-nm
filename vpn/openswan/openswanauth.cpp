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

#include "openswanauth.h"
#include "ui_openswanauth.h"
#include "nm-openswan-service.h"

#include <QString>

class OpenswanAuthDialogPrivate
{
public:
    Ui_OpenswanAuth ui;
    NetworkManager::VpnSetting::Ptr setting;
};

OpenswanAuthDialog::OpenswanAuthDialog(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent), d_ptr(new OpenswanAuthDialogPrivate)
{
    Q_D(OpenswanAuthDialog);
    d->ui.setupUi(this);
    d->setting = setting;
    connect(d->ui.cbShowPasswords, SIGNAL(toggled(bool)), this, SLOT(showPasswordsChanged(bool)));

    readSecrets();

    KAcceleratorManager::manage(this);
}

OpenswanAuthDialog::~OpenswanAuthDialog()
{
    delete d_ptr;
}

void OpenswanAuthDialog::readSecrets()
{
    Q_D(OpenswanAuthDialog);
    const NMStringMap data = d->setting->data();
    const NMStringMap secrets = d->setting->secrets();

    const QString groupName = data.value(NM_OPENSWAN_LEFTID);
    if (!groupName.isEmpty()) {
        d->ui.leGroupName->setText(groupName);
    }

    bool haveUserPassword = true;
    if (data.value(NM_OPENSWAN_XAUTH_PASSWORD_INPUT_MODES) != NM_OPENSWAN_PW_TYPE_UNUSED) {
        d->ui.leUserPassword->setText(secrets.value(NM_OPENSWAN_XAUTH_PASSWORD));
    } else {
        d->ui.leUserPassword->setVisible(false);
        d->ui.userPasswordLabel->setVisible(false);
        haveUserPassword = false;
    }

    bool haveGroupPassword = true;
    if (data.value(NM_OPENSWAN_PSK_INPUT_MODES) != NM_OPENSWAN_PW_TYPE_UNUSED) {
        d->ui.leGroupPassword->setText(secrets.value(NM_OPENSWAN_PSK_VALUE));
    } else {
        d->ui.leGroupPassword->setVisible(false);
        d->ui.groupPasswordLabel->setVisible(false);
        haveGroupPassword = false;
    }

    if (haveUserPassword && d->ui.leUserPassword->text().isEmpty()) {
        d->ui.leUserPassword->setFocus(Qt::OtherFocusReason);
    } else if (haveGroupPassword && d->ui.leGroupPassword->text().isEmpty()) {
        d->ui.leGroupPassword->setFocus(Qt::OtherFocusReason);
    }
}

QVariantMap OpenswanAuthDialog::setting(bool agentOwned) const
{
    Q_D(const OpenswanAuthDialog);
    Q_UNUSED(agentOwned)

    NMStringMap secrets;
    QVariantMap result;

    if (!d->ui.leUserPassword->text().isEmpty()) {
        secrets.insert(NM_OPENSWAN_XAUTH_PASSWORD, d->ui.leUserPassword->text());
    }

    if (!d->ui.leGroupPassword->text().isEmpty()) {
        secrets.insert(NM_OPENSWAN_PSK_VALUE, d->ui.leGroupPassword->text());
    }

    result.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));

    return result;
}

void OpenswanAuthDialog::showPasswordsChanged(bool show)
{
    Q_D(OpenswanAuthDialog);
    if (show) {
        d->ui.leUserPassword->setEchoMode(QLineEdit::Normal);
        d->ui.leGroupPassword->setEchoMode(QLineEdit::Normal);
    } else {
        d->ui.leUserPassword->setEchoMode(QLineEdit::Password);
        d->ui.leGroupPassword->setEchoMode(QLineEdit::Password);
    }
}
