/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "iodineauth.h"

#include "nm-iodine-service.h"

IodineAuthSetting::IodineAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

IodineAuthSetting::~IodineAuthSetting() = default;

void IodineAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const QString password = setting->secrets().value(QLatin1String(NM_IODINE_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setPassword(password);
    }
}

QVariantMap IodineAuthSetting::setting() const
{
    NMStringMap secrets;

    if (!m_password.isEmpty()) {
        secrets.insert(QLatin1String(NM_IODINE_KEY_PASSWORD), m_password);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString IodineAuthSetting::password() const
{
    return m_password;
}

void IodineAuthSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
}

#include "moc_iodineauth.cpp"
