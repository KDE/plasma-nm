/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "l2tpauth.h"

#include "nm-l2tp-service.h"

namespace
{
bool isRequired(const NMStringMap &data, const char *flagsKey)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(flagsKey)).toInt());
    return !flags.testFlag(NetworkManager::Setting::NotRequired);
}
}

L2tpAuthSetting::L2tpAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

L2tpAuthSetting::~L2tpAuthSetting() = default;

void L2tpAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap data = setting->data();
    const NMStringMap secrets = setting->secrets();

    const QString userAuthType = data.value(QLatin1String(NM_L2TP_KEY_USER_AUTH_TYPE));
    const QString machineAuthType = data.value(QLatin1String(NM_L2TP_KEY_MACHINE_AUTH_TYPE));

    if (userAuthType.isEmpty() || userAuthType == QLatin1String(NM_L2TP_AUTHTYPE_PASSWORD)) {
        setUserPasswordRequired(isRequired(data, NM_L2TP_KEY_PASSWORD "-flags"));
        if (userPasswordRequired()) {
            setUserPassword(secrets.value(QLatin1String(NM_L2TP_KEY_PASSWORD)));
        }
    } else if (userAuthType == QLatin1String(NM_L2TP_AUTHTYPE_TLS)) {
        setUserCertPasswordRequired(isRequired(data, NM_L2TP_KEY_USER_CERTPASS "-flags"));
        if (userCertPasswordRequired()) {
            setUserCertPassword(secrets.value(QLatin1String(NM_L2TP_KEY_USER_CERTPASS)));
        }
    }

    if (machineAuthType == QLatin1String(NM_L2TP_AUTHTYPE_TLS)) {
        setMachineCertPasswordRequired(isRequired(data, NM_L2TP_KEY_MACHINE_CERTPASS "-flags"));
        if (machineCertPasswordRequired()) {
            setMachineCertPassword(secrets.value(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS)));
        }
    } else if (machineAuthType == QLatin1String(NM_L2TP_AUTHTYPE_PSK)) {
        setPresharedKeyRequired(isRequired(data, NM_L2TP_KEY_IPSEC_PSK "-flags"));
        if (presharedKeyRequired()) {
            setPresharedKey(secrets.value(QLatin1String(NM_L2TP_KEY_IPSEC_PSK)));
        }
    }
}

QVariantMap L2tpAuthSetting::setting() const
{
    NMStringMap secrets;

    if (m_userPasswordRequired && !m_userPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_L2TP_KEY_PASSWORD), m_userPassword);
    }

    if (m_userCertPasswordRequired && !m_userCertPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_L2TP_KEY_USER_CERTPASS), m_userCertPassword);
    }

    if (m_machineCertPasswordRequired && !m_machineCertPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_L2TP_KEY_MACHINE_CERTPASS), m_machineCertPassword);
    }

    if (m_presharedKeyRequired && !m_presharedKey.isEmpty()) {
        secrets.insert(QLatin1String(NM_L2TP_KEY_IPSEC_PSK), m_presharedKey);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString L2tpAuthSetting::userPassword() const
{
    return m_userPassword;
}

void L2tpAuthSetting::setUserPassword(const QString &password)
{
    if (m_userPassword == password) {
        return;
    }
    m_userPassword = password;
    Q_EMIT userPasswordChanged();
}

bool L2tpAuthSetting::userPasswordRequired() const
{
    return m_userPasswordRequired;
}

void L2tpAuthSetting::setUserPasswordRequired(bool required)
{
    if (m_userPasswordRequired == required) {
        return;
    }
    m_userPasswordRequired = required;
    Q_EMIT userPasswordRequiredChanged();
}

QString L2tpAuthSetting::userCertPassword() const
{
    return m_userCertPassword;
}

void L2tpAuthSetting::setUserCertPassword(const QString &password)
{
    if (m_userCertPassword == password) {
        return;
    }
    m_userCertPassword = password;
    Q_EMIT userCertPasswordChanged();
}

bool L2tpAuthSetting::userCertPasswordRequired() const
{
    return m_userCertPasswordRequired;
}

void L2tpAuthSetting::setUserCertPasswordRequired(bool required)
{
    if (m_userCertPasswordRequired == required) {
        return;
    }
    m_userCertPasswordRequired = required;
    Q_EMIT userCertPasswordRequiredChanged();
}

QString L2tpAuthSetting::machineCertPassword() const
{
    return m_machineCertPassword;
}

void L2tpAuthSetting::setMachineCertPassword(const QString &password)
{
    if (m_machineCertPassword == password) {
        return;
    }
    m_machineCertPassword = password;
    Q_EMIT machineCertPasswordChanged();
}

bool L2tpAuthSetting::machineCertPasswordRequired() const
{
    return m_machineCertPasswordRequired;
}

void L2tpAuthSetting::setMachineCertPasswordRequired(bool required)
{
    if (m_machineCertPasswordRequired == required) {
        return;
    }
    m_machineCertPasswordRequired = required;
    Q_EMIT machineCertPasswordRequiredChanged();
}

QString L2tpAuthSetting::presharedKey() const
{
    return m_presharedKey;
}

void L2tpAuthSetting::setPresharedKey(const QString &key)
{
    if (m_presharedKey == key) {
        return;
    }
    m_presharedKey = key;
    Q_EMIT presharedKeyChanged();
}

bool L2tpAuthSetting::presharedKeyRequired() const
{
    return m_presharedKeyRequired;
}

void L2tpAuthSetting::setPresharedKeyRequired(bool required)
{
    if (m_presharedKeyRequired == required) {
        return;
    }
    m_presharedKeyRequired = required;
    Q_EMIT presharedKeyRequiredChanged();
}

#include "moc_l2tpauth.cpp"
