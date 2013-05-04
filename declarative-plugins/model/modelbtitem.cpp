/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>
    Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>

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

#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/settings/Bluetooth>
#include <NetworkManagerQt/Manager>

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

void ModelBtItem::updateDetails()
{
    QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    m_details = "<qt><table>";

    // Initialize objects
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(m_devicePath);
    NetworkManager::BluetoothDevice::Ptr bt;
    if (device) {
        bt = device.objectCast<NetworkManager::BluetoothDevice>();
    }

    foreach (const QString & key, m_detailKeys) {
        if (key == "interface:type") {
            if (m_type != NetworkManager::Settings::ConnectionSettings::Unknown) {
                m_details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::Settings::ConnectionSettings::typeAsString(m_type));
            }
        } else if (key == "interface:status") {
            QString status = i18n("Disconnected");
            if (m_connecting) {
                status = i18n("Connecting");
            } else if (m_connected) {
                status = i18n("Connected");
            }
            m_details += QString(format).arg(i18n("Status"), status);
        } else if (key == "interface:name") {
            if (device) {
                QString name = device->ipInterfaceName();
                if (!name.isEmpty()) {
                    m_details += QString(format).arg(i18n("System name:"), name);
                }
            }
        } else if (key == "ipv4:address") {
            if (device && device->ipV4Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV4Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
            }
        } else if (key == "ipv4:gateway") {
            if (device && device->ipV4Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV4Config().addresses().first().gateway();
                m_details += QString(format).arg(i18n("IPv4 Gateway:"), addr.toString());
            }
        } else if (key == "ipv6:address") {
            if (device && device->ipV6Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV6Config().addresses().first().ip();
                m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
            }
        } else if (key == "ipv6:gateway") {
            if (device && device->ipV4Config().isValid() && m_connected) {
                QHostAddress addr = device->ipV6Config().addresses().first().gateway();
                m_details += QString(format).arg(i18n("IPv6 Gateway:"), addr.toString());
            }
        } else if (key == "bluetooth:name") {
            if (bt) {
                m_details += QString(format).arg(i18n("Name:"), bt->name());
            }
        } else if (key == "interface:hardwareAddress") {
            if (bt) {
                m_details += QString(format).arg(i18n("MAC Address:"), bt->hardwareAddress());
            }
        } else if (key == "interface:driver") {
            if (bt) {
                m_details += QString(format).arg(i18n("Driver:"), device->driver());
            }
        }
    }

    m_details += "</table></qt>";

    Q_EMIT itemChanged();
}
