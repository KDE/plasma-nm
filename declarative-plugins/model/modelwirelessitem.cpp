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

#include <QtNetworkManager/wirelessdevice.h>
#include <QtNetworkManager/settings/802-11-wireless.h>

#include <KLocalizedString>

#include "debug.h"

ModelWirelessItem::ModelWirelessItem(const NetworkManager::Device::Ptr &device, QObject* parent):
    ModelItem(device, parent),
    m_network(0),
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
        return "network-wireless-connected-0";
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

QString ModelWirelessItem::specificPath() const
{
    if (m_network) {
        return m_network->referenceAccessPoint();
    }

    return QString();
}

void ModelWirelessItem::updateDetailsContent()
{
    ModelItem::updateDetailsContent();

    QString format = "<tr><td align=\"right\"><b>%1</b></td><td align=\"left\">&nbsp;%2</td></tr>";

    if (m_device) {
        NetworkManager::WirelessDevice::Ptr wireless = device().objectCast<NetworkManager::WirelessDevice>();
        if (connected()) {
            if (wireless->bitRate() && wireless->bitRate() < 1000) {
                m_details += QString(format).arg(i18n("Connection speed:"), i18n("%1 Kb/s", wireless->bitRate()));
            } else if (wireless->bitRate()) {
                m_details += QString(format).arg(i18n("Connection speed:"), i18n("%1 Mb/s", wireless->bitRate()/1000));
            }
        }
        m_details += QString(format).arg(i18n("MAC Address:"), wireless->permanentHardwareAddress());
    }

    if (m_network) {
        NetworkManager::WirelessDevice::Ptr wifiDev = m_device.objectCast<NetworkManager::WirelessDevice>();
        NetworkManager::AccessPoint::Ptr ap = wifiDev.data()->findAccessPoint(m_network->referenceAccessPoint());

        m_details += QString(format).arg(i18n("Signal strength:"), i18n("%1%").arg(m_network->signalStrength()));
        m_details += QString(format).arg(i18n("Access point (SSID):"), m_network->ssid());
        m_details += QString(format).arg(i18n("Access point (BSSID):"), ap->hardwareAddress());
        m_details += QString(format).arg(i18nc("Wifi AP frequency", "Frequency:"), i18n("%1 Mhz", ap->frequency()));
    }
}

void ModelWirelessItem::setConnection(NetworkManager::Settings::Connection* connection)
{
    ModelItem::setConnection(connection);

    if (!m_connection) {
        if (m_network) {
            m_name = m_network->ssid();
        } else {
            m_ssid.clear();
            m_secure = false;
        }
    }
}

void ModelWirelessItem::setConnectionSettings(const QVariantMapMap& map)
{
    ModelItem::setConnectionSettings(map);

    NetworkManager::Settings::ConnectionSettings settings;
    settings.fromMap(map);

    if (settings.connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless) {
        NetworkManager::Settings::WirelessSetting * wirelessSetting = static_cast<NetworkManager::Settings::WirelessSetting*>(settings.setting(NetworkManager::Settings::Setting::Wireless));
        m_ssid = wirelessSetting->ssid();
        if (!wirelessSetting->security().isEmpty()) {
            m_secure = true;
        }
    }

    updateDetails();
}

void ModelWirelessItem::setWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr &network)
{
    m_network = network;

    if (m_network) {
        m_ssid = network->ssid();
        m_previousSignal = m_signal;
        m_signal = m_network->signalStrength();
        m_type = NetworkManager::Settings::ConnectionSettings::Wireless;

        if (m_device) {
            NetworkManager::WirelessDevice::Ptr wifiDev = m_device.objectCast<NetworkManager::WirelessDevice>();
            NetworkManager::AccessPoint::Ptr ap = wifiDev.data()->findAccessPoint(m_network->referenceAccessPoint());

            if (ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
                m_secure = true;
            }
        }

        if (!m_connection) {
            m_name = ssid();
        }

        connect(m_network.data(), SIGNAL(signalStrengthChanged(int)),
                SLOT(onSignalStrengthChanged(int)), Qt::UniqueConnection);
        connect(m_network.data(), SIGNAL(referenceAccessPointChanged(QString)),
                SLOT(onAccessPointChanged(QString)), Qt::UniqueConnection);
    } else {
        m_ssid.clear();
        m_signal = 0;
        m_type = NetworkManager::Settings::ConnectionSettings::Unknown;
        m_secure = false;
    }

    updateDetails();
}

NetworkManager::WirelessNetwork::Ptr ModelWirelessItem::wirelessNetwork() const
{
    if (m_network) {
        return m_network;
    }

    return NetworkManager::WirelessNetwork::Ptr();
}

void ModelWirelessItem::onAccessPointChanged(const QString& accessPoint)
{
    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": access point changed to " << accessPoint;
}

void ModelWirelessItem::onSignalStrengthChanged(int strength)
{
    m_previousSignal = m_signal;
    m_signal = strength;

    updateDetails();

    emit itemChanged();

    NMItemDebug() << name() << ": strength changed to " << strength;
}
