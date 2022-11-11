/*
    SPDX-FileCopyrightText: 2009 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2009 Pavel Andreev <apavelm@gmail.com>
    SPDX-FileCopyrightText: 2013 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pptpwidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QString>

#include "nm-pptp-service.h"

class PptpSettingWidgetPrivate
{
public:
};

PptpSettingWidget::PptpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
{
    ui.setupUi(this);

    m_setting = setting;

    ui.edt_password->setPasswordOptionsEnabled(true);
    ui.edt_password->setPasswordNotRequiredEnabled(true);

    connect(ui.btnAdvanced, &QPushButton::clicked, this, &PptpSettingWidget::doAdvancedDialog);

    advancedDlg = new QDialog(this);
    advancedWid = new QWidget(this);
    advUi.setupUi(advancedWid);
    auto layout = new QVBoxLayout(advancedDlg);
    layout->addWidget(advancedWid);
    advancedDlg->setLayout(layout);
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, advancedDlg);
    connect(buttons, &QDialogButtonBox::accepted, advancedDlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, advancedDlg, &QDialog::reject);
    layout->addWidget(buttons);

    // Connect for setting check
    watchChangedSetting();

    // Connect for validity check
    connect(ui.edt_gateway, &QLineEdit::textChanged, this, &PptpSettingWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (setting && !setting->isNull()) {
        loadConfig(setting);
    }
}

void PptpSettingWidget::doAdvancedDialog()
{
    advancedDlg->setModal(true);
    advancedDlg->show();
}

void PptpSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting)

    // General settings
    const NMStringMap dataMap = m_setting->data();

    // Authentication
    const QString sGateway = dataMap[NM_PPTP_KEY_GATEWAY];
    if (!sGateway.isEmpty()) {
        ui.edt_gateway->setText(sGateway);
    }

    const QString sLogin = dataMap[NM_PPTP_KEY_USER];
    if (!sLogin.isEmpty()) {
        ui.edt_login->setText(sLogin);
    }

    const QString sDomain = dataMap[NM_PPTP_KEY_DOMAIN];
    if (!sDomain.isEmpty()) {
        ui.edt_ntDomain->setText(sDomain);
    }

    // Options below is belongs to "Advanced" dialog

    // Authentication options
    const QString yesString = QLatin1String("yes");
    bool refuse_pap = (dataMap[NM_PPTP_KEY_REFUSE_PAP] == yesString);
    bool refuse_chap = (dataMap[NM_PPTP_KEY_REFUSE_CHAP] == yesString);
    bool refuse_mschap = (dataMap[NM_PPTP_KEY_REFUSE_MSCHAP] == yesString);
    bool refuse_mschapv2 = (dataMap[NM_PPTP_KEY_REFUSE_MSCHAPV2] == yesString);
    bool refuse_eap = (dataMap[NM_PPTP_KEY_REFUSE_EAP] == yesString);

    QListWidgetItem *item = nullptr;
    item = advUi.listWidget->item(0); // PAP
    item->setCheckState(refuse_pap ? Qt::Unchecked : Qt::Checked);
    item = advUi.listWidget->item(1); // CHAP
    item->setCheckState(refuse_chap ? Qt::Unchecked : Qt::Checked);
    item = advUi.listWidget->item(2); // MSCHAP
    item->setCheckState(refuse_mschap ? Qt::Unchecked : Qt::Checked);
    item = advUi.listWidget->item(3); // MSCHAPv2
    item->setCheckState(refuse_mschapv2 ? Qt::Unchecked : Qt::Checked);
    item = advUi.listWidget->item(4); // EAP
    item->setCheckState(refuse_eap ? Qt::Unchecked : Qt::Checked);

    // Cryptography and compression
    const bool mppe = (dataMap[NM_PPTP_KEY_REQUIRE_MPPE] == yesString);
    const bool mppe40 = (dataMap[NM_PPTP_KEY_REQUIRE_MPPE_40] == yesString);
    const bool mppe128 = (dataMap[NM_PPTP_KEY_REQUIRE_MPPE_128] == yesString);
    const bool mppe_stateful = (dataMap[NM_PPTP_KEY_MPPE_STATEFUL] == yesString);

    if (mppe || mppe40 || mppe128) { // If MPPE is use
        advUi.gb_MPPE->setChecked(mppe || mppe40 || mppe128);
        if (mppe128) {
            advUi.cb_MPPECrypto->setCurrentIndex(1); // 128 bit
        } else if (mppe40) {
            advUi.cb_MPPECrypto->setCurrentIndex(2); // 40 bit
        } else {
            advUi.cb_MPPECrypto->setCurrentIndex(0); // Any
        }
        advUi.cb_statefulEncryption->setChecked(mppe_stateful);
    }

    const bool nobsd = (dataMap[NM_PPTP_KEY_NOBSDCOMP] == yesString);
    advUi.cb_BSD->setChecked(!nobsd);

    const bool nodeflate = (dataMap[NM_PPTP_KEY_NODEFLATE] == yesString);
    advUi.cb_deflate->setChecked(!nodeflate);

    const bool novjcomp = (dataMap[NM_PPTP_KEY_NO_VJ_COMP] == yesString);
    advUi.cb_TCPheaders->setChecked(!novjcomp);

    // Echo
    const int lcp_echo_interval = QString(dataMap[NM_PPTP_KEY_LCP_ECHO_INTERVAL]).toInt();
    advUi.cb_sendEcho->setChecked(lcp_echo_interval > 0);

    // secrets
    const NetworkManager::Setting::SecretFlags type = (NetworkManager::Setting::SecretFlags)dataMap[NM_PPTP_KEY_PASSWORD "-flags"].toInt();
    fillOnePasswordCombo(ui.edt_password, type);

    loadSecrets(setting);
}

void PptpSettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();
        const QString keyPassword = secrets.value(QLatin1String(NM_PPTP_KEY_PASSWORD));
        if (!keyPassword.isEmpty()) {
            ui.edt_password->setText(keyPassword);
        }
    }
}

QVariantMap PptpSettingWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_PPTP));

    // save the main dialog's data in the setting
    // if the advanced dialog is dirty, save its data in the vpn setting too
    //
    NMStringMap data;
    NMStringMap secretData;

    data.insert(NM_PPTP_KEY_GATEWAY, ui.edt_gateway->text());
    data.insert(NM_PPTP_KEY_USER, ui.edt_login->text());
    if (!ui.edt_password->text().isEmpty()) {
        secretData.insert(QLatin1String(NM_PPTP_KEY_PASSWORD), ui.edt_password->text());
    }
    handleOnePasswordType(ui.edt_password, NM_PPTP_KEY_PASSWORD "-flags", data);
    if (!ui.edt_ntDomain->text().isEmpty()) {
        data.insert(NM_PPTP_KEY_DOMAIN, ui.edt_ntDomain->text());
    }

    // Advanced dialog settings

    // Authentication options
    QListWidgetItem *item = nullptr;
    item = advUi.listWidget->item(0); // PAP
    const QString yesString = QLatin1String("yes");
    if (item->checkState() == Qt::Unchecked) {
        data.insert(NM_PPTP_KEY_REFUSE_PAP, yesString);
    }
    item = advUi.listWidget->item(1); // CHAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(NM_PPTP_KEY_REFUSE_CHAP, yesString);
    }
    item = advUi.listWidget->item(2); // MSCHAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(NM_PPTP_KEY_REFUSE_MSCHAP, yesString);
    }
    item = advUi.listWidget->item(3); // MSCHAPv2
    if (item->checkState() == Qt::Unchecked) {
        data.insert(NM_PPTP_KEY_REFUSE_MSCHAPV2, yesString);
    }
    item = advUi.listWidget->item(4); // EAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(NM_PPTP_KEY_REFUSE_EAP, yesString);
    }

    // Cryptography and compression
    if (advUi.gb_MPPE->isChecked()) {
        int index = advUi.cb_MPPECrypto->currentIndex();

        switch (index) {
        case 0:
            data.insert(NM_PPTP_KEY_REQUIRE_MPPE, yesString);
            break;
        case 1:
            data.insert(NM_PPTP_KEY_REQUIRE_MPPE_128, yesString);
            break;
        case 2:
            data.insert(NM_PPTP_KEY_REQUIRE_MPPE_40, yesString);
            break;
        }

        if (advUi.cb_statefulEncryption->isChecked()) {
            data.insert(NM_PPTP_KEY_MPPE_STATEFUL, yesString);
        }
    }

    if (!advUi.cb_BSD->isChecked()) {
        data.insert(NM_PPTP_KEY_NOBSDCOMP, yesString);
    }
    if (!advUi.cb_deflate->isChecked()) {
        data.insert(NM_PPTP_KEY_NODEFLATE, yesString);
    }

    if (!advUi.cb_TCPheaders->isChecked()) {
        data.insert(NM_PPTP_KEY_NO_VJ_COMP, yesString);
    }
    // Echo
    if (advUi.cb_sendEcho->isChecked()) {
        data.insert(NM_PPTP_KEY_LCP_ECHO_FAILURE, "5");
        data.insert(NM_PPTP_KEY_LCP_ECHO_INTERVAL, "30");
    }

    // save it all
    setting.setData(data);
    setting.setSecrets(secretData);

    return setting.toMap();
}

void PptpSettingWidget::fillOnePasswordCombo(PasswordField *passwordField, NetworkManager::Setting::SecretFlags type)
{
    if (type.testFlag(NetworkManager::Setting::None)) {
        passwordField->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (type.testFlag(NetworkManager::Setting::AgentOwned)) {
        passwordField->setPasswordOption(PasswordField::StoreForUser);
    } else if (type.testFlag(NetworkManager::Setting::NotSaved)) {
        passwordField->setPasswordOption(PasswordField::AlwaysAsk);
    } else {
        passwordField->setPasswordOption(PasswordField::PasswordField::NotRequired);
    }
}

void PptpSettingWidget::handleOnePasswordType(const PasswordField *passwordField, const QString &key, NMStringMap &data) const
{
    const PasswordField::PasswordOption option = passwordField->passwordOption();
    switch (option) {
    case PasswordField::StoreForAllUsers:
        data.insert(key, QString::number(NetworkManager::Setting::None));
        break;
    case PasswordField::StoreForUser:
        data.insert(key, QString::number(NetworkManager::Setting::AgentOwned));
        break;
    case PasswordField::AlwaysAsk:
        data.insert(key, QString::number(NetworkManager::Setting::NotSaved));
        break;
    case PasswordField::NotRequired:
        data.insert(key, QString::number(NetworkManager::Setting::NotRequired));
        break;
    }
}

bool PptpSettingWidget::isValid() const
{
    return !ui.edt_gateway->text().isEmpty();
}
