/*
    Copyright 2018 Jan Grulich <jgrulich@redhat.com>

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

#include "utils.h"
#include "uiutils.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessNetwork>
#include <NetworkManagerQt/Utils>

#include <KLocalizedString>

Utils::Utils(QObject *parent)
    : QObject(parent)
{
}

Utils::~Utils()
{
}

QVariantList Utils::availableSsids(const QString &ssid)
{
    QVariantList list;
    QList<NetworkManager::WirelessDevice::Ptr> devices;
    QList<NetworkManager::WirelessNetwork::Ptr> networks;

    foreach (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();

            devices << wifiDevice;

            foreach (const NetworkManager::WirelessNetwork::Ptr &newNetwork, wifiDevice->networks()) {
                foreach (const NetworkManager::WirelessNetwork::Ptr &existingNetwork, networks) {
                    if (newNetwork->ssid() == existingNetwork->ssid() &&
                        newNetwork->signalStrength() > existingNetwork->signalStrength()) {
                        networks.removeOne(existingNetwork);
                        break;
                    }
                }

                networks << newNetwork;
            }
        }
    }

    std::sort(networks.begin(), networks.end(), [] (const NetworkManager::WirelessNetwork::Ptr &one, const NetworkManager::WirelessNetwork::Ptr &two) {
        return one->signalStrength() > two->signalStrength();
    });

    bool found = false;
    foreach (const NetworkManager::WirelessNetwork::Ptr &network, networks) {
        NetworkManager::AccessPoint::Ptr accessPoint = network->referenceAccessPoint();

        if (!accessPoint) {
            continue;
        }

        if (ssid == network->ssid()) {
            found = true;
        }

        foreach (const NetworkManager::WirelessDevice::Ptr &device, devices) {
            if (device->findNetwork(network->ssid()) == network) {
                NetworkManager::WirelessSecurityType security = NetworkManager::findBestWirelessSecurity(device->wirelessCapabilities(), true, (device->mode() == NetworkManager::WirelessDevice::Adhoc), accessPoint->capabilities(), accessPoint->wpaFlags(), accessPoint->rsnFlags());
                QVariantMap map;
                map.insert(QLatin1String("ssid"), accessPoint->ssid());
                map.insert(QLatin1String("signal"), network->signalStrength());
                if (security != NetworkManager::UnknownSecurity && security != NetworkManager::NoneSecurity) {
                    map.insert(QLatin1String("security"), UiUtils::labelFromWirelessSecurity(security));
                } else {
                    map.insert(QLatin1String("security"), i18n("Insecure"));
                }

                list << map;
            }
        }
    }

    if (!found) {
        QVariantMap map;
        map.insert(QLatin1String("ssid"), ssid);
        list.insert(0, map);
    }

    return list;
}

QVariantList Utils::availableBssids(const QString &ssid, const QString &bssid)
{
    QVariantList list;
    QList<NetworkManager::AccessPoint::Ptr> aps;

    foreach (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
            NetworkManager::WirelessNetwork::Ptr wifiNetwork = wifiDevice->findNetwork(ssid);

            if (!wifiNetwork) {
                continue;
            }

            foreach (const NetworkManager::AccessPoint::Ptr &newAp, wifiNetwork->accessPoints()) {
                foreach (const NetworkManager::AccessPoint::Ptr &existingAp, aps) {
                    if (newAp->hardwareAddress() == existingAp->hardwareAddress() &&
                        newAp->signalStrength() > existingAp->signalStrength()) {
                        aps.removeOne(existingAp);
                        break;
                    }
                }

                aps << newAp;
            }
        }
    }

    std::sort(aps.begin(), aps.end(), [] (const NetworkManager::AccessPoint::Ptr &one, const NetworkManager::AccessPoint::Ptr &two) {
        return one->signalStrength() > two->signalStrength();
    });

    bool found = false;

    foreach (const NetworkManager::AccessPoint::Ptr &ap, aps) {
        if (!ap) {
            continue;
        }

        if (bssid == ap->hardwareAddress()) {
            found = true;
        }

        QVariantMap map;
        map.insert(QLatin1String("bssid"), ap->hardwareAddress());
        map.insert(QLatin1String("signal"), ap->signalStrength());
        map.insert(QLatin1String("frequency"), ap->frequency());
        map.insert(QLatin1String("channel"), QString::number(NetworkManager::findChannel(ap->frequency())));

        list << map;
    }

    if (!found) {
        QVariantMap map;
        map.insert(QLatin1String("bssid"), bssid);
        list.insert(0, map);
    }

    return list;
}

QVariantList Utils::deviceHwAddresses(uint deviceType, const QString &address)
{
    bool found = false;
    QString deviceAddress;
    QString deviceName;
    QVariantList list;

    foreach (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        const NetworkManager::Device::Type type = device->type();
        if ((NetworkManager::Device::Type)deviceType == type) {
            // Only wifi and ethernet supported for now
            if (type == NetworkManager::Device::Wifi) {
                deviceAddress = device->as<NetworkManager::WirelessDevice>()->permanentHardwareAddress();
            } else if (type == NetworkManager::Device::Ethernet) {
                deviceAddress = device->as<NetworkManager::WiredDevice>()->permanentHardwareAddress();
            }

            if (device->state() == NetworkManager::Device::Activated) {
                deviceName = device->ipInterfaceName();
            } else {
                deviceName = device->interfaceName();
            }

            QVariantMap map;
            if (!deviceAddress.isEmpty()) {
                map.insert(QLatin1String("mac"), deviceAddress);
                map.insert(QLatin1String("device"), deviceName);
            }

            if (address == deviceAddress) {
                found = true;
                list.insert(0, map);
            } else {
                list << map;
            }
        }
    }

    if (!found) {
        list.insert(0, QVariantMap{{QLatin1String("mac"), address}});
    }

    return list;
}

QStringList Utils::firewallZones()
{
    QStringList firewallZones = {i18n("Default")};

    // Load list of firewall zones
    QDBusMessage msg = QDBusMessage::createMethodCall("org.fedoraproject.FirewallD1", "/org/fedoraproject/FirewallD1",
                                                      "org.fedoraproject.FirewallD1.zone", "getZones");
    QDBusPendingReply<QStringList> reply = QDBusConnection::systemBus().asyncCall(msg);
    reply.waitForFinished();
    if (reply.isValid()) {
        firewallZones += reply.value();
    }

    return firewallZones;
}

QStringList Utils::vpnConnections()
{
    QStringList vpnConnections;

    // Load list of VPN connections
    NetworkManager::Connection::List list = NetworkManager::listConnections();

    Q_FOREACH (const NetworkManager::Connection::Ptr & conn, list) {
        NetworkManager::ConnectionSettings::Ptr conSet = conn->settings();
        if (conSet->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
            vpnConnections << conSet->id();
        }
    }

    return vpnConnections;
}

QStringList Utils::wirelessChannels(uint band)
{
    QStringList result;
    QList<QPair<int, int> > channels;

    if ((NetworkManager::WirelessSetting::FrequencyBand)band == NetworkManager::WirelessSetting::A) {
        channels = NetworkManager::getAFreqs();
    } else if ((NetworkManager::WirelessSetting::FrequencyBand)band == NetworkManager::WirelessSetting::Bg) {
        channels = NetworkManager::getBFreqs();
    } else {
        return QStringList();
    }

    QListIterator<QPair<int,int> > i(channels);
    while (i.hasNext()) {
        QPair<int,int> channel = i.next();
        result << QStringLiteral("%1 (%2 MHz)").arg(channel.first).arg(channel.second);
    }

    return result;
}
