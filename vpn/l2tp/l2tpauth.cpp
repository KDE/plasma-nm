/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>
    Copyright 2020 Douglas Kosovic <doug@uq.edu.au>

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

#include "l2tpauth.h"
#include "passwordfield.h"

#include <QString>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>

#include <KLocalizedString>
#include <KAcceleratorManager>

#include "nm-l2tp-service.h"

class L2tpAuthWidgetPrivate
{
public:
    NetworkManager::VpnSetting::Ptr setting;
    QFormLayout *layout;
};

L2tpAuthWidget::L2tpAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent)
    , d_ptr(new L2tpAuthWidgetPrivate)
{
    Q_D(L2tpAuthWidget);
    d->setting = setting;
    d->layout = new QFormLayout(this);
    setLayout(d->layout);

    readSecrets();

    KAcceleratorManager::manage(this);
}

L2tpAuthWidget::~L2tpAuthWidget()
{
    delete d_ptr;
}

void L2tpAuthWidget::readSecrets()
{
    Q_D(L2tpAuthWidget);
    const NMStringMap secrets = d->setting->secrets();
    const NMStringMap dataMap = d->setting->data();
    const QString userAType = dataMap[NM_L2TP_KEY_USER_AUTH_TYPE];
    const QString machineAType = dataMap[NM_L2TP_KEY_MACHINE_AUTH_TYPE];
    QLabel *label;
    PasswordField *lineEdit;

    NetworkManager::Setting::SecretFlags passType = (NetworkManager::Setting::SecretFlags)dataMap[NM_L2TP_KEY_PASSWORD"-flags"].toInt();
    NetworkManager::Setting::SecretFlags userCertType = (NetworkManager::Setting::SecretFlags)dataMap[NM_L2TP_KEY_USER_CERTPASS"-flags"].toInt();
    NetworkManager::Setting::SecretFlags machineCertType = (NetworkManager::Setting::SecretFlags)dataMap[NM_L2TP_KEY_MACHINE_CERTPASS"-flags"].toInt();

    if ((userAType.isEmpty() || userAType == QLatin1String(NM_L2TP_AUTHTYPE_PASSWORD)) && !(passType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("User Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_L2TP_KEY_PASSWORD));
        lineEdit->setText(secrets.value(QLatin1String(NM_L2TP_KEY_PASSWORD)));
        d->layout->addRow(label, lineEdit);
    } else if (userAType == QLatin1String(NM_L2TP_AUTHTYPE_TLS) && !(userCertType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("User Certificate Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_L2TP_KEY_USER_CERTPASS));
        lineEdit->setText(secrets.value(QLatin1String(NM_L2TP_KEY_USER_CERTPASS)));
        d->layout->addRow(label, lineEdit);
    }

    if (machineAType == QLatin1String(NM_L2TP_AUTHTYPE_TLS)) {
        if (!(machineCertType.testFlag(NetworkManager::Setting::NotRequired))) {
            label = new QLabel(this);
            label->setText(i18n("Machine Certificate Password:"));
            lineEdit = new PasswordField(this);
            lineEdit->setPasswordModeEnabled(true);
            lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS));
            lineEdit->setText(secrets.value(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS)));
            d->layout->addRow(label, lineEdit);
        }
    }

    for (int i = 0; i < d->layout->rowCount(); i++) {
        PasswordField *le = qobject_cast<PasswordField*>(d->layout->itemAt(i, QFormLayout::FieldRole)->widget());
        if (le && le->text().isEmpty()) {
            le->setFocus(Qt::OtherFocusReason);
            break;
        }
    }
}

QVariantMap L2tpAuthWidget::setting() const
{
    Q_D(const L2tpAuthWidget);

    NMStringMap secrets;
    QVariantMap secretData;
    for (int i = 0; i < d->layout->rowCount(); i++) {
        PasswordField *le = qobject_cast<PasswordField*>(d->layout->itemAt(i, QFormLayout::FieldRole)->widget());
        if (le && !le->text().isEmpty()) {
            const QString key = le->property("nm_secrets_key").toString();
            secrets.insert(key, le->text());
        }
    }

    secretData.insert("secrets", QVariant::fromValue<NMStringMap>(secrets));
    return secretData;
}
