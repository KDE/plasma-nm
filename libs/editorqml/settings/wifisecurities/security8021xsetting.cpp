/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "security8021xsetting.h"

#include <networkmanagerqt/security8021xsetting.h>
#include <networkmanagerqt/setting.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/x509.h>
#include <qdir.h>
#include <qlatin1stringview.h>
#include <qnamespace.h>
#include <qurl.h>
#include <qvariant.h>

Security8021xSetting::Security8021xSetting(QObject *parent)
    : QObject(parent)
{
}

bool Security8021xSetting::pkcs12CanDecrypt(const QString &path, const QString &password)
{
    auto fp = fopen(path.toLatin1().data(), "rb");
    if (fp) {
        auto pkcs12 = d2i_PKCS12_fp(fp, nullptr);
        fclose(fp);

        if (pkcs12) {
            EVP_PKEY *pkey = nullptr;
            X509 *cert = nullptr;
            STACK_OF(X509) *ca = nullptr;

            auto alg = EVP_PKEY_NONE;
            if (PKCS12_parse(pkcs12, password.toUtf8().data(), &pkey, &cert, &ca)) {
                alg = EVP_PKEY_base_id(pkey);
            }
            PKCS12_free(pkcs12);
            EVP_PKEY_free(pkey);
            X509_free(cert);
            sk_X509_pop_free(ca, X509_free);
            return alg == EVP_PKEY_RSA;
        }
    }
    return false;
}

void Security8021xSetting::loadConfig(const NetworkManager::Setting::Ptr &setting)
{
    if (!setting)
        return;

    NetworkManager::Security8021xSetting::Ptr securitySetting = setting.staticCast<NetworkManager::Security8021xSetting>();

    const QList<NetworkManager::Security8021xSetting::EapMethod> eapMethods = securitySetting->eapMethods();
    const NetworkManager::Security8021xSetting::AuthMethod phase2AuthMethod = securitySetting->phase2AuthMethod();

    if (securitySetting->passwordFlags().testFlags(NetworkManager::Setting::None)) {
        setPasswordOptions(StoreForAllUsers);
    } else if (securitySetting->passwordFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
        setPasswordOptions(StoreForUser);
    } else {
        setPasswordOptions(AlwaysAsk);
    }

    if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodMd5)) {
        setEapMethod(EapMethodMd5);
        setMd5Username(securitySetting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTls)) {
        QStringList servers;
        setEapMethod(EapMethodTls);
        setTlsIdentity(securitySetting->identity());
        setTlsDomain(securitySetting->domainSuffixMatch());
        setTlsUserCert(QUrl::fromLocalFile(QString::fromUtf8(securitySetting->clientCertificate().chopped(1))).toString());
        setTlsCaCert(QUrl::fromLocalFile(securitySetting->caCertificate().removeLast()).toString());
        setTlsSubjectMatch(securitySetting->subjectMatch());
        setTlsAltSubjectMatches(securitySetting->altSubjectMatches().join(QLatin1String(", ")));

        for (const QString &match : securitySetting->altSubjectMatches()) {
            if (match.startsWith(QLatin1String("DNS:"))) {
                servers.append(match.mid(4));
            }
        }
        setTlsConnectToServers(servers.join(QLatin1String(", ")));
        setTlsPrivateKey(QUrl::fromLocalFile(securitySetting->privateKey().removeLast()).toString());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodLeap)) {
        setEapMethod(EapMethodLeap);
        setLeapUsername(securitySetting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPwd)) {
        setEapMethod(EapMethodPwd);
        setPwdUsername(securitySetting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodFast)) {
        setEapMethod(EapMethodFast);
        setFastAnonIdentity(securitySetting->anonymousIdentity());
        setFastAllowPacProvisioning((int)securitySetting->phase1FastProvisioning() > 0);
        setFastPacMethod(securitySetting->phase1FastProvisioning() - 1);
        setFastPacFile(QUrl::fromLocalFile(securitySetting->pacFile()).toString());

        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodGtc) {
            setFastInnerAuth(AuthMethodGtc);
        } else {
            setFastInnerAuth(AuthMethodMschapv2);
        }
        setFastUsername(securitySetting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTtls)) {
        setEapMethod(EapMethodTtls);

        setTtlsAnonIdentity(securitySetting->anonymousIdentity());
        setTtlsDomain(securitySetting->domainSuffixMatch());
        setTtlsCaCert(QUrl::fromLocalFile(QString::fromUtf8(securitySetting->caCertificate().chopped(1))).toString());
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodPap) {
            setTtlsInnerAuth(0);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschap) {
            setTtlsInnerAuth(1);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschapv2) {
            setTtlsInnerAuth(2);
        } else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodChap) {
            setTtlsInnerAuth(3);
        }
        setTtlsUsername(securitySetting->identity());
    } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPeap)) {
        setEapMethod(EapMethodPeap);
        setPeapAnonIdentity(securitySetting->anonymousIdentity());
        setPeapDomain(securitySetting->domainSuffixMatch());
        setPeapCaCert(QUrl::fromLocalFile(QString::fromUtf8(securitySetting->caCertificate().chopped(1))).toString());
        setPeapVersion(securitySetting->phase1PeapVersion());
        if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMschapv2)
            setPeapInnerAuth(0);
        else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodMd5)
            setPeapInnerAuth(1);
        else if (phase2AuthMethod == NetworkManager::Security8021xSetting::AuthMethodGtc)
            setPeapInnerAuth(2);
        setPeapUsername(securitySetting->identity());
    }

    loadSecrets(setting);

    Q_EMIT eapMethodChanged();
    Q_EMIT validChanged();
}

