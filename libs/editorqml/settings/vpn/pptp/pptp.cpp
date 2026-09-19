/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "pptp.h"

#include "nm-pptp-service.h"

namespace
{
PptpSetting::PasswordOption optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return PptpSetting::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return PptpSetting::StoreForUser;
    }
    if (flags.testFlag(NetworkManager::Setting::NotSaved)) {
        return PptpSetting::AlwaysAsk;
    }
    return PptpSetting::NotRequired;
}

QString flagsFromOption(PptpSetting::PasswordOption option)
{
    switch (option) {
    case PptpSetting::StoreForAllUsers:
        return QString::number(NetworkManager::Setting::None);
    case PptpSetting::StoreForUser:
        return QString::number(NetworkManager::Setting::AgentOwned);
    case PptpSetting::AlwaysAsk:
        return QString::number(NetworkManager::Setting::NotSaved);
    case PptpSetting::NotRequired:
        break;
    }
    return QString::number(NetworkManager::Setting::NotRequired);
}
}

PptpSetting::PptpSetting(QObject *parent)
    : QObject(parent)
{
}

PptpSetting::~PptpSetting() = default;

void PptpSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setPassword(QString());

    const NMStringMap data = setting->data();
    const QLatin1String yesString("yes");

    setGateway(data.value(QLatin1String(NM_PPTP_KEY_GATEWAY)));
    setLogin(data.value(QLatin1String(NM_PPTP_KEY_USER)));
    setNtDomain(data.value(QLatin1String(NM_PPTP_KEY_DOMAIN)));

    setPasswordOption(optionFromFlags(data.value(QLatin1String(NM_PPTP_KEY_PASSWORD "-flags"))));

    setAllowPap(data.value(QLatin1String(NM_PPTP_KEY_REFUSE_PAP)) != yesString);
    setAllowChap(data.value(QLatin1String(NM_PPTP_KEY_REFUSE_CHAP)) != yesString);
    setAllowMschap(data.value(QLatin1String(NM_PPTP_KEY_REFUSE_MSCHAP)) != yesString);
    setAllowMschapv2(data.value(QLatin1String(NM_PPTP_KEY_REFUSE_MSCHAPV2)) != yesString);
    setAllowEap(data.value(QLatin1String(NM_PPTP_KEY_REFUSE_EAP)) != yesString);

    const bool mppe = data.value(QLatin1String(NM_PPTP_KEY_REQUIRE_MPPE)) == yesString;
    const bool mppe40 = data.value(QLatin1String(NM_PPTP_KEY_REQUIRE_MPPE_40)) == yesString;
    const bool mppe128 = data.value(QLatin1String(NM_PPTP_KEY_REQUIRE_MPPE_128)) == yesString;

    setUseMppe(mppe || mppe40 || mppe128);
    if (useMppe()) {
        if (mppe128) {
            setMppeCrypto(Mppe128);
        } else if (mppe40) {
            setMppeCrypto(Mppe40);
        } else {
            setMppeCrypto(MppeAny);
        }
        setStatefulEncryption(data.value(QLatin1String(NM_PPTP_KEY_MPPE_STATEFUL)) == yesString);
    }

    setAllowBsdCompression(data.value(QLatin1String(NM_PPTP_KEY_NOBSDCOMP)) != yesString);
    setAllowDeflateCompression(data.value(QLatin1String(NM_PPTP_KEY_NODEFLATE)) != yesString);
    setAllowTcpHeaderCompression(data.value(QLatin1String(NM_PPTP_KEY_NO_VJ_COMP)) != yesString);

    setSendPppEchoPackets(data.value(QLatin1String(NM_PPTP_KEY_LCP_ECHO_INTERVAL)).toInt() > 0);

    loadSecrets(setting);
}

void PptpSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const QString password = setting->secrets().value(QLatin1String(NM_PPTP_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setPassword(password);
    }
}

QString PptpSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_PPTP);
}

