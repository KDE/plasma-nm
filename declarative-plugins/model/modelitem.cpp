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

#include "modelitem.h"

#include <KLocalizedString>
#include <NetworkManagerQt/manager.h>

#include "debug.h"

ModelItem::ModelItem(const NetworkManager::Device::Ptr &device, QObject * parent):
    QObject(parent),
    m_active(0),
    m_connection(0),
    m_connected(false),
    m_connecting(false),
    m_sectionType(ModelItem::Unknown),
    m_type(NetworkManager::Settings::ConnectionSettings::Unknown)
{
    if (device) {
        addDevice(device->uni());
    }
}

ModelItem::~ModelItem()
{
}

bool ModelItem::connected() const
{
    return m_connected;
}

bool ModelItem::connecting() const
{
    return m_connecting;
}

QString ModelItem::detailInformations() const
{
    return m_details;
}

QString ModelItem::icon() const
{
    switch (m_type) {
        case NetworkManager::Settings::ConnectionSettings::Adsl:
            return "modem";
            break;
        case NetworkManager::Settings::ConnectionSettings::Bluetooth:
            return "preferences-system-bluetooth";
            break;
        case NetworkManager::Settings::ConnectionSettings::Bond:
            break;
        case NetworkManager::Settings::ConnectionSettings::Bridge:
            break;
        case NetworkManager::Settings::ConnectionSettings::Cdma:
            return "phone";
            break;
        case NetworkManager::Settings::ConnectionSettings::Gsm:
            return "phone";
            break;
        case NetworkManager::Settings::ConnectionSettings::Infiniband:
            break;
        case NetworkManager::Settings::ConnectionSettings::OLPCMesh:
            break;
        case NetworkManager::Settings::ConnectionSettings::Pppoe:
            return "modem";
            break;
        case NetworkManager::Settings::ConnectionSettings::Vlan:
            break;
        case NetworkManager::Settings::ConnectionSettings::Vpn:
            return "secure-card";
            break;
        case NetworkManager::Settings::ConnectionSettings::Wimax:
            break;
        case NetworkManager::Settings::ConnectionSettings::Wired:
            if (connected()) {
                return "network-wired-activated";
            } else {
                return "network-wired";
            }
            break;
        default:
            return "network-wired";
            break;
    }

    return "network-wired";
}

QString ModelItem::name() const
{
    return m_name;
}

QString ModelItem::uuid() const
{
    return m_uuid;
}

QString ModelItem::ssid() const
{
    return QString();
}

int ModelItem::signal() const
{
    return 0;
}

bool ModelItem::secure() const
{
    return false;
}

QString ModelItem::sectionType() const
{
    if (m_connected) {
        return i18n("Active connections");
    } else if (m_uuid != "") {
        return i18n("Previous connections");
    } else {
        return i18n("Unknown connections");
    }
}

bool ModelItem::shouldBeRemoved() const
{
    if (m_devicePaths.isEmpty()) {
        return true;
    }

    if (m_connectionPath.isEmpty()) {
        return shouldBeRemovedSpecific();
    }

    return false;
}

bool ModelItem::shouldBeRemovedSpecific() const
{
    // No specific condition
    return true;
}

NetworkManager::Settings::ConnectionSettings::ConnectionType ModelItem::type() const
{
    return m_type;
}

void ModelItem::updateDetails()
{
}

bool ModelItem::operator==(ModelItem* item)
{
    if (((item->uuid() == this->uuid()) && !item->name().isEmpty() && !this->name().isEmpty()) ||
        item->name() == this->name()) {
        return true;
    }

    return false;
}

