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

#include "openvpnauth.h"
#include "passwordfield.h"

#include <QString>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>

#include <KAcceleratorManager>
#include <KLocalizedString>

#include "nm-openvpn-service.h"
#include "debug.h"

class OpenVpnAuthWidgetPrivate
{
public:
    NetworkManager::VpnSetting::Ptr setting;
    QFormLayout *layout;
};

OpenVpnAuthWidget::OpenVpnAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent)
    , d_ptr(new OpenVpnAuthWidgetPrivate)
{
    Q_D(OpenVpnAuthWidget);
    d->setting = setting;
    d->layout = new QFormLayout(this);
    setLayout(d->layout);

    readSecrets();

    KAcceleratorManager::manage(this);
}

OpenVpnAuthWidget::~OpenVpnAuthWidget()
{
    delete d_ptr;
}

void OpenVpnAuthWidget::readSecrets()
{
    Q_D(OpenVpnAuthWidget);
    const NMStringMap secrets = d->setting->secrets();
    const NMStringMap dataMap = d->setting->data();
    const QString cType = dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE];
    QLabel *label;
    PasswordField *lineEdit;

    NetworkManager::Setting::SecretFlags certType = (NetworkManager::Setting::SecretFlags)dataMap.value(NM_OPENVPN_KEY_CERTPASS"-flags").toInt();
    NetworkManager::Setting::SecretFlags passType = (NetworkManager::Setting::SecretFlags)dataMap.value(NM_OPENVPN_KEY_PASSWORD"-flags").toInt();
    NetworkManager::Setting::SecretFlags proxyType = (NetworkManager::Setting::SecretFlags)dataMap.value(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags").toInt();

    if (cType == QLatin1String(NM_OPENVPN_CONTYPE_TLS) && !(certType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("Key Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_OPENVPN_KEY_CERTPASS));
        lineEdit->setText(secrets.value(QLatin1String(NM_OPENVPN_KEY_CERTPASS)));
        d->layout->addRow(label, lineEdit);
    } else if (cType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD) && !(passType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_OPENVPN_KEY_PASSWORD));
        lineEdit->setText(secrets.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD)));
        d->layout->addRow(label, lineEdit);
    } else if (cType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS)) {
        if (!(passType.testFlag(NetworkManager::Setting::NotRequired))) {
            label = new QLabel(this);
            label->setText(i18n("Password:"));
            lineEdit = new PasswordField(this);
            lineEdit->setPasswordModeEnabled(true);
            lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_OPENVPN_KEY_PASSWORD));
            lineEdit->setText(secrets.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD)));
            d->layout->addRow(label, lineEdit);
        }
        if (!(certType.testFlag(NetworkManager::Setting::NotRequired))) {
            label = new QLabel(this);
            label->setText(i18n("Key Password:"));
            lineEdit = new PasswordField(this);
            lineEdit->setPasswordModeEnabled(true);
            lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_OPENVPN_KEY_CERTPASS));
            lineEdit->setText(secrets.value(QLatin1String(NM_OPENVPN_KEY_CERTPASS)));
            d->layout->addRow(label, lineEdit);
        }
    }

    if (dataMap.contains(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags") && !(proxyType.testFlag(NetworkManager::Setting::NotRequired))) {
        label = new QLabel(this);
        label->setText(i18n("Proxy Password:"));
        lineEdit = new PasswordField(this);
        lineEdit->setPasswordModeEnabled(true);
        lineEdit->setProperty("nm_secrets_key", QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD));
        lineEdit->setText(secrets.value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD)));
        d->layout->addRow(label, lineEdit);
    }

    for (int i = 0; i < d->layout->rowCount(); i++) {
        PasswordField *le = qobject_cast<PasswordField*>(d->layout->itemAt(i, QFormLayout::FieldRole)->widget());
        if (le && le->text().isEmpty()) {
            le->setFocus(Qt::OtherFocusReason);
            break;
        }
    }
}

QVariantMap OpenVpnAuthWidget::setting() const
{
    Q_D(const OpenVpnAuthWidget);

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
