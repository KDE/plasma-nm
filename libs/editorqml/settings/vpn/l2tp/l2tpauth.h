/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_L2TP_AUTH_QML_H
#define PLASMA_NM_L2TP_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT L2tpAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(bool userPasswordRequired READ userPasswordRequired NOTIFY userPasswordRequiredChanged)

    Q_PROPERTY(QString userCertPassword READ userCertPassword WRITE setUserCertPassword NOTIFY userCertPasswordChanged)
    Q_PROPERTY(bool userCertPasswordRequired READ userCertPasswordRequired NOTIFY userCertPasswordRequiredChanged)

    Q_PROPERTY(QString machineCertPassword READ machineCertPassword WRITE setMachineCertPassword NOTIFY machineCertPasswordChanged)
    Q_PROPERTY(bool machineCertPasswordRequired READ machineCertPasswordRequired NOTIFY machineCertPasswordRequiredChanged)

    Q_PROPERTY(QString presharedKey READ presharedKey WRITE setPresharedKey NOTIFY presharedKeyChanged)
    Q_PROPERTY(bool presharedKeyRequired READ presharedKeyRequired NOTIFY presharedKeyRequiredChanged)

public:
    explicit L2tpAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~L2tpAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString userPassword() const;
    void setUserPassword(const QString &password);
    bool userPasswordRequired() const;

    QString userCertPassword() const;
    void setUserCertPassword(const QString &password);
    bool userCertPasswordRequired() const;

    QString machineCertPassword() const;
    void setMachineCertPassword(const QString &password);
    bool machineCertPasswordRequired() const;

    QString presharedKey() const;
    void setPresharedKey(const QString &key);
    bool presharedKeyRequired() const;

Q_SIGNALS:
    void userPasswordChanged();
    void userPasswordRequiredChanged();
    void userCertPasswordChanged();
    void userCertPasswordRequiredChanged();
    void machineCertPasswordChanged();
    void machineCertPasswordRequiredChanged();
    void presharedKeyChanged();
    void presharedKeyRequiredChanged();

private:
    void setUserPasswordRequired(bool required);
    void setUserCertPasswordRequired(bool required);
    void setMachineCertPasswordRequired(bool required);
    void setPresharedKeyRequired(bool required);

    QStringList m_hints;

    QString m_userPassword;
    bool m_userPasswordRequired = false;

    QString m_userCertPassword;
    bool m_userCertPasswordRequired = false;

    QString m_machineCertPassword;
    bool m_machineCertPasswordRequired = false;

    QString m_presharedKey;
    bool m_presharedKeyRequired = false;
};

#endif // PLASMA_NM_L2TP_AUTH_QML_H
