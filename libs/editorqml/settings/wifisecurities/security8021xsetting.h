/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_SECURITY_8021_NH
#define PLASMA_NM_SECURITY_8021_NH

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/Security8021xSetting>
#include <QObject>
#include <qqmlintegration.h>

class PLASMANM_EDITORQML_EXPORT Security8021xSetting : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(EapMethod eapMethod READ eapMethod WRITE setEapMethod NOTIFY eapMethodChanged)

    Q_PROPERTY(SecurityType securityType READ securityType WRITE setSecurityType NOTIFY securityTypeChanged)

    Q_PROPERTY(QString md5Username READ md5Username WRITE setMd5Username NOTIFY md5UsernameChanged)
    Q_PROPERTY(QString md5Password READ md5Password WRITE setMd5Password NOTIFY md5PasswordChanged)
    Q_PROPERTY(PasswordOption md5PasswordOption READ md5PasswordOption WRITE setMd5PasswordOption NOTIFY md5PasswordOptionChanged)

    Q_PROPERTY(QString tlsIdentity READ tlsIdentity WRITE setTlsIdentity NOTIFY tlsIdentityChanged)
    Q_PROPERTY(QString tlsDomain READ tlsDomain WRITE setTlsDomain NOTIFY tlsDomainChanged)
    Q_PROPERTY(QString tlsUserCert READ tlsUserCert WRITE setTlsUserCert NOTIFY tlsUserCertChanged)
    Q_PROPERTY(QString tlsCaCert READ tlsCaCert WRITE setTlsCaCert NOTIFY tlsCaCertChanged)
    Q_PROPERTY(QString tlsPrivateKey READ tlsPrivateKey WRITE setTlsPrivateKey NOTIFY tlsPrivateKeyChanged)
    Q_PROPERTY(QString tlsPrivateKeyPassword READ tlsPrivateKeyPassword WRITE setTlsPrivateKeyPassword NOTIFY tlsPrivateKeyPasswordChanged)
    Q_PROPERTY(PasswordOption tlsPrivateKeyPasswordOption READ tlsPrivateKeyPasswordOption WRITE setTlsPrivateKeyPasswordOption NOTIFY
                   tlsPrivateKeyPasswordOptionChanged)
    Q_PROPERTY(QString tlsSubjectMatch READ tlsSubjectMatch WRITE setTlsSubjectMatch NOTIFY tlsSubjectMatchChanged)
    Q_PROPERTY(QString tlsAltSubjectMatches READ tlsAltSubjectMatches WRITE setTlsAltSubjectMatches NOTIFY tlsAltSubjectMatchesChanged)
    Q_PROPERTY(QString tlsConnectToServers READ tlsConnectToServers WRITE setTlsConnectToServers NOTIFY tlsConnectToServersChanged)

    Q_PROPERTY(QString leapUsername READ leapUsername WRITE setLeapUsername NOTIFY leapUsernameChanged)
    Q_PROPERTY(QString leapPassword READ leapPassword WRITE setLeapPassword NOTIFY leapPasswordChanged)
    Q_PROPERTY(PasswordOption leapPasswordOption READ leapPasswordOption WRITE setLeapPasswordOption NOTIFY leapPasswordOptionChanged)

    Q_PROPERTY(QString pwdUsername READ pwdUsername WRITE setPwdUsername NOTIFY pwdUsernameChanged)
    Q_PROPERTY(QString pwdPassword READ pwdPassword WRITE setPwdPassword NOTIFY pwdPasswordChanged)
    Q_PROPERTY(PasswordOption pwdPasswordOption READ pwdPasswordOption WRITE setPwdPasswordOption NOTIFY pwdPasswordOptionChanged)

    Q_PROPERTY(QString fastAnonIdentity READ fastAnonIdentity WRITE setFastAnonIdentity NOTIFY fastAnonIdentityChanged)
    Q_PROPERTY(bool fastAllowPacProvisioning READ fastAllowPacProvisioning WRITE setFastAllowPacProvisioning NOTIFY fastAllowPacProvisioningChanged)
    Q_PROPERTY(int fastPacMethod READ fastPacMethod WRITE setFastPacMethod NOTIFY fastPacMethodChanged)
    Q_PROPERTY(QString fastPacFile READ fastPacFile WRITE setFastPacFile NOTIFY fastPacFileChanged)
    Q_PROPERTY(FastInnerAuth fastInnerAuth READ fastInnerAuth WRITE setFastInnerAuth NOTIFY fastInnerAuthChanged)
    Q_PROPERTY(QString fastUsername READ fastUsername WRITE setFastUsername NOTIFY fastUsernameChanged)
    Q_PROPERTY(QString fastPassword READ fastPassword WRITE setFastPassword NOTIFY fastPasswordChanged)
    Q_PROPERTY(PasswordOption fastPasswordOption READ fastPasswordOption WRITE setFastPasswordOption NOTIFY fastPasswordOptionChanged)

    Q_PROPERTY(QString ttlsAnonIdentity READ ttlsAnonIdentity WRITE setTtlsAnonIdentity NOTIFY ttlsAnonIdentityChanged)
    Q_PROPERTY(QString ttlsDomain READ ttlsDomain WRITE setTtlsDomain NOTIFY ttlsDomainChanged)
    Q_PROPERTY(QString ttlsCaCert READ ttlsCaCert WRITE setTtlsCaCert NOTIFY ttlsCaCertChanged)
    Q_PROPERTY(int ttlsInnerAuth READ ttlsInnerAuth WRITE setTtlsInnerAuth NOTIFY ttlsInnerAuthChanged)
    Q_PROPERTY(QString ttlsUsername READ ttlsUsername WRITE setTtlsUsername NOTIFY ttlsUsernameChanged)
    Q_PROPERTY(QString ttlsPassword READ ttlsPassword WRITE setTtlsPassword NOTIFY ttlsPasswordChanged)
    Q_PROPERTY(PasswordOption ttlsPasswordOption READ ttlsPasswordOption WRITE setTtlsPasswordOption NOTIFY ttlsPasswordOptionChanged)

    Q_PROPERTY(QString peapAnonIdentity READ peapAnonIdentity WRITE setPeapAnonIdentity NOTIFY peapAnonIdentityChanged)
    Q_PROPERTY(QString peapDomain READ peapDomain WRITE setPeapDomain NOTIFY peapDomainChanged)
    Q_PROPERTY(QString peapCaCert READ peapCaCert WRITE setPeapCaCert NOTIFY peapCaCertChanged)
    Q_PROPERTY(int peapVersion READ peapVersion WRITE setPeapVersion NOTIFY peapVersionChanged)
    Q_PROPERTY(int peapInnerAuth READ peapInnerAuth WRITE setPeapInnerAuth NOTIFY peapInnerAuthChanged)
    Q_PROPERTY(QString peapUsername READ peapUsername WRITE setPeapUsername NOTIFY peapUsernameChanged)
    Q_PROPERTY(QString peapPassword READ peapPassword WRITE setPeapPassword NOTIFY peapPasswordChanged)
    Q_PROPERTY(PasswordOption peapPasswordOption READ peapPasswordOption WRITE setPeapPasswordOption NOTIFY peapPasswordOptionChanged)

    Q_PROPERTY(bool showMd5 READ showMd5 NOTIFY eapMethodChanged)
    Q_PROPERTY(bool showTls READ showTls NOTIFY eapMethodChanged)
    Q_PROPERTY(bool showLeap READ showLeap NOTIFY eapMethodChanged)
    Q_PROPERTY(bool showPwd READ showPwd NOTIFY eapMethodChanged)
    Q_PROPERTY(bool showFast READ showFast NOTIFY eapMethodChanged)
    Q_PROPERTY(bool showTtls READ showTtls NOTIFY eapMethodChanged)
    Q_PROPERTY(bool showPeap READ showPeap NOTIFY eapMethodChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum SecurityType {
        Ethernet = 0,
        WirelessWpaEap,
        WirelessWpaEapSuiteB192
    };
    Q_ENUM(SecurityType)

    enum EapMethod {
        EapMethodMd5 = NetworkManager::Security8021xSetting::EapMethodMd5,
        EapMethodTls = NetworkManager::Security8021xSetting::EapMethodTls,
        EapMethodLeap = NetworkManager::Security8021xSetting::EapMethodLeap,
        EapMethodPwd = NetworkManager::Security8021xSetting::EapMethodPwd,
        EapMethodFast = NetworkManager::Security8021xSetting::EapMethodFast,
        EapMethodTtls = NetworkManager::Security8021xSetting::EapMethodTtls,
        EapMethodPeap = NetworkManager::Security8021xSetting::EapMethodPeap
    };
    Q_ENUM(EapMethod)

    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers = 1,
        AlwaysAsk = 2,
        NotRequired = 4,
    };
    Q_ENUM(PasswordOption)

    enum FastInnerAuth {
        AuthMethodGtc = NetworkManager::Security8021xSetting::AuthMethodGtc,
        AuthMethodMschapv2 = NetworkManager::Security8021xSetting::AuthMethodMschapv2,
    };

    Q_ENUM(FastInnerAuth);

    explicit Security8021xSetting(QObject *parent = nullptr);

    Q_INVOKABLE void loadConfig(const NetworkManager::Setting::Ptr &setting);
    Q_INVOKABLE void loadSecrets(const NetworkManager::Setting::Ptr &setting);
    Q_INVOKABLE QVariantMap setting() const;
    Q_INVOKABLE void setPasswordOptions(PasswordOption option);
    bool isValid() const;

    SecurityType securityType() const;
    void setSecurityType(SecurityType type);

    EapMethod eapMethod() const;
    void setEapMethod(EapMethod method);

    bool showMd5() const;
    bool showTls() const;
    bool showLeap() const;
    bool showPwd() const;
    bool showFast() const;
    bool showTtls() const;
    bool showPeap() const;

    // MD5
    QString md5Username() const;
    void setMd5Username(const QString &v);
    QString md5Password() const;
    void setMd5Password(const QString &v);
    PasswordOption md5PasswordOption() const;
    void setMd5PasswordOption(PasswordOption v);

    // TLS
    QString tlsIdentity() const;
    void setTlsIdentity(const QString &v);
    QString tlsDomain() const;
    void setTlsDomain(const QString &v);
    QString tlsUserCert() const;
    void setTlsUserCert(const QString &v);
    QString tlsCaCert() const;
    void setTlsCaCert(const QString &v);
    QString tlsPrivateKey() const;
    void setTlsPrivateKey(const QString &v);
    QString tlsPrivateKeyPassword() const;
    void setTlsPrivateKeyPassword(const QString &v);
    PasswordOption tlsPrivateKeyPasswordOption() const;
    void setTlsPrivateKeyPasswordOption(PasswordOption v);
    QString tlsSubjectMatch() const;
    void setTlsSubjectMatch(const QString &v);
    QString tlsAltSubjectMatches() const;
    void setTlsAltSubjectMatches(const QString &v);
    QString tlsConnectToServers() const;
    void setTlsConnectToServers(const QString &v);

    // LEAP
    QString leapUsername() const;
    void setLeapUsername(const QString &v);
    QString leapPassword() const;
    void setLeapPassword(const QString &v);
    PasswordOption leapPasswordOption() const;
    void setLeapPasswordOption(PasswordOption v);

    // PWD
    QString pwdUsername() const;
    void setPwdUsername(const QString &v);
    QString pwdPassword() const;
    void setPwdPassword(const QString &v);
    PasswordOption pwdPasswordOption() const;
    void setPwdPasswordOption(PasswordOption v);

    // FAST
    QString fastAnonIdentity() const;
    void setFastAnonIdentity(const QString &v);
    bool fastAllowPacProvisioning() const;
    void setFastAllowPacProvisioning(bool v);
    int fastPacMethod() const;
    void setFastPacMethod(int v);
    QString fastPacFile() const;
    void setFastPacFile(const QString &v);
    FastInnerAuth fastInnerAuth() const;
    void setFastInnerAuth(FastInnerAuth v);
    QString fastUsername() const;
    void setFastUsername(const QString &v);
    QString fastPassword() const;
    void setFastPassword(const QString &v);
    PasswordOption fastPasswordOption() const;
    void setFastPasswordOption(PasswordOption v);

    // TTLS
    QString ttlsAnonIdentity() const;
    void setTtlsAnonIdentity(const QString &v);
    QString ttlsDomain() const;
    void setTtlsDomain(const QString &v);
    QString ttlsCaCert() const;
    void setTtlsCaCert(const QString &v);
    int ttlsInnerAuth() const;
    void setTtlsInnerAuth(int v);
    QString ttlsUsername() const;
    void setTtlsUsername(const QString &v);
    QString ttlsPassword() const;
    void setTtlsPassword(const QString &v);
    PasswordOption ttlsPasswordOption() const;
    void setTtlsPasswordOption(PasswordOption v);

    // PEAP
    QString peapAnonIdentity() const;
    void setPeapAnonIdentity(const QString &v);
    QString peapDomain() const;
    void setPeapDomain(const QString &v);
    QString peapCaCert() const;
    void setPeapCaCert(const QString &v);
    int peapVersion() const;
    void setPeapVersion(int v);
    int peapInnerAuth() const;
    void setPeapInnerAuth(int v);
    QString peapUsername() const;
    void setPeapUsername(const QString &v);
    QString peapPassword() const;
    void setPeapPassword(const QString &v);
    PasswordOption peapPasswordOption() const;
    void setPeapPasswordOption(PasswordOption v);

