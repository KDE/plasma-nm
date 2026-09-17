/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "libreswanauth.h"

#include "nm-libreswan-service.h"

LibreswanAuthSetting::LibreswanAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

LibreswanAuthSetting::~LibreswanAuthSetting() = default;

void LibreswanAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap data = setting->data();
    const NMStringMap secrets = setting->secrets();

    setGroupName(data.value(QLatin1String(NM_LIBRESWAN_LEFTID)));

    const QLatin1String unused(NM_LIBRESWAN_PW_TYPE_UNUSED);

    setUserPasswordRequired(data.value(QLatin1String(NM_LIBRESWAN_XAUTH_PASSWORD_INPUT_MODES)) != unused);
    if (userPasswordRequired()) {
        setUserPassword(secrets.value(QLatin1String(NM_LIBRESWAN_XAUTH_PASSWORD)));
    }

    setGroupPasswordRequired(data.value(QLatin1String(NM_LIBRESWAN_PSK_INPUT_MODES)) != unused);
    if (groupPasswordRequired()) {
        setGroupPassword(secrets.value(QLatin1String(NM_LIBRESWAN_PSK_VALUE)));
    }
}

QVariantMap LibreswanAuthSetting::setting() const
{
    NMStringMap secrets;

    if (!m_userPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_LIBRESWAN_XAUTH_PASSWORD), m_userPassword);
    }

    if (!m_groupPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_LIBRESWAN_PSK_VALUE), m_groupPassword);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString LibreswanAuthSetting::groupName() const
{
    return m_groupName;
}

void LibreswanAuthSetting::setGroupName(const QString &groupName)
{
    if (m_groupName == groupName) {
        return;
    }
    m_groupName = groupName;
    Q_EMIT groupNameChanged();
}

QString LibreswanAuthSetting::userPassword() const
{
    return m_userPassword;
}

void LibreswanAuthSetting::setUserPassword(const QString &password)
{
    if (m_userPassword == password) {
        return;
    }
    m_userPassword = password;
    Q_EMIT userPasswordChanged();
}

QString LibreswanAuthSetting::groupPassword() const
{
    return m_groupPassword;
}

void LibreswanAuthSetting::setGroupPassword(const QString &password)
{
    if (m_groupPassword == password) {
        return;
    }
    m_groupPassword = password;
    Q_EMIT groupPasswordChanged();
}

bool LibreswanAuthSetting::userPasswordRequired() const
{
    return m_userPasswordRequired;
}

void LibreswanAuthSetting::setUserPasswordRequired(bool required)
{
    if (m_userPasswordRequired == required) {
        return;
    }
    m_userPasswordRequired = required;
    Q_EMIT userPasswordRequiredChanged();
}

bool LibreswanAuthSetting::groupPasswordRequired() const
{
    return m_groupPasswordRequired;
}

void LibreswanAuthSetting::setGroupPasswordRequired(bool required)
{
    if (m_groupPasswordRequired == required) {
        return;
    }
    m_groupPasswordRequired = required;
    Q_EMIT groupPasswordRequiredChanged();
}

#include "moc_libreswanauth.cpp"
