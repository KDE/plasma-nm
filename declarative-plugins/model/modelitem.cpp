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
#include <QtNetworkManager/manager.h>

#include "debug.h"

ModelItem::ModelItem(const NetworkManager::Device::Ptr &device, QObject * parent):
    QObject(parent),
    m_active(0),
    m_connection(0),
    m_device(0),
    m_connected(false),
    m_connecting(false),
    m_sectionType(ModelItem::Unknown),
    m_type(NetworkManager::Settings::ConnectionSettings::Unknown)
{
    setDevice(device);
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

QString ModelItem::deviceUdi() const
{
    return m_deviceUdi;
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
            return "bluetooth";
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
            return "network-wired";
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

NetworkManager::Settings::ConnectionSettings::ConnectionType ModelItem::type() const
{
    return m_type;
}

void ModelItem::updateDetailsContent()
{
    QString format = "<tr><td align=\"right\"><b>%1</b></td><td align=\"left\">&nbsp;%2</td></tr>";

    if (m_type != NetworkManager::Settings::ConnectionSettings::Unknown) {
        m_details += QString(format).arg(i18nc("type of network device", "Type:"), NetworkManager::Settings::ConnectionSettings::typeAsString(m_type));
    }

    NetworkManager::Device::Ptr device = NetworkManager::findDeviceByIpFace(m_deviceUdi);

    if (device) {
        qDebug() << "DEVICE FOUND";
        if (device->ipV4Config().isValid() && connected()) {
            QHostAddress addr = device->ipV4Config().addresses().first().ip();
            m_details += QString(format).arg(i18n("IPv4 Address:"), addr.toString());
        }

        if (device->ipV6Config().isValid() && connected()) {
            QHostAddress addr = device->ipV6Config().addresses().first().ip();
            m_details += QString(format).arg(i18n("IPv6 Address:"), addr.toString());
        }

        QString name;
        if (device->ipInterfaceName().isEmpty()) {
            name = device->interfaceName();
        } else {
            name = device->ipInterfaceName();
        }
        m_details += QString(format).arg(i18n("System name:"), name);
    } else {
        qDebug() << "NO DEVICE FOUND";
    }
}

void ModelItem::updateDetails()
{
    m_details = "<qt><table>";
    updateDetailsContent();
    m_details += "<qt></table>";
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
        qDebug() << "active";
        if (m_active.data()->state() == NetworkManager::ActiveConnection::Activating) {
            m_connecting = true;
            m_connected = false;
            NMItemDebug() << name() << ": activating";
        } else if (m_active.data()->state() == NetworkManager::ActiveConnection::Activated) {
            NMItemDebug() << name() << ": activated";
            m_connected = true;
            m_connecting = false;

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

void ModelItem::setDevice(const NetworkManager::Device::Ptr & device)
{
    m_device = device;

    if (m_device) {
        m_deviceUdi = m_device->udi();
    } else {
        m_deviceUdi.clear();
    }

    updateDetails();
}

NetworkManager::Device::Ptr ModelItem::device() const
{
    return m_device;
}

void ModelItem::setConnection(const NetworkManager::Settings::Connection::Ptr & connection)
{
    m_connection = connection;

    if (m_connection) {
        setConnectionSettings(connection->settings());

        connect(connection.data(), SIGNAL(updated(QVariantMapMap)),
                SLOT(onConnectionUpdated(QVariantMapMap)), Qt::UniqueConnection);
    } else {
        m_name.clear();
        m_uuid.clear();
        m_active.clear();
    }
}

void ModelItem::setConnectionSettings(const QVariantMapMap& map)
{
    NetworkManager::Settings::ConnectionSettings settings;
    settings.fromMap(map);

    m_uuid = settings.uuid();
    m_name = settings.id();
    m_type = settings.connectionType();

    updateDetails();
}

NetworkManager::Settings::Connection::Ptr ModelItem::connection() const
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
    emit itemChanged();
}

void ModelItem::onConnectionUpdated(const QVariantMapMap& map)
{
    setConnectionSettings(map);

    emit itemChanged();

    NMItemDebug() << name() << ": connection changed";
}

void ModelItem::onDefaultRouteChanged(bool defaultRoute)
{
    Q_UNUSED(defaultRoute);

    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": default route changed";
}
