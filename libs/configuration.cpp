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

Q_GLOBAL_STATIC_WITH_ARGS(KSharedConfigPtr, config, (KSharedConfig::openConfig(QLatin1String("plasma-nm"))))

bool Configuration::unlockModemOnDetection()
{
    KConfigGroup grp(*config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("UnlockModemOnDetection"), true);
    }

    return true;
}

void Configuration::setUnlockModemOnDetection(bool unlock)
{
    KConfigGroup grp(*config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("UnlockModemOnDetection"), unlock);
    }
}

bool Configuration::manageVirtualConnections()
{
    KConfigGroup grp(*config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("ManageVirtualConnections"), false);
    }

    return true;
}

void Configuration::setManageVirtualConnections(bool manage)
{
    KConfigGroup grp(*config, QLatin1String("General"));

    if (grp.isValid()) {
        grp.writeEntry(QLatin1String("ManageVirtualConnections"), manage);
    }
}

bool Configuration::showPasswordDialog()
{
    KConfigGroup grp(*config, QLatin1String("General"));

    if (grp.isValid()) {
        return grp.readEntry(QLatin1String("ShowPasswordDialog"), true);
    }

    return true;
}

