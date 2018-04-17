#ifndef MOBILEWIFISETTINGS_H
#define MOBILEWIFISETTINGS_H

#include <QObject>
#include <KQuickAddons/ConfigModule>

class MobileWifiSettings : public KQuickAddons::ConfigModule
{
public:
    MobileWifiSettings(QObject* parent, const QVariantList& args);
    virtual ~MobileWifiSettings();
};

#endif // MOBILEWIFISETTINGS_H
