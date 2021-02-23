/*
    Copyright 2017 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
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

bool Configuration::airplaneModeEnabled()
{
    // Check whether other devices are disabled to assume airplane mode is enabled
    // after suspend
    const bool isWifiDisabled = !NetworkManager::isWirelessEnabled() || !NetworkManager::isWirelessHardwareEnabled();
    const bool isWwanDisabled = !NetworkManager::isWwanEnabled() || !NetworkManager::isWwanHardwareEnabled();

    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        if (grp.readEntry(QLatin1String("AirplaneModeEnabled"), false)) {
            // We can assume that airplane mode is still activated after resume
            if (isWifiDisabled && isWwanDisabled)
                return true;
            else {
                setAirplaneModeEnabled(false);
            }
        }
    }

    return false;
}

void Configuration::setAirplaneModeEnabled(bool enabled)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QLatin1String("plasma-nm"));
    KConfigGroup grp(config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("AirplaneModeEnabled"), enabled);
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

