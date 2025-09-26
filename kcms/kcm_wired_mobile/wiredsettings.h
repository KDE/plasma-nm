/*
    SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <NetworkManagerQt/WiredDevice>
#include <QQmlListProperty>


#include <KQuickConfigModule>

class WiredSettings : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<NetworkManager::WiredDevice> interfaces READ interfaces NOTIFY interfacesChanged);

public:

    WiredSettings(QObject *parent, const KPluginMetaData &metaData);

    QQmlListProperty<NetworkManager::WiredDevice> interfaces()
    {
        return QQmlListProperty<NetworkManager::WiredDevice>(this, &m_interfaces);
    }



    Q_INVOKABLE QVariantMap getConnectionSettings(const QString &connection, const QString &type);
    Q_INVOKABLE void addConnectionFromQML(const QVariantMap &QMLmap);
    Q_INVOKABLE void updateConnectionFromQML(const QString &path, const QVariantMap &map);

private Q_SLOTS:
    void deviceAdded(const QString &dev);
    void deviceRemoved(const QString &dev);

Q_SIGNALS:
    void interfacesChanged();


private:
    QList<NetworkManager::WiredDevice*> m_interfaces;
};