void ModelItem::setActiveConnection(const NetworkManager::ActiveConnection::Ptr & active)
{
    m_active = active;

    if (m_active) {
        if (m_active->state() == NetworkManager::ActiveConnection::Activating) {
            m_connecting = true;
            m_connected = false;
            NMItemDebug() << name() << ": activating";
        } else if (m_active->state() == NetworkManager::ActiveConnection::Activated) {
            NMItemDebug() << name() << ": activated";
            m_connected = true;
            m_connecting = false;
        }

        NetworkManager::Device::Ptr activeDevice = NetworkManager::findNetworkInterface(m_active->devices().first());

        if (activeDevice) {
            m_activeDevicePath = activeDevice->uni();
        }

        connect(m_active.data(), SIGNAL(default4Changed(bool)),
                SLOT(onDefaultRouteChanged(bool)), Qt::UniqueConnection);
        connect(m_active.data(), SIGNAL(default6Changed(bool)),
                SLOT(onDefaultRouteChanged(bool)), Qt::UniqueConnection);
        connect(m_active.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State)), Qt::UniqueConnection);
    } else {
        m_connecting = false;
        m_connected = false;
    }

    updateDetails();
}

NetworkManager::ActiveConnection::Ptr ModelItem::activeConnection() const
{
    return m_active;
}

void ModelItem::addDevice(const QString & device)
{
    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);

    if (dev && !m_devicePaths.contains(dev->uni())) {
        m_devicePaths << dev->uni();
        addSpecificDevice(dev);
        updateDetails();
    }
}

void ModelItem::addSpecificDevice(const NetworkManager::Device::Ptr& device)
{
    Q_UNUSED(device);
}

void ModelItem::removeDevice(const QString& device)
{
    m_devicePaths.removeOne(device);

    updateDetails();
}

QString ModelItem::activeDevicePath() const
{
    return m_activeDevicePath;
}

void ModelItem::setConnection(const NetworkManager::Settings::Connection::Ptr & connection)
{
    m_connection = connection;

    if (m_connection) {
        setConnectionSettings(connection->settings());

        m_connectionPath = m_connection->path();

        connect(connection.data(), SIGNAL(updated()),
                SLOT(onConnectionUpdated()), Qt::UniqueConnection);
    } else {
        m_connectionPath.clear();
        m_name.clear();
        m_uuid.clear();
        m_active.clear();
    }
}

void ModelItem::setConnectionSettings(const NetworkManager::Settings::ConnectionSettings::Ptr &settings)
{
    m_uuid = settings->uuid();
    m_name = settings->id();
    m_type = settings->connectionType();

    updateDetails();
}

NetworkManager::Settings::Connection::Ptr ModelItem::connection() const
{
    return m_connection;
}

QString ModelItem::connectionPath() const
{
    return m_connectionPath;
}

QStringList ModelItem::devicePaths() const
{
    return m_devicePaths;
}

QString ModelItem::specificParameter() const
{
    return QString();
}

void ModelItem::onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    if (state == NetworkManager::ActiveConnection::Deactivated ||
        state == NetworkManager::ActiveConnection::Deactivating) {
        NMItemDebug() << name() << ": disconnected";
        m_active.clear();
        m_connecting = false;
        m_connected = false;
    } else if (state == NetworkManager::ActiveConnection::Activated) {
        NMItemDebug() << name() << ": activated";
        m_connecting = false;
        m_connected = true;
    } else if (state == NetworkManager::ActiveConnection::Activating) {
        NMItemDebug() << name() << ": activating";
        m_connecting = true;
        m_connected = false;
    }

    updateDetails();

    NMItemDebug() << name() << ": state has been changed to " << state;
}

void ModelItem::onConnectionUpdated()
{
    NetworkManager::Settings::Connection *connection = qobject_cast<NetworkManager::Settings::Connection*>(sender());
    if (connection) {
        setConnectionSettings(connection->settings());

        NMItemDebug() << name() << ": connection changed";
    }
}

void ModelItem::onDefaultRouteChanged(bool defaultRoute)
{
    Q_UNUSED(defaultRoute);

    updateDetails();

    NMItemDebug() << name() << ": default route changed";
}
