/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2020 Douglas Kosovic <doug@uq.edu.au>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "l2tpauth.h"
#include "passwordfield.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLabel>
#include <QString>

#include <KLocalizedString>

#include "nm-l2tp-service.h"

L2tpAuthWidget::L2tpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
    : SettingWidget(setting, hints, parent)
{
    m_setting = setting;
    layout = new QFormLayout(this);
    setLayout(layout);

    readSecrets();

    KAcceleratorManager::manage(this);
}

void L2tpAuthWidget::readSecrets()
{
    const NMStringMap secrets = m_setting->secrets();
    const NMStringMap dataMap = m_setting->data();
    const QString userAType = dataMap[NM_L2TP_KEY_USER_AUTH_TYPE];
    const QString machineAType = dataMap[NM_L2TP_KEY_MACHINE_AUTH_TYPE];
    QLabel *label;
    PasswordField *lineEdit;

    NetworkManager::Setting::SecretFlags passType = (NetworkManager::Setting::SecretFlags)dataMap[NM_L2TP_KEY_PASSWORD "-flags"].toInt();
    NetworkManager::Setting::SecretFlags userCertType = (NetworkManager::Setting::SecretFlags)dataMap[NM_L2TP_KEY_USER_CERTPASS "-flags"].toInt();
    NetworkManager::Setting::SecretFlags machineCertType = (NetworkManager::Setting::SecretFlags)dataMap[NM_L2TP_KEY_MACHINE_CERTPASS "-flags"].toInt();

    if ((userAType.isEmpty() || userAType == QLatin1String(NM_L2TP_AUTHTYPE_PASSWORD)) && !(passType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("User Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_L2TP_KEY_PASSWORD));
        lineEdit->setText(secrets.value(QLatin1String(NM_L2TP_KEY_PASSWORD)));
        layout->addRow(label, lineEdit);
    } else if (userAType == QLatin1String(NM_L2TP_AUTHTYPE_TLS) && !(userCertType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("User Certificate Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_L2TP_KEY_USER_CERTPASS));
        lineEdit->setText(secrets.value(QLatin1String(NM_L2TP_KEY_USER_CERTPASS)));
        layout->addRow(label, lineEdit);
    }

    if (machineAType == QLatin1String(NM_L2TP_AUTHTYPE_TLS)) {
        if (!(machineCertType.testFlag(NetworkManager::Setting::NotRequired))) {
            label = new QLabel(this);
            label->setText(i18n("Machine Certificate Password:"));
            lineEdit = new PasswordField(this);
            lineEdit->setPasswordModeEnabled(true);
            lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS));
            lineEdit->setText(secrets.value(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS)));
            layout->addRow(label, lineEdit);
        }
    }

    for (int i = 0; i < layout->rowCount(); i++) {
        auto le = qobject_cast<PasswordField *>(layout->itemAt(i, QFormLayout::FieldRole)->widget());
        if (le && le->text().isEmpty()) {
            le->setFocus(Qt::OtherFocusReason);
            break;
        }
    }
}

QVariantMap L2tpAuthWidget::setting() const
{
    NMStringMap secrets;
    QVariantMap secretData;
    for (int i = 0; i < layout->rowCount(); i++) {
        auto le = qobject_cast<PasswordField *>(layout->itemAt(i, QFormLayout::FieldRole)->widget());
        if (le && !le->text().isEmpty()) {
            const QString key = le->property("nm_secrets_key").toString();
            secrets.insert(key, le->text());
        }
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
