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

#include "mobileutils.h"

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/ModemDevice>
#endif


MobileUtils::MobileUtils(QObject *parent)
    : QObject(parent)
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
    if (type == "ipv4") {
        NetworkManager::Ipv4Setting::Ptr ipSettings = NetworkManager::Ipv4Setting::Ptr(new NetworkManager::Ipv4Setting());
        ipSettings->fromMap(map);
        map.clear();
        if (ipSettings->method() == NetworkManager::Ipv4Setting::Automatic) {
            map.insert(QLatin1String("method"),QVariant(QLatin1String("auto")));
        }

        if (ipSettings->method() == NetworkManager::Ipv4Setting::Manual) {
            map.insert(QLatin1String("method"),QVariant(QLatin1String("manual")));
            map.insert(QLatin1String("address"),QVariant(ipSettings->addresses().first().ip().toString()));
            map.insert(QLatin1String("prefix"),QVariant(ipSettings->addresses().first().netmask().toString()));
            map.insert(QLatin1String("gateway"),QVariant(ipSettings->addresses().first().gateway().toString()));
            map.insert(QLatin1String("dns"),QVariant(ipSettings->dns().first().toString()));
        }
    }
    return map;
}

QVariantMap MobileUtils::getActiveConnectionInfo(const QString &connection)
{
    if (connection.isEmpty())
        return QVariantMap();

    NetworkManager::ActiveConnection::Ptr activeCon;
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    foreach (const NetworkManager::ActiveConnection::Ptr &active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid())
            activeCon = active;
    }

    if (!activeCon) {
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

void MobileUtils::addConnectionFromQML(const QVariantMap &QMLmap)
{
    if (QMLmap.isEmpty())
        return;

    if (QMLmap["mode"].toString() == "infrastructure") {
        NetworkManager::WirelessSetting::Ptr wirelessSettings = NetworkManager::WirelessSetting::Ptr(new NetworkManager::WirelessSetting());
        wirelessSettings->setSsid(QMLmap.value(QLatin1String("id")).toString().toUtf8());
        wirelessSettings->setMode(NetworkManager::WirelessSetting::Infrastructure);

        NetworkManager::Ipv4Setting::Ptr ipSettings = NetworkManager::Ipv4Setting::Ptr(new NetworkManager::Ipv4Setting());
        if (QMLmap["method"] == QLatin1String("auto")) {
            ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Automatic);
        } else {
            ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Manual);
            NetworkManager::IpAddress ipaddr;
            ipaddr.setIp(QHostAddress(QMLmap["address"].toString()));
            ipaddr.setPrefixLength(QMLmap["prefix"].toInt());
            ipaddr.setGateway(QHostAddress(QMLmap["gateway"].toString()));
            ipSettings->setAddresses(QList<NetworkManager::IpAddress>({ipaddr}));
            ipSettings->setDns(QList<QHostAddress>({QHostAddress(QMLmap["dns"].toString())}));
        }

        NetworkManager::ConnectionSettings::Ptr connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
        connectionSettings->setId(QMLmap.value(QLatin1String("id")).toString());
        connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

        NMVariantMapMap map = connectionSettings->toMap();
        map.insert("802-11-wireless",wirelessSettings->toMap());
        map.insert("ipv4",ipSettings->toMap());

        if (QMLmap.contains("802-11-wireless-security")) {
            QVariantMap securMap = QMLmap["802-11-wireless-security"].toMap();
            int type = securMap["type"].toInt();
            if (!type == NetworkManager::NoneSecurity) {
                NetworkManager::WirelessSecuritySetting::Ptr securitySettings = NetworkManager::WirelessSecuritySetting::Ptr(new NetworkManager::WirelessSecuritySetting());
                if (type == NetworkManager::Wpa2Psk ) {
                    securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk);
                    securitySettings->setAuthAlg(NetworkManager::WirelessSecuritySetting::AuthAlg::Open);
                    securitySettings->setPskFlags(NetworkManager::Setting::SecretFlagType::AgentOwned);
                    securitySettings->setPsk(securMap["password"].toString());
                }

                if (type == NetworkManager::StaticWep) {
                    securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep);
                    securitySettings->setAuthAlg(NetworkManager::WirelessSecuritySetting::AuthAlg::Open);
                    securitySettings->setWepKeyType(NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
                    securitySettings->setWepKeyFlags(NetworkManager::Setting::SecretFlagType::AgentOwned);
                    securitySettings->setWepKey0(securMap["password"].toString());
                }
                map.insert("802-11-wireless-security",securitySettings->toMap());
            }
        }
        //qWarning() << map;
        NetworkManager::addConnection(map);
    }
}

void MobileUtils::updateConnectionFromQML(const QString &path, const QVariantMap &map)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(path);
    if (!con)
        return;

    //qWarning() << map;
    NMVariantMapMap toUpdateMap = con->settings()->toMap();

    NetworkManager::Ipv4Setting::Ptr ipSetting = con->settings()->setting(NetworkManager::Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>();
    if (ipSetting->method() == NetworkManager::Ipv4Setting::Automatic || ipSetting->method() == NetworkManager::Ipv4Setting::Manual) {
        if (map.value("method") == "auto") {
            ipSetting->setMethod(NetworkManager::Ipv4Setting::Automatic);
        }

        if (map.value("method") == "manual") {
            ipSetting->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Manual);
            NetworkManager::IpAddress ipaddr;
            ipaddr.setIp(QHostAddress(map["address"].toString()));
            ipaddr.setPrefixLength(map["prefix"].toInt());
            ipaddr.setGateway(QHostAddress(map["gateway"].toString()));
            ipSetting->setAddresses(QList<NetworkManager::IpAddress>({ipaddr}));
            ipSetting->setDns(QList<QHostAddress>({QHostAddress(map["dns"].toString())}));
        }
        toUpdateMap.insert("ipv4",ipSetting->toMap());
    }

    if (map.contains("802-11-wireless-security")) {
        QVariantMap secMap = map.value("802-11-wireless-security").toMap();
        //qWarning() << secMap;
        NetworkManager::WirelessSecuritySetting::Ptr securitySetting = con->settings()->setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();
        if ((securitySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::Wep)
                && (secMap.value("type") == NetworkManager::StaticWep)) {
            securitySetting->setWepKey0(secMap["password"].toString());
        }

        if ((securitySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::WpaPsk)
                && (secMap.value("type") == NetworkManager::Wpa2Psk)) {
            securitySetting->setPsk(secMap["password"].toString());
        }
        toUpdateMap.insert("802-11-wireless-security",securitySetting->toMap());
    }
    con->update(toUpdateMap);
}
