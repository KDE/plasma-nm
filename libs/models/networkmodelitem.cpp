/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

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

#include "networkmodelitem.h"
#include "uiutils.h"

#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/VpnConnection>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/WimaxDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

#include <KLocale>
#include <KLocalizedString>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/manager.h>
#include <ModemManagerQt/modem.h>
#include <ModemManagerQt/modemdevice.h>
#include <ModemManagerQt/modem3gpp.h>
#include <ModemManagerQt/modemcdma.h>
#endif

NetworkModelItem::NetworkModelItem(QObject* parent)
    : QObject(parent)
    , m_connectionState(NetworkManager::ActiveConnection::Deactivated)
    , m_deviceState(NetworkManager::Device::UnknownState)
    , m_duplicate(false)
    , m_engine(0)
    , m_dataEngineConsumer(new Plasma::DataEngineConsumer)
    , m_mode(NetworkManager::WirelessSetting::Infrastructure)
    , m_securityType(NetworkManager::Utils::None)
    , m_signal(0)
    , m_slave(false)
    , m_type(NetworkManager::ConnectionSettings::Unknown)
    , m_updateEnabled(false)
    , m_vpnState(NetworkManager::VpnConnection::Unknown)
{
}

NetworkModelItem::NetworkModelItem(const NetworkModelItem* item, QObject* parent)
    : QObject(parent)
    , m_connectionPath(item->connectionPath())
    , m_connectionState(NetworkManager::ActiveConnection::Deactivated)
    , m_duplicate(true)
    , m_engine(0)
    , m_mode(item->mode())
    , m_name(item->name())
    , m_securityType(item->securityType())
    , m_slave(item->slave())
    , m_ssid(item->ssid())
    , m_timestamp(item->timestamp())
    , m_type(item->type())
    , m_uuid(item->uuid())
    , m_vpnState(NetworkManager::VpnConnection::Unknown)
{
}

NetworkModelItem::~NetworkModelItem()
{
}

QString NetworkModelItem::activeConnectionPath() const
{
    return m_activeConnectionPath;
}

void NetworkModelItem::setActiveConnectionPath(const QString& path)
{
    m_activeConnectionPath = path;
}

QString NetworkModelItem::connectionPath() const
{
    return m_connectionPath;
}

void NetworkModelItem::setConnectionPath(const QString& path)
{
    m_connectionPath = path;
}

NetworkManager::ActiveConnection::State NetworkModelItem::connectionState() const
{
    return m_connectionState;
}

void NetworkModelItem::setConnectionState(NetworkManager::ActiveConnection::State state)
{
    m_connectionState = state;

    if (m_connectionState == NetworkManager::ActiveConnection::Activated && !m_devicePath.isEmpty()) {
        initializeDataEngine();
    } else {
        removeDataEngine();
    }
}

QStringList NetworkModelItem::details() const
{
    return m_details;
}

QString NetworkModelItem::devicePath() const
{
    return m_devicePath;
}

void NetworkModelItem::setDeviceName(const QString& name)
{
    m_deviceName = name;
}

void NetworkModelItem::setDevicePath(const QString& path)
{
    m_devicePath = path;
}

QString NetworkModelItem::deviceState() const
{
    return UiUtils::connectionStateToString(m_deviceState);
}

void NetworkModelItem::setDeviceState(const NetworkManager::Device::State state)
{
    m_deviceState = state;
}

QString NetworkModelItem::download() const
{
    double download = m_download.toDouble();
    return KLocale::global()->formatByteSize(download*1024) + "/s";
}

bool NetworkModelItem::duplicate() const
{
    return m_duplicate;
}

