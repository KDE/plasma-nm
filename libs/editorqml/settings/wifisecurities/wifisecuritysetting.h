/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_WIFI_SECURITY_NH
#define PLASMA_NM_WIFI_SECURITY_NH

#include "plasmanm_editorqml_export.h"
#include "security8021xsetting.h"

#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>

#include <QObject>
#include <QQmlEngine>
#include <qqmlintegration.h>

class PLASMANM_EDITORQML_EXPORT WifiSecuritySetting : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE("")
    QML_ELEMENT

    Q_PROPERTY(Security8021xSetting *setting8021x READ setting8021x_pointer CONSTANT)
    Q_PROPERTY(SecurityType securityType READ securityType WRITE setSecurityType NOTIFY securityTypeChanged)

    Q_PROPERTY(QString psk READ psk WRITE setPsk NOTIFY pskChanged)
    Q_PROPERTY(PasswordOption pskOption READ pskOption WRITE setPskOption NOTIFY pskOptionChanged)

    Q_PROPERTY(QString wepKey READ wepKey WRITE setWepKey NOTIFY wepKeyChanged)
    Q_PROPERTY(int wepKeyIndex READ wepKeyIndex WRITE setWepKeyIndex NOTIFY wepKeyIndexChanged)
    Q_PROPERTY(int wepAuth READ wepAuth WRITE setWepAuth NOTIFY wepAuthChanged)
    Q_PROPERTY(PasswordOption wepKeyOption READ wepKeyOption WRITE setWepKeyOption NOTIFY wepKeyOptionChanged)

    Q_PROPERTY(QString leapUsername READ leapUsername WRITE setLeapUsername NOTIFY leapUsernameChanged)
    Q_PROPERTY(QString leapPassword READ leapPassword WRITE setLeapPassword NOTIFY leapPasswordChanged)
    Q_PROPERTY(PasswordOption leapPasswordOption READ leapPasswordOption WRITE setLeapPasswordOption NOTIFY leapPasswordOptionChanged)

    Q_PROPERTY(bool showPsk READ showPsk NOTIFY securityTypeChanged)
    Q_PROPERTY(bool showWep READ showWep NOTIFY securityTypeChanged)
    Q_PROPERTY(bool showLeap READ showLeap NOTIFY securityTypeChanged)
    Q_PROPERTY(bool showEnterprise READ showEnterprise NOTIFY securityTypeChanged)
    Q_PROPERTY(bool showWpa3Enterprise192 READ showWpa3Enterprise192 NOTIFY securityTypeChanged)

    Q_PROPERTY(bool enabled READ enabled NOTIFY securityTypeChanged)
    Q_PROPERTY(bool enabled8021x READ enabled8021x NOTIFY securityTypeChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum SecurityType {
        None = 0,
        WepHex,
        WepPassphrase,
        Leap,
        DynamicWep,
        WpaPsk,
        WpaEap,
        SAE,
        Wpa3SuiteB192,
        OWE
    };
    Q_ENUM(SecurityType)

    enum PasswordOption {
        StoreForAllUsers,
        StoreForUser,
        AlwaysAsk
    };
    Q_ENUM(PasswordOption)

    explicit WifiSecuritySetting(QObject *parent = nullptr);

    Q_INVOKABLE void loadConfig(const NetworkManager::WirelessSecuritySetting::Ptr &setting);
    Q_INVOKABLE void loadSecrets(const NetworkManager::WirelessSecuritySetting::Ptr &setting,
                                 const NetworkManager::Security8021xSetting::Ptr &security8021xSetting);

    [[nodiscard]] Q_INVOKABLE QVariantMap setting() const;
    [[nodiscard]] Q_INVOKABLE QVariantMap setting8021x() const;

    Q_INVOKABLE void onSsidChanged(const QString &ssid);

    [[nodiscard]] Security8021xSetting *setting8021x_pointer() const;
    [[nodiscard]] SecurityType securityType() const;
    void setSecurityType(SecurityType type);

    [[nodiscard]] QString psk() const;
    void setPsk(const QString &psk);
    [[nodiscard]] PasswordOption pskOption() const;
    void setPskOption(PasswordOption option);

    [[nodiscard]] QString wepKey() const;
    void setWepKey(const QString &key);
    [[nodiscard]] int wepKeyIndex() const;
    void setWepKeyIndex(int index);
    [[nodiscard]] int wepAuth() const;
    void setWepAuth(int auth);
    [[nodiscard]] PasswordOption wepKeyOption() const;
    void setWepKeyOption(PasswordOption option);

    [[nodiscard]] QString leapUsername() const;
    void setLeapUsername(const QString &username);
    [[nodiscard]] QString leapPassword() const;
    void setLeapPassword(const QString &password);
    [[nodiscard]] PasswordOption leapPasswordOption() const;
    void setLeapPasswordOption(PasswordOption option);

    [[nodiscard]] bool showPsk() const;
    [[nodiscard]] bool showWep() const;
    [[nodiscard]] bool showLeap() const;
    [[nodiscard]] bool showEnterprise() const;
    [[nodiscard]] bool showWpa3Enterprise192() const;
    [[nodiscard]] bool enabled() const;
    [[nodiscard]] bool enabled8021x() const;
    [[nodiscard]] bool isValid() const;

Q_SIGNALS:
    void securityTypeChanged();
    void pskChanged();
    void pskOptionChanged();
    void wepKeyChanged();
    void wepKeyIndexChanged();
    void wepAuthChanged();
    void wepKeyOptionChanged();
    void leapUsernameChanged();
    void leapPasswordChanged();
    void leapPasswordOptionChanged();
    void validChanged();

private:
    SecurityType m_securityType = None;

    QString m_psk;
    PasswordOption m_pskOption = StoreForUser;

    QString m_wepKeys[4];
    QString m_wepKey;
    int m_wepKeyIndex = 0;
    int m_wepAuth = 0;
    PasswordOption m_wepKeyOption = StoreForUser;

    QString m_leapUsername;
    QString m_leapPassword;
    PasswordOption m_leapPasswordOption = StoreForUser;

    Security8021xSetting *m_8021xSetting = nullptr;
    NetworkManager::WirelessSecuritySetting::Ptr m_wifiSecurity;
};

#endif
