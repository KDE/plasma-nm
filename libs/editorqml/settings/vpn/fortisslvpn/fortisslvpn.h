/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_FORTISSLVPN_QML_H
#define PLASMA_NM_FORTISSLVPN_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QUrl>

class PLASMANM_EDITORQML_EXPORT FortisslvpnSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    // General
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)

    // Authentication
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(PasswordOption passwordOption READ passwordOption WRITE setPasswordOption NOTIFY passwordOptionChanged)
    Q_PROPERTY(QString caCert READ caCert WRITE setCaCert NOTIFY caCertChanged)
    Q_PROPERTY(QString userCert READ userCert WRITE setUserCert NOTIFY userCertChanged)
    Q_PROPERTY(QString userKey READ userKey WRITE setUserKey NOTIFY userKeyChanged)

    // Advanced
    Q_PROPERTY(bool useOtp READ useOtp WRITE setUseOtp NOTIFY useOtpChanged)
    Q_PROPERTY(bool useTwoFactorAuth READ useTwoFactorAuth WRITE setUseTwoFactorAuth NOTIFY useTwoFactorAuthChanged)
    Q_PROPERTY(QString realm READ realm WRITE setRealm NOTIFY realmChanged)
    Q_PROPERTY(QString trustedCert READ trustedCert WRITE setTrustedCert NOTIFY trustedCertChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    explicit FortisslvpnSetting(QObject *parent = nullptr);
    ~FortisslvpnSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString user() const;
    void setUser(const QString &user);

    QString password() const;
    void setPassword(const QString &password);

    PasswordOption passwordOption() const;
    void setPasswordOption(PasswordOption option);

    QString caCert() const;
    void setCaCert(const QString &caCert);

    QString userCert() const;
    void setUserCert(const QString &userCert);

    QString userKey() const;
    void setUserKey(const QString &userKey);

    bool useOtp() const;
    void setUseOtp(bool use);

    bool useTwoFactorAuth() const;
    void setUseTwoFactorAuth(bool use);

    QString realm() const;
    void setRealm(const QString &realm);

    QString trustedCert() const;
    void setTrustedCert(const QString &trustedCert);

Q_SIGNALS:
    void gatewayChanged();

    void userChanged();
    void passwordChanged();
    void passwordOptionChanged();
    void caCertChanged();
    void userCertChanged();
    void userKeyChanged();

    void useOtpChanged();
    void useTwoFactorAuthChanged();
    void realmChanged();
    void trustedCertChanged();

    void validChanged();

private:
    QString m_gateway;

    QString m_user;
    QString m_password;
    PasswordOption m_passwordOption = StoreForUser;
    QString m_caCert;
    QString m_userCert;
    QString m_userKey;

    bool m_useOtp = false;
    bool m_useTwoFactorAuth = false;
    QString m_realm;
    QString m_trustedCert;
};

#endif // PLASMA_NM_FORTISSLVPN_QML_H
