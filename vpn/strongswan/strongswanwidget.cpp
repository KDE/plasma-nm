/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2010 Maurus Rohrer <maurus.rohrer@gmail.com>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "strongswanwidget.h"
#include "nm-strongswan-service.h"

#include <QString>
#include <QUrl>

StrongswanSettingWidget::StrongswanSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
{
    ui.setupUi(this);

    m_setting = setting;

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(ui.leGateway, &QLineEdit::textChanged, this, &StrongswanSettingWidget::slotWidgetChanged);
    connect(ui.proposal, &QGroupBox::toggled, this, &SettingWidget::settingChanged);

    KAcceleratorManager::manage(this);

    if (setting && !m_setting->isNull()) {
        loadConfig(setting);
    }
}

void StrongswanSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting)

    // General settings
    const NMStringMap dataMap = m_setting->data();
    // Gateway Address
    const QString gateway = dataMap[NM_STRONGSWAN_GATEWAY];
    if (!gateway.isEmpty()) {
        ui.leGateway->setText(gateway);
    }
    // Certificate
    ui.leGatewayCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_CERTIFICATE]));

    // Authentication
    const QString method = dataMap[NM_STRONGSWAN_METHOD];
    if (method == QLatin1String(NM_STRONGSWAN_AUTH_KEY)) {
        ui.cmbMethod->setCurrentIndex(StrongswanSettingWidget::PrivateKey);
        ui.leAuthPrivatekeyCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERCERT]));
        ui.leAuthPrivatekeyKey->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERKEY]));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT)) {
        ui.cmbMethod->setCurrentIndex(StrongswanSettingWidget::SshAgent);
        ui.leAuthSshCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_STRONGSWAN_USERCERT]));
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD)) {
        ui.cmbMethod->setCurrentIndex(StrongswanSettingWidget::Smartcard);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_EAP)) {
        ui.cmbMethod->setCurrentIndex(StrongswanSettingWidget::Eap);
        ui.leUserName->setText(dataMap[NM_STRONGSWAN_USER]);
    }

    // Settings
    ui.innerIP->setChecked(dataMap[NM_STRONGSWAN_INNERIP] == "yes");
    ui.udpEncap->setChecked(dataMap[NM_STRONGSWAN_ENCAP] == "yes");
    ui.ipComp->setChecked(dataMap[NM_STRONGSWAN_IPCOMP] == "yes");
    ui.proposal->setChecked(dataMap[NM_STRONGSWAN_PROPOSAL] == "yes");
    ui.ike->setText(dataMap[NM_STRONGSWAN_IKE]);
    ui.esp->setText(dataMap[NM_STRONGSWAN_ESP]);
}

void StrongswanSettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting);
}

QVariantMap StrongswanSettingWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_STRONGSWAN));

    NMStringMap data;
    NMStringMap secretData;

    // General settings
    // Gateway
    if (!ui.leGateway->text().isEmpty()) {
        data.insert(NM_STRONGSWAN_GATEWAY, ui.leGateway->text());
    }

    const QString certificate = ui.leGatewayCertificate->url().toLocalFile();
    if (!certificate.isEmpty()) {
        data.insert(NM_STRONGSWAN_CERTIFICATE, certificate);
    }

    // Authentication
    switch (ui.cmbMethod->currentIndex()) {
    case StrongswanSettingWidget::PrivateKey: {
        data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_KEY);
        const QString userPrivateCertificate = ui.leAuthPrivatekeyCertificate->url().toLocalFile();
        if (!userPrivateCertificate.isEmpty()) {
            data.insert(NM_STRONGSWAN_USERCERT, userPrivateCertificate);
        }
        const QString userKey = ui.leAuthPrivatekeyKey->url().toLocalFile();
        if (!userKey.isEmpty()) {
            data.insert(NM_STRONGSWAN_USERKEY, userKey);
        }
        break;
    }
    case StrongswanSettingWidget::SshAgent: {
        data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_AGENT);
        const QString userSshCertificate = ui.leAuthSshCertificate->url().toLocalFile();
        if (!userSshCertificate.isEmpty()) {
            data.insert(NM_STRONGSWAN_USERCERT, userSshCertificate);
        }
        break;
    }
    case StrongswanSettingWidget::Smartcard:
        data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_SMARTCARD);
        break;
    case StrongswanSettingWidget::Eap:
        data.insert(NM_STRONGSWAN_METHOD, NM_STRONGSWAN_AUTH_EAP);
        if (!ui.leUserName->text().isEmpty()) {
            data.insert(NM_STRONGSWAN_USER, ui.leUserName->text());
        }
        // StrongSwan-nm 1.2 does not appear to be able to save secrets, the must be entered through the auth dialog
    }

    // Options
    data.insert(NM_STRONGSWAN_INNERIP, ui.innerIP->isChecked() ? "yes" : "no");
    data.insert(NM_STRONGSWAN_ENCAP, ui.udpEncap->isChecked() ? "yes" : "no");
    data.insert(NM_STRONGSWAN_IPCOMP, ui.ipComp->isChecked() ? "yes" : "no");
    if (ui.proposal->isChecked()) {
        data.insert(NM_STRONGSWAN_PROPOSAL, "yes");
        data.insert(NM_STRONGSWAN_IKE, ui.ike->text());
        data.insert(NM_STRONGSWAN_ESP, ui.esp->text());
    } else
        data.insert(NM_STRONGSWAN_PROPOSAL, "no");

    // save it all
    setting.setData(data);
    setting.setSecrets(secretData);

    return setting.toMap();
}

bool StrongswanSettingWidget::isValid() const
{
    return !ui.leGateway->text().isEmpty();
}
