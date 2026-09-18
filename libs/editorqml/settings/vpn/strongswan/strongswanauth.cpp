/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "strongswanauth.h"

#include "nm-strongswan-service.h"

#include "plasma_nm_editorqml.h"

StrongswanAuthSetting::StrongswanAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

StrongswanAuthSetting::~StrongswanAuthSetting() = default;

void StrongswanAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap data = setting->data();
    const QString method = data.value(QLatin1String(NM_STRONGSWAN_METHOD));

    m_usesAgent = method == QLatin1String(NM_STRONGSWAN_AUTH_AGENT);

    const bool unused = data.value(QLatin1String(NM_STRONGSWAN_SECRET_TYPE)) == QLatin1String(NM_STRONGSWAN_PW_TYPE_UNUSED);
    setPasswordRequired(!m_usesAgent && !unused);

    if (method == QLatin1String(NM_STRONGSWAN_AUTH_KEY)) {
        setSecretKind(PrivateKeyPassword);
    } else if (method == QLatin1String(NM_STRONGSWAN_AUTH_SMARTCARD)) {
        setSecretKind(Pin);
    } else {
        setSecretKind(Password);
    }
}

QVariantMap StrongswanAuthSetting::setting() const
{
    NMStringMap secrets;

    if (m_usesAgent) {
        const QString agent = qEnvironmentVariable("SSH_AUTH_SOCK");
        if (!agent.isEmpty()) {
            secrets.insert(QLatin1String(NM_STRONGSWAN_AUTH_AGENT), agent);
        } else {
            qCWarning(PLASMA_NM_EDITORQML_LOG) << "Connection uses ssh-agent for authentication, but no ssh-agent is running";
        }
    } else {
        secrets.insert(QLatin1String(NM_STRONGSWAN_SECRET), m_password);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString StrongswanAuthSetting::password() const
{
    return m_password;
}

void StrongswanAuthSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
}

bool StrongswanAuthSetting::passwordRequired() const
{
    return m_passwordRequired;
}

void StrongswanAuthSetting::setPasswordRequired(bool required)
{
    if (m_passwordRequired == required) {
        return;
    }
    m_passwordRequired = required;
    Q_EMIT passwordRequiredChanged();
}

StrongswanAuthSetting::SecretKind StrongswanAuthSetting::secretKind() const
{
    return m_secretKind;
}

void StrongswanAuthSetting::setSecretKind(SecretKind kind)
{
    if (m_secretKind == kind) {
        return;
    }
    m_secretKind = kind;
    Q_EMIT secretKindChanged();
}

#include "moc_strongswanauth.cpp"
