/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_IODINE_AUTH_QML_H
#define PLASMA_NM_IODINE_AUTH_QML_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/VpnSetting>

#include <QObject>
#include <QStringList>

class PLASMANM_EDITORQML_EXPORT IodineAuthSetting : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit IodineAuthSetting(const QStringList &hints, QObject *parent = nullptr);
    ~IodineAuthSetting() override;

    void loadSecrets(const NetworkManager::VpnSetting::Ptr &setting);
    QVariantMap setting() const;

    QString password() const;
    void setPassword(const QString &password);

Q_SIGNALS:
    void passwordChanged();

private:
    QStringList m_hints;

    QString m_password;
};

#endif // PLASMA_NM_IODINE_AUTH_QML_H
