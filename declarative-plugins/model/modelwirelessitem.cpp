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

#include "modelwirelessitem.h"

#include <NetworkManagerQt/wirelessdevice.h>
#include <NetworkManagerQt/settings/802-11-wireless.h>
#include <NetworkManagerQt/manager.h>

#include <KLocalizedString>

#include "debug.h"

ModelWirelessItem::ModelWirelessItem(const NetworkManager::Device::Ptr &device, QObject* parent):
    ModelItem(device, parent),
    m_previousSignal(0),
    m_signal(0),
    m_secure(false)
{
    m_type = NetworkManager::Settings::ConnectionSettings::Wireless;
}

ModelWirelessItem::~ModelWirelessItem()
{
}

QString ModelWirelessItem::icon() const
{
    if (m_signal < 13) {
        return "network-wireless-connected-00";
    } else if (m_signal < 38) {
        return "network-wireless-connected-25";
    } else if (m_signal < 63) {
        return "network-wireless-connected-50";
    } else if (m_signal < 88) {
        return "network-wireless-connected-75";
    } else {
        return "network-wireless-connected-100";
    }
}

int ModelWirelessItem::signal() const
{
    return m_signal;
}

QString ModelWirelessItem::ssid() const
{
    return m_ssid;
}

bool ModelWirelessItem::secure() const
{
    return m_secure;
}

QString ModelWirelessItem::specificParameter() const
{
    return m_ssid;
}

void ModelWirelessItem::updateDetailsContent()
{
    QString format = "<tr><td align=\"right\"><b>%1</b></td><td align=\"left\">&nbsp;%2</td></tr>";

    if (m_type != NetworkManager::Settings::ConnectionSettings::Unknown) {
        m_details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::Settings::ConnectionSettings::typeAsString(m_type));
    }

    m_details += QString(format).arg("\n", "\n");

    if (!connected()) {
        foreach (const QString & path, m_devicePaths) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);

            if (device) {
                QString name;
                if (device->ipInterfaceName().isEmpty()) {
                    name = device->interfaceName();
                } else {
                    name = device->ipInterfaceName();
                }
                m_details += QString(format).arg(i18n("System name:"), name);

                if (device->ipV4Config().isValid() && connected()) {
                    QHostAddress addr = device->ipV4Config().addresses().first().ip();
                    m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
                }

                if (device->ipV6Config().isValid() && connected()) {
                    QHostAddress addr = device->ipV6Config().addresses().first().ip();
                    m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
                }

                NetworkManager::WirelessDevice::Ptr wireless = device.objectCast<NetworkManager::WirelessDevice>();
                m_details += QString(format).arg(i18n("MAC Address:"), wireless->permanentHardwareAddress());
                m_details += QString(format).arg(i18n("Driver:"), device->driver());

                NetworkManager::WirelessNetwork::Ptr network = wireless->findNetwork(m_ssid);

                if (network) {
                    NetworkManager::AccessPoint::Ptr ap = network->referenceAccessPoint();

                    m_details += QString(format).arg(i18n("Signal strength:"), i18n("%1%", network->signalStrength()));
                    m_details += QString(format).arg(i18n("Access point (SSID):"), network->ssid());

                    if (ap) {
                        m_details += QString(format).arg(i18n("Access point (BSSID):"), ap->hardwareAddress());
                        m_details += QString(format).arg(i18nc("Wifi AP frequency", "Frequency:"), i18n("%1 Mhz", ap->frequency()));
                    }
                }

                m_details += QString(format).arg("\n", "\n");
            }
        }
    } else {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_activeDevicePath);

        if (device) {
            QString name;
            if (device->ipInterfaceName().isEmpty()) {
                name = device->interfaceName();
            } else {
                name = device->ipInterfaceName();
            }
            m_details += QString(format).arg(i18n("System name:"), name);

            if (device->ipV4Config().isValid() && connected()) {
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }

            if (device->ipV6Config().isValid() && connected()) {
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }

            NetworkManager::WirelessDevice::Ptr wireless = device.objectCast<NetworkManager::WirelessDevice>();
            if (connected()) {
                if (wireless->bitRate() && wireless->bitRate() < 1000) {
                    m_details += QString(format).arg(i18n("Connection speed:"), i18n("%1 Kb/s", wireless->bitRate()));
                } else if (wireless->bitRate()) {
                    m_details += QString(format).arg(i18n("Connection speed:"), i18n("%1 Mb/s", wireless->bitRate()/1000));
                }
            }
            m_details += QString(format).arg(i18n("MAC Address:"), wireless->permanentHardwareAddress());
            m_details += QString(format).arg(i18n("Driver:"), device->driver());

            NetworkManager::WirelessNetwork::Ptr network = wireless->findNetwork(m_ssid);

            if (network) {
                NetworkManager::AccessPoint::Ptr ap = network->referenceAccessPoint();

                m_details += QString(format).arg(i18n("Signal strength:"), i18n("%1%", network->signalStrength()));
                m_details += QString(format).arg(i18n("Access point (SSID):"), network->ssid());

                if (ap) {
                    m_details += QString(format).arg(i18n("Access point (BSSID):"), ap->hardwareAddress());
                    m_details += QString(format).arg(i18nc("Wifi AP frequency", "Frequency:"), i18n("%1 Mhz", ap->frequency()));
                }
            }

            m_details += QString(format).arg("\n", "\n");
        }
    }
}

