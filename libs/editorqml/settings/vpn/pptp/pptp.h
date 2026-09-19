/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_PPTP_QML_H
#define PLASMA_NM_PPTP_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>

class PLASMANM_EDITORQML_EXPORT PptpSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    // General
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)

    // Additional
    Q_PROPERTY(QString login READ login WRITE setLogin NOTIFY loginChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(PasswordOption passwordOption READ passwordOption WRITE setPasswordOption NOTIFY passwordOptionChanged)
    Q_PROPERTY(QString ntDomain READ ntDomain WRITE setNtDomain NOTIFY ntDomainChanged)

    // Advanced - allowed authentication methods
    Q_PROPERTY(bool allowPap READ allowPap WRITE setAllowPap NOTIFY allowPapChanged)
    Q_PROPERTY(bool allowChap READ allowChap WRITE setAllowChap NOTIFY allowChapChanged)
    Q_PROPERTY(bool allowMschap READ allowMschap WRITE setAllowMschap NOTIFY allowMschapChanged)
    Q_PROPERTY(bool allowMschapv2 READ allowMschapv2 WRITE setAllowMschapv2 NOTIFY allowMschapv2Changed)
    Q_PROPERTY(bool allowEap READ allowEap WRITE setAllowEap NOTIFY allowEapChanged)

    // Advanced - MPPE encryption
    Q_PROPERTY(bool useMppe READ useMppe WRITE setUseMppe NOTIFY useMppeChanged)
    Q_PROPERTY(MppeCrypto mppeCrypto READ mppeCrypto WRITE setMppeCrypto NOTIFY mppeCryptoChanged)
    Q_PROPERTY(bool statefulEncryption READ statefulEncryption WRITE setStatefulEncryption NOTIFY statefulEncryptionChanged)

    // Advanced - compression
    Q_PROPERTY(bool allowBsdCompression READ allowBsdCompression WRITE setAllowBsdCompression NOTIFY allowBsdCompressionChanged)
    Q_PROPERTY(bool allowDeflateCompression READ allowDeflateCompression WRITE setAllowDeflateCompression NOTIFY allowDeflateCompressionChanged)
    Q_PROPERTY(bool allowTcpHeaderCompression READ allowTcpHeaderCompression WRITE setAllowTcpHeaderCompression NOTIFY allowTcpHeaderCompressionChanged)

    // Advanced - echo
    Q_PROPERTY(bool sendPppEchoPackets READ sendPppEchoPackets WRITE setSendPppEchoPackets NOTIFY sendPppEchoPacketsChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    enum MppeCrypto {
        MppeAny = 0,
        Mppe128,
        Mppe40
    };
    Q_ENUM(MppeCrypto)

    explicit PptpSetting(QObject *parent = nullptr);
    ~PptpSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString login() const;
    void setLogin(const QString &login);

    QString password() const;
    void setPassword(const QString &password);

    PasswordOption passwordOption() const;
    void setPasswordOption(PasswordOption option);

    QString ntDomain() const;
    void setNtDomain(const QString &ntDomain);

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

Q_SIGNALS:
    void gatewayChanged();
    void loginChanged();
    void passwordChanged();
    void passwordOptionChanged();
    void ntDomainChanged();

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

    void validChanged();

private:
    QString m_gateway;
    QString m_login;
    QString m_password;
    PasswordOption m_passwordOption = StoreForUser;
    QString m_ntDomain;

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
};

#endif // PLASMA_NM_PPTP_QML_H