void Security8021xSetting::loadSecrets(const NetworkManager::Setting::Ptr &setting)
{
    NetworkManager::Security8021xSetting::Ptr securitySetting = setting.staticCast<NetworkManager::Security8021xSetting>();

    const QString password = securitySetting->password();
    const QList<NetworkManager::Security8021xSetting::EapMethod> eapMethods = securitySetting->eapMethods();
    if (!password.isEmpty()) {
        if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodMd5)) {
            m_md5Password = securitySetting->password();
            Q_EMIT md5PasswordChanged();

        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodLeap)) {
            m_leapPassword = securitySetting->password();
            Q_EMIT leapPasswordChanged();

        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodFast)) {
            m_fastPassword = securitySetting->password();
            Q_EMIT fastPasswordChanged();

        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPwd)) {
            m_pwdPassword = securitySetting->password();
            Q_EMIT pwdPasswordChanged();

        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTtls)) {
            m_ttlsPassword = securitySetting->password();
            Q_EMIT ttlsPasswordChanged();

        } else if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodPeap)) {
            m_peapPassword = securitySetting->password();
            Q_EMIT peapPasswordChanged();
        }
    }

    if (eapMethods.contains(NetworkManager::Security8021xSetting::EapMethodTls)) {
        const QString privateKeyPassword = securitySetting->privateKeyPassword();
        if (!privateKeyPassword.isEmpty()) {
            m_tlsPrivateKeyPassword = securitySetting->privateKeyPassword();
            Q_EMIT tlsPrivateKeyPasswordChanged();
        }
    }
}

