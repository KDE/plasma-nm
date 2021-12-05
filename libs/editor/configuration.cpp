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
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("UnlockModemOnDetection"), true);
    }

    return true;
}

void Configuration::setUnlockModemOnDetection(bool unlock)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("UnlockModemOnDetection"), unlock);
    }
}

bool Configuration::manageVirtualConnections()
{
    // Avoid reading from the config file over and over
    if (propManageVirtualConnectionsInitialized) {
        return propManageVirtualConnections;
    }

    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        propManageVirtualConnections = grp.readEntry(QStringLiteral("ManageVirtualConnections"), false);
        propManageVirtualConnectionsInitialized = true;

        return propManageVirtualConnections;
    }

    return true;
}

void Configuration::setManageVirtualConnections(bool manage)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("ManageVirtualConnections"), manage);
        propManageVirtualConnections = manage;
        grp.sync();
        Q_EMIT manageVirtualConnectionsChanged(manage);
    }
}

bool Configuration::airplaneModeEnabled() const
{
    static KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    static KConfigGroup grp(config, QStringLiteral("General"));
    return grp.readEntry(QStringLiteral("AirplaneModeEnabled"), false);
}

void Configuration::setAirplaneModeEnabled(bool enabled)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("AirplaneModeEnabled"), enabled);
        grp.sync();
        Q_EMIT airplaneModeEnabledChanged();
    }
}

QString Configuration::hotspotName()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));
    KUser currentUser;

    const QString defaultName = currentUser.loginName() + QLatin1String("-hotspot");

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("HotspotName"), defaultName);
    }

    return defaultName;
}

void Configuration::setHotspotName(const QString &name)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("HotspotName"), name);
    }
}

QString Configuration::hotspotPassword()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("HotspotPassword"), QString());
    }

    return {};
}

void Configuration::setHotspotPassword(const QString &password)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("HotspotPassword"), password);
    }
}

QString Configuration::hotspotConnectionPath()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("HotspotConnectionPath"), QString());
    }

    return {};
}

void Configuration::setHotspotConnectionPath(const QString &path)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("HotspotConnectionPath"), path);
    }
}

bool Configuration::showPasswordDialog()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("ShowPasswordDialog"), true);
    }

    return true;
}
