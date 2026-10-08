/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_WIREGUARD_QML_H
#define PLASMA_NM_WIREGUARD_QML_H

#include "plasmanm_editorqml_export.h"
#include "wireguardpeermodel.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WireguardSetting>

#include <QObject>

class QValidator;

class PLASMANM_EDITORQML_EXPORT WireguardSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString interfaceName READ interfaceName WRITE setInterfaceName NOTIFY interfaceNameChanged)
    Q_PROPERTY(QString privateKey READ privateKey WRITE setPrivateKey NOTIFY privateKeyChanged)
    Q_PROPERTY(PasswordOption privateKeyOption READ privateKeyOption WRITE setPrivateKeyOption NOTIFY privateKeyOptionChanged)

    Q_PROPERTY(int listenPort READ listenPort WRITE setListenPort NOTIFY listenPortChanged)
    Q_PROPERTY(int fwmark READ fwmark WRITE setFwmark NOTIFY fwmarkChanged)
    Q_PROPERTY(int mtu READ mtu WRITE setMtu NOTIFY mtuChanged)

    Q_PROPERTY(bool peerRoutes READ peerRoutes WRITE setPeerRoutes NOTIFY peerRoutesChanged)

    Q_PROPERTY(WireguardPeerModel *peers READ peers CONSTANT)

    Q_PROPERTY(QValidator *keyValidator READ keyValidator CONSTANT)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        NotRequired,
    };
    Q_ENUM(PasswordOption)

    explicit WireguardSetting(QObject *parent = nullptr);
    ~WireguardSetting() override;

    static QString supportedFileExtensions();

    static NMVariantMapMap importConnectionSettings(const QString &fileName);

    void loadConfig(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void loadSecrets(const NetworkManager::Setting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    WireguardPeerModel *peers() const;
    QValidator *keyValidator() const;

    QString interfaceName() const;
    void setInterfaceName(const QString &interfaceName);

    QString privateKey() const;
    void setPrivateKey(const QString &privateKey);

    PasswordOption privateKeyOption() const;
    void setPrivateKeyOption(PasswordOption option);

    int listenPort() const;
    void setListenPort(int port);

    int fwmark() const;
    void setFwmark(int fwmark);

    int mtu() const;
    void setMtu(int mtu);

    bool peerRoutes() const;
    void setPeerRoutes(bool peerRoutes);

Q_SIGNALS:
    void interfaceNameChanged();
    void privateKeyChanged();
    void privateKeyOptionChanged();
    void listenPortChanged();
    void fwmarkChanged();
    void mtuChanged();
    void peerRoutesChanged();

    void validChanged();

private:
    WireguardPeerModel *const m_peers;
    QValidator *const m_keyValidator;

    QString m_interfaceName;
    QString m_privateKey;
    PasswordOption m_privateKeyOption = StoreForUser;
    int m_listenPort = 0;
    int m_fwmark = 0;
    int m_mtu = 0;
    bool m_peerRoutes = true;
};

#endif // PLASMA_NM_WIREGUARD_QML_H
