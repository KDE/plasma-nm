/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "strongswanauth.h"
#include "nm-strongswan-service.h"

#include <KMessageBox>
#include <QDialog>
#include <QProcessEnvironment>
#include <QString>

#include <KLocalizedString>

class StrongswanAuthWidgetPrivate
{
public:
};

StrongswanAuthWidget::StrongswanAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    m_setting = setting;
    ui.setupUi(this);
    acceptOnShow = false;

    readSecrets();

    KAcceleratorManager::manage(this);
}

void StrongswanAuthWidget::readSecrets()
{
    const NMStringMap dataMap = m_setting->data();

    const QString method = dataMap[NM_STRONGSWAN_METHOD];
    if (method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT) || dataMap[NM_STRONGSWAN_SECRET_TYPE] == QLatin1String(NM_STRONGSWAN_PW_TYPE_UNUSED)) {
        if (isVisible()) {
            acceptDialog();
        } else {
            acceptOnShow = true;
        }
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_KEY)) {
        ui.passwordLabel->setText(i18nc("@label:textbox password label for private key password", "Private Key Password:"));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD)) {
        ui.passwordLabel->setText(i18nc("@label:textbox password label for smartcard pin", "PIN:"));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP)) {
        ui.passwordLabel->setText(i18nc("@label:textbox password label for EAP password", "Password:"));
    }
}

void StrongswanAuthWidget::setVisible(bool visible)
{
    SettingWidget::setVisible(visible);

    if (visible) {
        if (acceptOnShow) {
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
    auto dialog = qobject_cast<QDialog *>(parentWidget());
    if (dialog) {
        dialog->accept();
    }
}

QVariantMap StrongswanAuthWidget::setting() const
{
    NMStringMap secrets;
    QVariantMap secretData;

    if (m_setting->data()[NM_STRONGSWAN_METHOD] == QLatin1String(NM_STRONGSWAN_AUTH_AGENT)) {
        const QString agent = QProcessEnvironment::systemEnvironment().value(QLatin1String("SSH_AUTH_SOCK"));
        if (!agent.isEmpty()) {
            secrets.insert(NM_STRONGSWAN_AUTH_AGENT, agent);
        } else {
            KMessageBox::error(nullptr,
                               i18nc("@label:textbox error message while saving configuration",
                                     "Configuration uses ssh-agent for authentication, but no ssh-agent found running."));
        }
    } else {
        secrets.insert(NM_STRONGSWAN_SECRET, ui.password->text());
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
