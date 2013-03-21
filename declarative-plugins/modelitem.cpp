/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#include "modelitem.h"

#include <QtNetworkManager/wireddevice.h>
#include <QtNetworkManager/wirelessdevice.h>
#include <QtNetworkManager/settings/802-11-wireless.h>

#include <KLocalizedString>

#include "debug.h"

ModelItem::ModelItem(QObject * parent):
    QObject(parent),
    m_network(0),
    m_active(0),
    m_connection(0),
    m_device(0)
{
}

ModelItem::~ModelItem()
{
}

QString ModelItem::name() const
{
    if (m_connection) {
        return m_connection->id();
    }

    if (m_network)
        return m_network->ssid();

    return QString();
}

QString ModelItem::uuid() const
{
    if (m_connection)
        return m_connection->uuid();

    return QString();
}

NetworkManager::Settings::ConnectionSettings::ConnectionType ModelItem::type() const
{
    if (m_connection) {
        NetworkManager::Settings::ConnectionSettings settings;
        settings.fromMap(m_connection->settings());
        return settings.connectionType();
    }

    if (m_network) {
        return NetworkManager::Settings::ConnectionSettings::Wireless;
    }

    return NetworkManager::Settings::ConnectionSettings::Unknown;
}

bool ModelItem::connected() const
{
    if (m_active) {
        if (m_active->state() == NetworkManager::ActiveConnection::Activated) {
            return true;
        }
    }

    return false;
}

bool ModelItem::connecting() const
{
    if (m_active) {
        if (m_active->state() == NetworkManager::ActiveConnection::Activating) {
            return true;
        }
    }

    return false;
}

QString ModelItem::ssid() const
{
    if (m_network)
        return m_network->ssid();

    if (m_connection) {
        NetworkManager::Settings::ConnectionSettings settings;
        settings.fromMap(m_connection->settings());

        if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
            NetworkManager::Settings::WirelessSetting * wirelessSetting = static_cast<NetworkManager::Settings::WirelessSetting*>(settings.setting(NetworkManager::Settings::Setting::Wireless));
            return wirelessSetting->ssid();
        }
    }

    return QString();
}

int ModelItem::signal() const
{
    if (m_network)
        return m_network->signalStrength();

    return 0;
}

bool ModelItem::secure() const
{
    if (m_network) {
        NetworkManager::WirelessDevice * wifiDev = qobject_cast<NetworkManager::WirelessDevice*>(m_device);
        NetworkManager::AccessPoint * ap = wifiDev->findAccessPoint(m_network->referenceAccessPoint());

        if (ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
            return true;
        }
    }

    if (m_connection) {
        NetworkManager::Settings::ConnectionSettings settings;
        settings.fromMap(m_connection->settings());

        if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
            NetworkManager::Settings::WirelessSetting * wirelessSetting = static_cast<NetworkManager::Settings::WirelessSetting*>(settings.setting(NetworkManager::Settings::Setting::Wireless));
            if (!wirelessSetting->security().isEmpty()) {
                return true;
            }
        }
    }

    return false;
}

QString ModelItem::detailInformations() const
{
    QString info = "<table>";

    if (m_connection) {
        NetworkManager::Settings::ConnectionSettings settings;
        settings.fromMap(m_connection->settings());

        info += QString("<tr>");
        info += QString("<td align=right><b>") + i18nc("type of network device", "Type:") + QString("</b></td>");
        info += QString("<td align=left>") + settings.typeAsString(settings.connectionType()) + QString("</td>");
        info += QString("</tr>");
    }

    if (m_device && connected()) {
        if (device()->ipV4Config().isValid()) {
            QHostAddress addr(device()->ipV4Config().addresses().first().address());

            info += QString("<tr>");
            info += QString("<td align=right><b>") + i18n("IPv4 Address:") + QString("</b></td>");
            info += QString("<td align=left>") + addr.toString() + QString("</td>");
            info += QString("</tr>");
        }

        if (device()->ipV6Config().isValid()) {
            QHostAddress addr(device()->ipV6Config().addresses().first().address());

            info += QString("<tr>");
            info += QString("<td align=right><b>") + i18n("IPv6 Address:") + QString("</b></td>");
            info += QString("<td align=left>") + addr.toString() + QString("</td>");
            info += QString("</tr>");
        }

        if (device()->type() == NetworkManager::Device::Ethernet) {
            NetworkManager::WiredDevice * wired = qobject_cast<NetworkManager::WiredDevice*>(device());

            info += QString("<tr>");
            info += QString("<td align=right><b>") + i18n("Connection speed:") + QString("</b></td>");
            info += QString("<td align=left>") + i18n("%1 MBit/s", wired->bitRate()) + QString("</td>");
            info += QString("</tr>");

            info += QString("<tr>");
            info += QString("<td align=right><b>") + i18n("MAC Address:") + QString("</b></td>");
            info += QString("<td align=left>") + wired->permanentHardwareAddress() + QString("</td>");
            info += QString("</tr>");

        } else if (device()->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice * wireless = qobject_cast<NetworkManager::WirelessDevice*>(device());

            info += QString("<tr>");
            info += QString("<td align=right><b>") + i18n("Connection speed:") + QString("</b></td>");
            info += QString("<td align=left>") + i18n("%1 Mb/s", wireless->bitRate()/1000) + QString("</td>");
            info += QString("</tr>");

            info += QString("<tr>");
            info += QString("<td align=right><b>") + i18n("MAC Address:") + QString("</b></td>");
            info += QString("<td align=left>") + wireless->permanentHardwareAddress() + QString("</td>");
            info += QString("</tr>");
        }

        info += QString("<tr>");
        info += QString("<td align=right><b>") + i18nc("name assigned by system", "System name:") + QString("</b></td>");
        if (device()->ipInterfaceName().isEmpty()) {
            info += QString("<td align=left>") + device()->interfaceName() + QString("</td>");
        } else {
            info += QString("<td align=left>") + device()->ipInterfaceName() + QString("</td>");
        }
        info += QString("</tr>");

        info += QString("<tr>");
        info += QString("<td align=right><b>") + i18nc("network device driver", "Driver:") + QString("</b></td>");
        info += QString("<td align=left>") + device()->driver() + QString("</td>");
        info += QString("</tr>");
    }

    if (m_network) {
        info += QString("<tr>");
        info += QString("<td align=right><b>") + i18n("Access point (SSID):") + QString("</b></td>");
        info += QString("<td align=left>") + m_network->ssid() + QString("</td>");
        info += QString("</tr>");

        NetworkManager::WirelessDevice * wifiDev = qobject_cast<NetworkManager::WirelessDevice*>(m_device);
        NetworkManager::AccessPoint * ap = wifiDev->findAccessPoint(m_network->referenceAccessPoint());

        info += QString("<tr>");
        info += QString("<td align=right><b>") + i18n("Access point (BSSID):") + QString("</b></td>");
        info += QString("<td align=left>") + ap->hardwareAddress() + QString("</td>");
        info += QString("</tr>");

        info += QString("<tr>");
        info += QString("<td align=right><b>") + i18nc("Wifi AP frequency", "Frequency:") + QString("</b></td>");
        info += QString("<td align=left>") + i18n("%1 Mhz", ap->frequency()) + QString("</td>");
        info += QString("</tr>");
    }

    info += QString("</table>");

    return info;
}

