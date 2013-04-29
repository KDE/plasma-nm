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

#include <NetworkManagerQt/manager.h>
#include <NetworkManagerQt/accesspoint.h>
#include <NetworkManagerQt/wireddevice.h>
#include <NetworkManagerQt/wirelessdevice.h>
#include <NetworkManagerQt/settings.h>
#include <NetworkManagerQt/settings/setting.h>
#include <NetworkManagerQt/settings/connection.h>
#include <NetworkManagerQt/settings/802-3-ethernet.h>
#include <NetworkManagerQt/settings/802-11-wireless.h>
#include <NetworkManagerQt/activeconnection.h>

#include <QInputDialog>

#include <KProcess>

Handler::Handler(QObject* parent):
    QObject(parent)
{
}

Handler::~Handler()
{
}

void Handler::activateConnection(const QString& connection, const QVariant& devices, const QString& specificObject)
{
    //TODO: implement some GUI for selecting devices
//     QString selectedDevicePath = QInputDialog::getItem(0, "Device select", "Choose device for this connection", devices.toStringList(), 0, false);
    QStringList availableDevicesPaths = devices.toStringList();
    QString selectedDevicePath;
    NetworkManager::Settings::Connection::Ptr con = NetworkManager::Settings::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to activate this connection";
        return;
    }

    NetworkManager::Settings::ConnectionSettings::Ptr connectionSettings = con->settings();
    if (connectionSettings->connectionType() == NetworkManager::Settings::ConnectionSettings::Wired && availableDevicesPaths.count() > 1) {
        NetworkManager::Settings::WiredSetting::Ptr wiredSetting = connectionSettings->setting(NetworkManager::Settings::Setting::Wired).staticCast<NetworkManager::Settings::WiredSetting>();
        QString macAddress = UiUtils::macAddressAsString(wiredSetting->macAddress());
        if (!macAddress.isEmpty()) {
            foreach (const QString & devicePath, availableDevicesPaths) {
                NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(devicePath);
                if (device && device->type() == NetworkManager::Device::Wifi) {
                    NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
                    if (wiredDevice && wiredDevice->permanentHardwareAddress() == macAddress) {
                        selectedDevicePath = devicePath;
                    }
                }
            }
        }
    } else if (connectionSettings->connectionType() == NetworkManager::Settings::ConnectionSettings::Wireless && availableDevicesPaths.count() > 1) {
        NetworkManager::Settings::WirelessSetting::Ptr wirelessSetting = connectionSettings->setting(NetworkManager::Settings::Setting::Wireless).staticCast<NetworkManager::Settings::WirelessSetting>();
        QString macAddress = UiUtils::macAddressAsString(wirelessSetting->macAddress());
        if (!macAddress.isEmpty()) {
            foreach (const QString & devicePath, availableDevicesPaths) {
                NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(devicePath);
                if (device && device->type() == NetworkManager::Device::Wifi) {
                    NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
                    if (wifiDevice && wifiDevice->permanentHardwareAddress() == macAddress) {
                        selectedDevicePath = devicePath;
                    }
                }
            }
        }
    }

    if (!availableDevicesPaths.isEmpty() && selectedDevicePath.isEmpty()) {
        selectedDevicePath = availableDevicesPaths.first();
    }

    NetworkManager::Device::Ptr selectedDevice = NetworkManager::findNetworkInterface(selectedDevicePath);

    if (!selectedDevice) {
        NMHandlerDebug() << "Not possible to activate this connection";
        return;
    }

    NMHandlerDebug() << "Activating " << con->name() << " connection";

    if (selectedDevice && selectedDevice->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = selectedDevice.objectCast<NetworkManager::WirelessDevice>();

        if (wirelessDevice) {
            NetworkManager::WirelessNetwork::Ptr network = wirelessDevice->findNetwork(specificObject);

            if (network) {
                NetworkManager::activateConnection(connection, selectedDevicePath, network->referenceAccessPoint()->uni());
            } else {
                NetworkManager::activateConnection(connection, selectedDevicePath, QString());
            }
        }
    } else {
        NetworkManager::activateConnection(connection, selectedDevicePath, specificObject);
    }
}

void Handler::addAndActivateConnection(const QVariant& devices, const QString& specificObject)
{
    //TODO: implement some GUI for selecting devices
//     QString selectedDevicePath = QInputDialog::getItem(0, "Device select", "Choose device for this connection", devices.toStringList(), 0, false);
    QStringList availableDevicesPaths = devices.toStringList();
    QString selectedDevicePath;
    if (!availableDevicesPaths.isEmpty()) {
        selectedDevicePath = availableDevicesPaths.first();
    }

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(selectedDevicePath);

    if (device && device->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();

        if (wirelessDevice) {
            NetworkManager::WirelessNetwork::Ptr network = wirelessDevice->findNetwork(specificObject);

            if (network) {
                NetworkManager::Settings::ConnectionSettings * settings = new NetworkManager::Settings::ConnectionSettings(NetworkManager::Settings::ConnectionSettings::Wireless);
                settings->setId(specificObject);
                settings->setUuid(NetworkManager::Settings::ConnectionSettings::createNewUuid());

                NetworkManager::Settings::WirelessSetting::Ptr wifiSetting;
                wifiSetting = settings->setting(NetworkManager::Settings::Setting::Wireless).dynamicCast<NetworkManager::Settings::WirelessSetting>();
                wifiSetting->setSsid(specificObject.toUtf8());

                NetworkManager::addAndActivateConnection(settings->toMap(), selectedDevicePath, network->referenceAccessPoint()->uni());
            }
        }
    }
}

void Handler::deactivateConnection(const QString& connection)
{
    NetworkManager::Settings::Connection::Ptr con = NetworkManager::Settings::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to deactivate this connection";
        return;
    }

    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid()) {
            if (active->vpn()) {
                NetworkManager::deactivateConnection(active->path());
            } else {
                NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
                if (device) {
                    device->disconnectInterface();
                }
            }
        }
    }

    NMHandlerDebug() << "Deactivating " << con->name() << " connection";
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
    NetworkManager::Settings::Connection::Ptr con = NetworkManager::Settings::findConnection(connection);

    if (!con) {
        NMHandlerDebug() << "Not possible to remove this connection";
        return;
    }

    con->remove();
}

void Handler::openEditor()
{
    KProcess::startDetached("kde-nm-connection-editor");
}
