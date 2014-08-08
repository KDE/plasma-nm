/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2009 Pavel Andreev <apavelm@gmail.com>
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

#include "pptpwidget.h"

#include "ui_pptpprop.h"
#include "ui_pptpadvanced.h"

#include <QString>
#include <QDialog>
#include <QDialogButtonBox>

#include "nm-pptp-service.h"

class PptpSettingWidgetPrivate
{
public:
    Ui_PptpProp ui;
    Ui_PptpAdvanced advUi;
    NetworkManager::VpnSetting::Ptr setting;
    QDialog * advancedDlg;
    QWidget * advancedWid;
};

PptpSettingWidget::PptpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
    : SettingWidget(setting, parent), d_ptr(new PptpSettingWidgetPrivate)
{
    Q_D(PptpSettingWidget);
    d->ui.setupUi(this);

    d->setting = setting;

    connect(d->ui.btnAdvanced, SIGNAL(clicked()), this, SLOT(doAdvancedDialog()));
    connect (d->ui.cmbPasswordStorage, SIGNAL(currentIndexChanged(int)), this, SLOT(passwordTypeChanged(int)));
    connect(d->ui.cb_showPassword, SIGNAL(toggled(bool)), this, SLOT(setShowPassword(bool)));
    d->advancedDlg = new QDialog(this);
    d->advancedWid = new QWidget(this);
    d->advUi.setupUi(d->advancedWid);
    QVBoxLayout * layout = new QVBoxLayout(d->advancedDlg);
    layout->addWidget(d->advancedWid);
    d->advancedDlg->setLayout(layout);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, d->advancedDlg);
    connect(buttons, SIGNAL(accepted()), d->advancedDlg, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), d->advancedDlg, SLOT(reject()));
    layout->addWidget(buttons);

    connect(d->ui.edt_gateway, SIGNAL(textChanged(QString)), SLOT(slotWidgetChanged()));

    KAcceleratorManager::manage(this);

    if (d->setting)
        loadConfig(d->setting);
}

PptpSettingWidget::~PptpSettingWidget()
{
    delete d_ptr;
}

void PptpSettingWidget::passwordTypeChanged(int index)
{
    Q_D(PptpSettingWidget);
    d->ui.edt_password->setEnabled(index == SettingWidget::EnumPasswordStorageType::Store);
}

void PptpSettingWidget::setShowPassword(bool show)
{
    Q_D(PptpSettingWidget);
    if (show)
        d->ui.edt_password->setEchoMode(QLineEdit::Normal);
    else
        d->ui.edt_password->setEchoMode(QLineEdit::Password);
}

void PptpSettingWidget::doAdvancedDialog()
{
    Q_D(PptpSettingWidget);
    d->advancedDlg->exec();
}

