/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_SSH_QML_H
#define PLASMA_NM_SSH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QUrl>

class PLASMANM_EDITORQML_EXPORT SshSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)

    // Network settings
    Q_PROPERTY(QString remoteIp READ remoteIp WRITE setRemoteIp NOTIFY remoteIpChanged)
    Q_PROPERTY(QString localIp READ localIp WRITE setLocalIp NOTIFY localIpChanged)
    Q_PROPERTY(QString netmask READ netmask WRITE setNetmask NOTIFY netmaskChanged)

    // IPv6 network settings
    Q_PROPERTY(bool useIpv6 READ useIpv6 WRITE setUseIpv6 NOTIFY useIpv6Changed)
    Q_PROPERTY(QString remoteIpv6 READ remoteIpv6 WRITE setRemoteIpv6 NOTIFY remoteIpv6Changed)
    Q_PROPERTY(QString localIpv6 READ localIpv6 WRITE setLocalIpv6 NOTIFY localIpv6Changed)
    Q_PROPERTY(QString netmaskIpv6 READ netmaskIpv6 WRITE setNetmaskIpv6 NOTIFY netmaskIpv6Changed)

    // Authentication
    Q_PROPERTY(AuthType authType READ authType WRITE setAuthType NOTIFY authTypeChanged)
    Q_PROPERTY(QString sshPassword READ sshPassword WRITE setSshPassword NOTIFY sshPasswordChanged)
    Q_PROPERTY(PasswordOption sshPasswordOption READ sshPasswordOption WRITE setSshPasswordOption NOTIFY sshPasswordOptionChanged)
    Q_PROPERTY(QString keyFile READ keyFile WRITE setKeyFile NOTIFY keyFileChanged)

    // Advanced
    Q_PROPERTY(bool useCustomPort READ useCustomPort WRITE setUseCustomPort NOTIFY useCustomPortChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool useCustomMtu READ useCustomMtu WRITE setUseCustomMtu NOTIFY useCustomMtuChanged)
    Q_PROPERTY(int mtu READ mtu WRITE setMtu NOTIFY mtuChanged)
    Q_PROPERTY(bool useExtraOptions READ useExtraOptions WRITE setUseExtraOptions NOTIFY useExtraOptionsChanged)
    Q_PROPERTY(QString extraOptions READ extraOptions WRITE setExtraOptions NOTIFY extraOptionsChanged)
    Q_PROPERTY(bool useRemoteDevice READ useRemoteDevice WRITE setUseRemoteDevice NOTIFY useRemoteDeviceChanged)
    Q_PROPERTY(int remoteDevice READ remoteDevice WRITE setRemoteDevice NOTIFY remoteDeviceChanged)
    Q_PROPERTY(bool useTapDevice READ useTapDevice WRITE setUseTapDevice NOTIFY useTapDeviceChanged)
    Q_PROPERTY(bool useRemoteUsername READ useRemoteUsername WRITE setUseRemoteUsername NOTIFY useRemoteUsernameChanged)
    Q_PROPERTY(QString remoteUsername READ remoteUsername WRITE setRemoteUsername NOTIFY remoteUsernameChanged)
    Q_PROPERTY(bool doNotReplaceDefaultRoute READ doNotReplaceDefaultRoute WRITE setDoNotReplaceDefaultRoute NOTIFY doNotReplaceDefaultRouteChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum AuthType {
        SshAgent = 0,
        Password,
        Key
    };
    Q_ENUM(AuthType)

    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk
    };
    Q_ENUM(PasswordOption)

    explicit SshSetting(QObject *parent = nullptr);
    ~SshSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString remoteIp() const;
    void setRemoteIp(const QString &remoteIp);
    QString localIp() const;
    void setLocalIp(const QString &localIp);
    QString netmask() const;
    void setNetmask(const QString &netmask);

    bool useIpv6() const;
    void setUseIpv6(bool useIpv6);
    QString remoteIpv6() const;
    void setRemoteIpv6(const QString &remoteIpv6);
    QString localIpv6() const;
    void setLocalIpv6(const QString &localIpv6);
    QString netmaskIpv6() const;
    void setNetmaskIpv6(const QString &netmaskIpv6);

    AuthType authType() const;
    void setAuthType(AuthType authType);
    QString sshPassword() const;
    void setSshPassword(const QString &password);
    PasswordOption sshPasswordOption() const;
    void setSshPasswordOption(PasswordOption option);
    QString keyFile() const;
    void setKeyFile(const QString &keyFile);

    bool useCustomPort() const;
    void setUseCustomPort(bool useCustomPort);
    int port() const;
    void setPort(int port);
    bool useCustomMtu() const;
    void setUseCustomMtu(bool useCustomMtu);
    int mtu() const;
    void setMtu(int mtu);
    bool useExtraOptions() const;
    void setUseExtraOptions(bool useExtraOptions);
    QString extraOptions() const;
    void setExtraOptions(const QString &extraOptions);
    bool useRemoteDevice() const;
    void setUseRemoteDevice(bool useRemoteDevice);
    int remoteDevice() const;
    void setRemoteDevice(int remoteDevice);
    bool useTapDevice() const;
    void setUseTapDevice(bool useTapDevice);
    bool useRemoteUsername() const;
    void setUseRemoteUsername(bool useRemoteUsername);
    QString remoteUsername() const;
    void setRemoteUsername(const QString &remoteUsername);
    bool doNotReplaceDefaultRoute() const;
    void setDoNotReplaceDefaultRoute(bool doNotReplaceDefaultRoute);

Q_SIGNALS:
    void gatewayChanged();
    void remoteIpChanged();
    void localIpChanged();
    void netmaskChanged();
    void useIpv6Changed();
    void remoteIpv6Changed();
    void localIpv6Changed();
    void netmaskIpv6Changed();
    void authTypeChanged();
    void sshPasswordChanged();
    void sshPasswordOptionChanged();
    void keyFileChanged();
    void useCustomPortChanged();
    void portChanged();
    void useCustomMtuChanged();
    void mtuChanged();
    void useExtraOptionsChanged();
    void extraOptionsChanged();
    void useRemoteDeviceChanged();
    void remoteDeviceChanged();
    void useTapDeviceChanged();
    void useRemoteUsernameChanged();
    void remoteUsernameChanged();
    void doNotReplaceDefaultRouteChanged();
    void validChanged();

private:
    QString m_gateway;

    QString m_remoteIp;
    QString m_localIp;
    QString m_netmask;

    bool m_useIpv6 = false;
    QString m_remoteIpv6;
    QString m_localIpv6;
    QString m_netmaskIpv6;

    AuthType m_authType = SshAgent;
    QString m_sshPassword;
    PasswordOption m_sshPasswordOption = StoreForUser;
    QString m_keyFile;

    bool m_useCustomPort = false;
    int m_port;
    bool m_useCustomMtu = false;
    int m_mtu;
    bool m_useExtraOptions = false;
    QString m_extraOptions;
    bool m_useRemoteDevice = false;
    int m_remoteDevice;
    bool m_useTapDevice = false;
    bool m_useRemoteUsername = false;
    QString m_remoteUsername;
    bool m_doNotReplaceDefaultRoute = false;
};

#endif // PLASMA_NM_SSH_QML_H
