/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_OPENVPN_ADVANCED_QML_H
#define PLASMA_NM_OPENVPN_ADVANCED_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT OpenvpnAdvancedSetting : public QObject
{
    Q_OBJECT

    // General
    Q_PROPERTY(bool useCustomPort READ useCustomPort WRITE setUseCustomPort NOTIFY useCustomPortChanged)
    Q_PROPERTY(int customPort READ customPort WRITE setCustomPort NOTIFY customPortChanged)
    Q_PROPERTY(bool useCustomReneg READ useCustomReneg WRITE setUseCustomReneg NOTIFY useCustomRenegChanged)
    Q_PROPERTY(int customReneg READ customReneg WRITE setCustomReneg NOTIFY customRenegChanged)
    Q_PROPERTY(bool useCompression READ useCompression WRITE setUseCompression NOTIFY useCompressionChanged)
    Q_PROPERTY(Compression compression READ compression WRITE setCompression NOTIFY compressionChanged)
    Q_PROPERTY(bool useAsymCompression READ useAsymCompression WRITE setUseAsymCompression NOTIFY useAsymCompressionChanged)
    Q_PROPERTY(bool useTcp READ useTcp WRITE setUseTcp NOTIFY useTcpChanged)
    Q_PROPERTY(bool useVirtualDeviceType READ useVirtualDeviceType WRITE setUseVirtualDeviceType NOTIFY useVirtualDeviceTypeChanged)
    Q_PROPERTY(DeviceType deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(bool useVirtualDeviceName READ useVirtualDeviceName WRITE setUseVirtualDeviceName NOTIFY useVirtualDeviceNameChanged)
    Q_PROPERTY(QString virtualDeviceName READ virtualDeviceName WRITE setVirtualDeviceName NOTIFY virtualDeviceNameChanged)
    Q_PROPERTY(bool useMtu READ useMtu WRITE setUseMtu NOTIFY useMtuChanged)
    Q_PROPERTY(int mtu READ mtu WRITE setMtu NOTIFY mtuChanged)
    Q_PROPERTY(bool useCustomFragmentSize READ useCustomFragmentSize WRITE setUseCustomFragmentSize NOTIFY useCustomFragmentSizeChanged)
    Q_PROPERTY(int customFragmentSize READ customFragmentSize WRITE setCustomFragmentSize NOTIFY customFragmentSizeChanged)
    Q_PROPERTY(bool mssRestrict READ mssRestrict WRITE setMssRestrict NOTIFY mssRestrictChanged)
    Q_PROPERTY(bool useMtuDisc READ useMtuDisc WRITE setUseMtuDisc NOTIFY useMtuDiscChanged)
    Q_PROPERTY(MtuDisc mtuDisc READ mtuDisc WRITE setMtuDisc NOTIFY mtuDiscChanged)
    Q_PROPERTY(bool randomizeRemoteHosts READ randomizeRemoteHosts WRITE setRandomizeRemoteHosts NOTIFY randomizeRemoteHostsChanged)
    Q_PROPERTY(bool randomizeRemoteHostname READ randomizeRemoteHostname WRITE setRandomizeRemoteHostname NOTIFY randomizeRemoteHostnameChanged)
    Q_PROPERTY(int connectTimeout READ connectTimeout WRITE setConnectTimeout NOTIFY connectTimeoutChanged)
    Q_PROPERTY(bool allowPullFqdn READ allowPullFqdn WRITE setAllowPullFqdn NOTIFY allowPullFqdnChanged)
    Q_PROPERTY(bool pushPeerInfo READ pushPeerInfo WRITE setPushPeerInfo NOTIFY pushPeerInfoChanged)
    Q_PROPERTY(bool ipv6TunLink READ ipv6TunLink WRITE setIpv6TunLink NOTIFY ipv6TunLinkChanged)
    Q_PROPERTY(bool usePingInterval READ usePingInterval WRITE setUsePingInterval NOTIFY usePingIntervalChanged)
    Q_PROPERTY(int pingInterval READ pingInterval WRITE setPingInterval NOTIFY pingIntervalChanged)
    Q_PROPERTY(bool useExitRestartPing READ useExitRestartPing WRITE setUseExitRestartPing NOTIFY useExitRestartPingChanged)
    Q_PROPERTY(PingMode exitRestartPingMode READ exitRestartPingMode WRITE setExitRestartPingMode NOTIFY exitRestartPingModeChanged)
    Q_PROPERTY(int exitRestartPing READ exitRestartPing WRITE setExitRestartPing NOTIFY exitRestartPingChanged)
    Q_PROPERTY(bool acceptAuthenticatedPackets READ acceptAuthenticatedPackets WRITE setAcceptAuthenticatedPackets NOTIFY acceptAuthenticatedPacketsChanged)
    Q_PROPERTY(bool useMaxRoutes READ useMaxRoutes WRITE setUseMaxRoutes NOTIFY useMaxRoutesChanged)
    Q_PROPERTY(int maxRoutes READ maxRoutes WRITE setMaxRoutes NOTIFY maxRoutesChanged)

    // Security. The cipher list comes from the openvpn binary, so it arrives
    // after probe() rather than being known up front.
    Q_PROPERTY(QStringList availableCiphers READ availableCiphers NOTIFY availableCiphersChanged)
    Q_PROPERTY(QString cipher READ cipher WRITE setCipher NOTIFY cipherChanged)
    Q_PROPERTY(QString dataCiphers READ dataCiphers WRITE setDataCiphers NOTIFY dataCiphersChanged)
    Q_PROPERTY(QString dataCiphersFallback READ dataCiphersFallback WRITE setDataCiphersFallback NOTIFY dataCiphersFallbackChanged)
    Q_PROPERTY(bool disableCipherNegotiation READ disableCipherNegotiation WRITE setDisableCipherNegotiation NOTIFY disableCipherNegotiationChanged)
    Q_PROPERTY(bool useCustomCipherKey READ useCustomCipherKey WRITE setUseCustomCipherKey NOTIFY useCustomCipherKeyChanged)
    Q_PROPERTY(int customCipherKey READ customCipherKey WRITE setCustomCipherKey NOTIFY customCipherKeyChanged)
    Q_PROPERTY(HmacAuth hmacAuth READ hmacAuth WRITE setHmacAuth NOTIFY hmacAuthChanged)

    // TLS Settings
    Q_PROPERTY(CertCheckType certCheckType READ certCheckType WRITE setCertCheckType NOTIFY certCheckTypeChanged)
    // tls-remote was dropped in OpenVPN 2.4, so the legacy check is only
    // offered when an older binary is found.
    Q_PROPERTY(bool supportsLegacySubjectMatch READ supportsLegacySubjectMatch NOTIFY supportsLegacySubjectMatchChanged)
    Q_PROPERTY(QString subjectMatch READ subjectMatch WRITE setSubjectMatch NOTIFY subjectMatchChanged)
    Q_PROPERTY(bool verifyRemoteCertTls READ verifyRemoteCertTls WRITE setVerifyRemoteCertTls NOTIFY verifyRemoteCertTlsChanged)
    Q_PROPERTY(PeerType remoteCertTlsType READ remoteCertTlsType WRITE setRemoteCertTlsType NOTIFY remoteCertTlsTypeChanged)
    Q_PROPERTY(bool verifyNsCertType READ verifyNsCertType WRITE setVerifyNsCertType NOTIFY verifyNsCertTypeChanged)
    Q_PROPERTY(PeerType nsCertType READ nsCertType WRITE setNsCertType NOTIFY nsCertTypeChanged)
    Q_PROPERTY(TlsMode tlsMode READ tlsMode WRITE setTlsMode NOTIFY tlsModeChanged)
    Q_PROPERTY(QString tlsAuthKey READ tlsAuthKey WRITE setTlsAuthKey NOTIFY tlsAuthKeyChanged)
    Q_PROPERTY(KeyDirection tlsAuthDirection READ tlsAuthDirection WRITE setTlsAuthDirection NOTIFY tlsAuthDirectionChanged)
    Q_PROPERTY(QString extraCerts READ extraCerts WRITE setExtraCerts NOTIFY extraCertsChanged)
    Q_PROPERTY(QString tlsVersionMin READ tlsVersionMin WRITE setTlsVersionMin NOTIFY tlsVersionMinChanged)
    Q_PROPERTY(bool tlsVersionMinOrHighest READ tlsVersionMinOrHighest WRITE setTlsVersionMinOrHighest NOTIFY tlsVersionMinOrHighestChanged)
    Q_PROPERTY(QString tlsVersionMax READ tlsVersionMax WRITE setTlsVersionMax NOTIFY tlsVersionMaxChanged)
    Q_PROPERTY(QString crlVerifyFile READ crlVerifyFile WRITE setCrlVerifyFile NOTIFY crlVerifyFileChanged)
    Q_PROPERTY(QString crlVerifyDir READ crlVerifyDir WRITE setCrlVerifyDir NOTIFY crlVerifyDirChanged)

    // Proxies
    Q_PROPERTY(ProxyType proxyType READ proxyType WRITE setProxyType NOTIFY proxyTypeChanged)
    Q_PROPERTY(QString proxyServer READ proxyServer WRITE setProxyServer NOTIFY proxyServerChanged)
    Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged)
    Q_PROPERTY(bool proxyRetry READ proxyRetry WRITE setProxyRetry NOTIFY proxyRetryChanged)
    Q_PROPERTY(QString proxyUsername READ proxyUsername WRITE setProxyUsername NOTIFY proxyUsernameChanged)
    Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged)
    Q_PROPERTY(PasswordOption proxyPasswordOption READ proxyPasswordOption WRITE setProxyPasswordOption NOTIFY proxyPasswordOptionChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    enum Compression {
        NoCompression = 0,
        Lzo,
        Lz4,
        Lz4v2,
        Adaptive,
        Automatic
    };
    Q_ENUM(Compression)

    enum DeviceType {
        Tun = 0,
        Tap
    };
    Q_ENUM(DeviceType)

    enum MtuDisc {
        MtuDiscNo = 0,
        MtuDiscMaybe,
        MtuDiscYes
    };
    Q_ENUM(MtuDisc)

    enum PingMode {
        PingExit = 0,
        PingRestart
    };
    Q_ENUM(PingMode)

    enum HmacAuth {
        HmacDefault = 0,
        HmacNone,
        HmacMd4,
        HmacMd5,
        HmacSha1,
        HmacSha224,
        HmacSha256,
        HmacSha384,
        HmacSha512,
        HmacRipemd160
    };
    Q_ENUM(HmacAuth)

    enum CertCheckType {
        DontVerify = 0,
        VerifyWholeSubjectExactly,
        VerifyNameExactly,
        VerifyNameByPrefix,
        VerifySubjectPartially
    };
    Q_ENUM(CertCheckType)

    enum PeerType {
        Server = 0,
        Client
    };
    Q_ENUM(PeerType)

    enum TlsMode {
        TlsNone = 0,
        TlsAuth,
        TlsCrypt,
        TlsCryptV2
    };
    Q_ENUM(TlsMode)

    enum KeyDirection {
        NoDirection = 0,
        Direction0,
        Direction1
    };
    Q_ENUM(KeyDirection)

    enum ProxyType {
        ProxyNotRequired = 0,
        ProxyHttp,
        ProxySocks
    };
    Q_ENUM(ProxyType)

    explicit OpenvpnAdvancedSetting(QObject *parent = nullptr);
    ~OpenvpnAdvancedSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);

    // The keys this dialog owns, for the main setting to merge into its own.
    NMStringMap data() const;
    NMStringMap secrets() const;

    // Asks the openvpn binary for its cipher list and version. Spawning two
    // processes is not worth doing until the dialog is opened, so the dialog
    // calls this and the results arrive through the change signals.
    Q_INVOKABLE void probe();

    bool useCustomPort() const;
    void setUseCustomPort(bool use);

    int customPort() const;
    void setCustomPort(int port);

    bool useCustomReneg() const;
    void setUseCustomReneg(bool use);

    int customReneg() const;
    void setCustomReneg(int seconds);

    bool useCompression() const;
    void setUseCompression(bool use);

    Compression compression() const;
    void setCompression(Compression compression);

    bool useAsymCompression() const;
    void setUseAsymCompression(bool use);

    bool useTcp() const;
    void setUseTcp(bool use);

    bool useVirtualDeviceType() const;
    void setUseVirtualDeviceType(bool use);

    DeviceType deviceType() const;
    void setDeviceType(DeviceType type);

    bool useVirtualDeviceName() const;
    void setUseVirtualDeviceName(bool use);

    QString virtualDeviceName() const;
    void setVirtualDeviceName(const QString &name);

    bool useMtu() const;
    void setUseMtu(bool use);

    int mtu() const;
    void setMtu(int mtu);

    bool useCustomFragmentSize() const;
    void setUseCustomFragmentSize(bool use);

    int customFragmentSize() const;
    void setCustomFragmentSize(int size);

    bool mssRestrict() const;
    void setMssRestrict(bool restrict);

    bool useMtuDisc() const;
    void setUseMtuDisc(bool use);

    MtuDisc mtuDisc() const;
    void setMtuDisc(MtuDisc mtuDisc);

    bool randomizeRemoteHosts() const;
    void setRandomizeRemoteHosts(bool randomize);

    bool randomizeRemoteHostname() const;
    void setRandomizeRemoteHostname(bool randomize);

    int connectTimeout() const;
    void setConnectTimeout(int seconds);

    bool allowPullFqdn() const;
    void setAllowPullFqdn(bool allow);

    bool pushPeerInfo() const;
    void setPushPeerInfo(bool push);

    bool ipv6TunLink() const;
    void setIpv6TunLink(bool use);

    bool usePingInterval() const;
    void setUsePingInterval(bool use);

    int pingInterval() const;
    void setPingInterval(int seconds);

    bool useExitRestartPing() const;
    void setUseExitRestartPing(bool use);

    PingMode exitRestartPingMode() const;
    void setExitRestartPingMode(PingMode mode);

    int exitRestartPing() const;
    void setExitRestartPing(int seconds);

    bool acceptAuthenticatedPackets() const;
    void setAcceptAuthenticatedPackets(bool accept);

    bool useMaxRoutes() const;
    void setUseMaxRoutes(bool use);

    int maxRoutes() const;
    void setMaxRoutes(int routes);

    QStringList availableCiphers() const;

    QString cipher() const;
    void setCipher(const QString &cipher);

    QString dataCiphers() const;
    void setDataCiphers(const QString &ciphers);

    QString dataCiphersFallback() const;
    void setDataCiphersFallback(const QString &ciphers);

    bool disableCipherNegotiation() const;
    void setDisableCipherNegotiation(bool disable);

    bool useCustomCipherKey() const;
    void setUseCustomCipherKey(bool use);

    int customCipherKey() const;
    void setCustomCipherKey(int size);

    HmacAuth hmacAuth() const;
    void setHmacAuth(HmacAuth auth);

    CertCheckType certCheckType() const;
    void setCertCheckType(CertCheckType type);

    bool supportsLegacySubjectMatch() const;

    QString subjectMatch() const;
    void setSubjectMatch(const QString &subjectMatch);

    bool verifyRemoteCertTls() const;
    void setVerifyRemoteCertTls(bool verify);

    PeerType remoteCertTlsType() const;
    void setRemoteCertTlsType(PeerType type);

    bool verifyNsCertType() const;
    void setVerifyNsCertType(bool verify);

    PeerType nsCertType() const;
    void setNsCertType(PeerType type);

    TlsMode tlsMode() const;
    void setTlsMode(TlsMode mode);

    QString tlsAuthKey() const;
    void setTlsAuthKey(const QString &key);

    KeyDirection tlsAuthDirection() const;
    void setTlsAuthDirection(KeyDirection direction);

    QString extraCerts() const;
    void setExtraCerts(const QString &extraCerts);

    QString tlsVersionMin() const;
    void setTlsVersionMin(const QString &version);

    bool tlsVersionMinOrHighest() const;
    void setTlsVersionMinOrHighest(bool orHighest);

    QString tlsVersionMax() const;
    void setTlsVersionMax(const QString &version);

    QString crlVerifyFile() const;
    void setCrlVerifyFile(const QString &file);

    QString crlVerifyDir() const;
    void setCrlVerifyDir(const QString &dir);

    ProxyType proxyType() const;
    void setProxyType(ProxyType type);

    QString proxyServer() const;
    void setProxyServer(const QString &server);

    int proxyPort() const;
    void setProxyPort(int port);

    bool proxyRetry() const;
    void setProxyRetry(bool retry);

    QString proxyUsername() const;
    void setProxyUsername(const QString &username);

    QString proxyPassword() const;
    void setProxyPassword(const QString &password);

    PasswordOption proxyPasswordOption() const;
    void setProxyPasswordOption(PasswordOption option);

