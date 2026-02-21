/*
    SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef WIFISETTINGS_H
#define WIFISETTINGS_H

#include <KQuickConfigModule>

class WifiSettings : public KQuickConfigModule
{
    Q_OBJECT
    Q_PROPERTY(bool connectionNotificationsEnabled READ connectionNotificationsEnabled WRITE setConnectionNotificationsEnabled NOTIFY
                   connectionNotificationsEnabledChanged)
public:
    WifiSettings(QObject *parent, const KPluginMetaData &metaData);
    Q_INVOKABLE QVariantMap getConnectionSettings(const QString &connection, const QString &type);
    Q_INVOKABLE void addConnectionFromQML(const QVariantMap &QMLmap);
    Q_INVOKABLE void updateConnectionFromQML(const QString &path, const QVariantMap &map);

    bool connectionNotificationsEnabled() const;
    void setConnectionNotificationsEnabled(bool enabled);

Q_SIGNALS:
    void connectionNotificationsEnabledChanged(bool enabled);
};

#endif // WIFISETTINGS_H