QVariantMap Security8021xSetting::setting() const
{
    NetworkManager::Security8021xSetting setting;
    NetworkManager::Security8021xSetting::EapMethod method = static_cast<NetworkManager::Security8021xSetting::EapMethod>(m_eapMethod);
    setting.setEapMethods({static_cast<NetworkManager::Security8021xSetting::EapMethod>(m_eapMethod)});

    if (method == NetworkManager::Security8021xSetting::EapMethodMd5) {
        if (!m_md5Username.isEmpty()) {
            setting.setIdentity(m_md5Username);
        }
        if (m_md5PasswordOption == StoreForAllUsers) {
            setting.setPasswordFlags(NetworkManager::Setting::None);
        } else if (m_md5PasswordOption == StoreForUser) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        }

        if (!m_md5Password.isEmpty()) {
            setting.setPassword(m_md5Password);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTls) {
        if (!m_tlsIdentity.isEmpty()) {
            setting.setIdentity(m_tlsIdentity);
        }
        if (!m_tlsDomain.isEmpty()) {
            setting.setDomainSuffixMatch(m_tlsDomain);
        }

        QUrl userCertUrl(m_tlsUserCert);
        if (userCertUrl.isValid()) {
            const auto fmt = userCertUrl.scheme() == "file" ? QUrl::PrettyDecoded : QUrl::FullyEncoded;
            setting.setClientCertificate(userCertUrl.toString(fmt).toUtf8().append('\0'));
        }
        QUrl caCertUrl(m_tlsCaCert);
        if (caCertUrl.isValid()) {
            const auto fmt = caCertUrl.scheme() == "file" ? QUrl::PrettyDecoded : QUrl::FullyEncoded;
            setting.setCaCertificate(caCertUrl.toString(fmt).toUtf8().append('\0'));
        }

        QStringList altSubject = m_tlsAltSubjectMatches.split(QLatin1Char(','), Qt::SkipEmptyParts);
        for (QString &match : m_tlsConnectToServers.split(QLatin1Char(','), Qt::SkipEmptyParts)) {
            match = match.trimmed();
            const QString tempstr = QLatin1String("DNS:") + match;
            if (!altSubject.contains(tempstr)) {
                altSubject.append(tempstr);
            }
        }
        setting.setSubjectMatch(m_tlsSubjectMatch);
        setting.setAltSubjectMatches(altSubject);

        QUrl privateKeyUrl(m_tlsPrivateKey);
        if (privateKeyUrl.isValid()) {
            const auto fmt = privateKeyUrl.scheme() == "file" ? QUrl::PrettyDecoded : QUrl::FullyEncoded;
            setting.setPrivateKey(privateKeyUrl.toString(fmt).toUtf8().append('\0'));
        }
        if (!m_tlsPrivateKeyPassword.isEmpty())
            setting.setPrivateKeyPassword(m_tlsPrivateKeyPassword);

        if (pkcs12CanDecrypt(QUrl(m_tlsPrivateKey).toLocalFile(), m_tlsPrivateKeyPassword)) {
            setting.setClientCertificate(m_tlsPrivateKey.toUtf8().append('\0'));
        }

        if (m_tlsPrivateKeyPasswordOption == StoreForAllUsers) {
            setting.setPrivateKeyPasswordFlags(NetworkManager::Setting::None);
        } else if (m_tlsPrivateKeyPasswordOption == StoreForUser) {
            setting.setPrivateKeyPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else if (m_tlsPrivateKeyPasswordOption == NotRequired) {
            setting.setPrivateKeyPasswordFlags(NetworkManager::Setting::NotRequired);
        } else {
            setting.setPrivateKeyPasswordFlags(NetworkManager::Setting::NotSaved);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodLeap) {
        if (!m_leapUsername.isEmpty())
            setting.setIdentity(m_leapUsername);
        if (!m_leapPassword.isEmpty())
            setting.setPassword(m_leapPassword);

        if (m_leapPasswordOption == StoreForAllUsers) {
            setting.setPasswordFlags(NetworkManager::Setting::None);
        } else if (m_leapPasswordOption == StoreForUser) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodPwd) {
        if (!m_pwdUsername.isEmpty())
            setting.setIdentity(m_pwdUsername);

        if (m_pwdPasswordOption == StoreForAllUsers) {
            setting.setPasswordFlags(NetworkManager::Setting::None);
        } else if (m_pwdPasswordOption == StoreForUser) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        }
        if (!m_pwdPassword.isEmpty())
            setting.setPassword(m_pwdPassword);
    } else if (method == NetworkManager::Security8021xSetting::EapMethodFast) {
        if (!m_fastAnonIdentity.isEmpty())
            setting.setAnonymousIdentity(m_fastAnonIdentity);
        if (!m_fastAllowPacProvisioning) {
            setting.setPhase1FastProvisioning(NetworkManager::Security8021xSetting::FastProvisioningDisabled);
        } else {
            setting.setPhase1FastProvisioning(static_cast<NetworkManager::Security8021xSetting::FastProvisioning>(m_fastPacMethod + 1));
        }
        QUrl pacUrl(m_fastPacFile);
        if (pacUrl.isValid())
            setting.setPacFile(QFile::encodeName(pacUrl.toLocalFile()));

        if (m_fastInnerAuth == AuthMethodGtc) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
        } else {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);
        }

        if (!m_fastUsername.isEmpty()) {
            setting.setIdentity(m_fastUsername);
        }

        if (!m_fastPassword.isEmpty()) {
            setting.setPassword(m_fastPassword);
        }

        if (m_fastPasswordOption == StoreForAllUsers) {
            setting.setPasswordFlags(NetworkManager::Setting::None);
        } else if (m_fastPasswordOption == StoreForUser) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTtls) {
        if (!m_ttlsAnonIdentity.isEmpty())
            setting.setAnonymousIdentity(m_ttlsAnonIdentity);
        if (!m_ttlsDomain.isEmpty())
            setting.setDomainSuffixMatch(m_ttlsDomain);
        QUrl caCertUrl(m_ttlsCaCert);
        if (caCertUrl.isValid())
            setting.setCaCertificate(caCertUrl.toString().toUtf8().append('\0'));
        if (m_ttlsInnerAuth == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodPap);

        } else if (m_ttlsInnerAuth == 1) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschap);

        } else if (m_ttlsInnerAuth == 2) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);

        } else if (m_ttlsInnerAuth == 3) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodChap);
        }
        if (!m_ttlsUsername.isEmpty())
            setting.setIdentity(m_ttlsUsername);
        if (!m_ttlsPassword.isEmpty())
            setting.setPassword(m_ttlsPassword);
        if (m_ttlsPasswordOption == StoreForAllUsers) {
            setting.setPasswordFlags(NetworkManager::Setting::None);
        } else if (m_ttlsPasswordOption == StoreForUser) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodPeap) {
        if (!m_peapAnonIdentity.isEmpty())
            setting.setAnonymousIdentity(m_peapAnonIdentity);
        if (!m_peapDomain.isEmpty())
            setting.setDomainSuffixMatch(m_peapDomain);
        {
            QUrl caCertUrl(m_peapCaCert);
            if (caCertUrl.isValid())
                setting.setCaCertificate(caCertUrl.toString().toUtf8().append('\0'));
        }
        setting.setPhase1PeapVersion(static_cast<NetworkManager::Security8021xSetting::PeapVersion>(m_peapVersion - 1));
        if (m_peapInnerAuth == 0) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMschapv2);

        } else if (m_peapInnerAuth == 1) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodMd5);

        } else if (m_peapInnerAuth == 2) {
            setting.setPhase2AuthMethod(NetworkManager::Security8021xSetting::AuthMethodGtc);
        }
        if (!m_peapUsername.isEmpty())
            setting.setIdentity(m_peapUsername);
        if (!m_peapPassword.isEmpty())
            setting.setPassword(m_peapPassword);
        if (m_peapPasswordOption == StoreForAllUsers) {
            setting.setPasswordFlags(NetworkManager::Setting::None);
        } else if (m_peapPasswordOption == StoreForUser) {
            setting.setPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            setting.setPasswordFlags(NetworkManager::Setting::NotSaved);
        }
    }
    return setting.toMap();
}

