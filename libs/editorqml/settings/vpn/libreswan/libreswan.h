/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_LIBRESWAN_QML_H
#define PLASMA_NM_LIBRESWAN_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>

class PLASMANM_EDITORQML_EXPORT LibreswanSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)

    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(PasswordOption userPasswordOption READ userPasswordOption WRITE setUserPasswordOption NOTIFY userPasswordOptionChanged)

    Q_PROPERTY(QString groupPassword READ groupPassword WRITE setGroupPassword NOTIFY groupPasswordChanged)
    Q_PROPERTY(PasswordOption groupPasswordOption READ groupPasswordOption WRITE setGroupPasswordOption NOTIFY groupPasswordOptionChanged)

    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString phase1Algorithms READ phase1Algorithms WRITE setPhase1Algorithms NOTIFY phase1AlgorithmsChanged)
    Q_PROPERTY(QString phase2Algorithms READ phase2Algorithms WRITE setPhase2Algorithms NOTIFY phase2AlgorithmsChanged)
    Q_PROPERTY(QString domain READ domain WRITE setDomain NOTIFY domainChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk
    };
    Q_ENUM(PasswordOption)

    explicit LibreswanSetting(QObject *parent = nullptr);
    ~LibreswanSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString gateway() const;
    void setGateway(const QString &gateway);

    QString groupName() const;
    void setGroupName(const QString &groupName);

    QString userPassword() const;
    void setUserPassword(const QString &password);

    PasswordOption userPasswordOption() const;
    void setUserPasswordOption(PasswordOption option);

    QString groupPassword() const;
    void setGroupPassword(const QString &password);

    PasswordOption groupPasswordOption() const;
    void setGroupPasswordOption(PasswordOption option);

    QString username() const;
    void setUsername(const QString &username);

    QString phase1Algorithms() const;
    void setPhase1Algorithms(const QString &algorithms);

    QString phase2Algorithms() const;
    void setPhase2Algorithms(const QString &algorithms);

    QString domain() const;
    void setDomain(const QString &domain);

Q_SIGNALS:
    void gatewayChanged();
    void groupNameChanged();

    void userPasswordChanged();
    void userPasswordOptionChanged();

    void groupPasswordChanged();
    void groupPasswordOptionChanged();

    void usernameChanged();
    void phase1AlgorithmsChanged();
    void phase2AlgorithmsChanged();
    void domainChanged();

    void validChanged();

private:
    QString m_gateway;
    QString m_groupName;

    QString m_userPassword;
    PasswordOption m_userPasswordOption = StoreForUser;

    QString m_groupPassword;
    PasswordOption m_groupPasswordOption = StoreForUser;

    QString m_username;
    QString m_phase1Algorithms;
    QString m_phase2Algorithms;
    QString m_domain;
};

#endif // PLASMA_NM_LIBRESWAN_QML_H
