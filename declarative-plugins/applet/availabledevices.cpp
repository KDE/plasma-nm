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

#include "availabledevices.h"

#include <NetworkManagerQt/Manager>

AvailableDevices::AvailableDevices(QObject* parent):
    QObject(parent),
    m_wiredAvailable(false),
    m_wirelessAvailable(false),
    m_wimaxAvailable(false),
    m_wwanAvailable(false)
{
}

AvailableDevices::~AvailableDevices()
{
}

void AvailableDevices::init()
{
    foreach (const NetworkManager::Device::Ptr& device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Modem) {
            m_wwanDevicePath = device->uni();
            Q_EMIT wwanDevicePathChanged(m_wwanDevicePath);
            m_wwanAvailable = true;
            Q_EMIT wwanAvailableChanged(true);
        } else if (device->type() == NetworkManager::Device::Ethernet) {
            m_wiredDevicePath = device->uni();
            Q_EMIT wiredDevicePathChanged(m_wiredDevicePath);
            m_wiredAvailable = true;
            Q_EMIT wiredAvailableChanged(true);
        } else if (device->type() == NetworkManager::Device::Wifi) {
            m_wirelessDevicePath = device->uni();
            Q_EMIT wirelessDevicePathChanged(m_wirelessDevicePath);
            m_wirelessAvailable = true;
            Q_EMIT wirelessAvailableChanged(true);
        } else if (device->type() == NetworkManager::Device::Wimax) {
            m_wimaxDevicePath = device->uni();
            Q_EMIT wimaxDevicePathChanged(m_wimaxDevicePath);
            m_wimaxAvailable = true;
            Q_EMIT wimaxAvailableChanged(true);
        }
    }

    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            SLOT(deviceRemoved()));
}

QString AvailableDevices::wiredDevicePath() const
{
    return m_wiredDevicePath;
}

QString AvailableDevices::wirelessDevicePath() const
{
    return m_wirelessDevicePath;
}

QString AvailableDevices::wimaxDevicePath() const
{
    return m_wimaxDevicePath;
}

QString AvailableDevices::wwanDevicePath() const
{
    return m_wwanDevicePath;
}

bool AvailableDevices::isWiredAvailable() const
{
    return m_wiredAvailable;
}

bool AvailableDevices::isWirelessAvailable() const
{
    return m_wirelessAvailable;
}

bool AvailableDevices::isWimaxAvailable() const
{
    return m_wimaxAvailable;
}

bool AvailableDevices::isWwanAvailable() const
{
    return m_wwanAvailable;
}

void AvailableDevices::deviceAdded(const QString& dev)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(dev);

    if (device) {
        if (device->type() == NetworkManager::Device::Modem && !m_wwanAvailable) {
            m_wwanDevicePath = device->uni();
            Q_EMIT wwanDevicePathChanged(m_wwanDevicePath);
            m_wwanAvailable = true;
            Q_EMIT wwanAvailableChanged(true);
        } else if (device->type() == NetworkManager::Device::Ethernet && !m_wiredAvailable) {
            m_wiredDevicePath = device->uni();
            Q_EMIT wiredDevicePathChanged(m_wiredDevicePath);
            m_wiredAvailable = true;
            Q_EMIT wiredAvailableChanged(true);
        } else if (device->type() == NetworkManager::Device::Wifi && !m_wirelessAvailable) {
            m_wirelessDevicePath = device->uni();
            Q_EMIT wirelessDevicePathChanged(m_wirelessDevicePath);
            m_wirelessAvailable = true;
            Q_EMIT wirelessAvailableChanged(true);
        } else if (device->type() == NetworkManager::Device::Wimax && !m_wimaxAvailable) {
            m_wimaxDevicePath = device->uni();
            Q_EMIT wimaxDevicePathChanged(m_wimaxDevicePath);
            m_wimaxAvailable = true;
            Q_EMIT wimaxAvailableChanged(true);
        }
    }
}

void AvailableDevices::deviceRemoved()
{
    bool wired = false;
    bool wireless = false;
    bool wimax = false;
    bool wwan = false;

    foreach (const NetworkManager::Device::Ptr& device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Modem) {
            wwan = true;
        } else if (device->type() == NetworkManager::Device::Ethernet) {
            wired = true;
        } else if (device->type() == NetworkManager::Device::Wifi) {
            wireless = true;
        } else if (device->type() == NetworkManager::Device::Wimax) {
            wimax = true;
        }
    }

    if (!wired && m_wiredAvailable) {
        m_wiredDevicePath.clear();
        Q_EMIT wiredDevicePathChanged(QString());
        m_wiredAvailable = false;
        Q_EMIT wiredAvailableChanged(false);
    }

    if (!wireless && m_wirelessAvailable) {
        m_wirelessDevicePath.clear();
        Q_EMIT wirelessDevicePathChanged(QString());
        m_wirelessAvailable = false;
        Q_EMIT wirelessAvailableChanged(false);
    }

    if (!wimax && m_wimaxAvailable) {
        m_wimaxDevicePath.clear();
        Q_EMIT wimaxDevicePathChanged(QString());
        m_wwanAvailable = false;
        Q_EMIT wwanAvailableChanged(false);
    }

    if (!wwan && m_wwanAvailable) {
        m_wwanDevicePath.clear();
        Q_EMIT wwanDevicePathChanged(QString());
        m_wwanAvailable = false;
        Q_EMIT wwanAvailableChanged(false);
    }
}
