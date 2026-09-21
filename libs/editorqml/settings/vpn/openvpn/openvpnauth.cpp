/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "openvpnauth.h"

#include "nm-openvpn-service.h"

#include <KLocalizedString>

#include <optional>

OpenvpnAuthSetting::OpenvpnAuthSetting(const QStringList &hints, QObject *parent)
    : QObject(parent)
    , m_hints(hints)
{
    readHints();
}

OpenvpnAuthSetting::~OpenvpnAuthSetting() = default;

void OpenvpnAuthSetting::readHints()
{
    if (m_hints.isEmpty()) {
        return;
    }

    const QString vpnMessage = QStringLiteral("x-vpn-message:");
    const QString tagDynChallNoEcho = QStringLiteral("x-dynamic-challenge:");
    const QString tagDynChallEcho = QStringLiteral("x-dynamic-challenge-echo:");

    std::optional<bool> masked;
    QString prompt;

    for (const QString &hint : std::as_const(m_hints)) {
        if (hint.startsWith(vpnMessage)) {
            prompt = hint.sliced(vpnMessage.length());
        } else {
            if (hint.startsWith(tagDynChallNoEcho)) {
                masked = true;
            } else if (hint.startsWith(tagDynChallEcho)) {
                masked = false;
            }
            m_challengeSecretKey = hint;
        }
    }

    if (prompt.isEmpty()) {
        if (m_challengeSecretKey == QLatin1String(NM_OPENVPN_KEY_CERTPASS)) {
            prompt = i18n("Key Password:");
        } else if (m_challengeSecretKey == QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD)) {
            prompt = i18n("Proxy Password:");
        } else {
            prompt = i18n("Password:");
        }
    } else if (prompt.endsWith(QLatin1Char('.'))) {
        prompt = prompt.replace(prompt.length() - 1, 1, QLatin1Char(':'));
    } else if (!prompt.endsWith(QLatin1Char(':'))) {
        prompt += QLatin1Char(':');
    }

    if (!masked.has_value()) {
        const QStringList tokens = {i18n("OTP"), i18n("authenticator"), i18n("code"), i18n("token"), i18n("one-time password")};

        for (const QString &token : tokens) {
            if (prompt.toLower().contains(token.toLower())) {
                masked = false;
                break;
            }
        }
    }

    m_usesChallenge = true;
    setChallengeLabel(prompt);
    setChallengeMasked(masked.value_or(true));
    Q_EMIT usesChallengeChanged();
}

void OpenvpnAuthSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    if (m_usesChallenge) {
        return;
    }

    const NMStringMap data = setting->data();
    const NMStringMap secrets = setting->secrets();

    const QString connectionType = data.value(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE));

    const auto certFlags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_OPENVPN_KEY_CERTPASS_FLAGS)).toInt());
    const auto passFlags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD_FLAGS)).toInt());
    const auto proxyFlags = static_cast<NetworkManager::Setting::SecretFlags>(data.value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD_FLAGS)).toInt());

    if (connectionType == QLatin1String(NM_OPENVPN_CONTYPE_TLS) || connectionType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS)) {
        if (connectionType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD_TLS) && !passFlags.testFlag(NetworkManager::Setting::NotRequired)) {
            setPasswordRequired(true);
            setPassword(secrets.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD)));
        }

        // Only an encrypted private key needs a password.
        if (data.contains(QLatin1String(NM_OPENVPN_KEY_KEY)) && !certFlags.testFlag(NetworkManager::Setting::NotRequired)) {
            setPrivateKeyPasswordRequired(true);
            setPrivateKeyPassword(secrets.value(QLatin1String(NM_OPENVPN_KEY_CERTPASS)));
        }
    } else if (connectionType == QLatin1String(NM_OPENVPN_CONTYPE_PASSWORD)) {
        setPasswordRequired(true);
        setPassword(secrets.value(QLatin1String(NM_OPENVPN_KEY_PASSWORD)));
    }

    if (data.contains(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER)) && !proxyFlags.testFlag(NetworkManager::Setting::NotRequired)) {
        setProxyPasswordRequired(true);
        setProxyPassword(secrets.value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD)));
    }
}

