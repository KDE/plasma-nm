/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    : SettingWidget(setting, parent)
    , d_ptr(new OpenswanAuthDialogPrivate)
{
    Q_D(OpenswanAuthDialog);
    d->ui.setupUi(this);
    d->setting = setting;

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

QVariantMap OpenswanAuthDialog::setting() const
{
    Q_D(const OpenswanAuthDialog);

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