void Security8021xSetting::setPasswordOptions(PasswordOption option)
{
    m_md5PasswordOption = option;
    m_leapPasswordOption = option;
    m_pwdPasswordOption = option;
    m_fastPasswordOption = option;
    m_ttlsPasswordOption = option;
    m_peapPasswordOption = option;
    m_tlsPrivateKeyPasswordOption = option;

    Q_EMIT md5PasswordOptionChanged();
    Q_EMIT leapPasswordOptionChanged();
    Q_EMIT pwdPasswordOptionChanged();
    Q_EMIT fastPasswordOptionChanged();
    Q_EMIT ttlsPasswordOptionChanged();
    Q_EMIT peapPasswordOptionChanged();
    Q_EMIT tlsPrivateKeyPasswordOptionChanged();
}

bool Security8021xSetting::isValid() const
{
    NetworkManager::Security8021xSetting::EapMethod method = static_cast<NetworkManager::Security8021xSetting::EapMethod>(m_eapMethod);

    if (method == NetworkManager::Security8021xSetting::EapMethodMd5) {
        return !m_md5Username.isEmpty() && (!m_md5Password.isEmpty() || m_md5PasswordOption == AlwaysAsk);
    } else if (method == NetworkManager::Security8021xSetting::EapMethodTls) {
        if (m_tlsIdentity.isEmpty())
            return false;
        if (m_tlsPrivateKey.isEmpty())
            return false;
        if (m_tlsPrivateKeyPasswordOption == AlwaysAsk)
            return true;
        if (m_tlsPrivateKeyPassword.isEmpty())
            return false;
        if (pkcs12CanDecrypt(QUrl(m_tlsPrivateKey).toLocalFile(), m_tlsPrivateKeyPassword))
            return true;
        if (m_tlsUserCert.isEmpty())
            return false;
        auto fp = fopen(QUrl(m_tlsPrivateKey).toLocalFile().toUtf8().data(), "r");
        if (fp) {
            auto pkey = PEM_read_PrivateKey(fp, nullptr, nullptr, const_cast<char *>(m_tlsPrivateKeyPassword.toUtf8().data()));
            fclose(fp);
            if (pkey) {
                auto canDecrypt = EVP_PKEY_base_id(pkey) == EVP_PKEY_RSA;
                EVP_PKEY_free(pkey);
                return canDecrypt;
            }
        }
    } else if (method == NetworkManager::Security8021xSetting::EapMethodLeap) {
        return !m_leapUsername.isEmpty() && (!m_leapPassword.isEmpty() || m_leapPasswordOption == AlwaysAsk);

    } else if (method == NetworkManager::Security8021xSetting::EapMethodPwd) {
        return !m_pwdUsername.isEmpty() && (!m_pwdPassword.isEmpty() || m_pwdPasswordOption == AlwaysAsk);

    } else if (method == NetworkManager::Security8021xSetting::EapMethodFast) {
        if (!m_fastAllowPacProvisioning && m_fastPacFile.isEmpty()) {
            return false;
        }
        return !m_fastUsername.isEmpty() && (!m_fastPassword.isEmpty() || m_fastPasswordOption == AlwaysAsk);

    } else if (method == NetworkManager::Security8021xSetting::EapMethodTtls) {
        return !m_ttlsUsername.isEmpty() && (!m_ttlsPassword.isEmpty() || m_ttlsPasswordOption == AlwaysAsk);

    } else if (method == NetworkManager::Security8021xSetting::EapMethodPeap) {
        return !m_peapUsername.isEmpty() && (!m_peapPassword.isEmpty() || m_peapPasswordOption == AlwaysAsk);
    }
    return true;
}