QString NetworkModelItem::icon() const
{
    switch (m_type) {
        case NetworkManager::ConnectionSettings::Adsl:
            return "network-mobile-100";
            break;
        case NetworkManager::ConnectionSettings::Bluetooth:
            if (connectionState() == NetworkManager::ActiveConnection::Activated) {
                return "network-bluetooth-activated";
            } else {
                return "network-bluetooth";
            }
            break;
        case NetworkManager::ConnectionSettings::Bond:
            break;
        case NetworkManager::ConnectionSettings::Bridge:
            break;
        case NetworkManager::ConnectionSettings::Cdma:
        case NetworkManager::ConnectionSettings::Gsm:
            if (m_signal == 0 ) {
                return "network-mobile-0";
            } else if (m_signal < 20) {
                return "network-mobile-20";
            } else if (m_signal < 40) {
                return "network-mobile-40";
            } else if (m_signal < 60) {
                return "network-mobile-60";
            } else if (m_signal < 80) {
                return "network-mobile-80";
            } else {
                return "network-mobile-100";
            }
            break;
        case NetworkManager::ConnectionSettings::Infiniband:
            break;
        case NetworkManager::ConnectionSettings::OLPCMesh:
            break;
        case NetworkManager::ConnectionSettings::Pppoe:
            return "network-mobile-100";
            break;
        case NetworkManager::ConnectionSettings::Vlan:
            break;
        case NetworkManager::ConnectionSettings::Vpn:
            return "network-vpn";
            break;
        case NetworkManager::ConnectionSettings::Wimax:
            break;
        case NetworkManager::ConnectionSettings::Wired:
            if (connectionState() == NetworkManager::ActiveConnection::Activated) {
                return "network-wired-activated";
            } else {
                return "network-wired";
            }
            break;
        case NetworkManager::ConnectionSettings::Wireless:
            if (m_signal == 0 ) {
                if (m_mode == NetworkManager::WirelessSetting::Adhoc || m_mode == NetworkManager::WirelessSetting::Ap) {
                    return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-100" : "network-wireless-100-locked";
                }
                return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-0" : "network-wireless-0-locked";
            } else if (m_signal < 20) {
                return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-20" : "network-wireless-20-locked";
            } else if (m_signal < 40) {
                return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-40" : "network-wireless-40-locked";
            } else if (m_signal < 60) {
                return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-60" : "network-wireless-60-locked";
            } else if (m_signal < 80) {
                return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-80" : "network-wireless-80-locked";
            } else {
                return (m_securityType == NetworkManager::Utils::None) ? "network-wireless-100" : "network-wireless-100-locked";
            }
            break;
        default:
            if (connectionState() == NetworkManager::ActiveConnection::Activated) {
                return "network-wired-activated";
            } else {
                return "network-wired";
            }
            break;
    }

    return "network-wired";
}

NetworkModelItem::ItemType NetworkModelItem::itemType() const
{
    if (!m_devicePath.isEmpty() ||
        m_type == NetworkManager::ConnectionSettings::Bond || 
        ((NetworkManager::status() == NetworkManager::Connected ||
          NetworkManager::status() == NetworkManager::ConnectedLinkLocal ||
          NetworkManager::status() == NetworkManager::ConnectedSiteOnly) && m_type == NetworkManager::ConnectionSettings::Vpn)) {
        if (m_connectionPath.isEmpty() && m_type == NetworkManager::ConnectionSettings::Wireless) {
            return NetworkModelItem::AvailableAccessPoint;
        } else {
            return NetworkModelItem::AvailableConnection;
        }
    }
    return NetworkModelItem::UnavailableConnection;
}

NetworkManager::WirelessSetting::NetworkMode NetworkModelItem::mode() const
{
    return m_mode;
}

void NetworkModelItem::setMode(const NetworkManager::WirelessSetting::NetworkMode mode)
{
    m_mode = mode;
}

QString NetworkModelItem::name() const
{
    return m_name;
}

void NetworkModelItem::setName(const QString& name)
{
    m_name = name;
}

QString NetworkModelItem::originalName() const
{
    if (m_deviceName.isEmpty()) {
        return m_name;
    }
    return m_name + " (" + m_deviceName + ')';
}

QString NetworkModelItem::sectionType() const
{
    if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
        return i18n("Active connections");
    }  else {
        return i18n("Available connections");
    }
}

NetworkManager::Utils::WirelessSecurityType NetworkModelItem::securityType() const
{
    return m_securityType;
}

void NetworkModelItem::setSecurityType(NetworkManager::Utils::WirelessSecurityType type)
{
    m_securityType = type;
}

int NetworkModelItem::signal() const
{
    return m_signal;
}

void NetworkModelItem::setSignal(int signal)
{
    m_signal = signal;
}