void ModelItem::setWirelessNetwork(NetworkManager::WirelessNetwork * network)
{
    // Just for sure disconnect the previous one, if exists
    if (m_network) {
        disconnect(m_network, SIGNAL(signalStrengthChanged(int)));
        disconnect(m_network, SIGNAL(referenceAccessPointChanged(QString)));
    }

    m_network = network;

    if (m_network) {
        connect(m_network, SIGNAL(signalStrengthChanged(int)), SLOT(onSignalStrengthChanged(int)));
        connect(m_network, SIGNAL(referenceAccessPointChanged(QString)), SLOT(onAccessPointChanged(QString)));
    }
}

NetworkManager::WirelessNetwork* ModelItem::wirelessNetwork() const
{
    return m_network;
}

void ModelItem::setActiveConnection(NetworkManager::ActiveConnection* active)
{
    // Just for sure disconnect the previous one, if exists
    if (m_active) {
        disconnect(m_active, SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)));
    }

    m_active = active;

    if (m_active) {
        connect(m_active, SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State)));
    }
}

NetworkManager::ActiveConnection* ModelItem::activeConnection() const
{
    return m_active;
}

void ModelItem::setDevice(NetworkManager::Device* device)
{
    m_device = device;
}

NetworkManager::Device* ModelItem::device() const
{
    return m_device;
}

void ModelItem::setConnection(NetworkManager::Settings::Connection* connection)
{
    // Just for sure disconnect the previous one, if exists
    if (m_connection) {
        disconnect(m_connection, SIGNAL(updated(QVariantMapMap)));
    }

    m_connection = connection;

    if (m_connection) {
        NetworkManager::Settings::ConnectionSettings settings;
        settings.fromMap(connection->settings());

        connect(connection, SIGNAL(updated(QVariantMapMap)), SLOT(onConnectionUpdated(QVariantMapMap)));
    }
}

NetworkManager::Settings::Connection* ModelItem::connection() const
{
    return m_connection;
}

QString ModelItem::connectionPath() const
{
    if (m_connection) {
        return m_connection->path();
    }

    return QString();
}

QString ModelItem::devicePath() const
{
    if (m_device) {
        return m_device->uni();
    }

    return QString();
}

QString ModelItem::specificPath() const
{
    if (m_network) {
        return m_network->referenceAccessPoint();
    }

    return QString();
}

void ModelItem::onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    if (state == NetworkManager::ActiveConnection::Deactivated) {
        NMItemDebug() << name() << ": has been disconnected";
        m_active = 0;
    }

    NMItemDebug() << name() << ": state has been changed to " << state;
    emit stateChanged();
}

void ModelItem::onConnectionUpdated(const QVariantMapMap& map)
{
    Q_UNUSED(map);

    emit connectionChanged();

    NMItemDebug() << name() << ": connection changed";
}

void ModelItem::onSignalStrengthChanged(int strength)
{
    emit signalChanged();

    NMItemDebug() << name() << ": strength changed to " << strength;
}

void ModelItem::onAccessPointChanged(const QString& accessPoint)
{
    emit accessPointChanged();

    NMItemDebug() << name() << ": access point changed to " << accessPoint;
}

