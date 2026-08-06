/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_ADVANCED_PERMISSIONS_H
#define PLASMA_NM_ADVANCED_PERMISSIONS_H

#include "plasmanm_editorqml_export.h"

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <qqmlregistration.h>

class PLASMANM_EDITORQML_EXPORT AdvancedPermissionsModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList users READ users NOTIFY usersChanged)
    Q_PROPERTY(QVariantMap permissions READ permissions WRITE setPermissions NOTIFY usersChanged)

public:
    explicit AdvancedPermissionsModel(QObject *parent = nullptr);

    QVariantList users() const;

    QVariantMap permissions() const;
    void setPermissions(const QVariantMap &permissions);

    Q_INVOKABLE void allow(const QString &loginName);
    Q_INVOKABLE void disallow(const QString &loginName);

Q_SIGNALS:
    void usersChanged();

private:
    struct Item {
        QString fullName;
        QString loginName;
        QString tags;
        bool allowed = false;
        bool isCurrentUser = false;
    };

    QList<Item> m_users;
};

#endif