bool NetworkModelItem::slave() const
{
    return m_slave;
}

void NetworkModelItem::setSlave(bool slave)
{
    m_slave = slave;
}

QString NetworkModelItem::specificPath() const
{
    return m_specificPath;
}

void NetworkModelItem::setSpecificPath(const QString& path)
{
    m_specificPath = path;
}

QString NetworkModelItem::ssid() const
{
    return m_ssid;
}

void NetworkModelItem::setSsid(const QString& ssid)
{
    m_ssid = ssid;
}

NetworkManager::ConnectionSettings::ConnectionType NetworkModelItem::type() const
{
    return m_type;
}

QDateTime NetworkModelItem::timestamp() const
{
    return m_timestamp;
}

void NetworkModelItem::setTimestamp(const QDateTime& date)
{
    m_timestamp = date;
}

void NetworkModelItem::setType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    m_type = type;
}

QString NetworkModelItem::uni() const
{
    if (m_type == NetworkManager::ConnectionSettings::Wireless && m_uuid.isEmpty()) {
        return m_ssid + '%' + m_devicePath;
    } else {
        return m_connectionPath + '%' + m_devicePath;
    }
}

QString NetworkModelItem::upload() const
{
    double upload = m_upload.toDouble();
    return KLocale::global()->formatByteSize(upload*1024) + "/s";
}

QString NetworkModelItem::uuid() const
{
    return m_uuid;
}

void NetworkModelItem::setUuid(const QString& uuid)
{
    m_uuid = uuid;
}

QString NetworkModelItem::vpnState() const
{
    return UiUtils::vpnConnectionStateToString(m_vpnState);
}

void NetworkModelItem::setVpnState(NetworkManager::VpnConnection::State state)
{
    m_vpnState = state;
}

bool NetworkModelItem::operator==(const NetworkModelItem* item) const
{
    if (!item->uuid().isEmpty() && !uuid().isEmpty()) {
        if (item->devicePath() == devicePath() && item->uuid() == uuid()) {
            return true;
        }
    } else if (item->type() == NetworkManager::ConnectionSettings::Wireless && type() == NetworkManager::ConnectionSettings::Wireless) {
        if (item->ssid() == ssid() && item->devicePath() == devicePath()) {
            return true;
        }
    }

    return false;
}

