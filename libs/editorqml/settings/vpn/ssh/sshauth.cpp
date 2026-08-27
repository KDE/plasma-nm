/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "sshauth.h"

#include "nm-ssh-service.h"

SshAuthSetting::SshAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

SshAuthSetting::~SshAuthSetting() = default;

void SshAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setPassword(setting->secrets().value(QLatin1String(NM_SSH_KEY_PASSWORD)));
}

QVariantMap SshAuthSetting::setting() const
{
    NMStringMap secrets;

    if (!m_password.isEmpty()) {
        secrets.insert(QLatin1String(NM_SSH_KEY_PASSWORD), m_password);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString SshAuthSetting::password() const
{
    return m_password;
}

void SshAuthSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
}

#include "moc_sshauth.cpp"