// replaces currentAuthChanged in old code
bool Security8021xSetting::showMd5() const
{
    return m_eapMethod == EapMethodMd5;
}
bool Security8021xSetting::showTls() const
{
    return m_eapMethod == EapMethodTls;
}
bool Security8021xSetting::showLeap() const
{
    return m_eapMethod == EapMethodLeap;
}
bool Security8021xSetting::showPwd() const
{
    return m_eapMethod == EapMethodPwd;
}
bool Security8021xSetting::showFast() const
{
    return m_eapMethod == EapMethodFast;
}
bool Security8021xSetting::showTtls() const
{
    return m_eapMethod == EapMethodTtls;
}
bool Security8021xSetting::showPeap() const
{
    return m_eapMethod == EapMethodPeap;
}

Security8021xSetting::SecurityType Security8021xSetting::securityType() const
{
    return m_securityType;
}

void Security8021xSetting::setSecurityType(SecurityType type)
{
    if (m_securityType == type)
        return;
    m_securityType = type;
    if (type == WirelessWpaEapSuiteB192) {
        m_eapMethod = EapMethodTls;
    } else {
        m_eapMethod = EapMethodPeap;
    }
    Q_EMIT securityTypeChanged();
    Q_EMIT validChanged();
    Q_EMIT eapMethodChanged();
}

Security8021xSetting::EapMethod Security8021xSetting::eapMethod() const
{
    return m_eapMethod;
}
void Security8021xSetting::setEapMethod(EapMethod m)
{
    if (m_eapMethod == m)
        return;
    m_eapMethod = m;
    Q_EMIT eapMethodChanged();
    Q_EMIT validChanged();
}

