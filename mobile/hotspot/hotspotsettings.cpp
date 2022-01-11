/*
    SPDX-FileCopyrightText: 2020 Tobias Fella <fella@posteo.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "hotspotsettings.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(HotspotSettings, "hotspotsettings.json")

HotspotSettings::HotspotSettings(QObject *parent, const QVariantList &args)
    : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData *about = new KAboutData("kcm_mobile_hotspot", i18n("Hotspot"), "0.1", QString(), KAboutLicense::LGPL);
    about->addAuthor(i18n("Tobias Fella"), QString(), "fella@posteo.de");
    setAboutData(about);
}

HotspotSettings::~HotspotSettings()
{
}

#include "hotspotsettings.moc"