Q_SIGNALS:
    void useCustomPortChanged();
    void customPortChanged();
    void useCustomRenegChanged();
    void customRenegChanged();
    void useCompressionChanged();
    void compressionChanged();
    void useAsymCompressionChanged();
    void useTcpChanged();
    void useVirtualDeviceTypeChanged();
    void deviceTypeChanged();
    void useVirtualDeviceNameChanged();
    void virtualDeviceNameChanged();
    void useMtuChanged();
    void mtuChanged();
    void useCustomFragmentSizeChanged();
    void customFragmentSizeChanged();
    void mssRestrictChanged();
    void useMtuDiscChanged();
    void mtuDiscChanged();
    void randomizeRemoteHostsChanged();
    void randomizeRemoteHostnameChanged();
    void connectTimeoutChanged();
    void allowPullFqdnChanged();
    void pushPeerInfoChanged();
    void ipv6TunLinkChanged();
    void usePingIntervalChanged();
    void pingIntervalChanged();
    void useExitRestartPingChanged();
    void exitRestartPingModeChanged();
    void exitRestartPingChanged();
    void acceptAuthenticatedPacketsChanged();
    void useMaxRoutesChanged();
    void maxRoutesChanged();

    void availableCiphersChanged();
    void cipherChanged();
    void dataCiphersChanged();
    void dataCiphersFallbackChanged();
    void disableCipherNegotiationChanged();
    void useCustomCipherKeyChanged();
    void customCipherKeyChanged();
    void hmacAuthChanged();

    void certCheckTypeChanged();
    void supportsLegacySubjectMatchChanged();
    void subjectMatchChanged();
    void verifyRemoteCertTlsChanged();
    void remoteCertTlsTypeChanged();
    void verifyNsCertTypeChanged();
    void nsCertTypeChanged();
    void tlsModeChanged();
    void tlsAuthKeyChanged();
    void tlsAuthDirectionChanged();
    void extraCertsChanged();
    void tlsVersionMinChanged();
    void tlsVersionMinOrHighestChanged();
    void tlsVersionMaxChanged();
    void crlVerifyFileChanged();
    void crlVerifyDirChanged();

    void proxyTypeChanged();
    void proxyServerChanged();
    void proxyPortChanged();
    void proxyRetryChanged();
    void proxyUsernameChanged();
    void proxyPasswordChanged();
    void proxyPasswordOptionChanged();

    // Raised by any change, so the main setting can pass validity on to the KCM.
    void changed();