void PptpSettingWidget::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    Q_D(PptpSettingWidget);
    Q_UNUSED(setting)

    // General settings
    const NMStringMap dataMap = d->setting->data();
    const NMStringMap secrets = d->setting->secrets();

    // Authentication
    const QString sGateway = dataMap[NM_PPTP_KEY_GATEWAY];
    if (!sGateway.isEmpty())
    {
        d->ui.edt_gateway->setText(sGateway);
    }

    const QString sLogin = dataMap[NM_PPTP_KEY_USER];
    if (!sLogin.isEmpty())
    {
        d->ui.edt_login->setText(sLogin);
    }

    const QString sPassword = secrets[NM_PPTP_KEY_PASSWORD];
    if (!sPassword.isEmpty()) {
        d->ui.edt_password->setText(sPassword);
    }

    const QString sDomain = dataMap[NM_PPTP_KEY_DOMAIN];
    if (!sDomain.isEmpty())
    {
        d->ui.edt_ntDomain->setText(sDomain);
    }

    // Options below is belongs to "Advanced" dialog

    // Authentication options
    const QString yesString = QLatin1String("yes");
    bool refuse_pap = (dataMap[NM_PPTP_KEY_REFUSE_PAP] == yesString);
    bool refuse_chap = (dataMap[NM_PPTP_KEY_REFUSE_CHAP] == yesString);
    bool refuse_mschap = (dataMap[NM_PPTP_KEY_REFUSE_MSCHAP] == yesString);
    bool refuse_mschapv2 = (dataMap[NM_PPTP_KEY_REFUSE_MSCHAPV2] == yesString);
    bool refuse_eap = (dataMap[NM_PPTP_KEY_REFUSE_EAP] == yesString);

    QListWidgetItem * item = 0;
    item = d->advUi.listWidget->item(0); // PAP
    item->setCheckState(refuse_pap ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(1); // CHAP
    item->setCheckState(refuse_chap ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(2); // MSCHAP
    item->setCheckState(refuse_mschap ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(3); // MSCHAPv2
    item->setCheckState(refuse_mschapv2 ? Qt::Unchecked : Qt::Checked);
    item = d->advUi.listWidget->item(4); // EAP
    item->setCheckState(refuse_eap ? Qt::Unchecked : Qt::Checked);

    // Cryptography and compression
    const bool mppe = (dataMap[NM_PPTP_KEY_REQUIRE_MPPE] == yesString);
    const bool mppe40 = (dataMap[NM_PPTP_KEY_REQUIRE_MPPE_40] == yesString);
    const bool mppe128 = (dataMap[NM_PPTP_KEY_REQUIRE_MPPE_128] == yesString);
    const bool mppe_stateful = (dataMap[NM_PPTP_KEY_MPPE_STATEFUL] == yesString);

    if (mppe || mppe40 || mppe128) { // If MPPE is use
        d->advUi.gb_MPPE->setChecked(mppe || mppe40 || mppe128);
        if (mppe128) {
            d->advUi.cb_MPPECrypto->setCurrentIndex(1); // 128 bit
        }
        else if (mppe40) {
            d->advUi.cb_MPPECrypto->setCurrentIndex(2); // 40 bit
        }
        else {
            d->advUi.cb_MPPECrypto->setCurrentIndex(0); // Any
        }
        d->advUi.cb_statefulEncryption->setChecked(mppe_stateful);
    }

    const bool nobsd = (dataMap[NM_PPTP_KEY_NOBSDCOMP] == yesString);
    d->advUi.cb_BSD->setChecked(!nobsd);

    const bool nodeflate = (dataMap[NM_PPTP_KEY_NODEFLATE] == yesString);
    d->advUi.cb_deflate->setChecked(!nodeflate);

    const bool novjcomp = (dataMap[NM_PPTP_KEY_NO_VJ_COMP] == yesString);
    d->advUi.cb_TCPheaders->setChecked(!novjcomp);

    // Echo
    const int lcp_echo_interval = QString(dataMap[NM_PPTP_KEY_LCP_ECHO_INTERVAL]).toInt();
    d->advUi.cb_sendEcho->setChecked(lcp_echo_interval > 0);

    // secrets
    const NetworkManager::Setting::SecretFlags type = (NetworkManager::Setting::SecretFlags)dataMap[NM_PPTP_KEY_PASSWORD"-flags"].toInt();
    if (type & NetworkManager::Setting::AgentOwned || type & NetworkManager::Setting::None || !type) {
        d->ui.edt_password->setText(secrets.value(QLatin1String(NM_PPTP_KEY_PASSWORD)));
    }
    fillOnePasswordCombo(d->ui.cmbPasswordStorage, type);
}

QVariantMap PptpSettingWidget::setting(bool agentOwned) const
{
    Q_D(const PptpSettingWidget);

    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_PPTP));

    // save the main dialog's data in the setting
    // if the advanced dialog is dirty, save its data in the vpn setting too
    //
    NMStringMap data;
    NMStringMap secretData;

    data.insert(NM_PPTP_KEY_GATEWAY,  d->ui.edt_gateway->text());
    data.insert(NM_PPTP_KEY_USER, d->ui.edt_login->text());
    if (!d->ui.edt_password->text().isEmpty()) {
        secretData.insert(QLatin1String(NM_PPTP_KEY_PASSWORD), d->ui.edt_password->text());
    }
    handleOnePasswordType(d->ui.cmbPasswordStorage, NM_PPTP_KEY_PASSWORD"-flags", data, agentOwned);
    if (!d->ui.edt_ntDomain->text().isEmpty()) {
        data.insert(NM_PPTP_KEY_DOMAIN,  d->ui.edt_ntDomain->text());
    }

    // Advanced dialog settings

    // Authenfication options
    QListWidgetItem * item = 0;
    item = d->advUi.listWidget->item(0); // PAP
    const QString yesString = QLatin1String("yes");
    if (item->checkState() == Qt::Unchecked)
        data.insert(NM_PPTP_KEY_REFUSE_PAP, yesString);
    item = d->advUi.listWidget->item(1); // CHAP
    if (item->checkState() == Qt::Unchecked)
        data.insert(NM_PPTP_KEY_REFUSE_CHAP, yesString);
    item = d->advUi.listWidget->item(2); // MSCHAP
    if (item->checkState() == Qt::Unchecked)
        data.insert(NM_PPTP_KEY_REFUSE_MSCHAP, yesString);
    item = d->advUi.listWidget->item(3); // MSCHAPv2
    if (item->checkState() == Qt::Unchecked)
        data.insert(NM_PPTP_KEY_REFUSE_MSCHAPV2, yesString);
    item = d->advUi.listWidget->item(4); // EAP
    if (item->checkState() == Qt::Unchecked)
        data.insert(NM_PPTP_KEY_REFUSE_EAP, yesString);

    // Cryptography and compression
    if (d->advUi.gb_MPPE->isChecked())
    {
        int index = d->advUi.cb_MPPECrypto->currentIndex();

        switch (index)
        {
            case 0:
                {
                    // "Any"
                    data.insert(NM_PPTP_KEY_REQUIRE_MPPE, yesString);
                }
                break;
            case 1:
                {
                    // "128 bit"
                    data.insert(NM_PPTP_KEY_REQUIRE_MPPE_128, yesString);
                }
                break;
            case 2:
                {
                    // "40 bit"
                    data.insert(NM_PPTP_KEY_REQUIRE_MPPE_40, yesString);
                }
                break;
        }

        if (d->advUi.cb_statefulEncryption->isChecked()) {
            data.insert(NM_PPTP_KEY_MPPE_STATEFUL, yesString);
        }
    }

    if (!d->advUi.cb_BSD->isChecked()) {
        data.insert(NM_PPTP_KEY_NOBSDCOMP, yesString);

    }
    if (!d->advUi.cb_deflate->isChecked()) {
        data.insert(NM_PPTP_KEY_NODEFLATE, yesString);
    }

    if (!d->advUi.cb_TCPheaders->isChecked()) {
        data.insert(NM_PPTP_KEY_NO_VJ_COMP, yesString);
    }
    // Echo
    if (d->advUi.cb_sendEcho->isChecked()) {
        data.insert(NM_PPTP_KEY_LCP_ECHO_FAILURE, "5");
        data.insert(NM_PPTP_KEY_LCP_ECHO_INTERVAL, "30");
    }

    // save it all
    setting.setData(data);
    setting.setSecrets(secretData);

    return setting.toMap();
}

