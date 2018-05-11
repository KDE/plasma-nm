/*
 *   Copyright 2018 Martin Kacej <m.kacej@atlas.sk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "wifisettings.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(WifiSettingsFactory, "wifisettings.json", registerPlugin<WifiSettings>();)

WifiSettings::WifiSettings(QObject* parent, const QVariantList& args) : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData* about = new KAboutData("kcm_mobile_wifi", i18n("Configure Wi-Fi networks"),
                                       "0.1", QString(), KAboutLicense::LGPL);
    about->addAuthor(i18n("Martin Kacej"), QString(), "m.kacej@atlas.sk");
    setAboutData(about);
}

WifiSettings::~WifiSettings()
{
}

#include "wifisettings.moc"
