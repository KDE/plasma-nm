/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fortisslvpnauth.h"

#include "nm-fortisslvpn-service.h"

FortisslvpnAuthSetting::FortisslvpnAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
}

FortisslvpnAuthSetting::~FortisslvpnAuthSetting() = default;

void FortisslvpnAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap data = setting->data();

    m_rawOtpFlags = data.value(QLatin1String(NM_FORTISSLVPN_KEY_OTP "-flags"));
    m_rawTwoFactorAuthFlags = data.value(QLatin1String(NM_FORTISSLVPN_KEY_2FA "-flags"));

    const auto passwordFlags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD "-flags")).toInt());
    setPasswordRequired(passwordFlags == NetworkManager::Setting::NotSaved);

    const auto otpFlags = static_cast<NetworkManager::Setting::SecretFlags>(m_rawOtpFlags.toInt());
    setOtpRequired(otpFlags == NetworkManager::Setting::NotSaved);

    if (m_hints.count() == 2) {
        setOtpLabel(m_hints.at(0));
        setOtpHeadline(m_hints.at(1).section(QLatin1Char(':'), -2));
        setOtpRequired(true);
        setPasswordRequired(false);
    }
}

QVariantMap FortisslvpnAuthSetting::setting() const
{
    NMStringMap secrets;

    if (!m_password.isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_PASSWORD), m_password);
    }

    if (!m_rawOtpFlags.isEmpty()) {
        const auto otpFlags = static_cast<NetworkManager::Setting::SecretFlags>(m_rawOtpFlags.toInt());
        if (otpFlags == NetworkManager::Setting::NotSaved && !m_otp.isEmpty()) {
            secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_OTP), m_otp);
        }
    }

    // 2FA reuses the same field, only under a different secret name.
    if (!m_rawTwoFactorAuthFlags.isEmpty()) {
        secrets.insert(QLatin1String(NM_FORTISSLVPN_KEY_2FA), m_otp);
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

QString FortisslvpnAuthSetting::password() const
{
    return m_password;
}

void FortisslvpnAuthSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
}

QString FortisslvpnAuthSetting::otp() const
{
    return m_otp;
}

void FortisslvpnAuthSetting::setOtp(const QString &otp)
{
    if (m_otp == otp) {
        return;
    }
    m_otp = otp;
    Q_EMIT otpChanged();
}

bool FortisslvpnAuthSetting::passwordRequired() const
{
    return m_passwordRequired;
}

void FortisslvpnAuthSetting::setPasswordRequired(bool required)
{
    if (m_passwordRequired == required) {
        return;
    }
    m_passwordRequired = required;
    Q_EMIT passwordRequiredChanged();
}

bool FortisslvpnAuthSetting::otpRequired() const
{
    return m_otpRequired;
}

void FortisslvpnAuthSetting::setOtpRequired(bool required)
{
    if (m_otpRequired == required) {
        return;
    }
    m_otpRequired = required;
    Q_EMIT otpRequiredChanged();
}

QString FortisslvpnAuthSetting::otpHeadline() const
{
    return m_otpHeadline;
}

void FortisslvpnAuthSetting::setOtpHeadline(const QString &headline)
{
    if (m_otpHeadline == headline) {
        return;
    }
    m_otpHeadline = headline;
    Q_EMIT otpHeadlineChanged();
}

QString FortisslvpnAuthSetting::otpLabel() const
{
    return m_otpLabel;
}

void FortisslvpnAuthSetting::setOtpLabel(const QString &label)
{
    if (m_otpLabel == label) {
        return;
    }
    m_otpLabel = label;
    Q_EMIT otpLabelChanged();
}

#include "moc_fortisslvpnauth.cpp"
