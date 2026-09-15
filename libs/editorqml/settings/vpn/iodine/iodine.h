/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_IODINE_QML_H
#define PLASMA_NM_IODINE_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/VpnSetting>

#include <QObject>

class PLASMANM_EDITORQML_EXPORT IodineSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType CONSTANT)

    Q_PROPERTY(QString topLevelDomain READ topLevelDomain WRITE setTopLevelDomain NOTIFY topLevelDomainChanged)
    Q_PROPERTY(QString nameserver READ nameserver WRITE setNameserver NOTIFY nameserverChanged)
    Q_PROPERTY(QString iodinePassword READ iodinePassword WRITE setIodinePassword NOTIFY iodinePasswordChanged)
    Q_PROPERTY(PasswordOption iodinePasswordOption READ iodinePasswordOption WRITE setIodinePasswordOption NOTIFY iodinePasswordOptionChanged)

    Q_PROPERTY(int fragmentSize READ fragmentSize WRITE setFragmentSize NOTIFY fragmentSizeChanged)

    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    enum PasswordOption {
        StoreForUser = 0,
        StoreForAllUsers,
        AlwaysAsk
    };
    Q_ENUM(PasswordOption)

    explicit IodineSetting(QObject *parent = nullptr);
    ~IodineSetting() override;

    void loadConfig(const NetworkManager::VpnSetting::Ptr &setting);
    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;
    bool isValid() const;

    QString serviceType() const;

    QString topLevelDomain() const;
    void setTopLevelDomain(const QString &topLevelDomain);

    QString nameserver() const;
    void setNameserver(const QString &nameserver);

    QString iodinePassword() const;
    void setIodinePassword(const QString &password);

    PasswordOption iodinePasswordOption() const;
    void setIodinePasswordOption(PasswordOption option);

    int fragmentSize() const;
    void setFragmentSize(int fragmentSize);

Q_SIGNALS:
    void topLevelDomainChanged();
    void nameserverChanged();
    void iodinePasswordChanged();
    void iodinePasswordOptionChanged();
    void fragmentSizeChanged();

    void validChanged();

private:
    QString m_topLevelDomain;
    QString m_nameserver;
    QString m_iodinePassword;
    PasswordOption m_iodinePasswordOption = StoreForUser;
    int m_fragmentSize = 0;
};

#endif // PLASMA_NM_IODINE_QML_H
