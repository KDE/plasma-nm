/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "advancedpermissionsmodel.h"

#include <KLocalizedString>
#include <KUser>

#include <algorithm>

AdvancedPermissionsModel::AdvancedPermissionsModel(QObject *parent)
    : QObject(parent)
{
    const QString currentLogin = KUser().loginName();

    for (const KUser &user : KUser::allUsers()) {
        if (user.userId().nativeId() < 1000 || user.loginName() == QLatin1String("nobody")) {
            continue;
        }
        Item item;
        item.loginName = user.loginName();
        item.fullName = user.property(KUser::FullName).toString();
        item.isCurrentUser = (item.loginName == currentLogin);
        item.allowed = item.isCurrentUser;
        m_users.append(item);
    }
}

QVariantList AdvancedPermissionsModel::users() const
{
    QList<const Item *> sorted;
    for (const Item &item : m_users) {
        sorted.append(&item);
    }

    std::sort(sorted.begin(), sorted.end(), [](const Item *a, const Item *b) {
        const QString an = a->fullName.isEmpty() ? a->loginName : a->fullName;
        const QString bn = b->fullName.isEmpty() ? b->loginName : b->fullName;
        return QString::localeAwareCompare(an, bn) < 0;
    });

    QVariantList list;
    for (const Item *item : std::as_const(sorted)) {
        list.append(QVariantMap{
            {QStringLiteral("fullName"), item->fullName.isEmpty() ? i18nc("@item real name not available", "N/A") : item->fullName},
            {QStringLiteral("loginName"), item->loginName},
            {QStringLiteral("allowed"), item->allowed},
            {QStringLiteral("isCurrentUser"), item->isCurrentUser},
        });
    }

    return list;
}

QVariantMap AdvancedPermissionsModel::permissions() const
{
    QVariantMap map;
    for (const Item &item : m_users) {
        if (item.allowed) {
            map.insert(item.loginName, item.tags);
        }
    }
    return map;
}

void AdvancedPermissionsModel::setPermissions(const QVariantMap &permissions)
{
    for (Item &item : m_users) {
        const bool inMap = permissions.contains(item.loginName);
        item.allowed = inMap;
        item.tags = inMap ? permissions.value(item.loginName).toString() : QString();
    }
    Q_EMIT usersChanged();
}

void AdvancedPermissionsModel::allow(const QString &loginName)
{
    for (Item &item : m_users) {
        if (item.loginName == loginName && !item.allowed) {
            item.allowed = true;
            Q_EMIT usersChanged();
            return;
        }
    }
}

void AdvancedPermissionsModel::disallow(const QString &loginName)
{
    for (Item &item : m_users) {
        if (item.loginName == loginName && item.allowed && !item.isCurrentUser) {
            item.allowed = false;
            Q_EMIT usersChanged();
            return;
        }
    }
}

#include "moc_advancedpermissionsmodel.cpp"
