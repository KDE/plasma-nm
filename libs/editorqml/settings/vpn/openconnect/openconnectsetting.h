/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_OPENCONNECT_QML_H
#define PLASMA_NM_OPENCONNECT_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>

class PLASMANM_EDITORQML_EXPORT OpenconnectSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(Protocol protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(QString caCert READ caCert WRITE setCaCert NOTIFY caCertChanged)
    Q_PROPERTY(QString proxy READ proxy WRITE setProxy NOTIFY proxyChanged)
    Q_PROPERTY(QString userAgent READ userAgent WRITE setUserAgent NOTIFY userAgentChanged)
    Q_PROPERTY(QString csdWrapper READ csdWrapper WRITE setCsdWrapper NOTIFY csdWrapperChanged)
    Q_PROPERTY(bool allowTrojan READ allowTrojan WRITE setAllowTrojan NOTIFY allowTrojanChanged)
    Q_PROPERTY(ReportedOs reportedOs READ reportedOs WRITE setReportedOs NOTIFY reportedOsChanged)
    Q_PROPERTY(QString reportedVersion READ reportedVersion WRITE setReportedVersion NOTIFY reportedVersionChanged)

    // Certificate authentication
    Q_PROPERTY(QString machineCert READ machineCert WRITE setMachineCert NOTIFY machineCertChanged)
    Q_PROPERTY(QString machineKey READ machineKey WRITE setMachineKey NOTIFY machineKeyChanged)
    Q_PROPERTY(QString userCert READ userCert WRITE setUserCert NOTIFY userCertChanged)
    Q_PROPERTY(QString userKey READ userKey WRITE setUserKey NOTIFY userKeyChanged)
    Q_PROPERTY(bool useFsid READ useFsid WRITE setUseFsid NOTIFY useFsidChanged)
    Q_PROPERTY(bool preventInvalidCert READ preventInvalidCert WRITE setPreventInvalidCert NOTIFY preventInvalidCertChanged)

    // Software token, from the separate dialog the button opens
    Q_PROPERTY(TokenMode tokenMode READ tokenMode WRITE setTokenMode NOTIFY tokenModeChanged)
    Q_PROPERTY(QString tokenSecret READ tokenSecret WRITE setTokenSecret NOTIFY tokenSecretChanged)
    Q_PROPERTY(PasswordOption tokenSecretOption READ tokenSecretOption WRITE setTokenSecretOption NOTIFY tokenSecretOptionChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk
    };
    Q_ENUM(PasswordOption)

    enum Protocol {
        AnyConnect = 0,
        JuniperNetworkConnect,
        PanGlobalProtect,
        PulseConnectSecure,
        F5BigIp,
        Fortinet,
        Array
    };
    Q_ENUM(Protocol)

    enum ReportedOs {
        OsDefault = 0,
        Linux,
        Linux64,
        Windows,
        MacOsX,
        Android,
        AppleIos
    };
    Q_ENUM(ReportedOs)

    enum TokenMode {
        TokenDisabled = 0,
        TokenStokenrc,
        TokenManual,
        TokenTotp,
        TokenHotp,
        TokenYubioath
    };
    Q_ENUM(TokenMode)

    explicit OpenconnectSetting(QObject *parent = nullptr);
    ~OpenconnectSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    Protocol protocol() const;
    void setProtocol(Protocol protocol);

    QString caCert() const;
    void setCaCert(const QString &caCert);

    QString proxy() const;
    void setProxy(const QString &proxy);

    QString userAgent() const;
    void setUserAgent(const QString &userAgent);

    QString csdWrapper() const;
    void setCsdWrapper(const QString &csdWrapper);

    bool allowTrojan() const;
    void setAllowTrojan(bool allow);

    ReportedOs reportedOs() const;
    void setReportedOs(ReportedOs reportedOs);

    QString reportedVersion() const;
    void setReportedVersion(const QString &version);

    QString machineCert() const;
    void setMachineCert(const QString &machineCert);

    QString machineKey() const;
    void setMachineKey(const QString &machineKey);

    QString userCert() const;
    void setUserCert(const QString &userCert);

    QString userKey() const;
    void setUserKey(const QString &userKey);

    bool useFsid() const;
    void setUseFsid(bool use);

    bool preventInvalidCert() const;
    void setPreventInvalidCert(bool prevent);

    TokenMode tokenMode() const;
    void setTokenMode(TokenMode mode);

    QString tokenSecret() const;
    void setTokenSecret(const QString &secret);

    PasswordOption tokenSecretOption() const;
    void setTokenSecretOption(PasswordOption option);

Q_SIGNALS:
    void gatewayChanged();
    void protocolChanged();
    void caCertChanged();
    void proxyChanged();
    void userAgentChanged();
    void csdWrapperChanged();
    void allowTrojanChanged();
    void reportedOsChanged();
    void reportedVersionChanged();

    void machineCertChanged();
    void machineKeyChanged();
    void userCertChanged();
    void userKeyChanged();
    void useFsidChanged();
    void preventInvalidCertChanged();

    void tokenModeChanged();
    void tokenSecretChanged();
    void tokenSecretOptionChanged();

    void validChanged();

private:
    QString m_gateway;
    Protocol m_protocol = AnyConnect;
    QString m_caCert;
    QString m_proxy;
    QString m_userAgent;
    QString m_csdWrapper;
    bool m_allowTrojan = false;
    ReportedOs m_reportedOs = OsDefault;
    QString m_reportedVersion;

    QString m_machineCert;
    QString m_machineKey;
    QString m_userCert;
    QString m_userKey;
    bool m_useFsid = false;
    bool m_preventInvalidCert = false;

    NMStringMap m_originalData;

    TokenMode m_tokenMode = TokenDisabled;
    QString m_tokenSecret;
    PasswordOption m_tokenSecretOption = StoreForUser;
};

#endif // PLASMA_NM_OPENCONNECT_QML_H