void NetworkModelItem::updateDetails()
{
    m_details.clear();

    if (itemType() == NetworkModelItem::UnavailableConnection) {
        return;
    }

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);

    // Get IPv[46]Address
    if (device && device->ipV4Config().isValid() && m_connectionState == NetworkManager::ActiveConnection::Activated) {
        if (!device->ipV4Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV4Config().addresses().first().ip();
            if (!addr.isNull()) {
                m_details << i18n("IPv4 Address") << addr.toString();
            }
        }
    }

    if (device && device->ipV6Config().isValid() && m_connectionState == NetworkManager::ActiveConnection::Activated) {
        if (!device->ipV6Config().addresses().isEmpty()) {
            QHostAddress addr = device->ipV6Config().addresses().first().ip();
            if (!addr.isNull()) {
                m_details << i18n("IPv6 Address") << addr.toString();
            }
        }
    }

    if (m_type == NetworkManager::ConnectionSettings::Wired) {
        NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        if (wiredDevice) {
            if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
                m_details << i18n("Connection speed") << UiUtils::connectionSpeed(wiredDevice->bitRate());
            }
            m_details << i18n("MAC Address") << wiredDevice->permanentHardwareAddress();
        }
    } else if (m_type == NetworkManager::ConnectionSettings::Wireless) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        m_details << i18n("Access point (SSID)") << m_ssid;
        m_details << i18n("Signal strength") << i18n("%1%", m_signal);
        if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
            m_details << i18n("Security type") << UiUtils::labelFromWirelessSecurity(m_securityType);
        }
        if (wirelessDevice) {
            if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
                m_details << i18n("Connection speed") << UiUtils::connectionSpeed(wirelessDevice->bitRate());
            }
            m_details << i18n("MAC Address") << wirelessDevice->permanentHardwareAddress();
        }
    } else if (m_type == NetworkManager::ConnectionSettings::Gsm || m_type == NetworkManager::ConnectionSettings::Cdma) {
#if WITH_MODEMMANAGER_SUPPORT
        NetworkManager::ModemDevice::Ptr modemDevice = device.objectCast<NetworkManager::ModemDevice>();
        ModemManager::ModemDevice::Ptr modem = ModemManager::findModemDevice(modemDevice->udi());
        ModemManager::Modem::Ptr modemNetwork = modem->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();

        if (m_type == NetworkManager::ConnectionSettings::Gsm) {
            ModemManager::Modem3gpp::Ptr gsmNet = modem->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
            m_details << i18n("Operator") << gsmNet->operatorName();
        } else {
            ModemManager::ModemCdma::Ptr cdmaNet = modem->interface(ModemManager::ModemDevice::CdmaInterface).objectCast<ModemManager::ModemCdma>();
            m_details << i18n("Network ID") << QString("%1%").arg(cdmaNet->nid());
        }
        m_details << i18n("Signal Quality") << QString("%1%").arg(modemNetwork->signalQuality().signal);
        m_details << i18n("Access Technology") << UiUtils::convertAccessTechnologyToString(modemNetwork->accessTechnologies());
#endif
    } else if (m_type == NetworkManager::ConnectionSettings::Vpn) {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(m_connectionPath);
        NetworkManager::ConnectionSettings::Ptr connectionSettings;
        NetworkManager::VpnSetting::Ptr vpnSetting;

        if (connection) {
            connectionSettings = connection->settings();
        }
        if (connectionSettings) {
            vpnSetting = connectionSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
        }

        if (vpnSetting) {
            m_details << i18n("VPN plugin") << vpnSetting->serviceType().section('.', -1);
        }

        if (m_connectionState == NetworkManager::ActiveConnection::Activated) {
            NetworkManager::ActiveConnection::Ptr active = NetworkManager::findActiveConnection(m_activeConnectionPath);
            NetworkManager::VpnConnection::Ptr vpnConnection;

            if (active) {
                vpnConnection = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(active->path()), &QObject::deleteLater);
            }

            if (vpnConnection) {
                m_details << i18n("Banner") << vpnConnection->banner().simplified();
            }
        }
    } else if (m_type == NetworkManager::ConnectionSettings::Bluetooth) {
        // TODO
//      Bluetooth
//      - Bluetooth HW address
//      - BT name
//      - BT capabilities
    } else if (m_type == NetworkManager::ConnectionSettings::Wimax) {
        // TODO
//      Wimax
//      - MAC address
//      - NSP
//      - Signal
    }
}


void NetworkModelItem::dataUpdated(const QString& sourceName, const Plasma::DataEngine::Data& data)
{
    if (sourceName == m_uploadSource) {
        m_upload = data["value"].toString();
    } else if (sourceName == m_downloadSource) {
        m_download = data["value"].toString();
    }
    Q_EMIT itemUpdated();
}

void NetworkModelItem::initializeDataEngine()
{
    m_engine = m_dataEngineConsumer->dataEngine("systemmonitor");

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);
    if (!device) {
        removeDataEngine();
        return;
    }

    QString interfaceName = device->ipInterfaceName();
    if (interfaceName.isEmpty()) {
        interfaceName = device->interfaceName();
    }

    m_downloadSource = QString("network/interfaces/%1/receiver/data").arg(interfaceName);
    m_uploadSource = QString("network/interfaces/%1/transmitter/data").arg(interfaceName);

    setUpdateEnabled(true);
}

void NetworkModelItem::removeDataEngine()
{
    setUpdateEnabled(false);
}

void NetworkModelItem::setUpdateEnabled(bool enabled)
{
    Plasma::DataEngine * engine = m_dataEngineConsumer->dataEngine("systemmonitor");
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);
    if (engine->isValid()) {
        int interval = 2000;
        if (enabled) {
            if (device) {
                engine->connectSource(m_downloadSource, this, interval);
                engine->connectSource(m_uploadSource, this, interval);
            }
        } else {
            engine->disconnectSource(m_downloadSource, this);
            engine->disconnectSource(m_uploadSource, this);
        }
    }
    m_updateEnabled = enabled;
}