void ModelWirelessItem::setConnection(const NetworkManager::Settings::Connection::Ptr & connection)
{
    ModelItem::setConnection(connection);

    if (!m_connection) {
        if (!m_networks.isEmpty()) {
            m_name = m_networks.value(m_devicePaths.first())->ssid();
        } else {
            m_ssid.clear();
            m_secure = false;
        }
    }
}

void ModelWirelessItem::setConnectionSettings(const NetworkManager::Settings::ConnectionSettings::Ptr &settings)
{
    ModelItem::setConnectionSettings(settings);
    bool changed = false;
    QString previousSsid;
    if (settings->connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
        NetworkManager::Settings::WirelessSetting::Ptr wirelessSetting;
        wirelessSetting = settings->setting(NetworkManager::Settings::Setting::Wireless).dynamicCast<NetworkManager::Settings::WirelessSetting>();
        if (m_ssid != wirelessSetting->ssid()) {
            if (!m_ssid.isEmpty()) {
                changed = true;
            }
            previousSsid = m_ssid;
            m_ssid = wirelessSetting->ssid();
            if (!wirelessSetting->security().isEmpty()) {
                m_secure = true;
            }
        }

        if (!changed) {
            return;
        }

        bool found = false;
        QMap<NetworkManager::Device::Ptr,NetworkManager::WirelessNetwork::Ptr> networks;
        foreach (const QString & path, devicePaths()) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);
            if (!device) {
                return;
            }
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
            if (!wifiDevice) {
                return;
            }
            NetworkManager::WirelessNetwork::Ptr newWifiNetwork = wifiDevice->findNetwork(m_ssid);

            if (newWifiNetwork) {
                found = true;
                networks.insert(device, newWifiNetwork);
            }
        }

        if (found) {
            foreach (const NetworkManager::Device::Ptr & deviceKey, networks.keys()) {
                m_networks.clear();
                addWirelessNetwork(networks.value(deviceKey), deviceKey);
            }
        } else {
            m_ssid = previousSsid;
            setConnection(NetworkManager::Settings::Connection::Ptr());
        }
    }

    updateDetails();
}

void ModelWirelessItem::addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network, const NetworkManager::Device::Ptr & device)
{
    if (m_networks.contains(device->uni()) && m_networks.value(device->uni()) == network) {
        return;
    }
    m_networks.insert(device->uni(), network);

    if (network) {
        m_type = NetworkManager::Settings::ConnectionSettings::Wireless;
        m_ssid = network->ssid();
        m_previousSignal = m_signal;
        if (network->signalStrength() > m_signal) {
            m_signal = network->signalStrength();
        }

        if (!m_devicePaths.isEmpty()) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePaths.first());

            if (device) {
                NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
                NetworkManager::AccessPoint::Ptr ap = wifiDev->findAccessPoint(network->referenceAccessPoint()->uni());

                if (ap && ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
                    m_secure = true;
                }
            }
        }

        if (!m_connection) {
            m_name = ssid();
        }

        connect(network.data(), SIGNAL(signalStrengthChanged(int)),
                SLOT(onSignalStrengthChanged(int)), Qt::UniqueConnection);
        connect(network.data(), SIGNAL(referenceAccessPointChanged(QString)),
                SLOT(onAccessPointChanged(QString)), Qt::UniqueConnection);
    }

    updateDetails();
}

void ModelWirelessItem::removeWirelessNetwork(const NetworkManager::Device::Ptr & device)
{
    m_networks.remove(device->uni());
    m_devicePaths.removeOne(device->uni());

    foreach (const QString & key, m_networks.keys()) {
        if (m_networks.value(key)->signalStrength() > m_signal) {
            m_previousSignal = m_signal;
            m_signal = m_networks.value(key)->signalStrength();
        }
    }

    updateDetails();

    emit itemChanged();
}

QMap<QString, NetworkManager::WirelessNetwork::Ptr> ModelWirelessItem::wirelessNetworks() const
{
    if (!m_networks.isEmpty()) {
        return m_networks;
    }

    return QMap<QString, NetworkManager::WirelessNetwork::Ptr>();
}

void ModelWirelessItem::onAccessPointChanged(const QString& accessPoint)
{
    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": access point changed";
}

void ModelWirelessItem::onSignalStrengthChanged(int strength)
{
    foreach (const QString & key, m_networks.keys()) {
        if (m_networks.value(key)->signalStrength() > m_signal) {
            m_previousSignal = m_signal;
            m_signal = strength;
        }
    }

    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": strength changed to " << strength;
}
