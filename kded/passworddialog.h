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

#ifndef PLASMA_NM_PASSWORD_DIALOG_H
#define PLASMA_NM_PASSWORD_DIALOG_H

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/SecretAgent>

#include <QDialog>

namespace Ui {
class PasswordDialog;
}

class SettingWidget;

class Q_DECL_EXPORT PasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PasswordDialog(const NetworkManager::ConnectionSettings::Ptr &connectionSettings,
                            NetworkManager::SecretAgent::GetSecretsFlags flags,
                            const QString &setting_name, const QStringList &hints = QStringList(),
                            QWidget *parent = nullptr);
    ~PasswordDialog() override;

    bool hasError() const;
    NetworkManager::SecretAgent::Error error() const;
    QString errorMessage() const;

    NMVariantMapMap secrets() const;

private:
    void initializeUi();

    Ui::PasswordDialog *m_ui;
    bool m_hasError;
    QString m_errorMessage;
    QString m_settingName;
    QStringList m_neededSecrets;
    NetworkManager::ConnectionSettings::Ptr m_connectionSettings;
    NetworkManager::SecretAgent::Error m_error;
    NetworkManager::SecretAgent::GetSecretsFlags m_flags;
    SettingWidget *m_vpnWidget;
    QStringList m_hints;

};

#endif // PLASMA_NM_PASSWORD_DIALOG_H
