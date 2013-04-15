/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>
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

#include "secretagent.h"
#include "passworddialog.h"
#include "vpnuiplugin.h"

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/settings/802-11-wireless.h>
#include <QtNetworkManager/generic-types.h>
#include <QtNetworkManager/settings/vpn.h>

#include <KServiceTypeTrader>
#include <KPluginFactory>
#include <KWindowSystem>
#include <KDialog>
#include <kwallet.h>

#include <KDebug>

SecretAgent::SecretAgent(QObject* parent):
    NetworkManager::SecretAgent("org.kde.plasma-nm", parent)
{
}

SecretAgent::~SecretAgent()
{
}

NMVariantMapMap SecretAgent::GetSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path, const QString &setting_name,
                                       const QStringList &hints, uint flags)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Path:" << connection_path.path();
    qDebug() << "Setting name:" << setting_name;
    qDebug() << "Hints:" << hints;
    qDebug() << "Flags:" << flags;

    NetworkManager::Settings::ConnectionSettings::Ptr connectionSettings = NetworkManager::Settings::ConnectionSettings::Ptr(new NetworkManager::Settings::ConnectionSettings());
    connectionSettings->fromMap(connection);

    NetworkManager::Settings::Setting::Ptr setting = connectionSettings->setting(NetworkManager::Settings::Setting::typeFromString(setting_name));

    if (!KWallet::Wallet::isEnabled()) {
        kWarning() << "KWallet is disabled, please enable it. Secrets not loaded.";
    } else {
        KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);

        if (wallet) {
            if (wallet->isOpen() && wallet->hasFolder("plasma-nm") && wallet->setFolder("plasma-nm")) {
                QString key = connectionSettings->uuid() + ";" + setting->name();
                QMap<QString,QString> map;
                wallet->readMap(key, map);
                QMap<QString, QVariant> secretsMap;
                foreach (const QString & key, map.keys()) {
                    secretsMap.insert(key, map.value(key));
                }
                setting->secretsFromMap(secretsMap);
                wallet->deleteLater();
            }
        } else {
            kWarning() << "Error opening kwallet. Secrets not loaded.";
        }
    }

    NetworkManager::SecretAgent::GetSecretsFlags secretsFlags(static_cast<int>(flags));
    const bool requestNew = secretsFlags.testFlag(RequestNew);
    const bool userRequested = secretsFlags.testFlag(UserRequested);
    const bool allowInteraction = secretsFlags.testFlag(AllowInteraction);
    const bool isVpn = (setting->type() == NetworkManager::Settings::Setting::Vpn);

    if (requestNew || (allowInteraction && !setting->needSecrets(requestNew).isEmpty()) || (allowInteraction && userRequested) || (isVpn && allowInteraction)) {
        NMVariantMapMap result;
        if (isVpn) {
            QString error;
            VpnUiPlugin * vpnPlugin = 0;
            NetworkManager::Settings::VpnSetting::Ptr vpnSetting =
                    connectionSettings->setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
            if (!vpnSetting) {
                qDebug() << "Missing VPN setting!";
            } else {
                const QString serviceType = vpnSetting->serviceType();
                //qDebug() << "Agent loading VPN plugin" << serviceType << "from DBUS" << calledFromDBus();
                //vpnSetting->printSetting();
                vpnPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QString::fromLatin1("PlasmaNM/VpnUiPlugin"),
                                                                                     QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(serviceType),
                                                                                     this, QVariantList(), &error);
                if (vpnPlugin && error.isEmpty()) {
                    const QString shortName = serviceType.section('.', -1);
                    KDialog * dlg = new KDialog;
                    dlg->setMainWidget(vpnPlugin->askUser(vpnSetting));
                    dlg->setButtons(KDialog::Ok | KDialog::Cancel);
                    dlg->setCaption(i18n("VPN secrets (%1)", shortName));

                    KWindowSystem::setState(dlg->winId(), NET::KeepAbove);
                    KWindowSystem::forceActiveWindow(dlg->winId());

                    if (dlg->exec() == KDialog::Accepted) {
                        result.insert(setting_name, static_cast<SettingWidget *>(dlg->mainWidget())->setting());
                        delete dlg;
                        return result;
                    }
                    delete dlg;
                } else {
                    qDebug() << error << ", serviceType == " << serviceType;
                }
            }
        } else {
            NetworkManager::Settings::WirelessSetting::Ptr wifi = connectionSettings->setting(NetworkManager::Settings::Setting::Wireless).dynamicCast<NetworkManager::Settings::WirelessSetting>();
            QString ssid;
            if (wifi)
                ssid = wifi->ssid();

            PasswordDialog * dlg = new PasswordDialog(setting, setting->needSecrets(requestNew), ssid);

            KWindowSystem::setState(dlg->winId(), NET::KeepAbove);
            KWindowSystem::forceActiveWindow(dlg->winId());

            if (dlg->exec() == KDialog::Accepted) {
                result.insert(setting_name, dlg->secrets());
                delete dlg;
                return result;
            }
            delete dlg;
        }
    } else if (isVpn && userRequested) { // just return what we have
        NMVariantMapMap result;
        NetworkManager::Settings::VpnSetting::Ptr vpnSetting =
                connectionSettings->setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
        result.insert("vpn", vpnSetting->secretsToMap());
        return result;
    }

    return connectionSettings->toMap();
}

