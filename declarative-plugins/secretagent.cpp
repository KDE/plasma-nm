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

#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/settings/802-11-wireless.h>
#include <QtNetworkManager/generic-types.h>

#include <KWindowSystem>

SecretAgent::SecretAgent(QObject* parent):
    NetworkManager::SecretAgent("org.kde.plasma-nm", parent)
{
}

SecretAgent::~SecretAgent()
{
}

QVariantMapMap SecretAgent::GetSecrets(const QVariantMapMap &connection, const QDBusObjectPath &connection_path, const QString &setting_name,
                                       const QStringList &hints, uint flags)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "Path:" << connection_path.path();
    qDebug() << "Setting name:" << setting_name;
    qDebug() << "Hints:" << hints;
    qDebug() << "Flags:" << flags;

#if 0
    QStringMap secrets = qdbus_cast<QStringMap>(connection.value("vpn").value("secrets"));
    qDebug() << "SECRETS: " << secrets;
    foreach (const QString & secret, secrets.keys()) {
        qDebug() << "Needed secret:" << secret;
    }
#endif

    NetworkManager::Settings::ConnectionSettings * settings = new NetworkManager::Settings::ConnectionSettings();
    settings->fromMap(connection);

    NetworkManager::Settings::Setting * setting = settings->setting(NetworkManager::Settings::Setting::typeFromString(setting_name));

    NetworkManager::SecretAgent::GetSecretsFlags secretsFlags(static_cast<int>(flags));
    const bool requestNew = secretsFlags.testFlag(RequestNew);
    const bool userRequested = secretsFlags.testFlag(UserRequested);

    if (requestNew || userRequested || (secretsFlags.testFlag(AllowInteraction) && !setting->needSecrets(requestNew).isEmpty())) {
        NetworkManager::Settings::WirelessSetting * wifi = static_cast<NetworkManager::Settings::WirelessSetting *>(settings->setting(NetworkManager::Settings::Setting::Wireless));
        QString ssid;
        if (wifi)
            ssid = wifi->ssid();

        PasswordDialog * dlg = new PasswordDialog(setting, setting->needSecrets(requestNew), ssid);
        QVariantMapMap result;
        dlg->setWindowModality(Qt::WindowModal);
        KWindowSystem::setMainWindow(dlg, 0);
        KWindowSystem::activateWindow(dlg->winId());
        if (dlg->exec() == QDialog::Accepted) {
            result.insert(setting_name, dlg->secrets());
            delete dlg;
            return result;
        }
    }

    return QVariantMapMap();
}

void SecretAgent::SaveSecrets(const QVariantMapMap &connection, const QDBusObjectPath &connection_path)
{

}

void SecretAgent::DeleteSecrets(const QVariantMapMap &connection, const QDBusObjectPath &connection_path)
{

}

void SecretAgent::CancelGetSecrets(const QDBusObjectPath &connection_path, const QString &setting_name)
{

}
