/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openvpnadvanced.h"

#include "nm-openvpn-service.h"
#include "openvpnhelpers_p.h"

#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

namespace
{
QStringList parseCiphers(const QByteArray &output)
{
    static const QRegularExpression cipherRe(QStringLiteral("(.*)  \\(.*\\)"));

    QStringList ciphers;
    bool pastPreamble = false;

    const QList<QByteArray> lines = output.split('\n');
    for (const QByteArray &line : lines) {
        if (line.isEmpty()) {
            pastPreamble = true;
        } else if (pastPreamble) {
            const auto match = cipherRe.match(QString::fromUtf8(line));
            if (match.hasMatch()) {
                ciphers.append(match.captured(1));
            }
        }
    }

    return ciphers;
}
}

OpenvpnAdvancedSetting::OpenvpnAdvancedSetting(QObject *parent)
    : QObject(parent)
{
}

OpenvpnAdvancedSetting::~OpenvpnAdvancedSetting() = default;

void OpenvpnAdvancedSetting::probe()
{
    if (m_probed) {
        return;
    }
    m_probed = true;

    const QString openVpnBinary = QStandardPaths::findExecutable(QStringLiteral("openvpn"), {QStringLiteral("/sbin"), QStringLiteral("/usr/sbin")});

    if (openVpnBinary.isEmpty()) {
        setSupportsLegacySubjectMatch(false);
        return;
    }

    auto *cipherProcess = new QProcess(this);
    cipherProcess->setProgram(openVpnBinary);
    cipherProcess->setArguments({QStringLiteral("--show-ciphers")});

    connect(cipherProcess, &QProcess::finished, this, [this, cipherProcess](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
            setAvailableCiphers(parseCiphers(cipherProcess->readAllStandardOutput()));
        }
        cipherProcess->deleteLater();
    });
    connect(cipherProcess, &QProcess::errorOccurred, cipherProcess, &QObject::deleteLater);

    cipherProcess->start();

    auto *versionProcess = new QProcess(this);
    versionProcess->setProgram(openVpnBinary);
    versionProcess->setArguments({QStringLiteral("--version")});

    connect(versionProcess, &QProcess::finished, this, [this, versionProcess](int, QProcess::ExitStatus exitStatus) {
        bool legacy = false;

        if (exitStatus == QProcess::NormalExit) {
            const QStringList fields = QString::fromUtf8(versionProcess->readAllStandardOutput()).split(QLatin1Char(' '));
            if (fields.count() > 2) {
                const QStringList version = fields.at(1).split(QLatin1Char('.'));
                if (version.count() == 3) {
                    const int major = version.at(0).toInt();
                    legacy = major < 2 || (major == 2 && version.at(1).toInt() < 4);
                }
            }
        }

        setSupportsLegacySubjectMatch(legacy);
        versionProcess->deleteLater();
    });
    connect(versionProcess, &QProcess::errorOccurred, this, [this, versionProcess]() {
        setSupportsLegacySubjectMatch(false);
        versionProcess->deleteLater();
    });

    versionProcess->start();
}

void OpenvpnAdvancedSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setProxyPassword(QString());

    const NMStringMap data = setting->data();

    // General
    const QString rawPort = data.value(QLatin1String(NM_OPENVPN_KEY_PORT));
    setUseCustomPort(!rawPort.isEmpty());
    setCustomPort(rawPort.isEmpty() ? 1194 : rawPort.toInt());

    const QString rawMtu = data.value(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU));
    setUseMtu(!rawMtu.isEmpty());
    setMtu(rawMtu.isEmpty() ? 1500 : rawMtu.toInt());

    const QString rawFragment = data.value(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE));
    setUseCustomFragmentSize(!rawFragment.isEmpty());
    setCustomFragmentSize(rawFragment.isEmpty() ? 1300 : rawFragment.toInt());

    const QString rawReneg = data.value(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS));
    setUseCustomReneg(!rawReneg.isEmpty());
    setCustomReneg(rawReneg.toInt());

    const QString rawMtuDisc = data.value(QLatin1String(NM_OPENVPN_KEY_MTU_DISC));
    setUseMtuDisc(!rawMtuDisc.isEmpty());
    if (rawMtuDisc == QLatin1String("maybe")) {
        setMtuDisc(MtuDiscMaybe);
    } else if (rawMtuDisc == OpenvpnHelpers::YesString) {
        setMtuDisc(MtuDiscYes);
    } else {
        setMtuDisc(MtuDiscNo);
    }

    const QString compLzo = data.value(QLatin1String(NM_OPENVPN_KEY_COMP_LZO));
    const QString compress = data.value(QLatin1String(NM_OPENVPN_KEY_COMPRESS));

    setUseCompression(!compLzo.isEmpty() || !compress.isEmpty());
    if (!compress.isEmpty()) {
        if (compress == QLatin1String("lz4")) {
            setCompression(Lz4);
        } else if (compress == QLatin1String("lz4-v2")) {
            setCompression(Lz4v2);
        } else if (compress == QLatin1String("lzo")) {
            setCompression(Lzo);
        } else {
            setCompression(Automatic);
        }
    } else if (!compLzo.isEmpty()) {
        if (compLzo == QLatin1String("no-by-default")) {
            setCompression(NoCompression);
        } else if (compLzo == OpenvpnHelpers::YesString) {
            setCompression(Lzo);
        } else {
            setCompression(Adaptive);
        }
    }
    setUseAsymCompression(data.value(QLatin1String(NM_OPENVPN_KEY_ALLOW_COMPRESSION)) == QLatin1String("asym"));

    setUseTcp(data.value(QLatin1String(NM_OPENVPN_KEY_PROTO_TCP)) == OpenvpnHelpers::YesString);

    const QString rawDevType = data.value(QLatin1String(NM_OPENVPN_KEY_DEV_TYPE));
    setUseVirtualDeviceType(!rawDevType.isEmpty());
    setDeviceType(rawDevType == QLatin1String("tap") ? Tap : Tun);

    const QString rawDev = data.value(QLatin1String(NM_OPENVPN_KEY_DEV));
    setUseVirtualDeviceName(!rawDev.isEmpty());
    setVirtualDeviceName(rawDev);

    setMssRestrict(data.value(QLatin1String(NM_OPENVPN_KEY_MSSFIX)) == OpenvpnHelpers::YesString);
    setRandomizeRemoteHosts(data.value(QLatin1String(NM_OPENVPN_KEY_REMOTE_RANDOM)) == OpenvpnHelpers::YesString);
    setRandomizeRemoteHostname(data.value(QLatin1String(NM_OPENVPN_KEY_REMOTE_RANDOM_HOSTNAME)) == OpenvpnHelpers::YesString);
    setPushPeerInfo(data.value(QLatin1String(NM_OPENVPN_KEY_PUSH_PEER_INFO)) == OpenvpnHelpers::YesString);
    setAllowPullFqdn(data.value(QLatin1String(NM_OPENVPN_KEY_ALLOW_PULL_FQDN)) == OpenvpnHelpers::YesString);
    setIpv6TunLink(data.value(QLatin1String(NM_OPENVPN_KEY_TUN_IPV6)) == OpenvpnHelpers::YesString);

    setConnectTimeout(data.value(QLatin1String(NM_OPENVPN_KEY_CONNECT_TIMEOUT)).toInt());

    const QString rawPing = data.value(QLatin1String(NM_OPENVPN_KEY_PING));
    setUsePingInterval(!rawPing.isEmpty());
    if (!rawPing.isEmpty()) {
        setPingInterval(rawPing.toInt());
    }

    const QString rawPingExit = data.value(QLatin1String(NM_OPENVPN_KEY_PING_EXIT));
    const QString rawPingRestart = data.value(QLatin1String(NM_OPENVPN_KEY_PING_RESTART));
    setUseExitRestartPing(!rawPingExit.isEmpty() || !rawPingRestart.isEmpty());
    if (!rawPingExit.isEmpty()) {
        setExitRestartPingMode(PingExit);
        setExitRestartPing(rawPingExit.toInt());
    } else if (!rawPingRestart.isEmpty()) {
        setExitRestartPingMode(PingRestart);
        setExitRestartPing(rawPingRestart.toInt());
    }

    setAcceptAuthenticatedPackets(data.value(QLatin1String(NM_OPENVPN_KEY_FLOAT)) == OpenvpnHelpers::YesString);

    const QString rawMaxRoutes = data.value(QLatin1String(NM_OPENVPN_KEY_MAX_ROUTES));
    setUseMaxRoutes(!rawMaxRoutes.isEmpty());
    if (!rawMaxRoutes.isEmpty()) {
        setMaxRoutes(rawMaxRoutes.toInt());
    }

    // Security
    const QString hmac = data.value(QLatin1String(NM_OPENVPN_KEY_AUTH));
    if (hmac == QLatin1String(NM_OPENVPN_AUTH_NONE)) {
        setHmacAuth(HmacNone);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_RSA_MD4)) {
        setHmacAuth(HmacMd4);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_MD5)) {
        setHmacAuth(HmacMd5);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_SHA1)) {
        setHmacAuth(HmacSha1);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_SHA224)) {
        setHmacAuth(HmacSha224);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_SHA256)) {
        setHmacAuth(HmacSha256);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_SHA384)) {
        setHmacAuth(HmacSha384);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_SHA512)) {
        setHmacAuth(HmacSha512);
    } else if (hmac == QLatin1String(NM_OPENVPN_AUTH_RIPEMD160)) {
        setHmacAuth(HmacRipemd160);
    } else {
        setHmacAuth(HmacDefault);
    }

    const QString rawKeysize = data.value(QLatin1String(NM_OPENVPN_KEY_KEYSIZE));
    setUseCustomCipherKey(!rawKeysize.isEmpty());
    if (!rawKeysize.isEmpty()) {
        setCustomCipherKey(rawKeysize.toInt());
    }

    setCipher(data.value(QLatin1String(NM_OPENVPN_KEY_CIPHER)));
    setDataCiphers(data.value(QLatin1String(NM_OPENVPN_KEY_DATA_CIPHERS)));
    setDataCiphersFallback(data.value(QLatin1String(NM_OPENVPN_KEY_DATA_CIPHERS_FALLBACK)));
    setDisableCipherNegotiation(data.value(QLatin1String(NM_OPENVPN_KEY_NCP_DISABLE)) == OpenvpnHelpers::YesString);

    // TLS Settings
    const QString tlsRemote = data.value(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE));
    const QString verifyX509 = data.value(QLatin1String(NM_OPENVPN_KEY_VERIFY_X509_NAME));

    if (!verifyX509.isEmpty()) {
        const QStringList parts = verifyX509.split(QLatin1Char(':'));
        if (parts.size() == 2) {
            if (parts.at(0) == QLatin1String(NM_OPENVPN_VERIFY_X509_NAME_TYPE_SUBJECT)) {
                setCertCheckType(VerifyWholeSubjectExactly);
            } else if (parts.at(0) == QLatin1String(NM_OPENVPN_VERIFY_X509_NAME_TYPE_NAME)) {
                setCertCheckType(VerifyNameExactly);
            } else if (parts.at(0) == QLatin1String(NM_OPENVPN_VERIFY_X509_NAME_TYPE_NAME_PREFIX)) {
                setCertCheckType(VerifyNameByPrefix);
            }
            setSubjectMatch(parts.at(1));
        }
    } else if (!tlsRemote.isEmpty()) {
        setCertCheckType(VerifySubjectPartially);
        setSubjectMatch(tlsRemote);
    } else {
        setCertCheckType(DontVerify);
    }

    const QString remoteCertTls = data.value(QLatin1String(NM_OPENVPN_KEY_REMOTE_CERT_TLS));
    setVerifyRemoteCertTls(!remoteCertTls.isEmpty());
    setRemoteCertTlsType(remoteCertTls == QLatin1String(NM_OPENVPN_REM_CERT_TLS_CLIENT) ? Client : Server);

    const QString nsCertType = data.value(QLatin1String(NM_OPENVPN_KEY_NS_CERT_TYPE));
    setVerifyNsCertType(!nsCertType.isEmpty());
    setNsCertType(nsCertType == QLatin1String(NM_OPENVPN_NS_CERT_TYPE_CLIENT) ? Client : Server);

    const QString keyTa = data.value(QLatin1String(NM_OPENVPN_KEY_TA));
    const QString keyTlsCrypt = data.value(QLatin1String(NM_OPENVPN_KEY_TLS_CRYPT));
    const QString keyTlsCryptV2 = data.value(QLatin1String(NM_OPENVPN_KEY_TLS_CRYPT_V2));

    if (!keyTlsCryptV2.isEmpty()) {
        setTlsMode(TlsCryptV2);
        setTlsAuthKey(OpenvpnHelpers::pathToUrl(keyTlsCryptV2));
    } else if (!keyTlsCrypt.isEmpty()) {
        setTlsMode(TlsCrypt);
        setTlsAuthKey(OpenvpnHelpers::pathToUrl(keyTlsCrypt));
    } else if (!keyTa.isEmpty()) {
        setTlsMode(TlsAuth);
        setTlsAuthKey(OpenvpnHelpers::pathToUrl(keyTa));
        if (data.contains(QLatin1String(NM_OPENVPN_KEY_TA_DIR))) {
            setTlsAuthDirection(data.value(QLatin1String(NM_OPENVPN_KEY_TA_DIR)).toUInt() == 1 ? Direction1 : Direction0);
        }
    } else {
        setTlsMode(TlsNone);
    }

    setExtraCerts(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_EXTRA_CERTS))));
    setCrlVerifyFile(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_CRL_VERIFY_FILE))));
    setCrlVerifyDir(OpenvpnHelpers::pathToUrl(data.value(QLatin1String(NM_OPENVPN_KEY_CRL_VERIFY_DIR))));

    setTlsVersionMin(data.value(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MIN)));
    setTlsVersionMax(data.value(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MAX)));
    if (data.contains(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MIN_OR_HIGHEST))) {
        const QString orHighest = data.value(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MIN_OR_HIGHEST));
        setTlsVersionMinOrHighest(orHighest == OpenvpnHelpers::YesString || orHighest.isEmpty());
    }

    // Proxies
    const QString proxyType = data.value(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE));
    if (proxyType == QLatin1String("http")) {
        setProxyType(ProxyHttp);
    } else if (proxyType == QLatin1String("socks")) {
        setProxyType(ProxySocks);
    } else {
        setProxyType(ProxyNotRequired);
    }

    setProxyServer(data.value(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER)));
    setProxyPort(data.value(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT)).toInt());
    setProxyRetry(data.value(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY)) == OpenvpnHelpers::YesString);
    setProxyUsername(data.value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME)));
    setProxyPasswordOption(static_cast<PasswordOption>(OpenvpnHelpers::optionFromFlags(data.value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD_FLAGS)))));

    loadSecrets(setting);
}

void OpenvpnAdvancedSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    if (m_proxyPasswordOption == AlwaysAsk || m_proxyPasswordOption == NotRequired) {
        return;
    }

    const QString proxyPassword = setting->secrets().value(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD));
    if (!proxyPassword.isEmpty()) {
        setProxyPassword(proxyPassword);
    }
}

NMStringMap OpenvpnAdvancedSetting::data() const
{
    NMStringMap data;

    const QLatin1String yes = OpenvpnHelpers::YesString;
    const QLatin1String no = OpenvpnHelpers::NoString;

    // General
    if (m_useCustomPort) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_PORT), QString::number(m_customPort));
    }
    if (m_useMtu) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU), QString::number(m_mtu));
    }
    if (m_useCustomFragmentSize) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE), QString::number(m_customFragmentSize));
    }
    if (m_useCustomReneg) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS), QString::number(m_customReneg));
    }

    data.insert(QLatin1String(NM_OPENVPN_KEY_PROTO_TCP), m_useTcp ? yes : no);

    if (m_useCompression) {
        switch (m_compression) {
        case NoCompression:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMP_LZO), QLatin1String("no-by-default"));
            break;
        case Lzo:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), QLatin1String("lzo"));
            break;
        case Lz4:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), QLatin1String("lz4"));
            break;
        case Lz4v2:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), QLatin1String("lz4-v2"));
            break;
        case Adaptive:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMP_LZO), QLatin1String("adaptive"));
            break;
        case Automatic:
            data.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), yes);
            break;
        }
        data.insert(QLatin1String(NM_OPENVPN_KEY_ALLOW_COMPRESSION), m_useAsymCompression ? QLatin1String("asym") : yes);
    } else {
        data.insert(QLatin1String(NM_OPENVPN_KEY_ALLOW_COMPRESSION), no);
    }

    if (m_useVirtualDeviceType) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_DEV_TYPE), m_deviceType == Tap ? QLatin1String("tap") : QLatin1String("tun"));
    }
    if (m_useVirtualDeviceName) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_DEV), m_virtualDeviceName);
    }

    data.insert(QLatin1String(NM_OPENVPN_KEY_MSSFIX), m_mssRestrict ? yes : no);

    if (m_useMtuDisc) {
        switch (m_mtuDisc) {
        case MtuDiscNo:
            data.insert(QLatin1String(NM_OPENVPN_KEY_MTU_DISC), no);
            break;
        case MtuDiscMaybe:
            data.insert(QLatin1String(NM_OPENVPN_KEY_MTU_DISC), QLatin1String("maybe"));
            break;
        case MtuDiscYes:
            data.insert(QLatin1String(NM_OPENVPN_KEY_MTU_DISC), yes);
            break;
        }
    }

    data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_RANDOM), m_randomizeRemoteHosts ? yes : no);
    data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_RANDOM_HOSTNAME), m_randomizeRemoteHostname ? yes : no);
    data.insert(QLatin1String(NM_OPENVPN_KEY_PUSH_PEER_INFO), m_pushPeerInfo ? yes : no);
    data.insert(QLatin1String(NM_OPENVPN_KEY_ALLOW_PULL_FQDN), m_allowPullFqdn ? yes : no);
    data.insert(QLatin1String(NM_OPENVPN_KEY_TUN_IPV6), m_ipv6TunLink ? yes : no);

    if (m_usePingInterval) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_PING), QString::number(m_pingInterval));
    }

    // Zero means automatic, which is the absence of the key.
    if (m_connectTimeout > 0) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CONNECT_TIMEOUT), QString::number(m_connectTimeout));
    }

    if (m_useExitRestartPing) {
        if (m_exitRestartPingMode == PingExit) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_PING_EXIT), QString::number(m_exitRestartPing));
        } else {
            data.insert(QLatin1String(NM_OPENVPN_KEY_PING_RESTART), QString::number(m_exitRestartPing));
        }
    }

    data.insert(QLatin1String(NM_OPENVPN_KEY_FLOAT), m_acceptAuthenticatedPackets ? yes : no);

    if (m_useMaxRoutes) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_MAX_ROUTES), QString::number(m_maxRoutes));
    }

    // Security
    switch (m_hmacAuth) {
    case HmacDefault:
        break;
    case HmacNone:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_NONE));
        break;
    case HmacMd4:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_RSA_MD4));
        break;
    case HmacMd5:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_MD5));
        break;
    case HmacSha1:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA1));
        break;
    case HmacSha224:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA224));
        break;
    case HmacSha256:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA256));
        break;
    case HmacSha384:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA384));
        break;
    case HmacSha512:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_SHA512));
        break;
    case HmacRipemd160:
        data.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), QLatin1String(NM_OPENVPN_AUTH_RIPEMD160));
        break;
    }

    if (m_useCustomCipherKey) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_KEYSIZE), QString::number(m_customCipherKey));
    }

    if (!m_cipher.isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CIPHER), m_cipher);
    }
    if (!m_dataCiphers.isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_DATA_CIPHERS), m_dataCiphers);
    }
    if (!m_dataCiphersFallback.isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_DATA_CIPHERS_FALLBACK), m_dataCiphersFallback);
    }
    data.insert(QLatin1String(NM_OPENVPN_KEY_NCP_DISABLE), m_disableCipherNegotiation ? yes : no);

    // TLS Settings
    switch (m_certCheckType) {
    case DontVerify:
        break;
    case VerifyWholeSubjectExactly:
        data.insert(QLatin1String(NM_OPENVPN_KEY_VERIFY_X509_NAME),
                    QStringLiteral("%1:%2").arg(QLatin1String(NM_OPENVPN_VERIFY_X509_NAME_TYPE_SUBJECT), m_subjectMatch));
        break;
    case VerifyNameExactly:
        data.insert(QLatin1String(NM_OPENVPN_KEY_VERIFY_X509_NAME),
                    QStringLiteral("%1:%2").arg(QLatin1String(NM_OPENVPN_VERIFY_X509_NAME_TYPE_NAME), m_subjectMatch));
        break;
    case VerifyNameByPrefix:
        data.insert(QLatin1String(NM_OPENVPN_KEY_VERIFY_X509_NAME),
                    QStringLiteral("%1:%2").arg(QLatin1String(NM_OPENVPN_VERIFY_X509_NAME_TYPE_NAME_PREFIX), m_subjectMatch));
        break;
    case VerifySubjectPartially:
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE), m_subjectMatch);
        break;
    }

    if (m_verifyRemoteCertTls) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_CERT_TLS),
                    m_remoteCertTlsType == Client ? QLatin1String(NM_OPENVPN_REM_CERT_TLS_CLIENT) : QLatin1String(NM_OPENVPN_REM_CERT_TLS_SERVER));
    }

    if (m_verifyNsCertType) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_NS_CERT_TYPE),
                    m_nsCertType == Client ? QLatin1String(NM_OPENVPN_NS_CERT_TYPE_CLIENT) : QLatin1String(NM_OPENVPN_NS_CERT_TYPE_SERVER));
    }

    const QString tlsAuthKeyPath = OpenvpnHelpers::urlToPath(m_tlsAuthKey);
    switch (m_tlsMode) {
    case TlsNone:
        break;
    case TlsAuth:
        if (!tlsAuthKeyPath.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TA), tlsAuthKeyPath);
        }
        if (m_tlsAuthDirection != NoDirection) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TA_DIR), QString::number(m_tlsAuthDirection == Direction1 ? 1 : 0));
        }
        break;
    case TlsCrypt:
        if (!tlsAuthKeyPath.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_CRYPT), tlsAuthKeyPath);
        }
        break;
    case TlsCryptV2:
        if (!tlsAuthKeyPath.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_CRYPT_V2), tlsAuthKeyPath);
        }
        break;
    }

    if (!m_tlsVersionMin.trimmed().isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MIN), m_tlsVersionMin.trimmed());
    }
    if (m_tlsVersionMinOrHighest) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MIN_OR_HIGHEST), yes);
    }
    if (!m_tlsVersionMax.trimmed().isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_TLS_VERSION_MAX), m_tlsVersionMax.trimmed());
    }

    const QString extraCertsPath = OpenvpnHelpers::urlToPath(m_extraCerts);
    if (!extraCertsPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_EXTRA_CERTS), extraCertsPath);
    }

    // A CRL is either a file or a directory, never both.
    const QString crlFilePath = OpenvpnHelpers::urlToPath(m_crlVerifyFile);
    const QString crlDirPath = OpenvpnHelpers::urlToPath(m_crlVerifyDir);
    if (!crlFilePath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CRL_VERIFY_FILE), crlFilePath);
    } else if (!crlDirPath.isEmpty()) {
        data.insert(QLatin1String(NM_OPENVPN_KEY_CRL_VERIFY_DIR), crlDirPath);
    }

    // Proxies
    switch (m_proxyType) {
    case ProxyNotRequired:
        break;
    case ProxyHttp:
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), QLatin1String("http"));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), m_proxyServer);
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(m_proxyPort));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), m_proxyRetry ? yes : no);
        // The password only means anything alongside a user name.
        if (!m_proxyUsername.isEmpty()) {
            data.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME), m_proxyUsername);
            data.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD_FLAGS), OpenvpnHelpers::flagsFromOption(m_proxyPasswordOption));
        }
        break;
    case ProxySocks:
        // SOCKS takes no credentials.
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), QLatin1String("socks"));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), m_proxyServer);
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), QString::number(m_proxyPort));
        data.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), m_proxyRetry ? yes : no);
        break;
    }

    return data;
}

