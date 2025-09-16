/*
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KQuickConfigModule>

class WifiSettings : public KQuickConfigModule
{
    Q_OBJECT
public:
    WifiSettings(QObject *parent, const KPluginMetaData &metaData);
    Q_INVOKABLE QVariantMap getConnectionSettings(const QString &connection, const QString &type);
    Q_INVOKABLE void addConnectionFromQML(const QVariantMap &QMLmap);
    Q_INVOKABLE void updateConnectionFromQML(const QString &path, const QVariantMap &map);
};

