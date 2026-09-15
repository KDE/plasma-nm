/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "vpncauth.h"

#include "nm-vpnc-service.h"

VpncAuthSetting::VpncAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

VpncAuthSetting::~VpncAuthSetting() = default;

void VpncAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap data = setting->data();
    const NMStringMap secrets = setting->secrets();

    setUser(data.value(QLatin1String(NM_VPNC_KEY_XAUTH_USER)));
    setGroup(data.value(QLatin1String(NM_VPNC_KEY_ID)));

    const auto userFlags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD "-flags")).toInt());
    setUserPasswordRequired(!userFlags.testFlag(NetworkManager::Setting::NotRequired));
    if (userPasswordRequired()) {
        setUserPassword(secrets.value(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD)));
    }

    const auto groupFlags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_VPNC_KEY_SECRET "-flags")).toInt());
    setGroupPasswordRequired(!groupFlags.testFlag(NetworkManager::Setting::NotRequired));
    if (groupPasswordRequired()) {
        setGroupPassword(secrets.value(QLatin1String(NM_VPNC_KEY_SECRET)));
    }
}

QVariantMap VpncAuthSetting::setting() const
{
    NMStringMap secrets;

    if (!m_userPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD), m_userPassword);
    }

    if (!m_groupPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_VPNC_KEY_SECRET), m_groupPassword);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString VpncAuthSetting::user() const
{
    return m_user;
}

void VpncAuthSetting::setUser(const QString &user)
{
    if (m_user == user) {
        return;
    }
    m_user = user;
    Q_EMIT userChanged();
}

QString VpncAuthSetting::group() const
{
    return m_group;
}

void VpncAuthSetting::setGroup(const QString &group)
{
    if (m_group == group) {
        return;
    }
    m_group = group;
    Q_EMIT groupChanged();
}

QString VpncAuthSetting::userPassword() const
{
    return m_userPassword;
}

void VpncAuthSetting::setUserPassword(const QString &password)
{
    if (m_userPassword == password) {
        return;
    }
    m_userPassword = password;
    Q_EMIT userPasswordChanged();
}

QString VpncAuthSetting::groupPassword() const
{
    return m_groupPassword;
}

void VpncAuthSetting::setGroupPassword(const QString &password)
{
    if (m_groupPassword == password) {
        return;
    }
    m_groupPassword = password;
    Q_EMIT groupPasswordChanged();
}

bool VpncAuthSetting::userPasswordRequired() const
{
    return m_userPasswordRequired;
}

void VpncAuthSetting::setUserPasswordRequired(bool required)
{
    if (m_userPasswordRequired == required) {
        return;
    }
    m_userPasswordRequired = required;
    Q_EMIT userPasswordRequiredChanged();
}

bool VpncAuthSetting::groupPasswordRequired() const
{
    return m_groupPasswordRequired;
}

void VpncAuthSetting::setGroupPasswordRequired(bool required)
{
    if (m_groupPasswordRequired == required) {
        return;
    }
    m_groupPasswordRequired = required;
    Q_EMIT groupPasswordRequiredChanged();
}

#include "moc_vpncauth.cpp"
