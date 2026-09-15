/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_VPNC_QML_H
#define PLASMA_NM_VPNC_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QUrl>

class PLASMANM_EDITORQML_EXPORT VpncSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    // General
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)

    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(PasswordOption userPasswordOption READ userPasswordOption WRITE setUserPasswordOption NOTIFY userPasswordOptionChanged)

    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
    Q_PROPERTY(QString groupPassword READ groupPassword WRITE setGroupPassword NOTIFY groupPasswordChanged)
    Q_PROPERTY(PasswordOption groupPasswordOption READ groupPasswordOption WRITE setGroupPasswordOption NOTIFY groupPasswordOptionChanged)

    Q_PROPERTY(bool useHybridAuth READ useHybridAuth WRITE setUseHybridAuth NOTIFY useHybridAuthChanged)
    Q_PROPERTY(QString caFile READ caFile WRITE setCaFile NOTIFY caFileChanged)

    // Advanced - Identification
    Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)
    Q_PROPERTY(Vendor vendor READ vendor WRITE setVendor NOTIFY vendorChanged)

    // Advanced - Transport and Security
    Q_PROPERTY(Encryption encryption READ encryption WRITE setEncryption NOTIFY encryptionChanged)
    Q_PROPERTY(NatTraversal natTraversal READ natTraversal WRITE setNatTraversal NOTIFY natTraversalChanged)
    Q_PROPERTY(DhGroup dhGroup READ dhGroup WRITE setDhGroup NOTIFY dhGroupChanged)
    Q_PROPERTY(Pfs perfectForwardSecrecy READ perfectForwardSecrecy WRITE setPerfectForwardSecrecy NOTIFY perfectForwardSecrecyChanged)
    Q_PROPERTY(int localPort READ localPort WRITE setLocalPort NOTIFY localPortChanged)
    Q_PROPERTY(bool disableDeadPeerDetection READ disableDeadPeerDetection WRITE setDisableDeadPeerDetection NOTIFY disableDeadPeerDetectionChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk,
        NotRequired
    };
    Q_ENUM(PasswordOption)

    enum Vendor {
        Cisco = 0,
        Netscreen,
        Fortigate
    };
    Q_ENUM(Vendor)

    enum Encryption {
        Secure = 0,
        Weak,
        NoEncryption
    };
    Q_ENUM(Encryption)

    enum NatTraversal {
        NattWhenAvailable = 0,
        NattAlways,
        CiscoUdp,
        NattDisabled
    };
    Q_ENUM(NatTraversal)

    enum DhGroup {
        DhGroup1 = 0,
        DhGroup2,
        DhGroup5
    };
    Q_ENUM(DhGroup)

    enum Pfs {
        PfsServer = 0,
        PfsNone,
        PfsDh1,
        PfsDh2,
        PfsDh5
    };
    Q_ENUM(Pfs)

    explicit VpncSetting(QObject *parent = nullptr);
    ~VpncSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString user() const;
    void setUser(const QString &user);

    QString userPassword() const;
    void setUserPassword(const QString &password);

    PasswordOption userPasswordOption() const;
    void setUserPasswordOption(PasswordOption option);

    QString group() const;
    void setGroup(const QString &group);

    QString groupPassword() const;
    void setGroupPassword(const QString &password);

    PasswordOption groupPasswordOption() const;
    void setGroupPasswordOption(PasswordOption option);

    bool useHybridAuth() const;
    void setUseHybridAuth(bool use);

    QString caFile() const;
    void setCaFile(const QString &caFile);

    QString domain() const;
    void setDomain(const QString &domain);

    Vendor vendor() const;
    void setVendor(Vendor vendor);

    Encryption encryption() const;
    void setEncryption(Encryption encryption);

    NatTraversal natTraversal() const;
    void setNatTraversal(NatTraversal natTraversal);

    DhGroup dhGroup() const;
    void setDhGroup(DhGroup dhGroup);

    Pfs perfectForwardSecrecy() const;
    void setPerfectForwardSecrecy(Pfs pfs);

    int localPort() const;
    void setLocalPort(int port);

    bool disableDeadPeerDetection() const;
    void setDisableDeadPeerDetection(bool disable);

Q_SIGNALS:
    void gatewayChanged();

    void userChanged();
    void userPasswordChanged();
    void userPasswordOptionChanged();

    void groupChanged();
    void groupPasswordChanged();
    void groupPasswordOptionChanged();

    void useHybridAuthChanged();
    void caFileChanged();

    void domainChanged();
    void vendorChanged();

    void encryptionChanged();
    void natTraversalChanged();
    void dhGroupChanged();
    void perfectForwardSecrecyChanged();
    void localPortChanged();
    void disableDeadPeerDetectionChanged();

    void validChanged();

private:
    QString m_gateway;

    QString m_user;
    QString m_userPassword;
    PasswordOption m_userPasswordOption = StoreForUser;

    QString m_group;
    QString m_groupPassword;
    PasswordOption m_groupPasswordOption = StoreForUser;

    bool m_useHybridAuth = false;
    QString m_caFile;

    QString m_domain;
    Vendor m_vendor = Cisco;

    Encryption m_encryption = Secure;
    NatTraversal m_natTraversal = NattWhenAvailable;
    DhGroup m_dhGroup = DhGroup2; // vpnc's default
    Pfs m_perfectForwardSecrecy = PfsServer;
    int m_localPort = 0; // 0 means a random port
    bool m_disableDeadPeerDetection = false;
};

#endif // PLASMA_NM_VPNC_QML_H
