/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_LIBRESWAN_AUTH_QML_H
#define PLASMA_NM_LIBRESWAN_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT LibreswanAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString groupName READ groupName NOTIFY groupNameChanged)

    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QString groupPassword READ groupPassword WRITE setGroupPassword NOTIFY groupPasswordChanged)

    Q_PROPERTY(bool userPasswordRequired READ userPasswordRequired NOTIFY userPasswordRequiredChanged)
    Q_PROPERTY(bool groupPasswordRequired READ groupPasswordRequired NOTIFY groupPasswordRequiredChanged)

public:
    explicit LibreswanAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~LibreswanAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString groupName() const;

    QString userPassword() const;
    void setUserPassword(const QString &password);

    QString groupPassword() const;
    void setGroupPassword(const QString &password);

    bool userPasswordRequired() const;
    bool groupPasswordRequired() const;

Q_SIGNALS:
    void groupNameChanged();
    void userPasswordChanged();
    void groupPasswordChanged();
    void userPasswordRequiredChanged();
    void groupPasswordRequiredChanged();

private:
    void setGroupName(const QString &groupName);
    void setUserPasswordRequired(bool required);
    void setGroupPasswordRequired(bool required);

    QStringList m_hints;

    QString m_groupName;
    QString m_userPassword;
    QString m_groupPassword;
    bool m_userPasswordRequired = true;
    bool m_groupPasswordRequired = true;
};

#endif // PLASMA_NM_LIBRESWAN_AUTH_QML_H
