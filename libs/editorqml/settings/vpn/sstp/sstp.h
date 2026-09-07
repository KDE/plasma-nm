/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_SSTP_QML_H
#define PLASMA_NM_SSTP_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QUrl>

class PLASMANM_EDITORQML_EXPORT SstpSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    // General
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)

    // Optional
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString sstpPassword READ sstpPassword WRITE setSstpPassword NOTIFY sstpPasswordChanged)
    Q_PROPERTY(PasswordOption sstpPasswordOption READ sstpPasswordOption WRITE setSstpPasswordOption NOTIFY sstpPasswordOptionChanged)
    Q_PROPERTY(QString ntDomain READ ntDomain WRITE setNtDomain NOTIFY ntDomainChanged)
    Q_PROPERTY(QString caCert READ caCert WRITE setCaCert NOTIFY caCertChanged)
    Q_PROPERTY(bool ignoreCertificateWarnings READ ignoreCertificateWarnings WRITE setIgnoreCertificateWarnings NOTIFY ignoreCertificateWarningsChanged)

    // Advanced - Point-to-Point: allowed authentication methods
    Q_PROPERTY(bool allowPap READ allowPap WRITE setAllowPap NOTIFY allowPapChanged)
    Q_PROPERTY(bool allowChap READ allowChap WRITE setAllowChap NOTIFY allowChapChanged)
    Q_PROPERTY(bool allowMschap READ allowMschap WRITE setAllowMschap NOTIFY allowMschapChanged)
    Q_PROPERTY(bool allowMschapv2 READ allowMschapv2 WRITE setAllowMschapv2 NOTIFY allowMschapv2Changed)
    Q_PROPERTY(bool allowEap READ allowEap WRITE setAllowEap NOTIFY allowEapChanged)

    // Advanced - Point-to-Point: MPPE encryption
    Q_PROPERTY(bool useMppe READ useMppe WRITE setUseMppe NOTIFY useMppeChanged)
    Q_PROPERTY(MppeCrypto mppeCrypto READ mppeCrypto WRITE setMppeCrypto NOTIFY mppeCryptoChanged)
    Q_PROPERTY(bool statefulEncryption READ statefulEncryption WRITE setStatefulEncryption NOTIFY statefulEncryptionChanged)

    // Advanced - Point-to-Point: compression
    Q_PROPERTY(bool allowBsdCompression READ allowBsdCompression WRITE setAllowBsdCompression NOTIFY allowBsdCompressionChanged)
    Q_PROPERTY(bool allowDeflateCompression READ allowDeflateCompression WRITE setAllowDeflateCompression NOTIFY allowDeflateCompressionChanged)
    Q_PROPERTY(bool allowTcpHeaderCompression READ allowTcpHeaderCompression WRITE setAllowTcpHeaderCompression NOTIFY allowTcpHeaderCompressionChanged)

    // Advanced - Point-to-Point: echo
    Q_PROPERTY(bool sendPppEchoPackets READ sendPppEchoPackets WRITE setSendPppEchoPackets NOTIFY sendPppEchoPacketsChanged)

    // Advanced - Point-to-Point: misc
    Q_PROPERTY(bool useCustomUnitNumber READ useCustomUnitNumber WRITE setUseCustomUnitNumber NOTIFY useCustomUnitNumberChanged)
    Q_PROPERTY(int customUnitNumber READ customUnitNumber WRITE setCustomUnitNumber NOTIFY customUnitNumberChanged)

    // Advanced - Proxy
    Q_PROPERTY(QString proxyAddress READ proxyAddress WRITE setProxyAddress NOTIFY proxyAddressChanged)
    Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged)
    Q_PROPERTY(QString proxyUsername READ proxyUsername WRITE setProxyUsername NOTIFY proxyUsernameChanged)
    Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk
    };
    Q_ENUM(PasswordOption)

    enum MppeCrypto {
        MppeAny = 0,
        Mppe128,
        Mppe40
    };
    Q_ENUM(MppeCrypto)

    explicit SstpSetting(QObject *parent = nullptr);
    ~SstpSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString username() const;
    void setUsername(const QString &username);

    QString sstpPassword() const;
    void setSstpPassword(const QString &password);

    PasswordOption sstpPasswordOption() const;
    void setSstpPasswordOption(PasswordOption option);

    QString ntDomain() const;
    void setNtDomain(const QString &ntDomain);

    QString caCert() const;
    void setCaCert(const QString &caCert);

    bool ignoreCertificateWarnings() const;
    void setIgnoreCertificateWarnings(bool ignore);

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

    bool sendPppEchoPackets() const;
    void setSendPppEchoPackets(bool send);

    bool useCustomUnitNumber() const;
    void setUseCustomUnitNumber(bool use);

    int customUnitNumber() const;
    void setCustomUnitNumber(int unitNumber);

    QString proxyAddress() const;
    void setProxyAddress(const QString &address);

    int proxyPort() const;
    void setProxyPort(int port);

    QString proxyUsername() const;
    void setProxyUsername(const QString &username);

    QString proxyPassword() const;
    void setProxyPassword(const QString &password);

Q_SIGNALS:
    void gatewayChanged();
    void usernameChanged();
    void sstpPasswordChanged();
    void sstpPasswordOptionChanged();
    void ntDomainChanged();
    void caCertChanged();
    void ignoreCertificateWarningsChanged();

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

    void sendPppEchoPacketsChanged();

    void useCustomUnitNumberChanged();
    void customUnitNumberChanged();

    void proxyAddressChanged();
    void proxyPortChanged();
    void proxyUsernameChanged();
    void proxyPasswordChanged();

    void validChanged();

private:
    QString m_gateway;
    QString m_username;
    QString m_sstpPassword;
    PasswordOption m_sstpPasswordOption = StoreForUser;
    QString m_ntDomain;
    QString m_caCert;
    bool m_ignoreCertificateWarnings = false;

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

    bool m_sendPppEchoPackets = false;

    bool m_useCustomUnitNumber = false;
    int m_customUnitNumber = 0;

    QString m_proxyAddress;
    int m_proxyPort = 0;
    QString m_proxyUsername;
    QString m_proxyPassword;
};

#endif // PLASMA_NM_SSTP_QML_H
