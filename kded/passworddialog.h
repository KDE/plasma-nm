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

#ifndef PLASMA_NM_PASSWOR_DDIALOG_H
#define PLASMA_NM_PASSWORD_DIALOG_H

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/SecretAgent>

#include <KDialog>

#include <kdemacros.h>

namespace Ui {
class PasswordDialog;
}
class SettingWidget;
class KDE_EXPORT PasswordDialog : public KDialog
{
    Q_OBJECT
public:
    explicit PasswordDialog(const NMVariantMapMap &connection,
                            NetworkManager::SecretAgent::GetSecretsFlags flags,
                            const QString &setting_name,
                            QWidget *parent = 0);
    ~PasswordDialog();
    void setupGenericUi(const NetworkManager::ConnectionSettings &connectionSettings);
    void setupVpnUi(const NetworkManager::ConnectionSettings &connectionSettings);

    bool hasError() const;
    NetworkManager::SecretAgent::Error error() const;
    QString errorMessage() const;

    NMVariantMapMap secrets() const;

private Q_SLOTS:
    void showPassword(bool show);

private:
    Ui::PasswordDialog *ui;
    SettingWidget *vpnWidget;
    NMVariantMapMap m_connection;
    NetworkManager::SecretAgent::GetSecretsFlags m_flags;
    QString m_settingName;
    QStringList m_neededSecrets;
    bool m_hasError;
    NetworkManager::SecretAgent::Error m_error;
    QString m_errorMessage;
};

#endif // PLASMA_NM_PASSWORD_DIALOG_H
