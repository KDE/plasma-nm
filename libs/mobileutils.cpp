#include "mobileutils.h"


#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Setting>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Ipv4Setting>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/ModemDevice>
#endif

MobileUtils::MobileUtils(QObject *parent) : QObject(parent)
{

}
QVariantMap MobileUtils::getConnectionSettings(const QString &connection, const QString &type)
{
    if (type.isEmpty())
        return QVariantMap();
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    if (!con)
        return QVariantMap();
    QVariantMap map = con->settings()->toMap().value(type);
    //qCWarning(PLASMA_NM) << "Map:" <<con->settings()->toMap().value(type);
    return map;
}

QVariantMap MobileUtils::getActiveConnectionInfo(const QString &connection)
{
    if (connection.isEmpty())
        return QVariantMap();
    NetworkManager::ActiveConnection::Ptr activeCon;
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    Q_FOREACH (const NetworkManager::ActiveConnection::Ptr &active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid())
            activeCon = active;
    }
    if(!activeCon){
        qWarning() << "Active" << connection << "not found";
        return QVariantMap();
    }
    QVariantMap map;
    map.insert("address",QVariant(activeCon->ipV4Config().addresses().first().ip().toString()));
    map.insert("prefix",QVariant(activeCon->ipV4Config().addresses().first().netmask().toString()));
    map.insert("gateway",QVariant(activeCon->ipV4Config().gateway()));
    map.insert("dns",QVariant(activeCon->ipV4Config().nameservers().first().toString()));
    //qWarning() << map;
    return map;
}