// getters/setters
QString Security8021xSetting::md5Username() const
{
    return m_md5Username;
}
void Security8021xSetting::setMd5Username(const QString &v)
{
    if (m_md5Username == v)
        return;
    m_md5Username = v;
    Q_EMIT md5UsernameChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::md5Password() const
{
    return m_md5Password;
}
void Security8021xSetting::setMd5Password(const QString &v)
{
    if (m_md5Password == v)
        return;
    m_md5Password = v;
    Q_EMIT md5PasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::md5PasswordOption() const
{
    return m_md5PasswordOption;
}
void Security8021xSetting::setMd5PasswordOption(PasswordOption v)
{
    if (m_md5PasswordOption == v)
        return;
    m_md5PasswordOption = v;
    Q_EMIT md5PasswordOptionChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::tlsIdentity() const
{
    return m_tlsIdentity;
}
void Security8021xSetting::setTlsIdentity(const QString &v)
{
    if (m_tlsIdentity == v)
        return;
    m_tlsIdentity = v;
    Q_EMIT tlsIdentityChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsDomain() const
{
    return m_tlsDomain;
}
void Security8021xSetting::setTlsDomain(const QString &v)
{
    if (m_tlsDomain == v)
        return;
    m_tlsDomain = v;
    Q_EMIT tlsDomainChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsUserCert() const
{
    return m_tlsUserCert;
}
void Security8021xSetting::setTlsUserCert(const QString &v)
{
    if (m_tlsUserCert == v)
        return;
    m_tlsUserCert = v;
    Q_EMIT tlsUserCertChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsCaCert() const
{
    return m_tlsCaCert;
}
void Security8021xSetting::setTlsCaCert(const QString &v)
{
    if (m_tlsCaCert == v)
        return;
    m_tlsCaCert = v;
    Q_EMIT tlsCaCertChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsPrivateKey() const
{
    return m_tlsPrivateKey;
}
void Security8021xSetting::setTlsPrivateKey(const QString &v)
{
    if (m_tlsPrivateKey == v)
        return;
    m_tlsPrivateKey = v;
    Q_EMIT tlsPrivateKeyChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsPrivateKeyPassword() const
{
    return m_tlsPrivateKeyPassword;
}
void Security8021xSetting::setTlsPrivateKeyPassword(const QString &v)
{
    if (m_tlsPrivateKeyPassword == v)
        return;
    m_tlsPrivateKeyPassword = v;
    Q_EMIT tlsPrivateKeyPasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::tlsPrivateKeyPasswordOption() const
{
    return m_tlsPrivateKeyPasswordOption;
}
void Security8021xSetting::setTlsPrivateKeyPasswordOption(PasswordOption v)
{
    if (m_tlsPrivateKeyPasswordOption == v)
        return;
    m_tlsPrivateKeyPasswordOption = v;
    Q_EMIT tlsPrivateKeyPasswordOptionChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsSubjectMatch() const
{
    return m_tlsSubjectMatch;
}
void Security8021xSetting::setTlsSubjectMatch(const QString &v)
{
    if (m_tlsSubjectMatch == v)
        return;
    m_tlsSubjectMatch = v;
    Q_EMIT tlsSubjectMatchChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsAltSubjectMatches() const
{
    return m_tlsAltSubjectMatches;
}
void Security8021xSetting::setTlsAltSubjectMatches(const QString &v)
{
    if (m_tlsAltSubjectMatches == v)
        return;
    m_tlsAltSubjectMatches = v;
    Q_EMIT tlsAltSubjectMatchesChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::tlsConnectToServers() const
{
    return m_tlsConnectToServers;
}
void Security8021xSetting::setTlsConnectToServers(const QString &v)
{
    if (m_tlsConnectToServers == v)
        return;
    m_tlsConnectToServers = v;
    Q_EMIT tlsConnectToServersChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::leapUsername() const
{
    return m_leapUsername;
}
void Security8021xSetting::setLeapUsername(const QString &v)
{
    if (m_leapUsername == v)
        return;
    m_leapUsername = v;
    Q_EMIT leapUsernameChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::leapPassword() const
{
    return m_leapPassword;
}
void Security8021xSetting::setLeapPassword(const QString &v)
{
    if (m_leapPassword == v)
        return;
    m_leapPassword = v;
    Q_EMIT leapPasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::leapPasswordOption() const
{
    return m_leapPasswordOption;
}
void Security8021xSetting::setLeapPasswordOption(PasswordOption v)
{
    if (m_leapPasswordOption == v)
        return;
    m_leapPasswordOption = v;
    Q_EMIT leapPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::pwdUsername() const
{
    return m_pwdUsername;
}
void Security8021xSetting::setPwdUsername(const QString &v)
{
    if (m_pwdUsername == v)
        return;
    m_pwdUsername = v;
    Q_EMIT pwdUsernameChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::pwdPassword() const
{
    return m_pwdPassword;
}
void Security8021xSetting::setPwdPassword(const QString &v)
{
    if (m_pwdPassword == v)
        return;
    m_pwdPassword = v;
    Q_EMIT pwdPasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::pwdPasswordOption() const
{
    return m_pwdPasswordOption;
}
void Security8021xSetting::setPwdPasswordOption(PasswordOption v)
{
    if (m_pwdPasswordOption == v)
        return;
    m_pwdPasswordOption = v;
    Q_EMIT pwdPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::fastAnonIdentity() const
{
    return m_fastAnonIdentity;
}
void Security8021xSetting::setFastAnonIdentity(const QString &v)
{
    if (m_fastAnonIdentity == v)
        return;
    m_fastAnonIdentity = v;
    Q_EMIT fastAnonIdentityChanged();
    Q_EMIT validChanged();
}
bool Security8021xSetting::fastAllowPacProvisioning() const
{
    return m_fastAllowPacProvisioning;
}
void Security8021xSetting::setFastAllowPacProvisioning(bool v)
{
    if (m_fastAllowPacProvisioning == v)
        return;
    m_fastAllowPacProvisioning = v;
    Q_EMIT fastAllowPacProvisioningChanged();
    Q_EMIT validChanged();
}
int Security8021xSetting::fastPacMethod() const
{
    return m_fastPacMethod;
}
void Security8021xSetting::setFastPacMethod(int v)
{
    if (m_fastPacMethod == v)
        return;
    m_fastPacMethod = v;
    Q_EMIT fastPacMethodChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::fastPacFile() const
{
    return m_fastPacFile;
}
void Security8021xSetting::setFastPacFile(const QString &v)
{
    if (m_fastPacFile == v)
        return;
    m_fastPacFile = v;
    Q_EMIT fastPacFileChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::FastInnerAuth Security8021xSetting::fastInnerAuth() const
{
    return m_fastInnerAuth;
}

void Security8021xSetting::setFastInnerAuth(FastInnerAuth v)
{
    if (m_fastInnerAuth == v)
        return;

    m_fastInnerAuth = v;
    Q_EMIT fastInnerAuthChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::fastUsername() const
{
    return m_fastUsername;
}
void Security8021xSetting::setFastUsername(const QString &v)
{
    if (m_fastUsername == v)
        return;
    m_fastUsername = v;
    Q_EMIT fastUsernameChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::fastPassword() const
{
    return m_fastPassword;
}
void Security8021xSetting::setFastPassword(const QString &v)
{
    if (m_fastPassword == v)
        return;
    m_fastPassword = v;
    Q_EMIT fastPasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::fastPasswordOption() const
{
    return m_fastPasswordOption;
}
void Security8021xSetting::setFastPasswordOption(PasswordOption v)
{
    if (m_fastPasswordOption == v)
        return;
    m_fastPasswordOption = v;
    Q_EMIT fastPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::ttlsAnonIdentity() const
{
    return m_ttlsAnonIdentity;
}
void Security8021xSetting::setTtlsAnonIdentity(const QString &v)
{
    if (m_ttlsAnonIdentity == v)
        return;
    m_ttlsAnonIdentity = v;
    Q_EMIT ttlsAnonIdentityChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::ttlsDomain() const
{
    return m_ttlsDomain;
}
void Security8021xSetting::setTtlsDomain(const QString &v)
{
    if (m_ttlsDomain == v)
        return;
    m_ttlsDomain = v;
    Q_EMIT ttlsDomainChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::ttlsCaCert() const
{
    return m_ttlsCaCert;
}
void Security8021xSetting::setTtlsCaCert(const QString &v)
{
    if (m_ttlsCaCert == v)
        return;
    m_ttlsCaCert = v;
    Q_EMIT ttlsCaCertChanged();
    Q_EMIT validChanged();
}
int Security8021xSetting::ttlsInnerAuth() const
{
    return m_ttlsInnerAuth;
}
void Security8021xSetting::setTtlsInnerAuth(int v)
{
    if (m_ttlsInnerAuth == v)
        return;
    m_ttlsInnerAuth = v;
    Q_EMIT ttlsInnerAuthChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::ttlsUsername() const
{
    return m_ttlsUsername;
}
void Security8021xSetting::setTtlsUsername(const QString &v)
{
    if (m_ttlsUsername == v)
        return;
    m_ttlsUsername = v;
    Q_EMIT ttlsUsernameChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::ttlsPassword() const
{
    return m_ttlsPassword;
}
void Security8021xSetting::setTtlsPassword(const QString &v)
{
    if (m_ttlsPassword == v)
        return;
    m_ttlsPassword = v;
    Q_EMIT ttlsPasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::ttlsPasswordOption() const
{
    return m_ttlsPasswordOption;
}
void Security8021xSetting::setTtlsPasswordOption(PasswordOption v)
{
    if (m_ttlsPasswordOption == v)
        return;
    m_ttlsPasswordOption = v;
    Q_EMIT ttlsPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString Security8021xSetting::peapAnonIdentity() const
{
    return m_peapAnonIdentity;
}
void Security8021xSetting::setPeapAnonIdentity(const QString &v)
{
    if (m_peapAnonIdentity == v)
        return;
    m_peapAnonIdentity = v;
    Q_EMIT peapAnonIdentityChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::peapDomain() const
{
    return m_peapDomain;
}
void Security8021xSetting::setPeapDomain(const QString &v)
{
    if (m_peapDomain == v)
        return;
    m_peapDomain = v;
    Q_EMIT peapDomainChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::peapCaCert() const
{
    return m_peapCaCert;
}
void Security8021xSetting::setPeapCaCert(const QString &v)
{
    if (m_peapCaCert == v)
        return;
    m_peapCaCert = v;
    Q_EMIT peapCaCertChanged();
    Q_EMIT validChanged();
}
int Security8021xSetting::peapVersion() const
{
    return m_peapVersion;
}
void Security8021xSetting::setPeapVersion(int v)
{
    if (m_peapVersion == v)
        return;
    m_peapVersion = v;
    Q_EMIT peapVersionChanged();
    Q_EMIT validChanged();
}
int Security8021xSetting::peapInnerAuth() const
{
    return m_peapInnerAuth;
}
void Security8021xSetting::setPeapInnerAuth(int v)
{
    if (m_peapInnerAuth == v)
        return;
    m_peapInnerAuth = v;
    Q_EMIT peapInnerAuthChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::peapUsername() const
{
    return m_peapUsername;
}
void Security8021xSetting::setPeapUsername(const QString &v)
{
    if (m_peapUsername == v)
        return;
    m_peapUsername = v;
    Q_EMIT peapUsernameChanged();
    Q_EMIT validChanged();
}
QString Security8021xSetting::peapPassword() const
{
    return m_peapPassword;
}
void Security8021xSetting::setPeapPassword(const QString &v)
{
    if (m_peapPassword == v)
        return;
    m_peapPassword = v;
    Q_EMIT peapPasswordChanged();
    Q_EMIT validChanged();
}
Security8021xSetting::PasswordOption Security8021xSetting::peapPasswordOption() const
{
    return m_peapPasswordOption;
}
void Security8021xSetting::setPeapPasswordOption(PasswordOption v)
{
    if (m_peapPasswordOption == v)
        return;
    m_peapPasswordOption = v;
    Q_EMIT peapPasswordOptionChanged();
    Q_EMIT validChanged();
}

#include "moc_security8021xsetting.cpp"
