/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMA_NM_VPNC_AUTH_QML_H
#define PLASMA_NM_VPNC_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT VpncAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString user READ user NOTIFY userChanged)
    Q_PROPERTY(QString group READ group NOTIFY groupChanged)

    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QString groupPassword READ groupPassword WRITE setGroupPassword NOTIFY groupPasswordChanged)

    Q_PROPERTY(bool userPasswordRequired READ userPasswordRequired NOTIFY userPasswordRequiredChanged)
    Q_PROPERTY(bool groupPasswordRequired READ groupPasswordRequired NOTIFY groupPasswordRequiredChanged)

public:
    explicit VpncAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~VpncAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString user() const;
    QString group() const;

    QString userPassword() const;
    void setUserPassword(const QString &password);

    QString groupPassword() const;
    void setGroupPassword(const QString &password);

    bool userPasswordRequired() const;
    bool groupPasswordRequired() const;

Q_SIGNALS:
    void userChanged();
    void groupChanged();
    void userPasswordChanged();
    void groupPasswordChanged();
    void userPasswordRequiredChanged();
    void groupPasswordRequiredChanged();

private:
    void setUser(const QString &user);
    void setGroup(const QString &group);
    void setUserPasswordRequired(bool required);
    void setGroupPasswordRequired(bool required);

    QStringList m_hints;

    QString m_user;
    QString m_group;
    QString m_userPassword;
    QString m_groupPassword;
    bool m_userPasswordRequired = true;
    bool m_groupPasswordRequired = true;
};

#endif // PLASMA_NM_VPNC_AUTH_QML_H