NMStringMap OpenvpnAdvancedSetting::secrets() const
{
    NMStringMap secrets;

    if (m_proxyType == ProxyHttp && !m_proxyUsername.isEmpty() && !m_proxyPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD), m_proxyPassword);
    }

    return secrets;
}

QStringList OpenvpnAdvancedSetting::availableCiphers() const
{
    return m_availableCiphers;
}

void OpenvpnAdvancedSetting::setAvailableCiphers(const QStringList &ciphers)
{
    if (m_availableCiphers == ciphers) {
        return;
    }
    m_availableCiphers = ciphers;
    Q_EMIT availableCiphersChanged();
}

bool OpenvpnAdvancedSetting::supportsLegacySubjectMatch() const
{
    return m_supportsLegacySubjectMatch;
}

void OpenvpnAdvancedSetting::setSupportsLegacySubjectMatch(bool supports)
{
    if (m_supportsLegacySubjectMatch == supports) {
        return;
    }
    m_supportsLegacySubjectMatch = supports;
    Q_EMIT supportsLegacySubjectMatchChanged();
}

#define ADVANCED_PROPERTY(Type, name, Name, member)                                                                                                            \
    Type OpenvpnAdvancedSetting::name() const                                                                                                                  \
    {                                                                                                                                                          \
        return member;                                                                                                                                         \
    }                                                                                                                                                          \
    void OpenvpnAdvancedSetting::set##Name(Type value)                                                                                                         \
    {                                                                                                                                                          \
        if (member == value) {                                                                                                                                 \
            return;                                                                                                                                            \
        }                                                                                                                                                      \
        member = value;                                                                                                                                        \
        Q_EMIT name##Changed();                                                                                                                                \
        Q_EMIT changed();                                                                                                                                      \
    }

