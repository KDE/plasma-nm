/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_STRONGSWAN_AUTH_QML_H
#define PLASMA_NM_STRONGSWAN_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT StrongswanAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    Q_PROPERTY(bool passwordRequired READ passwordRequired NOTIFY passwordRequiredChanged)

    Q_PROPERTY(SecretKind secretKind READ secretKind NOTIFY secretKindChanged)

public:
    enum SecretKind {
        Password = 0,
        PrivateKeyPassword,
        Pin
    };
    Q_ENUM(SecretKind)

    explicit StrongswanAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~StrongswanAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString password() const;
    void setPassword(const QString &password);

    bool passwordRequired() const;
    SecretKind secretKind() const;

Q_SIGNALS:
    void passwordChanged();
    void passwordRequiredChanged();
    void secretKindChanged();

private:
    void setPasswordRequired(bool required);
    void setSecretKind(SecretKind kind);

    QStringList m_hints;

    QString m_password;
    bool m_passwordRequired = true;
    SecretKind m_secretKind = Password;

    bool m_usesAgent = false;
};

#endif // PLASMA_NM_STRONGSWAN_AUTH_QML_H
