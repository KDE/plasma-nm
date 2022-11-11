/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "sstpwidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QString>

#include "nm-sstp-service.h"

class SstpSettingWidgetPrivate
{
public:
};

SstpSettingWidget::SstpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
{
    ui.setupUi(this);

    m_setting = setting;

    ui.le_password->setPasswordOptionsEnabled(true);

    connect(ui.btn_advancedOption, &QPushButton::clicked, this, &SstpSettingWidget::doAdvancedDialog);

    advancedDlg = new QDialog(this);
    advancedDlg->setModal(true);
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
    connect(ui.le_gateway, &QLineEdit::textChanged, this, &SstpSettingWidget::slotWidgetChanged);
    connect(ui.le_username, &QLineEdit::textChanged, this, &SstpSettingWidget::slotWidgetChanged);
    connect(ui.le_password, &PasswordField::textChanged, this, &SstpSettingWidget::slotWidgetChanged);

    KAcceleratorManager::manage(this);

    if (setting && !setting->isNull()) {
        loadConfig(setting);
    }
}

void SstpSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_UNUSED(setting)

    const QString yesString = QLatin1String("yes");
    const NMStringMap dataMap = m_setting->data();

    // General
    const QString gateway = dataMap[QLatin1String(NM_SSTP_KEY_GATEWAY)];
    if (!gateway.isEmpty()) {
        ui.le_gateway->setText(gateway);
    }

    // Optional setting
    const QString username = dataMap[QLatin1String(NM_SSTP_KEY_USER)];
    if (!username.isEmpty()) {
        ui.le_username->setText(username);
    }

    // Authentication
    const NetworkManager::Setting::SecretFlags type = (NetworkManager::Setting::SecretFlags)dataMap[NM_SSTP_KEY_PASSWORD_FLAGS].toInt();
    fillOnePasswordCombo(ui.le_password, type);

    const QString ntDomain = dataMap[QLatin1String(NM_SSTP_KEY_DOMAIN)];
    if (!ntDomain.isEmpty()) {
        ui.le_ntDomain->setText(ntDomain);
    }

    const QString caCert = dataMap[QLatin1String(NM_SSTP_KEY_CA_CERT)];
    if (!caCert.isEmpty()) {
        ui.kurl_caCert->setUrl(QUrl::fromLocalFile(caCert));
    }

    const bool ignoreCertWarnings = (dataMap[QLatin1String(NM_SSTP_KEY_IGN_CERT_WARN)] == yesString);
    ui.chk_ignoreCertWarnings->setChecked(ignoreCertWarnings);

    // Advanced options - Point-to-Point
    bool refuse_pap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_PAP)] == yesString);
    bool refuse_chap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_CHAP)] == yesString);
    bool refuse_mschap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAP)] == yesString);
    bool refuse_mschapv2 = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAPV2)] == yesString);
    bool refuse_eap = (dataMap[QLatin1String(NM_SSTP_KEY_REFUSE_EAP)] == yesString);

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
    const bool mppe = (dataMap[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE)] == yesString);
    const bool mppe40 = (dataMap[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_40)] == yesString);
    const bool mppe128 = (dataMap[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_128)] == yesString);
    const bool mppe_stateful = (dataMap[QLatin1String(NM_SSTP_KEY_MPPE_STATEFUL)] == yesString);

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

    const bool nobsd = (dataMap[QLatin1String(NM_SSTP_KEY_NOBSDCOMP)] == yesString);
    advUi.cb_BSD->setChecked(!nobsd);

    const bool nodeflate = (dataMap[QLatin1String(NM_SSTP_KEY_NODEFLATE)] == yesString);
    advUi.cb_deflate->setChecked(!nodeflate);

    const bool novjcomp = (dataMap[QLatin1String(NM_SSTP_KEY_NO_VJ_COMP)] == yesString);
    advUi.cb_TCPheaders->setChecked(!novjcomp);

    // Echo
    const int lcp_echo_interval = QString(dataMap[QLatin1String(NM_SSTP_KEY_LCP_ECHO_INTERVAL)]).toInt();
    advUi.cb_sendEcho->setChecked(lcp_echo_interval > 0);

    if (dataMap.contains(QLatin1String(NM_SSTP_KEY_UNIT_NUM))) {
        advUi.chk_useCustomUnitNumber->setChecked(true);
        advUi.sb_customUnitNumber->setValue(dataMap[QLatin1String(NM_SSTP_KEY_UNIT_NUM)].toInt());
    }

    // Advanced options - Proxy
    const QString address = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_SERVER)];
    if (!address.isEmpty()) {
        advUi.le_address->setText(address);
    }

    const int port = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_PORT)].toInt();
    if (port >= 0) {
        advUi.sb_port->setValue(port);
    }

    const QString proxyUsername = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_USER)];
    if (!proxyUsername.isEmpty()) {
        advUi.le_username->setText(proxyUsername);
    }

    const QString proxyPassword = dataMap[QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD)];
    if (!proxyPassword.isEmpty()) {
        advUi.le_password->setText(proxyPassword);
    }

    loadSecrets(setting);
}

void SstpSettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();

        const QString keyPassword = secrets.value(NM_SSTP_KEY_PASSWORD);
        if (!keyPassword.isEmpty()) {
            ui.le_password->setText(keyPassword);
        }
    }
}

QVariantMap SstpSettingWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_SSTP));

    const QString yesString = QLatin1String("yes");

    NMStringMap data;
    NMStringMap secretData;

    data.insert(QLatin1String(NM_SSTP_KEY_GATEWAY), ui.le_gateway->text());

    if (!ui.le_username->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_USER), ui.le_username->text());
    }

    if (!ui.le_password->text().isEmpty()) {
        secretData.insert(QLatin1String(NM_SSTP_KEY_PASSWORD), ui.le_password->text());
    }
    handleOnePasswordType(ui.le_password, NM_SSTP_KEY_PASSWORD_FLAGS, data);

    if (!ui.le_ntDomain->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_DOMAIN), ui.le_ntDomain->text());
    }

    if (!ui.kurl_caCert->url().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_CA_CERT), ui.kurl_caCert->url().toLocalFile());
    }

    if (ui.chk_ignoreCertWarnings->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_IGN_CERT_WARN), yesString);
    }

    // Advanced configuration
    QListWidgetItem *item = nullptr;
    item = advUi.listWidget->item(0); // PAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_PAP), yesString);
    }
    item = advUi.listWidget->item(1); // CHAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_CHAP), yesString);
    }
    item = advUi.listWidget->item(2); // MSCHAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAP), yesString);
    }
    item = advUi.listWidget->item(3); // MSCHAPv2
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAPV2), yesString);
    }
    item = advUi.listWidget->item(4); // EAP
    if (item->checkState() == Qt::Unchecked) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_EAP), yesString);
    }

    // Cryptography and compression
    if (advUi.gb_MPPE->isChecked()) {
        int index = advUi.cb_MPPECrypto->currentIndex();

        switch (index) {
        case 0:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE), yesString);
            break;
        case 1:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_128), yesString);
            break;
        case 2:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_40), yesString);
            break;
        }

        if (advUi.cb_statefulEncryption->isChecked()) {
            data.insert(NM_SSTP_KEY_MPPE_STATEFUL, yesString);
        }
    }

    if (!advUi.cb_BSD->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_NOBSDCOMP), yesString);
    }

    if (!advUi.cb_deflate->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_NODEFLATE), yesString);
    }

    if (!advUi.cb_TCPheaders->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_NO_VJ_COMP), yesString);
    }

    // Echo
    if (advUi.cb_sendEcho->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_LCP_ECHO_FAILURE), "5");
        data.insert(QLatin1String(NM_SSTP_KEY_LCP_ECHO_INTERVAL), "30");
    }

    if (advUi.chk_useCustomUnitNumber->isChecked()) {
        data.insert(QLatin1String(NM_SSTP_KEY_UNIT_NUM), QString::number(advUi.sb_customUnitNumber->value()));
    }

    if (!advUi.le_address->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_SERVER), advUi.le_address->text());
    }

    if (advUi.sb_port->value() >= 0) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PORT), QString::number(advUi.sb_port->value()));
    }

    if (!advUi.le_username->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_USER), advUi.le_username->text());
    }

    if (!advUi.le_password->text().isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD), advUi.le_password->text());
    }
    handleOnePasswordType(advUi.le_password, NM_SSTP_KEY_PROXY_PASSWORD_FLAGS, data);

    // save it all
    setting.setData(data);
    setting.setSecrets(secretData);

    return setting.toMap();
}

void SstpSettingWidget::doAdvancedDialog()
{
    advancedDlg->show();
}

void SstpSettingWidget::fillOnePasswordCombo(PasswordField *passwordField, NetworkManager::Setting::SecretFlags type)
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

void SstpSettingWidget::handleOnePasswordType(const PasswordField *passwordField, const QString &key, NMStringMap &data) const
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

bool SstpSettingWidget::isValid() const
{
    return !ui.le_gateway->text().isEmpty();
}
