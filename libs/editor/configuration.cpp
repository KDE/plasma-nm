/*
    SPDX-FileCopyrightText: 2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "configuration.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <KUser>
#include <QMutexLocker>
#include <QRandomGenerator>

static bool propManageVirtualConnectionsInitialized = false;
static bool propManageVirtualConnections = false;
QMutex Configuration::sMutex;

Configuration::Configuration()
{
    // Add config watcher so that config is reparsed when changes happen externally
    m_configWatcher = KConfigWatcher::create(KSharedConfig::openConfig(QStringLiteral("plasma-nm")));
}

Configuration &Configuration::self()
{
    static Configuration c;
    return c;
}

bool Configuration::unlockModemOnDetection() const
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

bool Configuration::manageVirtualConnections() const
{
    // Avoid reading from the config file over and over
    if (propManageVirtualConnectionsInitialized) {
        return propManageVirtualConnections;
    }

    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        QMutexLocker locker(&sMutex);
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
        {
            QMutexLocker locker(&sMutex);
            grp.writeEntry(QStringLiteral("ManageVirtualConnections"), manage);
            grp.sync();
            propManageVirtualConnections = manage;
        }
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

QString Configuration::hotspotName() const
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
        grp.writeEntry(QStringLiteral("HotspotName"), name, KConfigBase::Notify);
        grp.sync();
    }
}

QString Configuration::hotspotPassword() const
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        QString pwd = grp.readEntry(QStringLiteral("HotspotPassword"), QString());
        if (pwd.isNull()) {
            // Use a default non empty password for the hotspot
            // Make it 26 characters long so it works for both WPA and WEP
            static const int base16 = 16;
            for (int i = 0; i < 26; ++i) {
                pwd += QString::number(QRandomGenerator::global()->bounded(base16), base16).toUpper();
            }
            grp.writeEntry(QStringLiteral("HotspotPassword"), pwd);
        }
        return pwd;
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

QString Configuration::hotspotConnectionPath() const
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

bool Configuration::showPasswordDialog() const
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("ShowPasswordDialog"), true);
    }

    return true;
}

bool Configuration::systemConnectionsByDefault() const
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        return grp.readEntry(QStringLiteral("SystemConnectionsByDefault"), false);
    }

    return false;
}

void Configuration::setSystemConnectionsByDefault(bool opt)
{
    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("plasma-nm"));
    KConfigGroup grp(config, QStringLiteral("General"));

    if (grp.isValid()) {
        grp.writeEntry(QStringLiteral("SystemConnectionsByDefault"), opt);
    }
}

#include "moc_configuration.cpp"
