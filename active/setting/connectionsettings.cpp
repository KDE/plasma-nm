/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <connectionsettings.h>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>

ConnectionSettings::ConnectionSettings(QObject* parent)
    : QObject(parent)
{
}

ConnectionSettings::~ConnectionSettings()
{
}

QVariantMap ConnectionSettings::loadSettings(const QString& uuid)
{
    QVariantMap resultingMap;

    if (uuid.isEmpty()) {
        return resultingMap;
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(uuid);

    if (!connection) {
        return resultingMap;
    }

    NetworkManager::Ipv4Setting::Ptr ipv4Setting = connection->settings()->setting(NetworkManager::Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>();

    if (ipv4Setting) {
        QVariantMap map;

        if (ipv4Setting->method() == NetworkManager::Ipv4Setting::Automatic) {
            map.insert("method", "auto");
        } else if (ipv4Setting->method() == NetworkManager::Ipv4Setting::Shared) {
            map.insert("method", "shared");
        } else if (ipv4Setting->method() == NetworkManager::Ipv4Setting::Manual) {
            map.insert("method", "manual");

            if (!ipv4Setting->addresses().isEmpty()) {
                NetworkManager::IpAddress ipAddress = ipv4Setting->addresses().first();
                if (ipAddress.isValid()) {
                    map.insert("address", ipAddress.ip().toString());
                    map.insert("gateway", ipAddress.gateway().toString());
                    map.insert("netmask", ipAddress.netmask().toString());
                }
            }

            if (!ipv4Setting->dns().isEmpty()) {
                QHostAddress dns1 = ipv4Setting->dns().first();
                map.insert("dns1", dns1.toString());
                if (ipv4Setting->dns().count() >= 2) {
                    QHostAddress dns2 = ipv4Setting->dns().at(1);
                    map.insert("dns2", dns2.toString());
                }
            }
        }

        resultingMap.insert("ipv4", QVariant::fromValue(map));
    }

    return resultingMap;
}


