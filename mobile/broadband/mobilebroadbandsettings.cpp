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

#include "mobilebroadbandsettings.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/CdmaSetting>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/ModemDevice>
#endif

K_PLUGIN_FACTORY_WITH_JSON(MobileBroadbandSettingsFactory, "mobilebroadbandsettings.json", registerPlugin<MobileBroadbandSettings>();)

MobileBroadbandSettings::MobileBroadbandSettings(QObject* parent, const QVariantList& args) : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData* about = new KAboutData("kcm_mobile_broadband", i18n("Configure mobile broadband"),
                                       "0.1", QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Martin Kacej"), QString(), "m.kacej@atlas.sk");
    setAboutData(about);
    ModemManager::scanDevices();
    this->getModemDevice();
    this->setupMobileNetwork();
}

MobileBroadbandSettings::~MobileBroadbandSettings()
{
}

bool MobileBroadbandSettings::mobileDataActive()
{
    return m_mobileDataActive;
}

void MobileBroadbandSettings::setMobileDataActive(bool active)
{
    m_mobileDataActive = active;
    emit mobileDataActiveChanged(m_mobileDataActive);
}

QString MobileBroadbandSettings::getModemDevice()
{
    ModemManager::ModemDevice::List list = ModemManager::modemDevices();
    if (list.length() == 0)
        return QString();
    ModemManager::ModemDevice::Ptr device;
    foreach (const ModemManager::ModemDevice::Ptr &md, list) {
        ModemManager::Modem::Ptr m = md->modemInterface();
        if (!m->isEnabled())
            continue;
        // TODO powerState ???
        if (m->state() <= MM_MODEM_STATE_REGISTERED)
            continue; // needs inspection
        if (m->accessTechnologies() <= MM_MODEM_ACCESS_TECHNOLOGY_GSM)
            continue;
        if (m->currentCapabilities() <= MM_MODEM_CAPABILITY_GSM_UMTS)
            continue;
        device = md;
    }
    if (device) {
        qWarning() << device->uni() << device->modemInterface()->uni();
        return device->uni();
    }
    return QString();
}

void MobileBroadbandSettings::setupMobileNetwork()
{
    ModemManager::ModemDevice::Ptr modem = ModemManager::findModemDevice(getModemDevice());
    if (!modem)
        return;
    if (modem->bearers().count() == 0) {
        qWarning() << "No bearers in modem found";
    } else {
        foreach (const ModemManager::Bearer::Ptr &p, modem->bearers()) {
            qWarning() << p->properties();
        }
    }
}

QString MobileBroadbandSettings::getAPN()
{
    return "some.ap.placeholder.com";
}

#include "mobilebroadbandsettings.moc"