void SecretAgent::SaveSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path)
{
    Q_UNUSED(connection_path)

    NetworkManager::Settings::ConnectionSettings::Ptr connectionSettings = NetworkManager::Settings::ConnectionSettings::Ptr(new NetworkManager::Settings::ConnectionSettings());
    connectionSettings->fromMap(connection);

    if (!KWallet::Wallet::isEnabled()) {
        kWarning() << "KWallet is disabled, please enable it or set your connection as system connection. Secrets not saved.";
        return;
    }

    KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous );

    if (!wallet) {
        kWarning() << "Error opening kwallet. Secrets not saved.";
        return;
    }

    if (wallet->isOpen()) {
        bool readyForWalletWrite = false;
        if (!wallet->hasFolder("plasma-nm"))
            wallet->createFolder("plasma-nm");
        if (wallet->setFolder("plasma-nm")) {
            readyForWalletWrite = true;
        }
        if (readyForWalletWrite) {
            bool saved = false;
            foreach (const QString & entry, wallet->entryList()) {
                if (entry.startsWith(connectionSettings->uuid() + ';')) {
                    kDebug() << "Removing entry " << entry << ")";
                    wallet->removeEntry(entry);
                }
            }

            foreach (const NetworkManager::Settings::Setting::Ptr & setting, connectionSettings->settings()) {
                QMap<QString, QString> map;
                qDebug() << setting->secretsToMap().keys();
                foreach (const QString & key, setting->secretsToMap().keys()) {
                    map.insert(key, setting->secretsToMap().value(key).toString());
                }
                if (!map.isEmpty()) {
                    saved = true;
                    QString entryName = connectionSettings->uuid() + ";" + setting->name();
                    wallet->writeMap(entryName, map);
                    kDebug() << "Writing entry " << entryName;
                }
            }

            if (!saved) {
                kWarning() << "No secret has been written to the kwallet.";
            }
        }
    }

    wallet->deleteLater();
}

void SecretAgent::DeleteSecrets(const NMVariantMapMap &connection, const QDBusObjectPath &connection_path)
{
    if (!KWallet::Wallet::isEnabled()) {
        kWarning() << "KWallet is disabled, please enable it. Secrets not deleted.";
        return;
    }

    KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);

    if (!wallet) {
        kWarning() << "Error opening kwallet. Secrets not deleted.";
        return;
    }

    if( wallet->isOpen() && wallet->hasFolder("plasma-nm") && wallet->setFolder("plasma-nm")) {
        NetworkManager::Settings::ConnectionSettings connectionSettings;
        connectionSettings.fromMap(connection);

        foreach (const QString & entry, wallet->entryList()) {
            if (entry.startsWith(connectionSettings.uuid() + ';'))
                wallet->removeEntry(entry);
        }
    }

    wallet->deleteLater();
}

void SecretAgent::CancelGetSecrets(const QDBusObjectPath &connection_path, const QString &setting_name)
{

}
