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

#include "celluralsettings.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(CelluralSettingsFactory, "mobile_cellural.json", registerPlugin<CelluralSettings>();)

CelluralSettings::CelluralSettings(QObject* parent, const QVariantList& args) : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData* about = new KAboutData("kcm_mobile_cellural", i18n("Configure cellural data network"),
                                       "0.1", QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Martin Kacej"), QString(), "m.kacej@atlas.sk");
    setAboutData(about);
}

CelluralSettings::~CelluralSettings()
{
}

bool CelluralSettings::mobileDataActive()
{
    return m_mobileDataActive;
}

void CelluralSettings::setMobileDataActive(bool active)
{
    m_mobileDataActive = active;
    emit mobileDataActiveChanged(m_mobileDataActive);
}

#include "celluralsettings.moc"
