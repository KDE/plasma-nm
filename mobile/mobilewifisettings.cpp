#include "mobilewifisettings.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>

K_PLUGIN_FACTORY_WITH_JSON(WifiSettingsFactory, "wifi.json", registerPlugin<MobileWifiSettings>();)

MobileWifiSettings::MobileWifiSettings(QObject* parent, const QVariantList& args) : KQuickAddons::ConfigModule(parent, args)
{

}

MobileWifiSettings::~MobileWifiSettings()
{
}

#include "mobilewifisettings.moc"
