/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2013 by Daniel Nicoletti <dantti12@gmail.com>

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

#include "debug.h"
#include "passworddialog.h"
#include "ui_passworddialog.h"
#include "uiutils.h"

#include <vpnuiplugin.h>

#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/Utils>

#include <KServiceTypeTrader>
#include <KLocalizedString>

#include <QIcon>
#include <QPushButton>

using namespace NetworkManager;

PasswordDialog::PasswordDialog(const NetworkManager::ConnectionSettings::Ptr &connectionSettings, SecretAgent::GetSecretsFlags flags,
                               const QString &setting_name, const QStringList &hints, QWidget *parent) :
    QDialog(parent),
    m_ui(nullptr),
    m_hasError(false),
    m_settingName(setting_name),
    m_connectionSettings(connectionSettings),
    m_error(SecretAgent::NoSecrets),
    m_flags(flags),
    m_vpnWidget(nullptr),
    m_hints(hints)
{
    setWindowIcon(QIcon::fromTheme(QStringLiteral("dialog-password")));

    initializeUi();
}

PasswordDialog::~PasswordDialog()
{
    delete m_ui;
}

void PasswordDialog::initializeUi()
{
    m_ui = new Ui::PasswordDialog;
    m_ui->setupUi(this);
    // TODO fix this for high DPI
    m_ui->labelIcon->setPixmap(QIcon::fromTheme(QStringLiteral("dialog-password")).pixmap(64));
    m_ui->labelHeadline->setText(i18n("Authenticate %1", m_connectionSettings->id()));

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &PasswordDialog::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected, this, &PasswordDialog::reject);
    connect(m_ui->password, &PasswordField::textChanged, [this](const QString &text){
        if (m_connectionSettings->connectionType() == NetworkManager::ConnectionSettings::Wireless) {
            NetworkManager::WirelessSecuritySetting::Ptr wirelessSecuritySetting =
                    m_connectionSettings->setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();
            bool valid = true;

            if (wirelessSecuritySetting) {
                switch (wirelessSecuritySetting->keyMgmt()) {
                case NetworkManager::WirelessSecuritySetting::WpaPsk:
                    valid = wpaPskIsValid(text);
                    break;
                case NetworkManager::WirelessSecuritySetting::Wep:
                    valid = wepKeyIsValid(text, wirelessSecuritySetting->wepKeyType());
                    break;
                default:
                    break;
                }
            }

            // disable button if key is not valid
            m_ui->buttonBox->button(QDialogButtonBox::Ok)->setDisabled(!valid);
        }
    });

    if (m_connectionSettings->connectionType() != NetworkManager::ConnectionSettings::Vpn) {
        NetworkManager::Setting::Ptr setting = m_connectionSettings->setting(m_settingName);
        m_neededSecrets = setting->needSecrets(m_flags & SecretAgent::RequestNew);

        if (m_neededSecrets.isEmpty()) {
            qCWarning(PLASMA_NM) << "list of secrets is empty!!!";
            m_hasError = true;
            m_error = SecretAgent::InternalError;
            m_errorMessage = QLatin1String("No secrets were requested");
            return;
        }

        WirelessSetting::Ptr wifi = m_connectionSettings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        Setting::SettingType connectionType = setting->type();
        if (wifi && (connectionType == Setting::WirelessSecurity || connectionType == Setting::Security8021x)) {
            const QString ssid = QString::fromUtf8(wifi->ssid());
            m_ui->labelText->setText(i18n("Provide the password for the wireless network '%1':", ssid));
        } else {
            m_ui->labelText->setText(i18n("Provide the password for the connection '%1':", m_connectionSettings->id()));
        }

        QString connectionLabel;
        UiUtils::iconAndTitleForConnectionSettingsType(m_connectionSettings->connectionType(), connectionLabel);
        setFocusProxy(m_ui->password);
        setWindowTitle(i18n("%1 password dialog", connectionLabel));
    } else {
        NetworkManager::VpnSetting::Ptr vpnSetting = m_connectionSettings->setting(Setting::Vpn).dynamicCast<VpnSetting>();
        if (!vpnSetting) {
            qCWarning(PLASMA_NM) << "Missing VPN setting!";
            m_hasError = true;
            m_error = SecretAgent::InternalError;
            m_errorMessage = QLatin1String("VPN settings are missing");
        } else {
            VpnUiPlugin *vpnUiPlugin;
            QString error;
            const QString serviceType = vpnSetting->serviceType();
            vpnUiPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QLatin1String("PlasmaNetworkManagement/VpnUiPlugin"),
                                                                                   QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(serviceType),
                                                                                   this, QVariantList(), &error);
            if (vpnUiPlugin && error.isEmpty()) {
                const QString shortName = serviceType.section('.', -1);
                NMStringMap data = vpnSetting->data();
                // If we have hints, make the user have them through the widget
                if (!m_hints.isEmpty() && m_hints.size() == 2) {
                    data.insert("hint", m_hints[0]);
                    data.insert("hint-msg", m_hints[1]);
                    vpnSetting->setData(data);
                }
                m_vpnWidget = vpnUiPlugin->askUser(vpnSetting, this);
                QVBoxLayout *layout = new QVBoxLayout();
                layout->addWidget(m_vpnWidget);
                m_ui->vpnWidget->setLayout(layout);
                m_ui->labelText->setText(i18n("Provide the secrets for the VPN connection '%1':", m_connectionSettings->id()));
                setWindowTitle(i18n("VPN secrets (%1) dialog", shortName));

                // Hide generic password field and OK button in case of openconnect dialog
                m_ui->labelPass->setVisible(false);
                m_ui->password->setVisible(false);
                if (shortName == QLatin1String("openconnect")) {
                    QAbstractButton *button = m_ui->buttonBox->button(QDialogButtonBox::Ok);
                    m_ui->buttonBox->removeButton(button);
                }

                setFocusProxy(m_vpnWidget);
                m_vpnWidget->setFocus(Qt::OtherFocusReason);

                // Delete hins from vpnSetting
                if (!m_hints.isEmpty()) {
                    data.remove("hint");
                    data.remove("hint-msg");
                    vpnSetting->setData(data);
                }
            } else {
                qCWarning(PLASMA_NM) << error << ", serviceType == " << serviceType;
                m_hasError = true;
                m_error = SecretAgent::InternalError;
                m_errorMessage = error;
            }
        }
    }

    // Workaround to force m_ui->password to get focus.
    focusNextChild();
}

bool PasswordDialog::hasError() const
{
    return m_hasError;
}

SecretAgent::Error PasswordDialog::error() const
{
    return m_error;
}

QString PasswordDialog::errorMessage() const
{
    return m_errorMessage;
}

NMVariantMapMap PasswordDialog::secrets() const
{
    NMVariantMapMap ret = m_connectionSettings->toMap();
    QVariantMap result;
    if (m_vpnWidget) {
        result = m_vpnWidget->setting();
    } else if (!m_ui->password->text().isEmpty() && !m_neededSecrets.isEmpty()) {
        result.insert(m_neededSecrets.first(), m_ui->password->text());
    }

    ret.insert(m_settingName, result);

    return ret;
}
