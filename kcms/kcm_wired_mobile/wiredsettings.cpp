/*
 *  SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>
 *  SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "wiredsettings.h"

#include <KLocalizedString>
#include <KPluginFactory>

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WiredDevice>

K_PLUGIN_CLASS_WITH_JSON(WiredSettings, "kcm_mobile_wired.json")

WiredSettings::WiredSettings(QObject *parent, const KPluginMetaData &metaData)
    : KQuickConfigModule(parent, metaData)
{
    setButtons({});
}

QVariantMap WiredSettings::getConnectionSettings(const QString &connection, const QString &type)
{
    if (type.isEmpty()) {
        qWarning() << "Connection type must not be empty.";
        return QVariantMap();
    }

    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    if (!con) {
        qWarning() << "Connection not found.";
        return QVariantMap();
    }

    QVariantMap map = con->settings()->toMap().value(type);
    if (type == "ipv4") {
        NetworkManager::Ipv4Setting::Ptr ipSettings = NetworkManager::Ipv4Setting::Ptr(new NetworkManager::Ipv4Setting());
        ipSettings->fromMap(map);
        map.clear();
        if (ipSettings->method() == NetworkManager::Ipv4Setting::Automatic) {
            map.insert(QLatin1String("method"), QVariant(QLatin1String("auto")));
        }

        if (ipSettings->method() == NetworkManager::Ipv4Setting::Manual) {
            map.insert(QLatin1String("method"), QVariant(QLatin1String("manual")));
            map.insert(QLatin1String("address"), QVariant(ipSettings->addresses().first().ip().toString()));
            map.insert(QLatin1String("prefix"), QVariant(ipSettings->addresses().first().prefixLength()));
            map.insert(QLatin1String("gateway"), QVariant(ipSettings->addresses().first().gateway().toString()));
            map.insert(QLatin1String("dns"), QVariant(ipSettings->dns().first().toString()));
        }
    }
    return map;
}

void WiredSettings::addConnectionFromQML(const QVariantMap &QMLmap)
{
    if (QMLmap.isEmpty())
        return;

    NetworkManager::ConnectionSettings::Ptr connectionSettings =
        NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
    connectionSettings->setId(QMLmap.value(QLatin1String("id")).toString());
    connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    NetworkManager::Ipv4Setting::Ptr ipSettings = NetworkManager::Ipv4Setting::Ptr(new NetworkManager::Ipv4Setting());
    if (QMLmap["method"] == QLatin1String("auto")) {
        ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Automatic);
    }
    if (QMLmap["method"] == QLatin1String("shared")) {
        ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Shared);
    }
    if (QMLmap["method"] == QLatin1String("manual")) {
        ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Manual);
        NetworkManager::IpAddress ipaddr;
        ipaddr.setIp(QHostAddress(QMLmap["address"].toString()));
        ipaddr.setPrefixLength(QMLmap["prefix"].toInt());
        ipaddr.setGateway(QHostAddress(QMLmap["gateway"].toString()));
        ipSettings->setAddresses(QList<NetworkManager::IpAddress>({ipaddr}));
        ipSettings->setDns(QList<QHostAddress>({QHostAddress(QMLmap["dns"].toString())}));
    }

    NMVariantMapMap map = connectionSettings->toMap();
    map.insert("ipv4", ipSettings->toMap());

    qWarning() << map;
    NetworkManager::addConnection(map);
}

void WiredSettings::updateConnectionFromQML(const QString &path, const QVariantMap &map)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(path);
    if (!con) {
        qWarning() << "Connection to update not found.";
        return;
    }

    // qWarning() << map;
    if (map.contains("id")) {
        con->settings()->setId(map.value("id").toString());
    }

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
        toUpdateMap.insert("ipv4", ipSetting->toMap());
    }

    qWarning() << toUpdateMap;
    con->update(toUpdateMap);
}

#include "wiredsettings.moc"
