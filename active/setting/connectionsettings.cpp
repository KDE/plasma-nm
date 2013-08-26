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

    NetworkManager::ConnectionSettings::Ptr connectionsettings = connection->settings();

    if (!connectionsettings) {
        return resultingMap;
    } else {
        QVariantMap connectionMap;

        connectionMap.insert("id", connectionsettings->id());
        connectionMap.insert("autoconnect", connectionsettings->autoconnect());

        resultingMap.insert("connection", connectionMap);
    }

    NetworkManager::Ipv4Setting::Ptr ipv4Setting = connectionsettings->setting(NetworkManager::Setting::Ipv4).staticCast<NetworkManager::Ipv4Setting>();

    if (ipv4Setting) {
        QVariantMap ipv4map;

        if (ipv4Setting->method() == NetworkManager::Ipv4Setting::Automatic) {
            ipv4map.insert("method", "auto");
        } else if (ipv4Setting->method() == NetworkManager::Ipv4Setting::Shared) {
            ipv4map.insert("method", "shared");
        } else if (ipv4Setting->method() == NetworkManager::Ipv4Setting::Manual) {
            ipv4map.insert("method", "manual");

            if (!ipv4Setting->addresses().isEmpty()) {
                NetworkManager::IpAddress ipAddress = ipv4Setting->addresses().first();
                if (ipAddress.isValid()) {
                    ipv4map.insert("address", ipAddress.ip().toString());
                    ipv4map.insert("gateway", ipAddress.gateway().toString());
                    ipv4map.insert("netmask", ipAddress.netmask().toString());
                }
            }

            if (!ipv4Setting->dns().isEmpty()) {
                QHostAddress dns1 = ipv4Setting->dns().first();
                ipv4map.insert("dns1", dns1.toString());
                if (ipv4Setting->dns().count() >= 2) {
                    QHostAddress dns2 = ipv4Setting->dns().at(1);
                    ipv4map.insert("dns2", dns2.toString());
                }
            }
        }

        resultingMap.insert("ipv4", QVariant::fromValue(ipv4map));
    }

    if (connectionsettings->connectionType() == NetworkManager::ConnectionSettings::Wireless) {
        NetworkManager::WirelessSetting::Ptr wirelessSetting = connectionsettings->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();

        if (wirelessSetting) {
            QVariantMap wirelessMap;

            wirelessMap.insert("ssid", wirelessSetting->ssid());
            if (wirelessSetting->mode() == NetworkManager::WirelessSetting::Infrastructure) {
                wirelessMap.insert("mode", "infrastructure");
            } else if (wirelessSetting->mode() == NetworkManager::WirelessSetting::Adhoc) {
                wirelessMap.insert("mode", "adhoc");
            } else if (wirelessSetting->mode() == NetworkManager::WirelessSetting::Ap) {
                wirelessMap.insert("mode", "ap");
            }

            resultingMap.insert("802-11-wireless", wirelessMap);

            if (wirelessSetting->security() == "802-11-wireless-security") {
                NetworkManager::WirelessSecuritySetting::Ptr wirelessSecuritySetting = connectionsettings->setting(NetworkManager::Setting::WirelessSecurity).staticCast<NetworkManager::WirelessSecuritySetting>();

                if (wirelessSecuritySetting) {
                    QVariantMap wifiSecurityMap;

                    if (wirelessSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::Wep) {
                        wifiSecurityMap.insert("key-mgmt", "none");
                        // TODO: maybe check wep-key index
                        wifiSecurityMap.insert("wep-key0", wirelessSecuritySetting->wepKey0());
                    } else if (wirelessSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::Ieee8021x) {
                        wifiSecurityMap.insert("key-mgmt", "ieee8021x");
                        if (wirelessSecuritySetting->authAlg() == NetworkManager::WirelessSecuritySetting::Leap) {
                            wifiSecurityMap.insert("auth-alg", "leap");
                            wifiSecurityMap.insert("leap-username", wirelessSecuritySetting->leapUsername());
                            wifiSecurityMap.insert("leap-password", wirelessSecuritySetting->leapPassword());
                        }
                    } else if (wirelessSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::WpaNone) {
                        wifiSecurityMap.insert("key-mgmt", "wpa-none");
                        wifiSecurityMap.insert("psk", wirelessSecuritySetting->psk());
                    } else if (wirelessSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::WpaPsk) {
                        wifiSecurityMap.insert("key-mgmt", "wpa-psk");
                        wifiSecurityMap.insert("psk", wirelessSecuritySetting->psk());
                    } else if (wirelessSecuritySetting->keyMgmt() == NetworkManager::WirelessSecuritySetting::WpaEap) {
                        wifiSecurityMap.insert("key-mgmt", "wpa-eap");
                        // TODO
                    }

                    resultingMap.insert("802-11-wireless-security", wifiSecurityMap);
                }
            }
        }
    }

    return resultingMap;
}


