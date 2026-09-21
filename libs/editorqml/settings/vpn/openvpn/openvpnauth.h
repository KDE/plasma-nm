/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_OPENVPN_AUTH_QML_H
#define PLASMA_NM_OPENVPN_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT OpenvpnAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool usesChallenge READ usesChallenge NOTIFY usesChallengeChanged)
    Q_PROPERTY(QString challengeLabel READ challengeLabel NOTIFY challengeLabelChanged)
    Q_PROPERTY(bool challengeMasked READ challengeMasked NOTIFY challengeMaskedChanged)
    Q_PROPERTY(QString challengePassword READ challengePassword WRITE setChallengePassword NOTIFY challengePasswordChanged)

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool passwordRequired READ passwordRequired NOTIFY passwordRequiredChanged)

    Q_PROPERTY(QString privateKeyPassword READ privateKeyPassword WRITE setPrivateKeyPassword NOTIFY privateKeyPasswordChanged)
    Q_PROPERTY(bool privateKeyPasswordRequired READ privateKeyPasswordRequired NOTIFY privateKeyPasswordRequiredChanged)

    Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged)
    Q_PROPERTY(bool proxyPasswordRequired READ proxyPasswordRequired NOTIFY proxyPasswordRequiredChanged)

public:
    explicit OpenvpnAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~OpenvpnAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    bool usesChallenge() const;
    QString challengeLabel() const;
    bool challengeMasked() const;

    QString challengePassword() const;
    void setChallengePassword(const QString &password);

    QString password() const;
    void setPassword(const QString &password);
    bool passwordRequired() const;

    QString privateKeyPassword() const;
    void setPrivateKeyPassword(const QString &password);
    bool privateKeyPasswordRequired() const;

    QString proxyPassword() const;
    void setProxyPassword(const QString &password);
    bool proxyPasswordRequired() const;

Q_SIGNALS:
    void usesChallengeChanged();
    void challengeLabelChanged();
    void challengeMaskedChanged();
    void challengePasswordChanged();

    void passwordChanged();
    void passwordRequiredChanged();

    void privateKeyPasswordChanged();
    void privateKeyPasswordRequiredChanged();

    void proxyPasswordChanged();
    void proxyPasswordRequiredChanged();

private:
    void readHints();
    void setChallengeLabel(const QString &label);
    void setChallengeMasked(bool masked);
    void setPasswordRequired(bool required);
    void setPrivateKeyPasswordRequired(bool required);
    void setProxyPasswordRequired(bool required);

    QStringList m_hints;

    bool m_usesChallenge = false;
    QString m_challengeLabel;
    bool m_challengeMasked = true;
    QString m_challengePassword;
    QString m_challengeSecretKey;

    QString m_password;
    bool m_passwordRequired = false;

    QString m_privateKeyPassword;
    bool m_privateKeyPasswordRequired = false;

    QString m_proxyPassword;
    bool m_proxyPasswordRequired = false;
};

#endif // PLASMA_NM_OPENVPN_AUTH_QML_H