void PptpSettingWidget::fillOnePasswordCombo(QComboBox * combo, NetworkManager::Setting::SecretFlags type)
{
    if (type.testFlag(NetworkManager::Setting::AgentOwned) || type.testFlag(NetworkManager::Setting::None)) { // store
        combo->setCurrentIndex(0);
    } else if (type.testFlag(NetworkManager::Setting::NotRequired)) { // not required
        combo->setCurrentIndex(2);
    } else if (type.testFlag(NetworkManager::Setting::NotSaved)) { // always ask
        combo->setCurrentIndex(1);
    }
}

uint PptpSettingWidget::handleOnePasswordType(const QComboBox *combo, const QString & key, NMStringMap & data, bool agentOwned) const
{
    const uint type = combo->currentIndex();
    switch (type) {
        case 1:
            data.insert(key, QString::number(NetworkManager::Setting::NotSaved)); // always ask
            break;
        case 0:
            if (agentOwned)
                data.insert(key, QString::number(NetworkManager::Setting::AgentOwned)); // store
            else
                data.insert(key, QString::number(NetworkManager::Setting::None));
            break;
        case 2:
            data.insert(key, QString::number(NetworkManager::Setting::NotRequired)); // not required
            break;
    }
    return type;
}

bool PptpSettingWidget::isValid() const
{
    Q_D(const PptpSettingWidget);

    return !d->ui.edt_gateway->text().isEmpty();
}