QVariantMap OpenvpnAuthSetting::setting() const
{
    NMStringMap secrets;

    if (m_usesChallenge) {
        if (!m_challengeSecretKey.isEmpty() && !m_challengePassword.isEmpty()) {
            secrets.insert(m_challengeSecretKey, m_challengePassword);
        }
    } else {
        if (m_passwordRequired && !m_password.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD), m_password);
        }
        if (m_privateKeyPasswordRequired && !m_privateKeyPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS), m_privateKeyPassword);
        }
        if (m_proxyPasswordRequired && !m_proxyPassword.isEmpty()) {
            secrets.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD), m_proxyPassword);
        }
    }

    QVariantMap secretData;
    secretData.insert(QLatin1String("secrets"), QVariant::fromValue<NMStringMap>(secrets));

    return secretData;
}

bool OpenvpnAuthSetting::usesChallenge() const
{
    return m_usesChallenge;
}

QString OpenvpnAuthSetting::challengeLabel() const
{
    return m_challengeLabel;
}

void OpenvpnAuthSetting::setChallengeLabel(const QString &label)
{
    if (m_challengeLabel == label) {
        return;
    }
    m_challengeLabel = label;
    Q_EMIT challengeLabelChanged();
}

bool OpenvpnAuthSetting::challengeMasked() const
{
    return m_challengeMasked;
}

void OpenvpnAuthSetting::setChallengeMasked(bool masked)
{
    if (m_challengeMasked == masked) {
        return;
    }
    m_challengeMasked = masked;
    Q_EMIT challengeMaskedChanged();
}

QString OpenvpnAuthSetting::challengePassword() const
{
    return m_challengePassword;
}

void OpenvpnAuthSetting::setChallengePassword(const QString &password)
{
    if (m_challengePassword == password) {
        return;
    }
    m_challengePassword = password;
    Q_EMIT challengePasswordChanged();
}

QString OpenvpnAuthSetting::password() const
{
    return m_password;
}

void OpenvpnAuthSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
}

bool OpenvpnAuthSetting::passwordRequired() const
{
    return m_passwordRequired;
}

void OpenvpnAuthSetting::setPasswordRequired(bool required)
{
    if (m_passwordRequired == required) {
        return;
    }
    m_passwordRequired = required;
    Q_EMIT passwordRequiredChanged();
}

QString OpenvpnAuthSetting::privateKeyPassword() const
{
    return m_privateKeyPassword;
}

void OpenvpnAuthSetting::setPrivateKeyPassword(const QString &password)
{
    if (m_privateKeyPassword == password) {
        return;
    }
    m_privateKeyPassword = password;
    Q_EMIT privateKeyPasswordChanged();
}

bool OpenvpnAuthSetting::privateKeyPasswordRequired() const
{
    return m_privateKeyPasswordRequired;
}

void OpenvpnAuthSetting::setPrivateKeyPasswordRequired(bool required)
{
    if (m_privateKeyPasswordRequired == required) {
        return;
    }
    m_privateKeyPasswordRequired = required;
    Q_EMIT privateKeyPasswordRequiredChanged();
}

QString OpenvpnAuthSetting::proxyPassword() const
{
    return m_proxyPassword;
}

void OpenvpnAuthSetting::setProxyPassword(const QString &password)
{
    if (m_proxyPassword == password) {
        return;
    }
    m_proxyPassword = password;
    Q_EMIT proxyPasswordChanged();
}

bool OpenvpnAuthSetting::proxyPasswordRequired() const
{
    return m_proxyPasswordRequired;
}

void OpenvpnAuthSetting::setProxyPasswordRequired(bool required)
{
    if (m_proxyPasswordRequired == required) {
        return;
    }
    m_proxyPasswordRequired = required;
    Q_EMIT proxyPasswordRequiredChanged();
}

#include "moc_openvpnauth.cpp"