Q_SIGNALS:
    void eapMethodChanged();
    void securityTypeChanged();

    void validChanged();

    void md5UsernameChanged();
    void md5PasswordChanged();
    void md5PasswordOptionChanged();

    void tlsIdentityChanged();
    void tlsDomainChanged();
    void tlsUserCertChanged();
    void tlsCaCertChanged();
    void tlsPrivateKeyChanged();
    void tlsPrivateKeyPasswordChanged();
    void tlsPrivateKeyPasswordOptionChanged();
    void tlsSubjectMatchChanged();
    void tlsAltSubjectMatchesChanged();
    void tlsConnectToServersChanged();

    void leapUsernameChanged();
    void leapPasswordChanged();
    void leapPasswordOptionChanged();

    void pwdUsernameChanged();
    void pwdPasswordChanged();
    void pwdPasswordOptionChanged();

    void fastAnonIdentityChanged();
    void fastAllowPacProvisioningChanged();
    void fastPacMethodChanged();
    void fastPacFileChanged();
    void fastInnerAuthChanged();
    void fastUsernameChanged();
    void fastPasswordChanged();
    void fastPasswordOptionChanged();

    void ttlsAnonIdentityChanged();
    void ttlsDomainChanged();
    void ttlsCaCertChanged();
    void ttlsInnerAuthChanged();
    void ttlsUsernameChanged();
    void ttlsPasswordChanged();
    void ttlsPasswordOptionChanged();

    void peapAnonIdentityChanged();
    void peapDomainChanged();
    void peapCaCertChanged();
    void peapVersionChanged();
    void peapInnerAuthChanged();
    void peapUsernameChanged();
    void peapPasswordChanged();
    void peapPasswordOptionChanged();

