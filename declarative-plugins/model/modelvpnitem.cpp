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

#include "model/modelvpnitem.h"

#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/settings/vpn.h>

#include <KLocalizedString>

#include "debug.h"

ModelVpnItem::ModelVpnItem(const NetworkManager::Device::Ptr &device, QObject* parent):
    ModelItem(device, parent),
    m_vpn(0)
{
    m_type = NetworkManager::Settings::ConnectionSettings::Vpn;
}

ModelVpnItem::~ModelVpnItem()
{
}

void ModelVpnItem::updateDetailsContent()
{
    ModelItem::updateDetailsContent();

    QString format = "<tr><td align=\"right\"><b>%1</b></td><td align=\"left\">&nbsp;%2</td></tr>";

    if (m_connection) {
        NetworkManager::Settings::ConnectionSettings settings;
        settings.fromMap(m_connection->settings());
        NetworkManager::Settings::VpnSetting::Ptr vpnSetting;
        vpnSetting = settings.setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
        if (vpnSetting) {
            m_details += QString(format).arg(i18n("VPN plugin:"), vpnSetting->serviceType().section('.', -1));
        }
    }
}

void ModelVpnItem::setActiveConnection(const NetworkManager::ActiveConnection::Ptr & active)
{
    ModelItem::setActiveConnection(active);

    if (m_active.data()->vpn()) {
        m_vpn =  NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(m_active->path()));

        connect(m_vpn.data(), SIGNAL(stateChanged(NetworkManager::VpnConnection::State)),
                SLOT(onVpnConnectionStateChanged(NetworkManager::VpnConnection::State)), Qt::UniqueConnection);
    }
}

void ModelVpnItem::onVpnConnectionStateChanged(NetworkManager::VpnConnection::State state)
{
    if (state == NetworkManager::VpnConnection::Disconnected) {
        m_connecting = false;
        m_connected = false;
        m_vpn.clear();
        NMItemDebug() << name() << ": disconnected";
        m_active.clear();
    }
}