private:
    void setAvailableCiphers(const QStringList &ciphers);
    void setSupportsLegacySubjectMatch(bool supports);

    bool m_useCustomPort = false;
    int m_customPort = 1194;
    bool m_useCustomReneg = false;
    int m_customReneg = 0;
    bool m_useCompression = false;
    Compression m_compression = NoCompression;
    bool m_useAsymCompression = false;
    bool m_useTcp = false;
    bool m_useVirtualDeviceType = false;
    DeviceType m_deviceType = Tun;
    bool m_useVirtualDeviceName = false;
    QString m_virtualDeviceName;
    bool m_useMtu = false;
    int m_mtu = 1500;
    bool m_useCustomFragmentSize = false;
    int m_customFragmentSize = 1300;
    bool m_mssRestrict = false;
    bool m_useMtuDisc = false;
    MtuDisc m_mtuDisc = MtuDiscNo;
    bool m_randomizeRemoteHosts = false;
    bool m_randomizeRemoteHostname = false;
    int m_connectTimeout = 0;
    bool m_allowPullFqdn = false;
    bool m_pushPeerInfo = false;
    bool m_ipv6TunLink = false;
    bool m_usePingInterval = false;
    int m_pingInterval = 30;
    bool m_useExitRestartPing = false;
    PingMode m_exitRestartPingMode = PingExit;
    int m_exitRestartPing = 30;
    bool m_acceptAuthenticatedPackets = false;
    bool m_useMaxRoutes = false;
    int m_maxRoutes = 100;

    QStringList m_availableCiphers;
    QString m_cipher;
    QString m_dataCiphers;
    QString m_dataCiphersFallback;
    bool m_disableCipherNegotiation = false;
    bool m_useCustomCipherKey = false;
    int m_customCipherKey = 128;
    HmacAuth m_hmacAuth = HmacDefault;

    CertCheckType m_certCheckType = DontVerify;
    bool m_supportsLegacySubjectMatch = true;
    QString m_subjectMatch;
    bool m_verifyRemoteCertTls = false;
    PeerType m_remoteCertTlsType = Server;
    bool m_verifyNsCertType = false;
    PeerType m_nsCertType = Server;
    TlsMode m_tlsMode = TlsNone;
    QString m_tlsAuthKey;
    KeyDirection m_tlsAuthDirection = NoDirection;
    QString m_extraCerts;
    QString m_tlsVersionMin;
    bool m_tlsVersionMinOrHighest = false;
    QString m_tlsVersionMax;
    QString m_crlVerifyFile;
    QString m_crlVerifyDir;

    ProxyType m_proxyType = ProxyNotRequired;
    QString m_proxyServer;
    int m_proxyPort = 0;
    bool m_proxyRetry = false;
    QString m_proxyUsername;
    QString m_proxyPassword;
    PasswordOption m_proxyPasswordOption = StoreForUser;

    bool m_probed = false;
};

#endif // PLASMA_NM_OPENVPN_ADVANCED_QML_H