private:
    static bool pkcs12CanDecrypt(const QString &path, const QString &password);

    SecurityType m_securityType = WirelessWpaEap;
    EapMethod m_eapMethod = EapMethodPeap;

    // MD5
    QString m_md5Username;
    QString m_md5Password;
    PasswordOption m_md5PasswordOption = StoreForUser;

    // TLS
    QString m_tlsIdentity;
    QString m_tlsDomain;
    QString m_tlsUserCert;
    QString m_tlsCaCert;
    QString m_tlsPrivateKey;
    QString m_tlsPrivateKeyPassword;
    PasswordOption m_tlsPrivateKeyPasswordOption = StoreForUser;
    QString m_tlsSubjectMatch;
    QString m_tlsAltSubjectMatches;
    QString m_tlsConnectToServers;

    // LEAP
    QString m_leapUsername;
    QString m_leapPassword;
    PasswordOption m_leapPasswordOption = StoreForUser;

    // PWD
    QString m_pwdUsername;
    QString m_pwdPassword;
    PasswordOption m_pwdPasswordOption = StoreForUser;

    // FAST
    QString m_fastAnonIdentity;
    bool m_fastAllowPacProvisioning = false;
    int m_fastPacMethod = 0;
    QString m_fastPacFile;
    FastInnerAuth m_fastInnerAuth = AuthMethodGtc;
    QString m_fastUsername;
    QString m_fastPassword;
    PasswordOption m_fastPasswordOption = StoreForUser;

    // TTLS
    QString m_ttlsAnonIdentity;
    QString m_ttlsDomain;
    QString m_ttlsCaCert;
    int m_ttlsInnerAuth = 0;
    QString m_ttlsUsername;
    QString m_ttlsPassword;
    PasswordOption m_ttlsPasswordOption = StoreForUser;

    // PEAP
    QString m_peapAnonIdentity;
    QString m_peapDomain;
    QString m_peapCaCert;
    int m_peapVersion = 0;
    int m_peapInnerAuth = 0;
    QString m_peapUsername;
    QString m_peapPassword;
    PasswordOption m_peapPasswordOption = StoreForUser;
};

#endif