#define ADVANCED_STRING_PROPERTY(name, Name, member)                                                                                                           \
    QString OpenvpnAdvancedSetting::name() const                                                                                                               \
    {                                                                                                                                                          \
        return member;                                                                                                                                         \
    }                                                                                                                                                          \
    void OpenvpnAdvancedSetting::set##Name(const QString &value)                                                                                               \
    {                                                                                                                                                          \
        if (member == value) {                                                                                                                                 \
            return;                                                                                                                                            \
        }                                                                                                                                                      \
        member = value;                                                                                                                                        \
        Q_EMIT name##Changed();                                                                                                                                \
        Q_EMIT changed();                                                                                                                                      \
    }

ADVANCED_PROPERTY(bool, useCustomPort, UseCustomPort, m_useCustomPort)
ADVANCED_PROPERTY(int, customPort, CustomPort, m_customPort)
ADVANCED_PROPERTY(bool, useCustomReneg, UseCustomReneg, m_useCustomReneg)
ADVANCED_PROPERTY(int, customReneg, CustomReneg, m_customReneg)
ADVANCED_PROPERTY(bool, useCompression, UseCompression, m_useCompression)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::Compression, compression, Compression, m_compression)
ADVANCED_PROPERTY(bool, useAsymCompression, UseAsymCompression, m_useAsymCompression)
ADVANCED_PROPERTY(bool, useTcp, UseTcp, m_useTcp)
ADVANCED_PROPERTY(bool, useVirtualDeviceType, UseVirtualDeviceType, m_useVirtualDeviceType)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::DeviceType, deviceType, DeviceType, m_deviceType)
ADVANCED_PROPERTY(bool, useVirtualDeviceName, UseVirtualDeviceName, m_useVirtualDeviceName)
ADVANCED_STRING_PROPERTY(virtualDeviceName, VirtualDeviceName, m_virtualDeviceName)
ADVANCED_PROPERTY(bool, useMtu, UseMtu, m_useMtu)
ADVANCED_PROPERTY(int, mtu, Mtu, m_mtu)
ADVANCED_PROPERTY(bool, useCustomFragmentSize, UseCustomFragmentSize, m_useCustomFragmentSize)
ADVANCED_PROPERTY(int, customFragmentSize, CustomFragmentSize, m_customFragmentSize)
ADVANCED_PROPERTY(bool, mssRestrict, MssRestrict, m_mssRestrict)
ADVANCED_PROPERTY(bool, useMtuDisc, UseMtuDisc, m_useMtuDisc)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::MtuDisc, mtuDisc, MtuDisc, m_mtuDisc)
ADVANCED_PROPERTY(bool, randomizeRemoteHosts, RandomizeRemoteHosts, m_randomizeRemoteHosts)
ADVANCED_PROPERTY(bool, randomizeRemoteHostname, RandomizeRemoteHostname, m_randomizeRemoteHostname)
ADVANCED_PROPERTY(int, connectTimeout, ConnectTimeout, m_connectTimeout)
ADVANCED_PROPERTY(bool, allowPullFqdn, AllowPullFqdn, m_allowPullFqdn)
ADVANCED_PROPERTY(bool, pushPeerInfo, PushPeerInfo, m_pushPeerInfo)
ADVANCED_PROPERTY(bool, ipv6TunLink, Ipv6TunLink, m_ipv6TunLink)
ADVANCED_PROPERTY(bool, usePingInterval, UsePingInterval, m_usePingInterval)
ADVANCED_PROPERTY(int, pingInterval, PingInterval, m_pingInterval)
ADVANCED_PROPERTY(bool, useExitRestartPing, UseExitRestartPing, m_useExitRestartPing)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::PingMode, exitRestartPingMode, ExitRestartPingMode, m_exitRestartPingMode)
ADVANCED_PROPERTY(int, exitRestartPing, ExitRestartPing, m_exitRestartPing)
ADVANCED_PROPERTY(bool, acceptAuthenticatedPackets, AcceptAuthenticatedPackets, m_acceptAuthenticatedPackets)
ADVANCED_PROPERTY(bool, useMaxRoutes, UseMaxRoutes, m_useMaxRoutes)
ADVANCED_PROPERTY(int, maxRoutes, MaxRoutes, m_maxRoutes)

