#include "mobilewifisettings.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(WifiSettingsFactory, "wifi.json", registerPlugin<MobileWifiSettings>();)

MobileWifiSettings::MobileWifiSettings(QObject* parent, const QVariantList& args) : KQuickAddons::ConfigModule(parent, args)
{
    KAboutData* about = new KAboutData("kcm_mobile_wifi", i18n("Configure Wi-Fi networks"),
                                       "0.1", QString(), KAboutLicense::LGPL);
    about->addAuthor(i18n("Martin Kacej"), QString(), "m.kacej@atlas.sk");
    setAboutData(about);
}

MobileWifiSettings::~MobileWifiSettings()
{
}

#include "mobilewifisettings.moc"
