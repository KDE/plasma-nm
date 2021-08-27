/*
    SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "configuration.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <KUser>

static bool propManageVirtualConnectionsInitialized = false;
static bool propManageVirtualConnections = false;

Configuration &Configuration::self()
{
    static Configuration c;
    return c;
}

bool Configuration::unlockModemOnDetection()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("UnlockModemOnDetection"), true);
    }

    return true;
}

void Configuration::setUnlockModemOnDetection(bool unlock)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("UnlockModemOnDetection"), unlock);
    }
}

bool Configuration::manageVirtualConnections()
{
    // Avoid reading from the config file over and over
    if (propManageVirtualConnectionsInitialized) {
        return propManageVirtualConnections;
    }

    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        propManageVirtualConnections = grp.readEntry(QLatin1String("ManageVirtualConnections"), false);
        propManageVirtualConnectionsInitialized = true;

        return propManageVirtualConnections;
    }

    return true;
}

void Configuration::setManageVirtualConnections(bool manage)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("ManageVirtualConnections"), manage);
        propManageVirtualConnections = manage;
    }
}

bool Configuration::airplaneModeEnabled() const
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    static KConfigGroup grp(config, QLatin1String("General"));
    return grp.readEntry(QLatin1String("AirplaneModeEnabled"), false);
}

void Configuration::setAirplaneModeEnabled(bool enabled)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("AirplaneModeEnabled"), enabled);
        grp.sync();
        Q_EMIT airplaneModeEnabledChanged();
    }
}

QString Configuration::hotspotName()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));
    KUser currentUser;

    const QString defaultName = currentUser.loginName() + QLatin1String("-hotspot");

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("HotspotName"), defaultName);
    }

    return defaultName;
}

void Configuration::setHotspotName(const QString &name)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("HotspotName"), name);
    }
}

QString Configuration::hotspotPassword()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("HotspotPassword"), QString());
    }

    return QString();
}

void Configuration::setHotspotPassword(const QString &password)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("HotspotPassword"), password);
    }
}

QString Configuration::hotspotConnectionPath()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("HotspotConnectionPath"), QString());
    }

    return QString();
}

void Configuration::setHotspotConnectionPath(const QString &path)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("HotspotConnectionPath"), path);
    }
}

bool Configuration::showPasswordDialog()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("ShowPasswordDialog"), true);
    }

    return true;
}
