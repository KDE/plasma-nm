/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_FORTISSLVPN_AUTH_QML_H
#define PLASMA_NM_FORTISSLVPN_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT FortisslvpnAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString otp READ otp WRITE setOtp NOTIFY otpChanged)

    Q_PROPERTY(bool passwordRequired READ passwordRequired NOTIFY passwordRequiredChanged)
    Q_PROPERTY(bool otpRequired READ otpRequired NOTIFY otpRequiredChanged)

    Q_PROPERTY(QString otpHeadline READ otpHeadline NOTIFY otpHeadlineChanged)
    Q_PROPERTY(QString otpLabel READ otpLabel NOTIFY otpLabelChanged)

public:
    explicit FortisslvpnAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~FortisslvpnAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString password() const;
    void setPassword(const QString &password);

    QString otp() const;
    void setOtp(const QString &otp);

    bool passwordRequired() const;
    bool otpRequired() const;

    QString otpHeadline() const;
    QString otpLabel() const;

Q_SIGNALS:
    void passwordChanged();
    void otpChanged();
    void passwordRequiredChanged();
    void otpRequiredChanged();
    void otpHeadlineChanged();
    void otpLabelChanged();

private:
    void setPasswordRequired(bool required);
    void setOtpRequired(bool required);
    void setOtpHeadline(const QString &headline);
    void setOtpLabel(const QString &label);

    QStringList m_hints;

    QString m_password;
    QString m_otp;
    bool m_passwordRequired = false;
    bool m_otpRequired = false;
    QString m_otpHeadline;
    QString m_otpLabel;

    // Kept verbatim so setting() can decide exactly like the connection does.
    QString m_rawOtpFlags;
    QString m_rawTwoFactorAuthFlags;
};

#endif // PLASMA_NM_FORTISSLVPN_AUTH_QML_H
