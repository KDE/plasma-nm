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

#include "modelwireditem.h"

#include <NetworkManagerQt/wireddevice.h>
#include <NetworkManagerQt/settings/802-3-ethernet.h>
#include <NetworkManagerQt/manager.h>

#include <KLocalizedString>

#include "debug.h"

ModelWiredItem::ModelWiredItem(const NetworkManager::Device::Ptr &device, QObject* parent):
    ModelItem(device, parent)
{
    m_type = NetworkManager::Settings::ConnectionSettings::Wired;
}

ModelWiredItem::~ModelWiredItem()
{
}

void ModelWiredItem::updateDetailsContent()
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

                NetworkManager::WiredDevice::Ptr wired = device.objectCast<NetworkManager::WiredDevice>();
                m_details += QString(format).arg(i18n("MAC Address:"), wired->permanentHardwareAddress());

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

            NetworkManager::WiredDevice::Ptr wired = device.objectCast<NetworkManager::WiredDevice>();
            if (connected()) {
                if (wired->bitRate() < 1000000) {
                    m_details += QString(format).arg(i18n("Connection speed:"), i18n("%1 Mb/s", wired->bitRate()/1000));
                } else {
                    m_details += QString(format).arg(i18n("Connection speed:"), i18n("%1 Gb/s", wired->bitRate()/1000000));
                }
            }
            m_details += QString(format).arg(i18n("MAC Address:"), wired->permanentHardwareAddress());

            m_details += QString(format).arg("\n", "\n");
        }
    }
}
