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

#include <NetworkManagerQt/settings.h>
#include <NetworkManagerQt/settings/connection.h>
#include <NetworkManagerQt/settings/vpn.h>

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

void ModelVpnItem::updateDetails()
{
    QString format = "<tr><td align=\"right\" width=\"50%\"><b>%1</b></td><td align=\"left\" width=\"50%\">&nbsp;%2</td></tr>";
    m_details = "<qt><table>";

    // Initialize objects
    NetworkManager::Settings::Connection::Ptr connection = NetworkManager::Settings::findConnection(m_connectionPath);
    NetworkManager::Settings::ConnectionSettings::Ptr connectionSettings;
    NetworkManager::Settings::VpnSetting::Ptr vpnSetting;
    if (connection) {
        connectionSettings = connection->settings();
    }
    if (connectionSettings) {
        vpnSetting = connectionSettings->setting(NetworkManager::Settings::Setting::Vpn).dynamicCast<NetworkManager::Settings::VpnSetting>();
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
        } else if (key == "vpn:plugin") {
            if (vpnSetting) {
                m_details += QString(format).arg(i18n("VPN plugin:"), vpnSetting->serviceType().section('.', -1));
            }
        } else if (key == "vpn:banner") {
            if (m_vpn) {
                m_details += QString(format).arg(i18n("Banner:"), m_vpn->banner().simplified());
            }
        }
    }

    m_details += "</table></qt>";

    Q_EMIT itemChanged();
}

void ModelVpnItem::setActiveConnection(const NetworkManager::ActiveConnection::Ptr & active)
{
    ModelItem::setActiveConnection(active);

    if (m_active->vpn()) {
        m_vpn = NetworkManager::VpnConnection::Ptr(new NetworkManager::VpnConnection(m_active->path()));

        connect(m_vpn.data(), SIGNAL(stateChanged(NetworkManager::VpnConnection::State)),
                SLOT(onVpnConnectionStateChanged(NetworkManager::VpnConnection::State)), Qt::UniqueConnection);
        connect(m_vpn.data(), SIGNAL(bannerChanged(QString)),
                SLOT(onBannerChanged(QString)), Qt::UniqueConnection);
    }

    updateDetails();
}

void ModelVpnItem::onVpnConnectionStateChanged(NetworkManager::VpnConnection::State state)
{
    if (state == NetworkManager::VpnConnection::Disconnected ||
        state == NetworkManager::VpnConnection::Failed) {
        m_connecting = false;
        m_connected = false;
        m_vpn.clear();
        NMItemDebug() << name() << ": disconnected";
        m_active.clear();
    }

    updateDetails();
}

void ModelVpnItem::onBannerChanged(const QString &banner)
{
    qDebug() << "VPN banner changed" << banner;

    updateDetails();
}
