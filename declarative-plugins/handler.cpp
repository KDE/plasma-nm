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

#include "handler.h"
#include "uiutils.h"
#include "debug.h"
#include "lib/editor/connectiondetaileditor.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Setting>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/ActiveConnection>

#include <QInputDialog>

#include <KProcess>

Handler::Handler(QObject* parent):
    QObject(parent)
{
}

Handler::~Handler()
{
}

void Handler::activateConnection(const QString& connection, const QString& device, const QString& specificObject)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to activate this connection";
        return;
    }
        NMHandlerDebug() << "Activating " << con->name() << " connection";

    NetworkManager::activateConnection(connection, device, specificObject);
}

void Handler::addAndActivateConnection(const QString& device, const QString& specificObject)
{
    NetworkManager::AccessPoint::Ptr ap;
    NetworkManager::WirelessDevice::Ptr wifiDev;
    foreach (const NetworkManager::Device::Ptr & dev, NetworkManager::networkInterfaces()) {
        if (dev->type() == NetworkManager::Device::Wifi) {
            wifiDev = dev.objectCast<NetworkManager::WirelessDevice>();
            ap = wifiDev->findAccessPoint(specificObject);
            if (ap) {
                break;
            }
        }
    }

    if (!ap) {
        return;
    }

    NetworkManager::ConnectionSettings::Ptr settings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
    settings->setId(ap->ssid());
    settings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

    NetworkManager::WirelessSetting::Ptr wifiSetting;
    wifiSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
    wifiSetting->setSsid(ap->ssid().toUtf8());

    NetworkManager::Utils::WirelessSecurityType securityType = NetworkManager::Utils::findBestWirelessSecurity(wifiDev->wirelessCapabilities(), true, (ap->mode() == NetworkManager::AccessPoint::Adhoc), ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());
    if (securityType == NetworkManager::Utils::DynamicWep ||
        securityType == NetworkManager::Utils::Wpa2Eap ||
        securityType == NetworkManager::Utils::WpaEap) {
        wifiSetting->setSecurity("802-11-wireless-security");
        NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity = settings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();
        if (securityType == NetworkManager::Utils::DynamicWep) {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
        } else {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
        }
        QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(settings, 0, 0, true);
        if (editor->exec() == QDialog::Accepted) {
            NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnectionByUuid(settings->uuid());
            if (newConnection) {
                activateConnection(newConnection->path(), device, specificObject);
            }
        }

        if (editor) {
            editor->deleteLater();
        }
    } else {
        NetworkManager::addAndActivateConnection(settings->toMap(), device, specificObject);
    }

    settings.clear();
}

void Handler::deactivateConnection(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to deactivate this connection";
        return;
    }

    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid()) {
            if (active->vpn()) {
                NetworkManager::deactivateConnection(active->path());
            } else {
                if (active->devices().isEmpty()) {
                    NetworkManager::deactivateConnection(connection);
                }
                NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
                if (device) {
                    device->disconnectInterface();
                }
            }
        }
    }

    NMHandlerDebug() << "Deactivating " << con->name() << " connection";
}

void Handler::disconnectAll()
{
    foreach (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        device->disconnectInterface();
    }
}

void Handler::enableNetworking(bool enable)
{
    NMHandlerDebug() << "Networking enabled: " << enable;
    NetworkManager::setNetworkingEnabled(enable);
}

void Handler::enableWireless(bool enable)
{
    NMHandlerDebug() << "Wireless enabled: " << enable;
    NetworkManager::setWirelessEnabled(enable);
}

void Handler::enableWimax(bool enable)
{
    NMHandlerDebug() << "Wimax enabled: " << enable;
    NetworkManager::setWimaxEnabled(enable);
}

void Handler::enableWwan(bool enable)
{
    NMHandlerDebug() << "Wwan enabled: " << enable;
    NetworkManager::setWwanEnabled(enable);
}

void Handler::editConnection(const QString& uuid)
{
    QStringList args;
    args << uuid;
    KProcess::startDetached("kde-nm-connection-editor", args);
}

void Handler::removeConnection(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to remove this connection";
        return;
    }

    foreach (const NetworkManager::Connection::Ptr &connection, NetworkManager::listConnections()) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == con->uuid()) {
            connection->remove();
        }
    }

    con->remove();
}

void Handler::openEditor()
{
    KProcess::startDetached("kde-nm-connection-editor");
}