ADVANCED_STRING_PROPERTY(cipher, Cipher, m_cipher)
ADVANCED_STRING_PROPERTY(dataCiphers, DataCiphers, m_dataCiphers)
ADVANCED_STRING_PROPERTY(dataCiphersFallback, DataCiphersFallback, m_dataCiphersFallback)
ADVANCED_PROPERTY(bool, disableCipherNegotiation, DisableCipherNegotiation, m_disableCipherNegotiation)
ADVANCED_PROPERTY(bool, useCustomCipherKey, UseCustomCipherKey, m_useCustomCipherKey)
ADVANCED_PROPERTY(int, customCipherKey, CustomCipherKey, m_customCipherKey)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::HmacAuth, hmacAuth, HmacAuth, m_hmacAuth)

ADVANCED_PROPERTY(OpenvpnAdvancedSetting::CertCheckType, certCheckType, CertCheckType, m_certCheckType)
ADVANCED_STRING_PROPERTY(subjectMatch, SubjectMatch, m_subjectMatch)
ADVANCED_PROPERTY(bool, verifyRemoteCertTls, VerifyRemoteCertTls, m_verifyRemoteCertTls)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::PeerType, remoteCertTlsType, RemoteCertTlsType, m_remoteCertTlsType)
ADVANCED_PROPERTY(bool, verifyNsCertType, VerifyNsCertType, m_verifyNsCertType)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::PeerType, nsCertType, NsCertType, m_nsCertType)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::TlsMode, tlsMode, TlsMode, m_tlsMode)
ADVANCED_STRING_PROPERTY(tlsAuthKey, TlsAuthKey, m_tlsAuthKey)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::KeyDirection, tlsAuthDirection, TlsAuthDirection, m_tlsAuthDirection)
ADVANCED_STRING_PROPERTY(extraCerts, ExtraCerts, m_extraCerts)
ADVANCED_STRING_PROPERTY(tlsVersionMin, TlsVersionMin, m_tlsVersionMin)
ADVANCED_PROPERTY(bool, tlsVersionMinOrHighest, TlsVersionMinOrHighest, m_tlsVersionMinOrHighest)
ADVANCED_STRING_PROPERTY(tlsVersionMax, TlsVersionMax, m_tlsVersionMax)
ADVANCED_STRING_PROPERTY(crlVerifyFile, CrlVerifyFile, m_crlVerifyFile)
ADVANCED_STRING_PROPERTY(crlVerifyDir, CrlVerifyDir, m_crlVerifyDir)

ADVANCED_PROPERTY(OpenvpnAdvancedSetting::ProxyType, proxyType, ProxyType, m_proxyType)
ADVANCED_STRING_PROPERTY(proxyServer, ProxyServer, m_proxyServer)
ADVANCED_PROPERTY(int, proxyPort, ProxyPort, m_proxyPort)
ADVANCED_PROPERTY(bool, proxyRetry, ProxyRetry, m_proxyRetry)
ADVANCED_STRING_PROPERTY(proxyUsername, ProxyUsername, m_proxyUsername)
ADVANCED_STRING_PROPERTY(proxyPassword, ProxyPassword, m_proxyPassword)
ADVANCED_PROPERTY(OpenvpnAdvancedSetting::PasswordOption, proxyPasswordOption, ProxyPasswordOption, m_proxyPasswordOption)

#undef ADVANCED_PROPERTY
#undef ADVANCED_STRING_PROPERTY

#include "moc_openvpnadvanced.cpp"
