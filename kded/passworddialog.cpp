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

#include "passworddialog.h"
#include "ui_passworddialog.h"

#include <vpnuiplugin.h>

#include <QtNetworkManager/settings/802-11-wireless.h>
#include <QtNetworkManager/settings/vpn.h>

#include <KServiceTypeTrader>
#include <KIcon>
#include <KDebug>

using namespace NetworkManager;

PasswordDialog::PasswordDialog(SecretAgent::GetSecretsFlags flags, const QString &setting_name, QWidget *parent) :
    KDialog(parent),
    ui(0),
    vpnWidget(0),
    m_flags(flags),
    m_settingName(setting_name),
    m_hasError(false),
    m_error(SecretAgent::NoSecrets)
{
    setWindowIcon(KIcon("dialog-password"));
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

void PasswordDialog::setupGenericUi(const Settings::ConnectionSettings &connectionSettings)
{
    NetworkManager::Settings::Setting::Ptr setting = connectionSettings.setting(m_settingName);

    ui = new Ui::PasswordDialog;
    ui->setupUi(mainWidget());
    // TODO fix this for high DPI
    ui->labelIcon->setPixmap(KIcon("dialog-password").pixmap(32));

    m_neededSecrets = setting->needSecrets(m_flags & SecretAgent::RequestNew);
    if (m_neededSecrets.isEmpty()) {
        kWarning() << "list of secrets is empty!!!";
        m_hasError = true;
        m_error = SecretAgent::InternalError;
        m_errorMessage = QLatin1String("No secrets were requested");
    }

    NetworkManager::Settings::WirelessSetting::Ptr wifi;
    wifi = connectionSettings.setting(Settings::Setting::Wireless).dynamicCast<Settings::WirelessSetting>();

    Settings::Setting::SettingType connectionType = setting->type();
    if (wifi && (connectionType == Settings::Setting::WirelessSecurity || connectionType == Settings::Setting::Security8021x)) {
        QString ssid = wifi->ssid();
        ui->labelText->setText(i18n("For accessing the wireless network '%1' you need to provide a password below", ssid));
    } else {
        ui->labelText->setText(i18n("Please provide a password below"));
    }

    ui->password->setPasswordMode(true);
    ui->password->setFocus();
    connect(ui->showPassword, SIGNAL(toggled(bool)), this, SLOT(showPassword(bool)));
}

void PasswordDialog::setupVpnUi(const Settings::ConnectionSettings &connectionSettings)
{
    NetworkManager::Settings::VpnSetting::Ptr vpnSetting;
    vpnSetting = connectionSettings.setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
    if (!vpnSetting) {
        kDebug() << "Missing VPN setting!";
        m_hasError = true;
        m_error = SecretAgent::InternalError;
        m_errorMessage = QLatin1String("VPN settings are missing");
    } else {
        VpnUiPlugin *vpnUiPlugin;
        QString error;
        const QString serviceType = vpnSetting->serviceType();
        //qDebug() << "Agent loading VPN plugin" << serviceType << "from DBUS" << calledFromDBus();
        //vpnSetting->printSetting();
        vpnUiPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QLatin1String("PlasmaNM/VpnUiPlugin"),
                                                                               QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(serviceType),
                                                                               this, QVariantList(), &error);
        if (vpnUiPlugin && error.isEmpty()) {
            const QString shortName = serviceType.section('.', -1);
            setCaption(i18n("VPN secrets (%1)", shortName));
            vpnWidget = vpnUiPlugin->askUser(vpnSetting, this);
            setMainWidget(vpnWidget);
        } else {
            kDebug() << error << ", serviceType == " << serviceType;
            m_hasError = true;
            m_error = SecretAgent::InternalError;
            m_errorMessage = error;
        }
    }
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
    NMVariantMapMap ret;
    QVariantMap result;
    if (vpnWidget) {
        result = vpnWidget->setting();
    } else if (!ui->password->text().isEmpty() && !m_neededSecrets.isEmpty()) {
        result.insert(m_neededSecrets.first(), ui->password->text());
    }

    ret.insert(m_settingName, result);

    return ret;
}

void PasswordDialog::showPassword(bool show)
{
    ui->password->setPasswordMode(!show);
}
