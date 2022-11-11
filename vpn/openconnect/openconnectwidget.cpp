/*
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.de>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openconnectwidget.h"
#include <QDialog>
#include <QStringList>
#include <QUrl>

#include "nm-openconnect-service.h"
#include <QDialogButtonBox>
#include <QString>

#include <openconnect.h>

#if !OPENCONNECT_CHECK_VER(2, 1)
#define openconnect_has_stoken_support() 0
#endif
#if !OPENCONNECT_CHECK_VER(2, 2)
#define openconnect_has_oath_support() 0
#endif
#if !OPENCONNECT_CHECK_VER(5, 0)
#define openconnect_has_yubioath_support() 0
#endif

OpenconnectSettingWidget::OpenconnectSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
    : SettingWidget(setting, parent)
{
    ui.setupUi(this);
    m_setting = setting;

    // Connect for validity check
    connect(ui.leGateway, &QLineEdit::textChanged, this, &OpenconnectSettingWidget::slotWidgetChanged);

    connect(ui.buTokens, &QPushButton::clicked, this, &OpenconnectSettingWidget::showTokens);

    tokenDlg = new QDialog(this);
    tokenUi.setupUi(tokenDlg);
    tokenUi.leTokenSecret->setPasswordModeEnabled(true);
    tokenUi.leTokenSecret->setPasswordOptionsEnabled(true);
    auto layout = new QVBoxLayout(tokenDlg);
    layout->addWidget(tokenDlg);
    tokenDlg->setLayout(layout);
    connect(tokenUi.buttonBox, &QDialogButtonBox::accepted, tokenDlg, &QDialog::accept);
    connect(tokenUi.buttonBox, &QDialogButtonBox::rejected, tokenDlg, &QDialog::reject);
    connect(tokenDlg, &QDialog::rejected, this, &OpenconnectSettingWidget::restoreTokens);
    connect(tokenDlg, &QDialog::accepted, this, &OpenconnectSettingWidget::saveTokens);

    connect(tokenUi.cmbTokenMode,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            QOverload<int>::of((&OpenconnectSettingWidget::handleTokenSecret)));

    // Connect for setting check
    watchChangedSetting();

    // Remove these from setting check:
    // Just popping up the tokenDlg changes nothing
    disconnect(ui.buTokens, &QPushButton::clicked, this, &SettingWidget::settingChanged);
    // User cancels means nothing should change here
    disconnect(tokenUi.buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SettingWidget::settingChanged);

    tokenUi.gbToken->setVisible(initTokenGroup());

    KAcceleratorManager::manage(this);

    if (m_setting) {
        loadConfig(m_setting);
    }
}

void OpenconnectSettingWidget::handleTokenSecret(int index)
{
    QVariant mode = tokenUi.cmbTokenMode->itemData(index);
    if (mode == QStringLiteral("disabled")) {
        tokenUi.leTokenSecret->setEnabled(false);
        tokenUi.leTokenSecret->setToolTip("No secrets needed.");
    } else if (mode == QStringLiteral("stokenrc")) {
        tokenUi.leTokenSecret->setEnabled(false);
        tokenUi.leTokenSecret->setToolTip("No secrets needed; will read them from ~/.stokenrc.");
    } else if (mode == QStringLiteral("manual")) {
        tokenUi.leTokenSecret->setToolTip("Insert the secret here. See the openconnect documentation for syntax.");
        tokenUi.leTokenSecret->setEnabled(true);
    } else if (mode == QStringLiteral("totp")) {
        tokenUi.leTokenSecret->setEnabled(true);
        tokenUi.leTokenSecret->setToolTip(
            "Insert the secret here, with a sha specification and a leading '0x' or 'base32:'. See the openconnect documentation for syntax.");
    } else if (mode == QStringLiteral("hotp")) {
        tokenUi.leTokenSecret->setEnabled(true);
        tokenUi.leTokenSecret->setToolTip(
            "Insert the secret here, with a leading '0x' or 'base32:' and a trailing counter after a comma (','), See the openconnect documentation for "
            "syntax.");
    } else if (mode == QStringLiteral("yubioath")) {
        tokenUi.leTokenSecret->setEnabled(true);
        tokenUi.leTokenSecret->setToolTip("Insert the token Id here, in the form company:username. Make sure to set your Yubikey in CCID mode");
    } else { // Not really needed now, but who knows?
        tokenUi.leTokenSecret->setEnabled(false);
        tokenUi.leTokenSecret->setToolTip("");
    }
}

bool OpenconnectSettingWidget::initTokenGroup()
{
    int validRows = 0;
    QStringList tokenLabelList{
        "Disabled",
        "RSA SecurID — read from ~/.stokenrc",
        "RSA SecurID — manually entered",
        "TOTP — manually entered",
        "HOTP — manually entered",
        "Yubikey",
    };
    QStringList tokenModeList{
        "disabled",
        "stokenrc",
        "manual",
        "totp",
        "hotp",
        "yubioath",
    };
    QComboBox *combo = tokenUi.cmbTokenMode;

    combo->addItem(tokenLabelList[validRows]);
    combo->setItemData(validRows, tokenModeList[validRows], Qt::UserRole);
    validRows++;
    if (openconnect_has_stoken_support()) {
        for (; validRows < 3; validRows++) {
            combo->addItem(tokenLabelList[validRows]);
            combo->setItemData(validRows, tokenModeList[validRows], Qt::UserRole);
        }
    }
    if (openconnect_has_oath_support()) {
        combo->addItem(tokenLabelList[validRows]);
        combo->setItemData(validRows, tokenModeList[validRows], Qt::UserRole);
        validRows++;
        if (OPENCONNECT_CHECK_VER(3, 4)) {
            combo->addItem(tokenLabelList[validRows]);
            combo->setItemData(validRows, tokenModeList[validRows], Qt::UserRole);
            validRows++;
        }
    }
    if (openconnect_has_yubioath_support()) {
        combo->addItem(tokenLabelList[validRows]);
        combo->setItemData(validRows, tokenModeList[validRows], Qt::UserRole);
    }
    return validRows > 0;
}

void OpenconnectSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    // General settings
    const NMStringMap dataMap = setting.staticCast<NetworkManager::VpnSetting>()->data();

    int cmbProtocolIndex;
    // No value corresponds to "anyconnect", matching GNOME and the openconnect binary itself.
    if (!dataMap.contains(NM_OPENCONNECT_KEY_PROTOCOL) || dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("anyconnect")) {
        cmbProtocolIndex = 0;
    } else if (dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("nc")) {
        cmbProtocolIndex = 1;
    } else if (dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("gp")) {
        cmbProtocolIndex = 2;
    } else if (dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("pulse")) {
        cmbProtocolIndex = 3; // pulse, Pulse Connect Secure
    } else if (dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("f5")) {
        cmbProtocolIndex = 4; // F5 BIG-IP SSL VPN
    } else if (dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("fortinet")) {
        cmbProtocolIndex = 5; // Fortinet SSL VPN
    } else if (dataMap[NM_OPENCONNECT_KEY_PROTOCOL] == QLatin1String("array")) {
        cmbProtocolIndex = 6; // Array SSL VPN
    } else {
        cmbProtocolIndex = 3; // pulse, Pulse Connect Secure is the default
    }

    int cmbReportedOsIndex;
    if (!dataMap.contains(NM_OPENCONNECT_KEY_REPORTED_OS)) {
        cmbReportedOsIndex = 0;
    } else if (dataMap[NM_OPENCONNECT_KEY_REPORTED_OS] == QLatin1String("linux")) {
        cmbReportedOsIndex = 1;
    } else if (dataMap[NM_OPENCONNECT_KEY_REPORTED_OS] == QLatin1String("linux-64")) {
        cmbReportedOsIndex = 2;
    } else if (dataMap[NM_OPENCONNECT_KEY_REPORTED_OS] == QLatin1String("win")) {
        cmbReportedOsIndex = 3;
    } else if (dataMap[NM_OPENCONNECT_KEY_REPORTED_OS] == QLatin1String("mac-intel")) {
        cmbReportedOsIndex = 4;
    } else if (dataMap[NM_OPENCONNECT_KEY_REPORTED_OS] == QLatin1String("android")) {
        cmbReportedOsIndex = 5;
    } else {
        cmbReportedOsIndex = 6; // apple-ios
    }

    ui.cmbProtocol->setCurrentIndex(cmbProtocolIndex);
    ui.leGateway->setText(dataMap[NM_OPENCONNECT_KEY_GATEWAY]);
    ui.leCaCertificate->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_CACERT]));
    ui.leProxy->setText(dataMap[NM_OPENCONNECT_KEY_PROXY]);
    ui.leUserAgent->setText(dataMap[NM_OPENCONNECT_KEY_USERAGENT]);
    ui.cmbReportedOs->setCurrentIndex(cmbReportedOsIndex);
    ui.chkAllowTrojan->setChecked(dataMap[NM_OPENCONNECT_KEY_CSD_ENABLE] == "yes");
    ui.leCsdWrapperScript->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_CSD_WRAPPER]));
    ui.leUserCert->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_USERCERT]));
    ui.leUserPrivateKey->setUrl(QUrl::fromLocalFile(dataMap[NM_OPENCONNECT_KEY_PRIVKEY]));
    ui.chkUseFsid->setChecked(dataMap[NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID] == "yes");
    ui.preventInvalidCert->setChecked(dataMap[NM_OPENCONNECT_KEY_PREVENT_INVALID_CERT] == "yes");

    // Token settings
    const NetworkManager::Setting::SecretFlags tokenSecretFlag =
        static_cast<NetworkManager::Setting::SecretFlags>(dataMap.value(NM_OPENCONNECT_KEY_TOKEN_SECRET "-flags").toInt());
    if (tokenSecretFlag == NetworkManager::Setting::None) {
        tokenUi.leTokenSecret->setPasswordOption(PasswordField::StoreForAllUsers);
    } else if (tokenSecretFlag == NetworkManager::Setting::AgentOwned) {
        tokenUi.leTokenSecret->setPasswordOption(PasswordField::StoreForUser);
    } else {
        tokenUi.leTokenSecret->setPasswordOption(PasswordField::AlwaysAsk);
    }
    for (int index = 0; index < tokenUi.cmbTokenMode->count(); index++) {
        if (tokenUi.cmbTokenMode->itemData(index, Qt::UserRole) == dataMap[NM_OPENCONNECT_KEY_TOKEN_MODE]) {
            tokenUi.cmbTokenMode->setCurrentIndex(index);
            token.tokenIndex = index;
            if (index > 1) {
                loadSecrets(setting);
            }
            break;
        }
    }
}

void OpenconnectSettingWidget::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = setting.staticCast<NetworkManager::VpnSetting>();

    if (vpnSetting) {
        const NMStringMap secrets = vpnSetting->secrets();
        tokenUi.leTokenSecret->setText(secrets.value(NM_OPENCONNECT_KEY_TOKEN_SECRET));
        token.tokenSecret = secrets.value(NM_OPENCONNECT_KEY_TOKEN_SECRET);
    }
}

QVariantMap OpenconnectSettingWidget::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_OPENCONNECT));

    NMStringMap data;
    NMStringMap secrets;
    QString protocol;
    switch (ui.cmbProtocol->currentIndex()) {
    case 0:
        protocol = QLatin1String("anyconnect");
        break;
    case 1:
        protocol = QLatin1String("nc");
        break;
    case 2:
        protocol = QLatin1String("gp");
        break;
    case 3:
        protocol = QLatin1String("pulse");
        break;
    case 4:
        protocol = QLatin1String("f5");
        break;
    case 5:
        protocol = QLatin1String("fortinet");
        break;
    case 6:
        protocol = QLatin1String("array");
        break;
    default:
        protocol = QLatin1String("pulse");
    }

    QString reportedOs;
    switch (ui.cmbReportedOs->currentIndex()) {
    case 0:
        reportedOs = QString();
        break;
    case 1:
        reportedOs = QLatin1String("linux");
        break;
    case 2:
        reportedOs = QLatin1String("linux-64");
        break;
    case 3:
        reportedOs = QLatin1String("win");
        break;
    case 4:
        reportedOs = QLatin1String("mac-intel");
        break;
    case 5:
        reportedOs = QLatin1String("android");
        break;
    default:
        reportedOs = QLatin1String("apple-ios");
    }

    data.insert(NM_OPENCONNECT_KEY_PROTOCOL, protocol);
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY), ui.leGateway->text());
    if (ui.leCaCertificate->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_CACERT), ui.leCaCertificate->url().toLocalFile());
    }
    if (!ui.leProxy->text().isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_PROXY), ui.leProxy->text());
    }
    if (!ui.leUserAgent->text().isEmpty()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_USERAGENT), ui.leUserAgent->text());
    }
    data.insert(NM_OPENCONNECT_KEY_REPORTED_OS, reportedOs);
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_CSD_ENABLE), ui.chkAllowTrojan->isChecked() ? "yes" : "no");
    if (ui.leCsdWrapperScript->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_CSD_WRAPPER), ui.leCsdWrapperScript->url().toLocalFile());
    }
    if (ui.leUserCert->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_USERCERT), ui.leUserCert->url().toLocalFile());
    }
    if (ui.leUserPrivateKey->url().isValid()) {
        data.insert(QLatin1String(NM_OPENCONNECT_KEY_PRIVKEY), ui.leUserPrivateKey->url().toLocalFile());
    }
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_PEM_PASSPHRASE_FSID), ui.chkUseFsid->isChecked() ? "yes" : "no");
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_PREVENT_INVALID_CERT), ui.preventInvalidCert->isChecked() ? "yes" : "no");

    int index = tokenUi.cmbTokenMode->currentIndex();
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_MODE), tokenUi.cmbTokenMode->itemData(index, Qt::UserRole).toString());
    secrets.insert(QLatin1String(NM_OPENCONNECT_KEY_TOKEN_SECRET), tokenUi.leTokenSecret->text());

    // Restore previous flags, this is necessary for keeping secrets stored in KWallet
    for (const QString &key : m_setting->data().keys()) {
        if (key.contains(QLatin1String("-flags"))) {
            data.insert(key, m_setting->data().value(key));
        }
    }

    if (tokenUi.leTokenSecret->passwordOption() == PasswordField::StoreForAllUsers) {
        data.insert(NM_OPENCONNECT_KEY_TOKEN_SECRET "-flags", QString::number(NetworkManager::Setting::None));
    } else if (tokenUi.leTokenSecret->passwordOption() == PasswordField::StoreForUser) {
        data.insert(NM_OPENCONNECT_KEY_TOKEN_SECRET "-flags", QString::number(NetworkManager::Setting::AgentOwned));
    } else {
        data.insert(NM_OPENCONNECT_KEY_TOKEN_SECRET "-flags", QString::number(NetworkManager::Setting::NotSaved));
    }

    /* These are different for every login session, and should not be stored */
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_COOKIE "-flags"), QString::number(NetworkManager::Setting::NotSaved));
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GWCERT "-flags"), QString::number(NetworkManager::Setting::NotSaved));
    data.insert(QLatin1String(NM_OPENCONNECT_KEY_GATEWAY "-flags"), QString::number(NetworkManager::Setting::NotSaved));

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

void OpenconnectSettingWidget::restoreTokens()
{
    tokenUi.cmbTokenMode->setCurrentIndex(token.tokenIndex);
    tokenUi.leTokenSecret->setText(token.tokenSecret);
}

void OpenconnectSettingWidget::saveTokens()
{
    token.tokenIndex = tokenUi.cmbTokenMode->currentIndex();
    token.tokenSecret = tokenUi.leTokenSecret->text();
}

void OpenconnectSettingWidget::showTokens()
{
    tokenDlg->show();
}

bool OpenconnectSettingWidget::isValid() const
{
    return !ui.leGateway->text().isEmpty();
}
