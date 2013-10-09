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

#include <KDialog>
#include <KMessageBox>
#include <QString>
#include <QProcessEnvironment>

#include <KLocalizedString>

#include "strongswanauth.h"
#include "ui_strongswanauth.h"
#include "nm-strongswan-service.h"

class StrongswanAuthWidgetPrivate
{
public:
    Ui_StrongswanAuth ui;
    bool acceptOnShow;
    NetworkManager::VpnSetting::Ptr setting;
};

StrongswanAuthWidget::StrongswanAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent), d_ptr(new StrongswanAuthWidgetPrivate)
{
    Q_D(StrongswanAuthWidget);
    d->setting = setting;
    d->ui.setupUi(this);
    d->acceptOnShow = false;
    connect(d->ui.chkShowPass, SIGNAL(toggled(bool)), this, SLOT(showPasswordsChanged(bool)));

    readSecrets();

    KAcceleratorManager::manage(this);
}

StrongswanAuthWidget::~StrongswanAuthWidget()
{
    delete d_ptr;
}

void StrongswanAuthWidget::readSecrets()
{
    Q_D(StrongswanAuthWidget);
    const NMStringMap dataMap = d->setting->data();

    const QString method = dataMap[NM_STRONGSWAN_METHOD];
    if (method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT) || dataMap[NM_STRONGSWAN_SECRET_TYPE] == QLatin1String(NM_STRONGSWAN_PW_TYPE_UNUSED)) {
        if (isVisible())
            acceptDialog();
        else
            d->acceptOnShow = true;
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_KEY)) {
        d->ui.passwordLabel->setText(i18nc("@label:textbox password label for private key password", "Private Key Password:"));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD)) {
        d->ui.passwordLabel->setText(i18nc("@label:textbox password label for smartcard pin", "PIN:"));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP)) {
        d->ui.passwordLabel->setText(i18nc("@label:textbox password label for EAP password", "Password:"));
    }
}

void StrongswanAuthWidget::setVisible(bool visible)
{
    Q_D(StrongswanAuthWidget);

    SettingWidget::setVisible(visible);

    if (visible) {
        if (d->acceptOnShow) {
            acceptDialog();
        } else {
            SettingWidget::setVisible(visible);
        }
    } else {
        SettingWidget::setVisible(visible);
    }
}

void StrongswanAuthWidget::acceptDialog()
{
    KDialog *dialog = qobject_cast<KDialog*>(parentWidget());
    if (dialog) {
        dialog->accept();
    }
}

QVariantMap StrongswanAuthWidget::setting(bool agentOwned) const
{
    Q_UNUSED(agentOwned)
    Q_D(const StrongswanAuthWidget);

    NMStringMap secrets;
    QVariantMap secretData;

    if (d->setting->data()[NM_STRONGSWAN_METHOD] == QLatin1String(NM_STRONGSWAN_AUTH_AGENT)) {
        const QString agent = QProcessEnvironment::systemEnvironment().value(QLatin1String("SSH_AUTH_SOCK"));
        if (!agent.isEmpty()) {
            secrets.insert(NM_STRONGSWAN_AUTH_AGENT, agent);
        } else {
            KMessageBox::error(0, i18nc("@label:textbox error message while saving configuration", "Configuration uses ssh-agent for authentication, but no ssh-agent found running."));
        }
    } else {
        secrets.insert(NM_STRONGSWAN_SECRET, d->ui.password->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}

void StrongswanAuthWidget::showPasswordsChanged(bool show)
{
    Q_D(StrongswanAuthWidget);
    d->ui.password->setPasswordMode(!show);
}
