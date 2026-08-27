/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_SSH_AUTH_QML_H
#define PLASMA_NM_SSH_AUTH_QML_H

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class SshAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit SshAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~SshAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString password() const;
    void setPassword(const QString &password);

Q_SIGNALS:
    void passwordChanged();

private:
    QStringList m_hints;
    QString m_password;
};

#endif // PLASMA_NM_SSH_AUTH_QML_H
