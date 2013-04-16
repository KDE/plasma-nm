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

#include <QtNetworkManager/settings.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/generic-types.h>
#include <QtNetworkManager/settings/vpn.h>

#include <QStringBuilder>

#include <KPluginFactory>
#include <KWindowSystem>
#include <KDialog>
#include <KWallet/Wallet>

#include <KDebug>

SecretAgent::SecretAgent(QObject* parent):
    NetworkManager::SecretAgent("org.kde.plasma-nm", parent)
{
    connect(NetworkManager::notifier(), SIGNAL(serviceDisappeared()),
            this, SLOT(killDialogs()));
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

    QString callId = connection_path.path() % setting_name;
    foreach (const GetSecretsRequest request, m_calls) {
        if (request == callId) {
            kWarning() << "GetSecrets was called again! This should not happen, cancelling first call" << connection_path.path() << setting_name;
            CancelGetSecrets(connection_path, setting_name);
            break;
        }
    }

    setDelayedReply(true);
    GetSecretsRequest request;
    request.callId = callId;
    request.connection = connection;
    request.connection_path = connection_path;
    request.flags = static_cast<NetworkManager::SecretAgent::GetSecretsFlags>(flags);
    request.hints = hints;
    request.setting_name = setting_name;
    request.message = message();
    request.dialog = 0;
    request.wallet = 0;
    m_calls << request;

    proccessNext();

    return NMVariantMapMap();
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
    Q_UNUSED(connection_path)
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
    kDebug() << connection_path.path() << setting_name;
    QString callId = connection_path.path() % setting_name;
    for (int i = 0; i < m_calls.size(); ++i) {
        GetSecretsRequest request = m_calls.at(i);
        if (callId == request.callId) {
            delete request.dialog;
            delete request.wallet;
            sendError(SecretAgent::AgentCanceled,
                      QLatin1String("Agent canceled the password dialog"),
                      request.message);
            m_calls.removeAt(i);
            break;
        }
    }

    proccessNext();
}

void SecretAgent::dialogAccepted()
{
    GetSecretsRequest request = m_calls.takeFirst();

    sendSecrets(request.dialog->secrets(), request.message);

    sender()->deleteLater();

    proccessNext();
}

void SecretAgent::dialogRejected()
{
    GetSecretsRequest request = m_calls.takeFirst();
    sendError(SecretAgent::UserCanceled,
              QLatin1String("User canceled the password dialog"),
              request.message);

    sender()->deleteLater();

    proccessNext();
}

void SecretAgent::killDialogs()
{
    foreach (const GetSecretsRequest &request, m_calls) {
        delete request.dialog;
        delete request.wallet;
    }
    m_calls.clear();
}

void SecretAgent::proccessNext()
{
    if (m_calls.isEmpty() || m_calls.first().dialog || m_calls.first().wallet) {
        return;
    }

    GetSecretsRequest &request = m_calls.first();

    NetworkManager::Settings::ConnectionSettings connectionSettings(request.connection);

    NetworkManager::Settings::Setting::Ptr setting = connectionSettings.setting(request.setting_name);

    if (!KWallet::Wallet::isEnabled()) {
        kWarning() << "KWallet is disabled, please enable it. Secrets not loaded.";
    } else {
        KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);

        if (wallet) {
            if (wallet->isOpen() && wallet->hasFolder("plasma-nm") && wallet->setFolder("plasma-nm")) {
                QString key = connectionSettings.uuid() % QLatin1Char(';') % request.setting_name;
                QMap<QString,QString> map;
                wallet->readMap(key, map);
                QVariantMap secretsMap;
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

    const bool requestNew = request.flags & RequestNew;
    const bool userRequested = request.flags & UserRequested;
    const bool allowInteraction = request.flags & AllowInteraction;
    const bool isVpn = (setting->type() == NetworkManager::Settings::Setting::Vpn);

    if (requestNew || (allowInteraction && !setting->needSecrets(requestNew).isEmpty()) || (allowInteraction && userRequested) || (isVpn && allowInteraction)) {
        PasswordDialog *dialog = new PasswordDialog(request.flags, request.setting_name);
        connect(dialog, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
        connect(dialog, SIGNAL(rejected()), this, SLOT(dialogRejected()));
        if (isVpn) {
            dialog->setupVpnUi(connectionSettings);
        } else {
            dialog->setupGenericUi(connectionSettings);
        }

        if (dialog->hasError()) {
            sendError(dialog->error(),
                      dialog->errorMessage(),
                      request.message);
            m_calls.removeFirst();
            proccessNext();
        } else {
            dialog->show();
            KWindowSystem::setState(dialog->winId(), NET::KeepAbove);
            KWindowSystem::forceActiveWindow(dialog->winId());
            request.dialog = dialog;
        }
    } else if (isVpn && userRequested) { // just return what we have
        NMVariantMapMap result;
        NetworkManager::Settings::VpnSetting::Ptr vpnSetting;
        vpnSetting = connectionSettings.setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
        result.insert("vpn", vpnSetting->secretsToMap());

        sendSecrets(result, request.message);
    } else if (setting->needSecrets().isEmpty()) {
        NMVariantMapMap result;
        result.insert(setting->name(), setting->secretsToMap());
        sendSecrets(result, request.message);
    } else {
        sendError(SecretAgent::InternalError,
                  QLatin1String("Plasma-nm did not know how to handle the request"),
                  request.message);
        m_calls.removeFirst();
        proccessNext();
    }
}

void SecretAgent::sendSecrets(const NMVariantMapMap &secrets, const QDBusMessage &message)
{
    QDBusMessage reply;
    reply = message.createReply(qVariantFromValue(secrets));
    if (!QDBusConnection::systemBus().send(reply)) {
        kWarning() << "Failed put the secret into the queue";
    }
}
