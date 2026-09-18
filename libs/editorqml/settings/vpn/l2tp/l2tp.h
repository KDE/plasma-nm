/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_L2TP_QML_H
#define PLASMA_NM_L2TP_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QUrl>

class PLASMANM_EDITORQML_EXPORT L2tpSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    // General
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(AuthType authType READ authType WRITE setAuthType NOTIFY authTypeChanged)

    // Authentication - password
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(PasswordOption passwordOption READ passwordOption WRITE setPasswordOption NOTIFY passwordOptionChanged)
    Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)

    // Authentication - certificates
    Q_PROPERTY(QString userCa READ userCa WRITE setUserCa NOTIFY userCaChanged)
    Q_PROPERTY(QString userCert READ userCert WRITE setUserCert NOTIFY userCertChanged)
    Q_PROPERTY(QString userKey READ userKey WRITE setUserKey NOTIFY userKeyChanged)
    Q_PROPERTY(QString userKeyPassword READ userKeyPassword WRITE setUserKeyPassword NOTIFY userKeyPasswordChanged)
    Q_PROPERTY(PasswordOption userKeyPasswordOption READ userKeyPasswordOption WRITE setUserKeyPasswordOption NOTIFY userKeyPasswordOptionChanged)

    Q_PROPERTY(bool ephemeralPort READ ephemeralPort WRITE setEphemeralPort NOTIFY ephemeralPortChanged)

    Q_PROPERTY(bool ipsecDaemonAvailable READ ipsecDaemonAvailable CONSTANT)
    Q_PROPERTY(bool ipsecSupportsPfs READ ipsecSupportsPfs CONSTANT)

    // IPsec - machine authentication
    Q_PROPERTY(bool ipsecEnabled READ ipsecEnabled WRITE setIpsecEnabled NOTIFY ipsecEnabledChanged)
    Q_PROPERTY(MachineAuthType machineAuthType READ machineAuthType WRITE setMachineAuthType NOTIFY machineAuthTypeChanged)

    Q_PROPERTY(QString presharedKey READ presharedKey WRITE setPresharedKey NOTIFY presharedKeyChanged)
    Q_PROPERTY(PasswordOption presharedKeyOption READ presharedKeyOption WRITE setPresharedKeyOption NOTIFY presharedKeyOptionChanged)

    Q_PROPERTY(QString machineCa READ machineCa WRITE setMachineCa NOTIFY machineCaChanged)
    Q_PROPERTY(QString machineCert READ machineCert WRITE setMachineCert NOTIFY machineCertChanged)
    Q_PROPERTY(QString machineKey READ machineKey WRITE setMachineKey NOTIFY machineKeyChanged)
    Q_PROPERTY(QString machineKeyPassword READ machineKeyPassword WRITE setMachineKeyPassword NOTIFY machineKeyPasswordChanged)
    Q_PROPERTY(PasswordOption machineKeyPasswordOption READ machineKeyPasswordOption WRITE setMachineKeyPasswordOption NOTIFY machineKeyPasswordOptionChanged)

    // IPsec - advanced
    Q_PROPERTY(QString remoteId READ remoteId WRITE setRemoteId NOTIFY remoteIdChanged)
    Q_PROPERTY(QString ipsecIke READ ipsecIke WRITE setIpsecIke NOTIFY ipsecIkeChanged)
    Q_PROPERTY(QString ipsecEsp READ ipsecEsp WRITE setIpsecEsp NOTIFY ipsecEspChanged)

    Q_PROPERTY(bool useIkeLifetime READ useIkeLifetime WRITE setUseIkeLifetime NOTIFY useIkeLifetimeChanged)
    Q_PROPERTY(int ikeLifetime READ ikeLifetime WRITE setIkeLifetime NOTIFY ikeLifetimeChanged)
    Q_PROPERTY(bool useSaLifetime READ useSaLifetime WRITE setUseSaLifetime NOTIFY useSaLifetimeChanged)
    Q_PROPERTY(int saLifetime READ saLifetime WRITE setSaLifetime NOTIFY saLifetimeChanged)

    Q_PROPERTY(bool enforceUdpEncapsulation READ enforceUdpEncapsulation WRITE setEnforceUdpEncapsulation NOTIFY enforceUdpEncapsulationChanged)
    Q_PROPERTY(bool useIpCompression READ useIpCompression WRITE setUseIpCompression NOTIFY useIpCompressionChanged)
    Q_PROPERTY(bool useIkev2 READ useIkev2 WRITE setUseIkev2 NOTIFY useIkev2Changed)
    Q_PROPERTY(bool disablePfs READ disablePfs WRITE setDisablePfs NOTIFY disablePfsChanged)

    // PPP - allowed authentication methods
    Q_PROPERTY(bool allowPap READ allowPap WRITE setAllowPap NOTIFY allowPapChanged)
    Q_PROPERTY(bool allowChap READ allowChap WRITE setAllowChap NOTIFY allowChapChanged)
    Q_PROPERTY(bool allowMschap READ allowMschap WRITE setAllowMschap NOTIFY allowMschapChanged)
    Q_PROPERTY(bool allowMschapv2 READ allowMschapv2 WRITE setAllowMschapv2 NOTIFY allowMschapv2Changed)
    Q_PROPERTY(bool allowEap READ allowEap WRITE setAllowEap NOTIFY allowEapChanged)

    // PPP - MPPE encryption
    Q_PROPERTY(bool useMppe READ useMppe WRITE setUseMppe NOTIFY useMppeChanged)
    Q_PROPERTY(MppeCrypto mppeCrypto READ mppeCrypto WRITE setMppeCrypto NOTIFY mppeCryptoChanged)
    Q_PROPERTY(bool statefulEncryption READ statefulEncryption WRITE setStatefulEncryption NOTIFY statefulEncryptionChanged)

    // PPP - compression
    Q_PROPERTY(bool allowBsdCompression READ allowBsdCompression WRITE setAllowBsdCompression NOTIFY allowBsdCompressionChanged)
    Q_PROPERTY(bool allowDeflateCompression READ allowDeflateCompression WRITE setAllowDeflateCompression NOTIFY allowDeflateCompressionChanged)
    Q_PROPERTY(bool allowTcpHeaderCompression READ allowTcpHeaderCompression WRITE setAllowTcpHeaderCompression NOTIFY allowTcpHeaderCompressionChanged)
    Q_PROPERTY(bool useProtocolFieldCompression READ useProtocolFieldCompression WRITE setUseProtocolFieldCompression NOTIFY useProtocolFieldCompressionChanged)
    Q_PROPERTY(
        bool useAddressControlCompression READ useAddressControlCompression WRITE setUseAddressControlCompression NOTIFY useAddressControlCompressionChanged)

    // PPP - echo
    Q_PROPERTY(bool sendPppEchoPackets READ sendPppEchoPackets WRITE setSendPppEchoPackets NOTIFY sendPppEchoPacketsChanged)

    // PPP - other
    Q_PROPERTY(bool useMrru READ useMrru WRITE setUseMrru NOTIFY useMrruChanged)
    Q_PROPERTY(int mrru READ mrru WRITE setMrru NOTIFY mrruChanged)
    Q_PROPERTY(int mru READ mru WRITE setMru NOTIFY mruChanged)
    Q_PROPERTY(int mtu READ mtu WRITE setMtu NOTIFY mtuChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    enum AuthType {
        PasswordAuth = 0,
        TlsAuth
    };
    Q_ENUM(AuthType)

    enum MachineAuthType {
        PresharedKeyAuth = 0,
        MachineTlsAuth
    };
    Q_ENUM(MachineAuthType)

    enum MppeCrypto {
        MppeAny = 0,
        Mppe128,
        Mppe40
    };
    Q_ENUM(MppeCrypto)

    explicit L2tpSetting(QObject *parent = nullptr);
    ~L2tpSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    AuthType authType() const;
    void setAuthType(AuthType type);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    PasswordOption passwordOption() const;
    void setPasswordOption(PasswordOption option);

    QString domain() const;
    void setDomain(const QString &domain);

    QString userCa() const;
    void setUserCa(const QString &userCa);

    QString userCert() const;
    void setUserCert(const QString &userCert);

    QString userKey() const;
    void setUserKey(const QString &userKey);

    QString userKeyPassword() const;
    void setUserKeyPassword(const QString &password);

    PasswordOption userKeyPasswordOption() const;
    void setUserKeyPasswordOption(PasswordOption option);

    bool ephemeralPort() const;
    void setEphemeralPort(bool ephemeral);

    bool ipsecDaemonAvailable() const;
    bool ipsecSupportsPfs() const;

    bool ipsecEnabled() const;
    void setIpsecEnabled(bool enabled);

    MachineAuthType machineAuthType() const;
    void setMachineAuthType(MachineAuthType type);

    QString presharedKey() const;
    void setPresharedKey(const QString &key);

    PasswordOption presharedKeyOption() const;
    void setPresharedKeyOption(PasswordOption option);

    QString machineCa() const;
    void setMachineCa(const QString &machineCa);

    QString machineCert() const;
    void setMachineCert(const QString &machineCert);

    QString machineKey() const;
    void setMachineKey(const QString &machineKey);

    QString machineKeyPassword() const;
    void setMachineKeyPassword(const QString &password);

    PasswordOption machineKeyPasswordOption() const;
    void setMachineKeyPasswordOption(PasswordOption option);

    QString remoteId() const;
    void setRemoteId(const QString &remoteId);

    QString ipsecIke() const;
    void setIpsecIke(const QString &ike);

    QString ipsecEsp() const;
    void setIpsecEsp(const QString &esp);

    bool useIkeLifetime() const;
    void setUseIkeLifetime(bool use);

    int ikeLifetime() const;
    void setIkeLifetime(int seconds);

    bool useSaLifetime() const;
    void setUseSaLifetime(bool use);

    int saLifetime() const;
    void setSaLifetime(int seconds);

    bool enforceUdpEncapsulation() const;
    void setEnforceUdpEncapsulation(bool enforce);

    bool useIpCompression() const;
    void setUseIpCompression(bool use);

    bool useIkev2() const;
    void setUseIkev2(bool use);

    bool disablePfs() const;
    void setDisablePfs(bool disable);

    bool allowPap() const;
    void setAllowPap(bool allow);

    bool allowChap() const;
    void setAllowChap(bool allow);

    bool allowMschap() const;
    void setAllowMschap(bool allow);

    bool allowMschapv2() const;
    void setAllowMschapv2(bool allow);

    bool allowEap() const;
    void setAllowEap(bool allow);

    bool useMppe() const;
    void setUseMppe(bool use);

    MppeCrypto mppeCrypto() const;
    void setMppeCrypto(MppeCrypto crypto);

    bool statefulEncryption() const;
    void setStatefulEncryption(bool stateful);

    bool allowBsdCompression() const;
    void setAllowBsdCompression(bool allow);

    bool allowDeflateCompression() const;
    void setAllowDeflateCompression(bool allow);

    bool allowTcpHeaderCompression() const;
    void setAllowTcpHeaderCompression(bool allow);

    bool useProtocolFieldCompression() const;
    void setUseProtocolFieldCompression(bool use);

    bool useAddressControlCompression() const;
    void setUseAddressControlCompression(bool use);

    bool sendPppEchoPackets() const;
    void setSendPppEchoPackets(bool send);

    bool useMrru() const;
    void setUseMrru(bool use);

    int mrru() const;
    void setMrru(int mrru);

    int mru() const;
    void setMru(int mru);

    int mtu() const;
    void setMtu(int mtu);

Q_SIGNALS:
    void gatewayChanged();
    void authTypeChanged();

    void usernameChanged();
    void passwordChanged();
    void passwordOptionChanged();
    void domainChanged();

    void userCaChanged();
    void userCertChanged();
    void userKeyChanged();
    void userKeyPasswordChanged();
    void userKeyPasswordOptionChanged();

    void ephemeralPortChanged();

    void ipsecEnabledChanged();
    void machineAuthTypeChanged();

    void presharedKeyChanged();
    void presharedKeyOptionChanged();

    void machineCaChanged();
    void machineCertChanged();
    void machineKeyChanged();
    void machineKeyPasswordChanged();
    void machineKeyPasswordOptionChanged();

    void remoteIdChanged();
    void ipsecIkeChanged();
    void ipsecEspChanged();

    void useIkeLifetimeChanged();
    void ikeLifetimeChanged();
    void useSaLifetimeChanged();
    void saLifetimeChanged();

    void enforceUdpEncapsulationChanged();
    void useIpCompressionChanged();
    void useIkev2Changed();
    void disablePfsChanged();

    void allowPapChanged();
    void allowChapChanged();
    void allowMschapChanged();
    void allowMschapv2Changed();
    void allowEapChanged();

    void useMppeChanged();
    void mppeCryptoChanged();
    void statefulEncryptionChanged();

    void allowBsdCompressionChanged();
    void allowDeflateCompressionChanged();
    void allowTcpHeaderCompressionChanged();
    void useProtocolFieldCompressionChanged();
    void useAddressControlCompressionChanged();

    void sendPppEchoPacketsChanged();

    void useMrruChanged();
    void mrruChanged();
    void mruChanged();
    void mtuChanged();

    void validChanged();

private:
    QString m_gateway;
    AuthType m_authType = PasswordAuth;

    QString m_username;
    QString m_password;
    PasswordOption m_passwordOption = StoreForUser;
    QString m_domain;

    QString m_userCa;
    QString m_userCert;
    QString m_userKey;
    QString m_userKeyPassword;
    PasswordOption m_userKeyPasswordOption = StoreForUser;

    bool m_ephemeralPort = false;

    bool m_ipsecEnabled = false;
    MachineAuthType m_machineAuthType = PresharedKeyAuth;

    QString m_presharedKey;
    PasswordOption m_presharedKeyOption = StoreForUser;

    QString m_machineCa;
    QString m_machineCert;
    QString m_machineKey;
    QString m_machineKeyPassword;
    PasswordOption m_machineKeyPasswordOption = StoreForUser;

    QString m_remoteId;
    QString m_ipsecIke;
    QString m_ipsecEsp;

    bool m_useIkeLifetime = false;
    int m_ikeLifetime;
    bool m_useSaLifetime = false;
    int m_saLifetime;

    bool m_enforceUdpEncapsulation = false;
    bool m_useIpCompression = false;
    bool m_useIkev2 = false;
    bool m_disablePfs = false;

    bool m_allowPap = true;
    bool m_allowChap = true;
    bool m_allowMschap = true;
    bool m_allowMschapv2 = true;
    bool m_allowEap = true;

    bool m_useMppe = false;
    MppeCrypto m_mppeCrypto = MppeAny;
    bool m_statefulEncryption = false;

    bool m_allowBsdCompression = true;
    bool m_allowDeflateCompression = true;
    bool m_allowTcpHeaderCompression = true;
    bool m_useProtocolFieldCompression = true;
    bool m_useAddressControlCompression = true;

    bool m_sendPppEchoPackets = false;

    bool m_useMrru = false;
    int m_mrru = 1600;
    int m_mru = 0;
    int m_mtu = 0;
};

#endif // PLASMA_NM_L2TP_QML_H
