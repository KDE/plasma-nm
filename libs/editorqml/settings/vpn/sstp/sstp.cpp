/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "sstp.h"

#include "nm-sstp-service.h"

#include <QUrl>

namespace
{
const QLatin1String YesString("yes");
}

SstpSetting::SstpSetting(QObject *parent)
    : QObject(parent)
{
}

SstpSetting::~SstpSetting() = default;

void SstpSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setSstpPassword(QString());
    setProxyPassword(QString());

    const NMStringMap data = setting->data();

    // General
    setGateway(data[QLatin1String(NM_SSTP_KEY_GATEWAY)]);

    // Optional
    setUsername(data[QLatin1String(NM_SSTP_KEY_USER)]);

    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(data[QLatin1String(NM_SSTP_KEY_PASSWORD_FLAGS)].toInt());
    if (flags.testFlag(NetworkManager::Setting::None)) {
        setSstpPasswordOption(StoreForAllUsers);
    } else if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        setSstpPasswordOption(StoreForUser);
    } else {
        setSstpPasswordOption(AlwaysAsk);
    }

    setNtDomain(data[QLatin1String(NM_SSTP_KEY_DOMAIN)]);

    const QString caCert = data[QLatin1String(NM_SSTP_KEY_CA_CERT)];
    setCaCert(caCert.isEmpty() ? QString() : QUrl::fromLocalFile(caCert).toString());

    setIgnoreCertificateWarnings(data[QLatin1String(NM_SSTP_KEY_IGN_CERT_WARN)] == YesString);

    // Advanced - allowed authentication methods. NM stores the refusals, so an
    // absent key means the method is allowed.
    setAllowPap(data[QLatin1String(NM_SSTP_KEY_REFUSE_PAP)] != YesString);
    setAllowChap(data[QLatin1String(NM_SSTP_KEY_REFUSE_CHAP)] != YesString);
    setAllowMschap(data[QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAP)] != YesString);
    setAllowMschapv2(data[QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAPV2)] != YesString);
    setAllowEap(data[QLatin1String(NM_SSTP_KEY_REFUSE_EAP)] != YesString);

    // Advanced - MPPE
    const bool mppe = data[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE)] == YesString;
    const bool mppe40 = data[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_40)] == YesString;
    const bool mppe128 = data[QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_128)] == YesString;

    setUseMppe(mppe || mppe40 || mppe128);
    if (useMppe()) {
        if (mppe128) {
            setMppeCrypto(Mppe128);
        } else if (mppe40) {
            setMppeCrypto(Mppe40);
        } else {
            setMppeCrypto(MppeAny);
        }

        setStatefulEncryption(data[QLatin1String(NM_SSTP_KEY_MPPE_STATEFUL)] == YesString);
    } else {
        setMppeCrypto(MppeAny);
        setStatefulEncryption(false);
    }

    // Advanced - compression.
    setAllowBsdCompression(data[QLatin1String(NM_SSTP_KEY_NOBSDCOMP)] != YesString);
    setAllowDeflateCompression(data[QLatin1String(NM_SSTP_KEY_NODEFLATE)] != YesString);
    setAllowTcpHeaderCompression(data[QLatin1String(NM_SSTP_KEY_NO_VJ_COMP)] != YesString);

    // Advanced - echo
    setSendPppEchoPackets(data[QLatin1String(NM_SSTP_KEY_LCP_ECHO_INTERVAL)].toInt() > 0);

    // Advanced - misc
    const bool hasUnitNumber = data.contains(QLatin1String(NM_SSTP_KEY_UNIT_NUM));
    setUseCustomUnitNumber(hasUnitNumber);
    setCustomUnitNumber(hasUnitNumber ? data[QLatin1String(NM_SSTP_KEY_UNIT_NUM)].toInt() : 0);

    // Advanced - proxy
    setProxyAddress(data[QLatin1String(NM_SSTP_KEY_PROXY_SERVER)]);
    setProxyPort(data[QLatin1String(NM_SSTP_KEY_PROXY_PORT)].toInt());
    setProxyUsername(data[QLatin1String(NM_SSTP_KEY_PROXY_USER)]);

    loadSecrets(setting);
}

void SstpSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap secrets = setting->secrets();

    const QString password = secrets.value(QLatin1String(NM_SSTP_KEY_PASSWORD));
    if (!password.isEmpty()) {
        setSstpPassword(password);
    }

    const QString proxyPassword = secrets.value(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD));
    if (!proxyPassword.isEmpty()) {
        setProxyPassword(proxyPassword);
    }
}

QString SstpSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_SSTP);
}

QVariantMap SstpSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_SSTP));

    NMStringMap data;
    NMStringMap secrets;

    data.insert(QLatin1String(NM_SSTP_KEY_GATEWAY), m_gateway);

    if (!m_username.isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_USER), m_username);
    }

    if (!m_sstpPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_SSTP_KEY_PASSWORD), m_sstpPassword);
    }

    NetworkManager::Setting::SecretFlagType passwordFlag = NetworkManager::Setting::AgentOwned;
    switch (m_sstpPasswordOption) {
    case StoreForAllUsers:
        passwordFlag = NetworkManager::Setting::None;
        break;
    case StoreForUser:
        passwordFlag = NetworkManager::Setting::AgentOwned;
        break;
    case AlwaysAsk:
        passwordFlag = NetworkManager::Setting::NotSaved;
        break;
    }
    data.insert(QLatin1String(NM_SSTP_KEY_PASSWORD_FLAGS), QString::number(passwordFlag));

    if (!m_ntDomain.isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_DOMAIN), m_ntDomain);
    }

    if (!m_caCert.isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_CA_CERT), QUrl(m_caCert).toLocalFile());
    }

    if (m_ignoreCertificateWarnings) {
        data.insert(QLatin1String(NM_SSTP_KEY_IGN_CERT_WARN), YesString);
    }

    // Advanced - allowed authentication methods
    if (!m_allowPap) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_PAP), YesString);
    }

    if (!m_allowChap) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_CHAP), YesString);
    }

    if (!m_allowMschap) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAP), YesString);
    }

    if (!m_allowMschapv2) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_MSCHAPV2), YesString);
    }

    if (!m_allowEap) {
        data.insert(QLatin1String(NM_SSTP_KEY_REFUSE_EAP), YesString);
    }

    // Advanced - MPPE
    if (m_useMppe) {
        switch (m_mppeCrypto) {
        case MppeAny:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE), YesString);
            break;
        case Mppe128:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_128), YesString);
            break;
        case Mppe40:
            data.insert(QLatin1String(NM_SSTP_KEY_REQUIRE_MPPE_40), YesString);
            break;
        }

        if (m_statefulEncryption) {
            data.insert(QLatin1String(NM_SSTP_KEY_MPPE_STATEFUL), YesString);
        }
    }

    // Advanced - compression
    if (!m_allowBsdCompression) {
        data.insert(QLatin1String(NM_SSTP_KEY_NOBSDCOMP), YesString);
    }

    if (!m_allowDeflateCompression) {
        data.insert(QLatin1String(NM_SSTP_KEY_NODEFLATE), YesString);
    }

    if (!m_allowTcpHeaderCompression) {
        data.insert(QLatin1String(NM_SSTP_KEY_NO_VJ_COMP), YesString);
    }

    // Advanced - echo
    if (m_sendPppEchoPackets) {
        data.insert(QLatin1String(NM_SSTP_KEY_LCP_ECHO_FAILURE), QStringLiteral("5"));
        data.insert(QLatin1String(NM_SSTP_KEY_LCP_ECHO_INTERVAL), QStringLiteral("30"));
    }

    // Advanced - misc
    if (m_useCustomUnitNumber) {
        data.insert(QLatin1String(NM_SSTP_KEY_UNIT_NUM), QString::number(m_customUnitNumber));
    }

    // Advanced - proxy
    if (!m_proxyAddress.isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_SERVER), m_proxyAddress);
    }

    if (m_proxyPort > 0) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PORT), QString::number(m_proxyPort));
    }

    if (!m_proxyUsername.isEmpty()) {
        data.insert(QLatin1String(NM_SSTP_KEY_PROXY_USER), m_proxyUsername);
    }

    if (!m_proxyPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD), m_proxyPassword);
    }

    data.insert(QLatin1String(NM_SSTP_KEY_PROXY_PASSWORD_FLAGS), QString::number(NetworkManager::Setting::AgentOwned));

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool SstpSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString SstpSetting::gateway() const
{
    return m_gateway;
}

void SstpSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::username() const
{
    return m_username;
}

void SstpSetting::setUsername(const QString &username)
{
    if (m_username == username) {
        return;
    }
    m_username = username;
    Q_EMIT usernameChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::sstpPassword() const
{
    return m_sstpPassword;
}

void SstpSetting::setSstpPassword(const QString &password)
{
    if (m_sstpPassword == password) {
        return;
    }
    m_sstpPassword = password;
    Q_EMIT sstpPasswordChanged();
    Q_EMIT validChanged();
}

SstpSetting::PasswordOption SstpSetting::sstpPasswordOption() const
{
    return m_sstpPasswordOption;
}

void SstpSetting::setSstpPasswordOption(PasswordOption option)
{
    if (m_sstpPasswordOption == option) {
        return;
    }
    m_sstpPasswordOption = option;
    Q_EMIT sstpPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::ntDomain() const
{
    return m_ntDomain;
}

void SstpSetting::setNtDomain(const QString &ntDomain)
{
    if (m_ntDomain == ntDomain) {
        return;
    }
    m_ntDomain = ntDomain;
    Q_EMIT ntDomainChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::caCert() const
{
    return m_caCert;
}

void SstpSetting::setCaCert(const QString &caCert)
{
    if (m_caCert == caCert) {
        return;
    }
    m_caCert = caCert;
    Q_EMIT caCertChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::ignoreCertificateWarnings() const
{
    return m_ignoreCertificateWarnings;
}

void SstpSetting::setIgnoreCertificateWarnings(bool ignore)
{
    if (m_ignoreCertificateWarnings == ignore) {
        return;
    }
    m_ignoreCertificateWarnings = ignore;
    Q_EMIT ignoreCertificateWarningsChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowPap() const
{
    return m_allowPap;
}

void SstpSetting::setAllowPap(bool allow)
{
    if (m_allowPap == allow) {
        return;
    }
    m_allowPap = allow;
    Q_EMIT allowPapChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowChap() const
{
    return m_allowChap;
}

void SstpSetting::setAllowChap(bool allow)
{
    if (m_allowChap == allow) {
        return;
    }
    m_allowChap = allow;
    Q_EMIT allowChapChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowMschap() const
{
    return m_allowMschap;
}

void SstpSetting::setAllowMschap(bool allow)
{
    if (m_allowMschap == allow) {
        return;
    }
    m_allowMschap = allow;
    Q_EMIT allowMschapChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowMschapv2() const
{
    return m_allowMschapv2;
}

void SstpSetting::setAllowMschapv2(bool allow)
{
    if (m_allowMschapv2 == allow) {
        return;
    }
    m_allowMschapv2 = allow;
    Q_EMIT allowMschapv2Changed();
    Q_EMIT validChanged();
}

bool SstpSetting::allowEap() const
{
    return m_allowEap;
}

void SstpSetting::setAllowEap(bool allow)
{
    if (m_allowEap == allow) {
        return;
    }
    m_allowEap = allow;
    Q_EMIT allowEapChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::useMppe() const
{
    return m_useMppe;
}

void SstpSetting::setUseMppe(bool use)
{
    if (m_useMppe == use) {
        return;
    }
    m_useMppe = use;
    Q_EMIT useMppeChanged();
    Q_EMIT validChanged();
}

SstpSetting::MppeCrypto SstpSetting::mppeCrypto() const
{
    return m_mppeCrypto;
}

void SstpSetting::setMppeCrypto(MppeCrypto crypto)
{
    if (m_mppeCrypto == crypto) {
        return;
    }
    m_mppeCrypto = crypto;
    Q_EMIT mppeCryptoChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::statefulEncryption() const
{
    return m_statefulEncryption;
}

void SstpSetting::setStatefulEncryption(bool stateful)
{
    if (m_statefulEncryption == stateful) {
        return;
    }
    m_statefulEncryption = stateful;
    Q_EMIT statefulEncryptionChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowBsdCompression() const
{
    return m_allowBsdCompression;
}

void SstpSetting::setAllowBsdCompression(bool allow)
{
    if (m_allowBsdCompression == allow) {
        return;
    }
    m_allowBsdCompression = allow;
    Q_EMIT allowBsdCompressionChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowDeflateCompression() const
{
    return m_allowDeflateCompression;
}

void SstpSetting::setAllowDeflateCompression(bool allow)
{
    if (m_allowDeflateCompression == allow) {
        return;
    }
    m_allowDeflateCompression = allow;
    Q_EMIT allowDeflateCompressionChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::allowTcpHeaderCompression() const
{
    return m_allowTcpHeaderCompression;
}

void SstpSetting::setAllowTcpHeaderCompression(bool allow)
{
    if (m_allowTcpHeaderCompression == allow) {
        return;
    }
    m_allowTcpHeaderCompression = allow;
    Q_EMIT allowTcpHeaderCompressionChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::sendPppEchoPackets() const
{
    return m_sendPppEchoPackets;
}

void SstpSetting::setSendPppEchoPackets(bool send)
{
    if (m_sendPppEchoPackets == send) {
        return;
    }
    m_sendPppEchoPackets = send;
    Q_EMIT sendPppEchoPacketsChanged();
    Q_EMIT validChanged();
}

bool SstpSetting::useCustomUnitNumber() const
{
    return m_useCustomUnitNumber;
}

void SstpSetting::setUseCustomUnitNumber(bool use)
{
    if (m_useCustomUnitNumber == use) {
        return;
    }
    m_useCustomUnitNumber = use;
    Q_EMIT useCustomUnitNumberChanged();
    Q_EMIT validChanged();
}

int SstpSetting::customUnitNumber() const
{
    return m_customUnitNumber;
}

void SstpSetting::setCustomUnitNumber(int unitNumber)
{
    if (m_customUnitNumber == unitNumber) {
        return;
    }
    m_customUnitNumber = unitNumber;
    Q_EMIT customUnitNumberChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::proxyAddress() const
{
    return m_proxyAddress;
}

void SstpSetting::setProxyAddress(const QString &address)
{
    if (m_proxyAddress == address) {
        return;
    }
    m_proxyAddress = address;
    Q_EMIT proxyAddressChanged();
    Q_EMIT validChanged();
}

int SstpSetting::proxyPort() const
{
    return m_proxyPort;
}

void SstpSetting::setProxyPort(int port)
{
    if (m_proxyPort == port) {
        return;
    }
    m_proxyPort = port;
    Q_EMIT proxyPortChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::proxyUsername() const
{
    return m_proxyUsername;
}

void SstpSetting::setProxyUsername(const QString &username)
{
    if (m_proxyUsername == username) {
        return;
    }
    m_proxyUsername = username;
    Q_EMIT proxyUsernameChanged();
    Q_EMIT validChanged();
}

QString SstpSetting::proxyPassword() const
{
    return m_proxyPassword;
}

void SstpSetting::setProxyPassword(const QString &password)
{
    if (m_proxyPassword == password) {
        return;
    }
    m_proxyPassword = password;
    Q_EMIT proxyPasswordChanged();
    Q_EMIT validChanged();
}

#include "moc_sstp.cpp"
