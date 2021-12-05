/*
    SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_CONFIGURATION_H
#define PLASMA_NM_CONFIGURATION_H

#include <QObject>

#include <NetworkManagerQt/Manager>

class Q_DECL_EXPORT Configuration : public QObject
{
    Q_PROPERTY(bool unlockModemOnDetection READ unlockModemOnDetection WRITE setUnlockModemOnDetection)
    Q_PROPERTY(bool manageVirtualConnections READ manageVirtualConnections WRITE setManageVirtualConnections NOTIFY manageVirtualConnectionsChanged)
    Q_PROPERTY(bool airplaneModeEnabled READ airplaneModeEnabled WRITE setAirplaneModeEnabled NOTIFY airplaneModeEnabledChanged)
    Q_PROPERTY(QString hotspotName READ hotspotName WRITE setHotspotName)
    Q_PROPERTY(QString hotspotPassword READ hotspotPassword WRITE setHotspotPassword)
    Q_PROPERTY(QString hotspotConnectionPath READ hotspotConnectionPath WRITE setHotspotConnectionPath)

    // Readonly constant property, as this value should only be set by the platform
    Q_PROPERTY(bool showPasswordDialog READ showPasswordDialog CONSTANT)
    Q_OBJECT
public:
    bool unlockModemOnDetection();
    void setUnlockModemOnDetection(bool unlock);

    bool manageVirtualConnections();
    void setManageVirtualConnections(bool manage);

    bool airplaneModeEnabled() const;
    void setAirplaneModeEnabled(bool enabled);
    Q_SIGNAL void airplaneModeEnabledChanged();

    QString hotspotName();
    void setHotspotName(const QString &name);

    QString hotspotPassword();
    void setHotspotPassword(const QString &password);

    QString hotspotConnectionPath();
    void setHotspotConnectionPath(const QString &path);

    bool showPasswordDialog();

    static Configuration &self();

signals:
    void manageVirtualConnectionsChanged(bool manage);

private:
    Configuration() = default;
};

#endif // PLAMA_NM_CONFIGURATION_H