QVariantMap PptpSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_PPTP));

    NMStringMap data;
    NMStringMap secrets;

    const QLatin1String yesString("yes");

    if (!m_gateway.isEmpty()) {
        data.insert(QLatin1String(NM_PPTP_KEY_GATEWAY), m_gateway);
    }

    if (!m_login.isEmpty()) {
        data.insert(QLatin1String(NM_PPTP_KEY_USER), m_login);
    }

    if (!m_password.isEmpty()) {
        secrets.insert(QLatin1String(NM_PPTP_KEY_PASSWORD), m_password);
    }
    data.insert(QLatin1String(NM_PPTP_KEY_PASSWORD "-flags"), flagsFromOption(m_passwordOption));

    if (!m_ntDomain.isEmpty()) {
        data.insert(QLatin1String(NM_PPTP_KEY_DOMAIN), m_ntDomain);
    }

    if (!m_allowPap) {
        data.insert(QLatin1String(NM_PPTP_KEY_REFUSE_PAP), yesString);
    }
    if (!m_allowChap) {
        data.insert(QLatin1String(NM_PPTP_KEY_REFUSE_CHAP), yesString);
    }
    if (!m_allowMschap) {
        data.insert(QLatin1String(NM_PPTP_KEY_REFUSE_MSCHAP), yesString);
    }
    if (!m_allowMschapv2) {
        data.insert(QLatin1String(NM_PPTP_KEY_REFUSE_MSCHAPV2), yesString);
    }
    if (!m_allowEap) {
        data.insert(QLatin1String(NM_PPTP_KEY_REFUSE_EAP), yesString);
    }

    if (m_useMppe) {
        switch (m_mppeCrypto) {
        case MppeAny:
            data.insert(QLatin1String(NM_PPTP_KEY_REQUIRE_MPPE), yesString);
            break;
        case Mppe128:
            data.insert(QLatin1String(NM_PPTP_KEY_REQUIRE_MPPE_128), yesString);
            break;
        case Mppe40:
            data.insert(QLatin1String(NM_PPTP_KEY_REQUIRE_MPPE_40), yesString);
            break;
        }

        if (m_statefulEncryption) {
            data.insert(QLatin1String(NM_PPTP_KEY_MPPE_STATEFUL), yesString);
        }
    }

    if (!m_allowBsdCompression) {
        data.insert(QLatin1String(NM_PPTP_KEY_NOBSDCOMP), yesString);
    }

    if (!m_allowDeflateCompression) {
        data.insert(QLatin1String(NM_PPTP_KEY_NODEFLATE), yesString);
    }

    if (!m_allowTcpHeaderCompression) {
        data.insert(QLatin1String(NM_PPTP_KEY_NO_VJ_COMP), yesString);
    }

    if (m_sendPppEchoPackets) {
        data.insert(QLatin1String(NM_PPTP_KEY_LCP_ECHO_FAILURE), QStringLiteral("5"));
        data.insert(QLatin1String(NM_PPTP_KEY_LCP_ECHO_INTERVAL), QStringLiteral("30"));
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool PptpSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString PptpSetting::gateway() const
{
    return m_gateway;
}

void PptpSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString PptpSetting::login() const
{
    return m_login;
}

void PptpSetting::setLogin(const QString &login)
{
    if (m_login == login) {
        return;
    }
    m_login = login;
    Q_EMIT loginChanged();
    Q_EMIT validChanged();
}

QString PptpSetting::password() const
{
    return m_password;
}

void PptpSetting::setPassword(const QString &password)
{
    if (m_password == password) {
        return;
    }
    m_password = password;
    Q_EMIT passwordChanged();
    Q_EMIT validChanged();
}

PptpSetting::PasswordOption PptpSetting::passwordOption() const
{
    return m_passwordOption;
}

void PptpSetting::setPasswordOption(PasswordOption option)
{
    if (m_passwordOption == option) {
        return;
    }
    m_passwordOption = option;
    Q_EMIT passwordOptionChanged();
    Q_EMIT validChanged();
}

QString PptpSetting::ntDomain() const
{
    return m_ntDomain;
}

void PptpSetting::setNtDomain(const QString &ntDomain)
{
    if (m_ntDomain == ntDomain) {
        return;
    }
    m_ntDomain = ntDomain;
    Q_EMIT ntDomainChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowPap() const
{
    return m_allowPap;
}

void PptpSetting::setAllowPap(bool allow)
{
    if (m_allowPap == allow) {
        return;
    }
    m_allowPap = allow;
    Q_EMIT allowPapChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowChap() const
{
    return m_allowChap;
}

void PptpSetting::setAllowChap(bool allow)
{
    if (m_allowChap == allow) {
        return;
    }
    m_allowChap = allow;
    Q_EMIT allowChapChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowMschap() const
{
    return m_allowMschap;
}

void PptpSetting::setAllowMschap(bool allow)
{
    if (m_allowMschap == allow) {
        return;
    }
    m_allowMschap = allow;
    Q_EMIT allowMschapChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowMschapv2() const
{
    return m_allowMschapv2;
}

void PptpSetting::setAllowMschapv2(bool allow)
{
    if (m_allowMschapv2 == allow) {
        return;
    }
    m_allowMschapv2 = allow;
    Q_EMIT allowMschapv2Changed();
    Q_EMIT validChanged();
}

bool PptpSetting::allowEap() const
{
    return m_allowEap;
}

void PptpSetting::setAllowEap(bool allow)
{
    if (m_allowEap == allow) {
        return;
    }
    m_allowEap = allow;
    Q_EMIT allowEapChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::useMppe() const
{
    return m_useMppe;
}

void PptpSetting::setUseMppe(bool use)
{
    if (m_useMppe == use) {
        return;
    }
    m_useMppe = use;
    Q_EMIT useMppeChanged();
    Q_EMIT validChanged();
}

PptpSetting::MppeCrypto PptpSetting::mppeCrypto() const
{
    return m_mppeCrypto;
}

void PptpSetting::setMppeCrypto(MppeCrypto crypto)
{
    if (m_mppeCrypto == crypto) {
        return;
    }
    m_mppeCrypto = crypto;
    Q_EMIT mppeCryptoChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::statefulEncryption() const
{
    return m_statefulEncryption;
}

void PptpSetting::setStatefulEncryption(bool stateful)
{
    if (m_statefulEncryption == stateful) {
        return;
    }
    m_statefulEncryption = stateful;
    Q_EMIT statefulEncryptionChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowBsdCompression() const
{
    return m_allowBsdCompression;
}

void PptpSetting::setAllowBsdCompression(bool allow)
{
    if (m_allowBsdCompression == allow) {
        return;
    }
    m_allowBsdCompression = allow;
    Q_EMIT allowBsdCompressionChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowDeflateCompression() const
{
    return m_allowDeflateCompression;
}

void PptpSetting::setAllowDeflateCompression(bool allow)
{
    if (m_allowDeflateCompression == allow) {
        return;
    }
    m_allowDeflateCompression = allow;
    Q_EMIT allowDeflateCompressionChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::allowTcpHeaderCompression() const
{
    return m_allowTcpHeaderCompression;
}

void PptpSetting::setAllowTcpHeaderCompression(bool allow)
{
    if (m_allowTcpHeaderCompression == allow) {
        return;
    }
    m_allowTcpHeaderCompression = allow;
    Q_EMIT allowTcpHeaderCompressionChanged();
    Q_EMIT validChanged();
}

bool PptpSetting::sendPppEchoPackets() const
{
    return m_sendPppEchoPackets;
}

void PptpSetting::setSendPppEchoPackets(bool send)
{
    if (m_sendPppEchoPackets == send) {
        return;
    }
    m_sendPppEchoPackets = send;
    Q_EMIT sendPppEchoPacketsChanged();
    Q_EMIT validChanged();
}

#include "moc_pptp.cpp"
