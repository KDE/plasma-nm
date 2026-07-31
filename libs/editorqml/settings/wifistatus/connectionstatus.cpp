/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "connectionstatus.h"
#include "connectiondetails.h"
#include "connectiondetailsmodel.h"

#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>

ConnectionStatus::ConnectionStatus(QObject *parent)
    : QObject(parent)
    , m_detailsModel(new ConnectionDetailsModel(this))
{
}

ConnectionStatus::~ConnectionStatus() = default;

void ConnectionStatus::setConnectionAndDevice(const NetworkManager::Connection::Ptr &connection,
                                              const NetworkManager::Device::Ptr &device,
                                              const QString &accessPointPath)
{
    m_connection = connection;
    m_device = device;
    m_accessPointPath = accessPointPath;
    m_connectionUuid = connection ? connection->uuid() : QString();
    updateConnectionDetails();
}

void ConnectionStatus::setConnectionUuid(const QString &uuid)
{
    if (m_connectionUuid != uuid) {
        m_connectionUuid = uuid;
        updateStatusWidget();
    }
}

ConnectionDetailsModel *ConnectionStatus::detailsModel() const
{
    return m_detailsModel;
}

bool ConnectionStatus::hasDetails() const
{
    return m_detailsModel->rowCount() > 0;
}

void ConnectionStatus::updateStatusWidget()
{
    m_connection = nullptr;
    m_device = nullptr;
    m_accessPointPath.clear();

    if (m_connectionUuid.isEmpty()) {
        updateConnectionDetails();
        return;
    }

    NetworkManager::Connection::Ptr nmConnection = NetworkManager::findConnectionByUuid(m_connectionUuid);
    if (!nmConnection) {
        return;
    }
    m_connection = nmConnection;

    NetworkManager::ActiveConnection::Ptr activeConnection;
    for (const NetworkManager::ActiveConnection::Ptr &active : NetworkManager::activeConnections()) {
        if (active->uuid() == m_connectionUuid) {
            activeConnection = active;
            break;
        }
    }

    if (activeConnection) {
        const QStringList devicePaths = activeConnection->devices();
        if (!devicePaths.isEmpty()) {
            m_device = NetworkManager::findNetworkInterface(devicePaths.first());
        }
    } else {
        const QString interfaceName = nmConnection->settings()->interfaceName();
        if (!interfaceName.isEmpty()) {
            for (const NetworkManager::Device::Ptr &dev : NetworkManager::networkInterfaces()) {
                if (dev->interfaceName() == interfaceName) {
                    m_device = dev;
                    break;
                }
            }
        }

        if (!m_device) {
            NetworkManager::ConnectionSettings::ConnectionType type = nmConnection->settings()->connectionType();
            for (const NetworkManager::Device::Ptr &dev : NetworkManager::networkInterfaces()) {
                if (type == NetworkManager::ConnectionSettings::Wireless && dev->type() == NetworkManager::Device::Wifi) {
                    m_device = dev;
                    break;
                } else if (type == NetworkManager::ConnectionSettings::Wired && dev->type() == NetworkManager::Device::Ethernet) {
                    m_device = dev;
                    break;
                }
            }
        }

        if (m_device && m_device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wirelessDevice = m_device.objectCast<NetworkManager::WirelessDevice>();
            if (wirelessDevice) {
                NetworkManager::WirelessSetting::Ptr wirelessSetting =
                    nmConnection->settings()->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                if (wirelessSetting) {
                    const QString bssid = wirelessSetting->bssid();
                    const QString ssid = wirelessSetting->ssid();

                    for (const QString &apPath : wirelessDevice->accessPoints()) {
                        NetworkManager::AccessPoint::Ptr ap = wirelessDevice->findAccessPoint(apPath);
                        if (!ap) {
                            continue;
                        }
                        if (!bssid.isEmpty()) {
                            if (ap->hardwareAddress().toLower() == bssid.toLower()) {
                                m_accessPointPath = apPath;
                                break;
                            }
                        } else if (ap->ssid() == ssid) {
                            m_accessPointPath = apPath;
                            break;
                        }
                    }
                }
            }
        }
    }

    setConnectionAndDevice(nmConnection, m_device, m_accessPointPath);
}

QList<ConnectionDetails::ConnectionDetailSection> ConnectionStatus::getConnectionDetails() const
{
    if (!m_device) {
        return {};
    }
    return ConnectionDetails::getConnectionDetails(m_connection, m_device, m_accessPointPath);
}

void ConnectionStatus::updateConnectionDetails()
{
    m_detailsModel->setDetailsList(getConnectionDetails());
    Q_EMIT detailsChanged();
}

#include "moc_connectionstatus.cpp"
