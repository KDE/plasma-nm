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

#include "modelbtitem.h"

#include <NetworkManagerQt/bluetoothdevice.h>
#include <NetworkManagerQt/settings/bluetooth.h>
#include <NetworkManagerQt/manager.h>

#include <KLocalizedString>

#include "debug.h"

ModelBtItem::ModelBtItem(const NetworkManager::Device::Ptr &device, QObject* parent):
    ModelItem(device, parent)
{
    m_type = NetworkManager::Settings::ConnectionSettings::Bluetooth;
}

ModelBtItem::~ModelBtItem()
{
}

void ModelBtItem::updateDetailsContent()
{
    QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";

    if (m_type != NetworkManager::Settings::ConnectionSettings::Unknown) {
        m_details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::Settings::ConnectionSettings::typeAsString(m_type));
    }

    m_details += QString(format).arg("\n", "\n");

    if (!connected()) {
        foreach (const QString & path, m_devicePaths) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);

            if (device) {
                if (device->ipV4Config().isValid() && connected()) {
                    QHostAddress addr = device->ipV4Config().addresses().first().ip();
                    m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
                }

                if (device->ipV6Config().isValid() && connected()) {
                    QHostAddress addr = device->ipV6Config().addresses().first().ip();
                    m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
                }

                NetworkManager::BluetoothDevice::Ptr bt = device.objectCast<NetworkManager::BluetoothDevice>();
                m_details += QString(format).arg(i18n("MAC Address:"), bt->hardwareAddress());
                m_details += QString(format).arg(i18n("Driver:"), device->driver());

                m_details += QString(format).arg("\n", "\n");
            }
        }
    } else {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_activeDevicePath);

        if (device) {
            NetworkManager::BluetoothDevice::Ptr bt = device.objectCast<NetworkManager::BluetoothDevice>();
            QString name = device->ipInterfaceName();
            if (!name.isEmpty()) {
                m_details += QString(format).arg(i18n("System name:"), name);
            }

            m_details += QString(format).arg(i18n("Name:"), bt->name());

            if (device->ipV4Config().isValid() && connected()) {
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }

            if (device->ipV6Config().isValid() && connected()) {
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }

            m_details += QString(format).arg(i18n("MAC Address:"), bt->hardwareAddress());
            m_details += QString(format).arg(i18n("Driver:"), device->driver());

            m_details += QString(format).arg("\n", "\n");
        }
    }
}
