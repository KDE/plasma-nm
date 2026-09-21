/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_OPENVPN_QML_H
#define PLASMA_NM_OPENVPN_QML_H

#include "openvpnadvanced.h"
#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>

class PLASMANM_EDITORQML_EXPORT OpenvpnSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    Q_PROPERTY(OpenvpnAdvancedSetting *advanced READ advanced CONSTANT)

    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(ConnectionType connectionType READ connectionType WRITE setConnectionType NOTIFY connectionTypeChanged)

    Q_PROPERTY(QString caCert READ caCert WRITE setCaCert NOTIFY caCertChanged)
    Q_PROPERTY(QString userCert READ userCert WRITE setUserCert NOTIFY userCertChanged)
    Q_PROPERTY(QString privateKey READ privateKey WRITE setPrivateKey NOTIFY privateKeyChanged)
    Q_PROPERTY(QString privateKeyPassword READ privateKeyPassword WRITE setPrivateKeyPassword NOTIFY privateKeyPasswordChanged)
    Q_PROPERTY(PasswordOption privateKeyPasswordOption READ privateKeyPasswordOption WRITE setPrivateKeyPasswordOption NOTIFY privateKeyPasswordOptionChanged)

    // Password based types
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(PasswordOption passwordOption READ passwordOption WRITE setPasswordOption NOTIFY passwordOptionChanged)

    // Static key
    Q_PROPERTY(QString staticKey READ staticKey WRITE setStaticKey NOTIFY staticKeyChanged)
    Q_PROPERTY(KeyDirection keyDirection READ keyDirection WRITE setKeyDirection NOTIFY keyDirectionChanged)
    Q_PROPERTY(QString localIp READ localIp WRITE setLocalIp NOTIFY localIpChanged)
    Q_PROPERTY(QString remoteIp READ remoteIp WRITE setRemoteIp NOTIFY remoteIpChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    enum ConnectionType {
        Certificates = 0,
        StaticKeyType,
        Password,
        CertsPassword
    };
    Q_ENUM(ConnectionType)

    enum KeyDirection {
        NoDirection = 0,
        Direction0,
        Direction1
    };
    Q_ENUM(KeyDirection)

    explicit OpenvpnSetting(QObject *parent = nullptr);
    ~OpenvpnSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    OpenvpnAdvancedSetting *advanced() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    ConnectionType connectionType() const;
    void setConnectionType(ConnectionType type);

    QString caCert() const;
    void setCaCert(const QString &caCert);

    QString userCert() const;
    void setUserCert(const QString &userCert);

    QString privateKey() const;
    void setPrivateKey(const QString &privateKey);

    QString privateKeyPassword() const;
    void setPrivateKeyPassword(const QString &password);

    PasswordOption privateKeyPasswordOption() const;
    void setPrivateKeyPasswordOption(PasswordOption option);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

    PasswordOption passwordOption() const;
    void setPasswordOption(PasswordOption option);

    QString staticKey() const;
    void setStaticKey(const QString &staticKey);

    KeyDirection keyDirection() const;
    void setKeyDirection(KeyDirection direction);

    QString localIp() const;
    void setLocalIp(const QString &localIp);

    QString remoteIp() const;
    void setRemoteIp(const QString &remoteIp);

Q_SIGNALS:
    void gatewayChanged();
    void connectionTypeChanged();

    void caCertChanged();
    void userCertChanged();
    void privateKeyChanged();
    void privateKeyPasswordChanged();
    void privateKeyPasswordOptionChanged();

    void usernameChanged();
    void passwordChanged();
    void passwordOptionChanged();

    void staticKeyChanged();
    void keyDirectionChanged();
    void localIpChanged();
    void remoteIpChanged();

    void validChanged();

private:
    OpenvpnAdvancedSetting *const m_advanced;

    QString m_gateway;
    ConnectionType m_connectionType = Certificates;

    QString m_caCert;
    QString m_userCert;
    QString m_privateKey;
    QString m_privateKeyPassword;
    PasswordOption m_privateKeyPasswordOption = StoreForUser;

    QString m_username;
    QString m_password;
    PasswordOption m_passwordOption = StoreForUser;

    QString m_staticKey;
    KeyDirection m_keyDirection = NoDirection;
    QString m_localIp;
    QString m_remoteIp;
};

#endif // PLASMA_NM_OPENVPN_QML_H
