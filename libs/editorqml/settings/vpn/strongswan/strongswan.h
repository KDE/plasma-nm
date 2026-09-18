/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_STRONGSWAN_QML_H
#define PLASMA_NM_STRONGSWAN_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QUrl>

class PLASMANM_EDITORQML_EXPORT StrongswanSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    // Gateway
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(QString gatewayCertificate READ gatewayCertificate WRITE setGatewayCertificate NOTIFY gatewayCertificateChanged)
    Q_PROPERTY(QString remoteIdentity READ remoteIdentity WRITE setRemoteIdentity NOTIFY remoteIdentityChanged)

    // Authentication
    Q_PROPERTY(AuthMethod authMethod READ authMethod WRITE setAuthMethod NOTIFY authMethodChanged)

    // Used by the certificate methods; the private key only by PrivateKey.
    Q_PROPERTY(QString userCertificate READ userCertificate WRITE setUserCertificate NOTIFY userCertificateChanged)
    Q_PROPERTY(QString userKey READ userKey WRITE setUserKey NOTIFY userKeyChanged)

    // Used by the EAP methods.
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(PasswordOption userPasswordOption READ userPasswordOption WRITE setUserPasswordOption NOTIFY userPasswordOptionChanged)

    // Options
    Q_PROPERTY(bool requestInnerIp READ requestInnerIp WRITE setRequestInnerIp NOTIFY requestInnerIpChanged)
    Q_PROPERTY(bool enforceUdpEncapsulation READ enforceUdpEncapsulation WRITE setEnforceUdpEncapsulation NOTIFY enforceUdpEncapsulationChanged)
    Q_PROPERTY(bool useIpCompression READ useIpCompression WRITE setUseIpCompression NOTIFY useIpCompressionChanged)

    // Custom cipher proposals
    Q_PROPERTY(bool useCustomProposals READ useCustomProposals WRITE setUseCustomProposals NOTIFY useCustomProposalsChanged)
    Q_PROPERTY(QString ike READ ike WRITE setIke NOTIFY ikeChanged)
    Q_PROPERTY(QString esp READ esp WRITE setEsp NOTIFY espChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    enum AuthMethod {
        PrivateKey = 0,
        SshAgent,
        Smartcard,
        Eap,
        EapTtls
    };
    Q_ENUM(AuthMethod)

    explicit StrongswanSetting(QObject *parent = nullptr);
    ~StrongswanSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString gatewayCertificate() const;
    void setGatewayCertificate(const QString &certificate);

    QString remoteIdentity() const;
    void setRemoteIdentity(const QString &identity);

    AuthMethod authMethod() const;
    void setAuthMethod(AuthMethod method);

    QString userCertificate() const;
    void setUserCertificate(const QString &certificate);

    QString userKey() const;
    void setUserKey(const QString &key);

    QString username() const;
    void setUsername(const QString &username);

    QString userPassword() const;
    void setUserPassword(const QString &password);

    PasswordOption userPasswordOption() const;
    void setUserPasswordOption(PasswordOption option);

    bool requestInnerIp() const;
    void setRequestInnerIp(bool request);

    bool enforceUdpEncapsulation() const;
    void setEnforceUdpEncapsulation(bool enforce);

    bool useIpCompression() const;
    void setUseIpCompression(bool use);

    bool useCustomProposals() const;
    void setUseCustomProposals(bool use);

    QString ike() const;
    void setIke(const QString &ike);

    QString esp() const;
    void setEsp(const QString &esp);

Q_SIGNALS:
    void gatewayChanged();
    void gatewayCertificateChanged();
    void remoteIdentityChanged();

    void authMethodChanged();
    void userCertificateChanged();
    void userKeyChanged();
    void usernameChanged();
    void userPasswordChanged();
    void userPasswordOptionChanged();

    void requestInnerIpChanged();
    void enforceUdpEncapsulationChanged();
    void useIpCompressionChanged();

    void useCustomProposalsChanged();
    void ikeChanged();
    void espChanged();

    void validChanged();

private:
    QString m_gateway;
    QString m_gatewayCertificate;
    QString m_remoteIdentity;

    AuthMethod m_authMethod = PrivateKey;
    QString m_userCertificate;
    QString m_userKey;
    QString m_username;
    QString m_userPassword;
    PasswordOption m_userPasswordOption = StoreForUser;

    bool m_requestInnerIp = false;
    bool m_enforceUdpEncapsulation = false;
    bool m_useIpCompression = false;

    bool m_useCustomProposals = false;
    QString m_ike;
    QString m_esp;
};

#endif // PLASMA_NM_STRONGSWAN_QML_H
