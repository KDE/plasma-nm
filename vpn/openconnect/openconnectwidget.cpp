/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.de>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "openconnectwidget.h"
#include <QDialog>
#include <QUrl>

#include "ui_openconnectprop.h"

#include <QString>
#include "nm-openconnect-service.h"

#include <KAcceleratorManager>

class OpenconnectSettingWidgetPrivate
{
public:
    Ui_OpenconnectProp ui;
    NetworkManager::VpnSetting::Ptr setting;
};

OpenconnectSettingWidget::OpenconnectSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent)
    , d_ptr(new OpenconnectSettingWidgetPrivate)
{
    Q_D(OpenconnectSettingWidget);
    d->ui.setupUi(this);
    d->setting = setting;

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(d->ui.leGateway, &QLineEdit::textChanged, this, &OpenconnectSettingWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (d->setting) {
        loadConfig(d->setting);
    }
}

OpenconnectSettingWidget::~OpenconnectSettingWidget()
{
    delete d_ptr;
}

void OpenconnectSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_D(OpenconnectSettingWidget);
    Q_UNUSED(setting)

    // General settings
    const NMStringMap dataMap = d->setting->data();

    d->ui.cmbProtocol->setCurrentIndex(dataMap[NM_OPENCONNECT_KEY_PROTOCOL] != QLatin1String("anyconnect"));
    d->ui.leGateway->setText(dataMap[NM_OPENCONNECT_KEY_GATEWAY]);
    d->ui.leCaCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_CACERT]));
    d->ui.leProxy->setText(dataMap[NM_OPENCONNECT_KEY_PROXY]);
    d->ui.chkAllowTrojan->setChecked(dataMap[NM_OPENCONNECT_KEY_CSD_ENABLE] == "yes");
    d->ui.leCsdWrapperScript->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_CSD_WRAPPER]));
    d->ui.leUserCert->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_USERCERT]));
    d->ui.leUserPrivateKey->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_PRIVKEY]));
    d->ui.chkUseFsid->setChecked(dataMap[NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID] == "yes");
}

QVariantMap OpenconnectSettingWidget::setting() const
{
    Q_D(const OpenconnectSettingWidget);

    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_OPENCONNECT));

    NMStringMap data;

    data.insert(NM_OPENCONNECT_KEY_PROTOCOL, d->ui.cmbProtocol->currentIndex() ? QLatin1String("nc") : QLatin1String("anyconnect"));
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY), d->ui.leGateway->text());
    if (d->ui.leCaCertificate->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_CACERT), d->ui.leCaCertificate->url().toLocalFile());
    }
    if (!d->ui.leProxy->text().isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_PROXY), d->ui.leProxy->text());
    }
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_CSD_ENABLE), d->ui.chkAllowTrojan->isChecked() ? "yes" : "no");
    if (d->ui.leCsdWrapperScript->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_CSD_WRAPPER), d->ui.leCsdWrapperScript->url().toLocalFile());
    }
    if (d->ui.leUserCert->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_USERCERT), d->ui.leUserCert->url().toLocalFile());
    }
    if (d->ui.leUserPrivateKey->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_PRIVKEY), d->ui.leUserPrivateKey->url().toLocalFile());
    }
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID), d->ui.chkUseFsid->isChecked() ? "yes" : "no");

    // Restore previous flags, this is necessary for keeping secrets stored in KWallet
    for (const QString &key : d->setting->data().keys()) {
        if (key.contains(QLatin1String("-flags"))) {
            data.insert(key, d->setting->data().value(key));
        }
    }

    /* These are different for every login session, and should not be stored */
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_COOKIE"-flags"), QString::number(NetworkManager::Setting::NotSaved));
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GWCERT"-flags"), QString::number(NetworkManager::Setting::NotSaved));
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY"-flags"), QString::number(NetworkManager::Setting::NotSaved));

    setting.setData(data);
    setting.setSecrets(d->setting->secrets());

    return setting.toMap();
}

bool OpenconnectSettingWidget::isValid() const
{
    Q_D(const OpenconnectSettingWidget);
    return !d->ui.leGateway->text().isEmpty();
}
