/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

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
#include "connectiondetaileditor.h"
#include "uiutils.h"

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
#include <QDBusError>
#include <QIcon>

#include <KNotification>
#include <KLocalizedString>
#include <KUser>
#include <KDebug>
#include <KProcess>
#include <KService>
#include <KServiceTypeTrader>
#include <KWindowSystem>
#include <KWallet/Wallet>

Handler::Handler(QObject* parent)
    : QObject(parent)
    , m_tmpBluetoothEnabled(isBtEnabled())
    , m_tmpWimaxEnabled(NetworkManager::isWimaxEnabled())
    , m_tmpWirelessEnabled(NetworkManager::isWirelessEnabled())
    , m_tmpWwanEnabled(NetworkManager::isWwanEnabled())
    , m_agentIface(QStringLiteral("org.kde.kded5"), QStringLiteral("/modules/networkmanagement"),
                   QStringLiteral("org.kde.plasmanetworkmanagement"))
{
    m_agentIface.call(QStringLiteral("init"));
}

Handler::~Handler()
{
}

void Handler::activateConnection(const QString& connection, const QString& device, const QString& specificObject)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        kWarning() << "Not possible to activate this connection";
        return;
    }

    if (con->settings()->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
        NetworkManager::VpnSetting::Ptr vpnSetting = con->settings()->setting(NetworkManager::Setting::Vpn).staticCast<NetworkManager::VpnSetting>();
        if (vpnSetting) {
            kWarning() << "Checking VPN" << con->name() << "type:" << vpnSetting->serviceType();
            // get the list of supported VPN service types
            const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin",
                                                                          QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(vpnSetting->serviceType()));
            if (services.isEmpty()) {
                kWarning() << "VPN" << vpnSetting->serviceType() << "not found, skipping";
                KNotification *notification = new KNotification("MissingVpnPlugin", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(con->name());
                notification->setText(i18n("Missing VPN plugin"));
                notification->setPixmap(QIcon::fromTheme("dialog-warning").pixmap(64, 64));
                notification->sendEvent();
                return;
            }
        }
    }

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::activateConnection(connection, device, specificObject);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", Handler::ActivateConnection);
    watcher->setProperty("connection", con->name());
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(replyFinished(QDBusPendingCallWatcher*)));
}

void Handler::addAndActivateConnection(const QString& device, const QString& specificObject, const QString& password)
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
    settings->setAutoconnect(true);
    settings->addToPermissions(KUser().loginName(), QString());

    NetworkManager::WirelessSetting::Ptr wifiSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
    wifiSetting->setInitialized(true);
    wifiSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
    wifiSetting->setSsid(ap->ssid().toUtf8());
    if (ap->mode() == NetworkManager::AccessPoint::Adhoc) {
        wifiSetting->setMode(NetworkManager::WirelessSetting::Adhoc);
    }
    NetworkManager::WirelessSecuritySetting::Ptr wifiSecurity = settings->setting(NetworkManager::Setting::WirelessSecurity).dynamicCast<NetworkManager::WirelessSecuritySetting>();

    NetworkManager::Utils::WirelessSecurityType securityType = NetworkManager::Utils::findBestWirelessSecurity(wifiDev->wirelessCapabilities(), true, (ap->mode() == NetworkManager::AccessPoint::Adhoc), ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());

    if (securityType != NetworkManager::Utils::None) {
        wifiSecurity->setInitialized(true);
        wifiSetting->setSecurity("802-11-wireless-security");
    }

    if (securityType == NetworkManager::Utils::Leap ||
        securityType == NetworkManager::Utils::DynamicWep ||
        securityType == NetworkManager::Utils::Wpa2Eap ||
        securityType == NetworkManager::Utils::WpaEap) {
        if (securityType == NetworkManager::Utils::DynamicWep || securityType == NetworkManager::Utils::Leap) {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
            if (securityType == NetworkManager::Utils::Leap) {
                wifiSecurity->setAuthAlg(NetworkManager::WirelessSecuritySetting::Leap);
            }
        } else {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
        }
        m_tmpConnectionUuid = settings->uuid();
        m_tmpDevicePath = device;
        m_tmpSpecificPath = specificObject;

        QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(settings, true);
        editor->show();
        KWindowSystem::setState(editor->winId(), NET::KeepAbove);
        KWindowSystem::forceActiveWindow(editor->winId());

        if (editor->exec() == QDialog::Accepted) {
            addConnection(editor->setting());
        }

        if (editor) {
            editor->deleteLater();
        }
    } else {
        if (securityType == NetworkManager::Utils::StaticWep) {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::Wep);
            wifiSecurity->setWepKey0(password);
            if (KWallet::Wallet::isEnabled()) {
                wifiSecurity->setWepKeyFlags(NetworkManager::Setting::AgentOwned);
            }
        } else {
            if (ap->mode() == NetworkManager::AccessPoint::Adhoc) {
                wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaNone);
            } else {
                wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
            }
            wifiSecurity->setPsk(password);
            if (KWallet::Wallet::isEnabled()) {
                wifiSecurity->setPskFlags(NetworkManager::Setting::AgentOwned);
            }
        }
        QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addAndActivateConnection(settings->toMap(), device, specificObject);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        watcher->setProperty("action", Handler::AddAndActivateConnection);
        watcher->setProperty("connection", settings->name());
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(replyFinished(QDBusPendingCallWatcher*)));
    }

    settings.clear();
}

void Handler::addConnection(const NMVariantMapMap& map)
{
    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addConnection(map);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", AddConnection);
    watcher->setProperty("connection", map.value("connection").value("id"));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
}

void Handler::deactivateConnection(const QString& connection, const QString& device)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        kWarning() << "Not possible to deactivate this connection";
        return;
    }

    QDBusPendingReply<> reply;
    foreach (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid() && ((!active->devices().isEmpty() && active->devices().first() == device) ||
                                               active->vpn())) {
            if (active->vpn()) {
                reply = NetworkManager::deactivateConnection(active->path());
            } else {
                if (active->devices().isEmpty()) {
                    reply = NetworkManager::deactivateConnection(connection);
                }
                NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
                if (device) {
                    reply = device->disconnectInterface();
                }
            }
        }
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", Handler::DeactivateConnection);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(replyFinished(QDBusPendingCallWatcher*)));
}

void Handler::disconnectAll()
{
    foreach (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        device->disconnectInterface();
    }
}

void Handler::enableAirplaneMode(bool enable)
{
    if (enable) {
        m_tmpBluetoothEnabled = isBtEnabled();
        m_tmpWimaxEnabled = NetworkManager::isWimaxEnabled();
        m_tmpWirelessEnabled = NetworkManager::isWirelessEnabled();
        m_tmpWwanEnabled = NetworkManager::isWwanEnabled();
        enableBt(false);
        enableWimax(false);
        enableWireless(false);
        enableWwan(false);
    } else {
        if (m_tmpBluetoothEnabled) {
            enableBt(true);
        }
        if (m_tmpWimaxEnabled) {
            enableWimax(true);
        }
        if (m_tmpWirelessEnabled) {
            enableWireless(true);
        }
        if (m_tmpWwanEnabled) {
            enableWwan(true);
        }
    }
}

void Handler::enableNetworking(bool enable)
{
    NetworkManager::setNetworkingEnabled(enable);
}

void Handler::enableWireless(bool enable)
{
    NetworkManager::setWirelessEnabled(enable);
}

void Handler::enableWimax(bool enable)
{
    NetworkManager::setWimaxEnabled(enable);
}

void Handler::enableWwan(bool enable)
{
    NetworkManager::setWwanEnabled(enable);
}

bool Handler::isBtEnabled()
{
    bool result = false;

    QDBusInterface managerIface("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", QDBusConnection::systemBus(), this);
    QDBusReply<QMap<QDBusObjectPath, NMVariantMapMap> > reply = managerIface.call("GetManagedObjects");
    if (reply.isValid()) {
        foreach(const QDBusObjectPath &path, reply.value().keys()) {
            const QString objPath = path.path();
            //qDebug() << "inspecting path" << objPath;
            const QStringList interfaces = reply.value().value(path).keys();
            //qDebug() << "interfaces:" << interfaces;
            if (interfaces.contains("org.bluez.Adapter1")) {
                QDBusInterface adapterIface("org.bluez", objPath, "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
                const bool adapterEnabled = adapterIface.property("Powered").toBool();
                //qDebug() << "Adapter" << objPath << "enabled:" << adapterEnabled;
                result |= adapterEnabled;
            }
        }
    } else {
        qDebug() << "Failed to enumerate BT adapters";
    }

    return result;
}

void Handler::enableBt(bool enable)
{
    QDBusInterface managerIface("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", QDBusConnection::systemBus(), this);
    QDBusReply<QMap<QDBusObjectPath, NMVariantMapMap> > reply = managerIface.call("GetManagedObjects");
    if (reply.isValid()) {
        foreach(const QDBusObjectPath &path, reply.value().keys()) {
            const QString objPath = path.path();
            //qDebug() << "inspecting path" << objPath;
            const QStringList interfaces = reply.value().value(path).keys();
            //qDebug() << "interfaces:" << interfaces;
            if (interfaces.contains("org.bluez.Adapter1")) {
                QDBusInterface adapterIface("org.bluez", objPath, "org.bluez.Adapter1", QDBusConnection::systemBus(), this);
                //qDebug() << "Enabling adapter:" << objPath << (enable && m_btEnabled);
                adapterIface.setProperty("Powered", enable);
            }
        }
    } else {
        qDebug() << "Failed to enumerate BT adapters";
    }
}

// void Handler::editConnection(const QString& uuid)
// {
//     QStringList args;
//     args << uuid;
//     KProcess::startDetached("kde5-nm-connection-editor", args);
// }

void Handler::removeConnection(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con || con->uuid().isEmpty()) {
        kWarning() << "Not possible to remove connection " << connection;
        return;
    }

    // Remove slave connections
    foreach (const NetworkManager::Connection::Ptr &connection, NetworkManager::listConnections()) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == con->uuid()) {
            connection->remove();
        }
    }

    QDBusPendingReply<> reply = con->remove();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", Handler::RemoveConnection);
    watcher->setProperty("connection", con->name());
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(replyFinished(QDBusPendingCallWatcher*)));
}

void Handler::updateConnection(const NetworkManager::Connection::Ptr& connection, const NMVariantMapMap& map)
{
    QDBusPendingReply<> reply = connection->update(map);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", UpdateConnection);
    watcher->setProperty("connection", connection->name());
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
}

void Handler::openEditor()
{
    KProcess::startDetached("kde5-nm-connection-editor");
}

void Handler::requestScan()
{
    foreach (NetworkManager::Device::Ptr device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
            if (wifiDevice) {
                QDBusPendingReply<> reply = wifiDevice->requestScan();
                QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
                watcher->setProperty("action", Handler::RequestScan);
                connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(replyFinished(QDBusPendingCallWatcher*)));
            }
        }
    }
}

void Handler::replyFinished(QDBusPendingCallWatcher * watcher)
{
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError() || !reply.isValid()) {
        KNotification *notification = 0;
        QString error = reply.error().message();
        Handler::HandlerAction action = (Handler::HandlerAction)watcher->property("action").toUInt();
        switch (action) {
            case Handler::ActivateConnection:
                notification = new KNotification("FailedToActivateConnection", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to activate %1", watcher->property("connection").toString()));
                break;
            case Handler::AddAndActivateConnection:
                notification = new KNotification("FailedToAddConnection", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to add %1", watcher->property("connection").toString()));
                break;
            case Handler::AddConnection:
                notification = new KNotification("FailedToAddConnection", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to add connection %1", watcher->property("connection").toString()));
                break;
            case Handler::DeactivateConnection:
                notification = new KNotification("FailedToDeactivateConnection", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to deactivate %1", watcher->property("connection").toString()));
                break;
            case Handler::RemoveConnection:
                notification = new KNotification("FailedToRemoveConnection", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to remove %1", watcher->property("connection").toString()));
                break;
            case Handler::RequestScan:
                notification = new KNotification("FailedToRequestScan", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to request scan"));
                break;
            case Handler::UpdateConnection:
                notification = new KNotification("FailedToUpdateConnection", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(i18n("Failed to update connection %1", watcher->property("connection").toString()));
                break;
            default:
                break;
        }

        if (notification) {
            notification->setText(error);
            notification->setPixmap(QIcon::fromTheme("dialog-warning").pixmap(64, 64));
            notification->sendEvent();
        }
    } else {
        KNotification *notification = 0;
        Handler::HandlerAction action = (Handler::HandlerAction)watcher->property("action").toUInt();

        switch (action) {
            case Handler::AddConnection:
                notification = new KNotification("ConnectionAdded", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(watcher->property("connection").toString());
                notification->setText(i18n("Connection %1 has been added", watcher->property("connection").toString()));
                break;
            case Handler::RemoveConnection:
                notification = new KNotification("ConnectionRemoved", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(watcher->property("connection").toString());
                notification->setText(i18n("Connection %1 has been removed", watcher->property("connection").toString()));
                break;
            case Handler::UpdateConnection:
                notification = new KNotification("ConnectionUpdated", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(watcher->property("connection").toString());
                notification->setText(i18n("Connection %1 has been updated", watcher->property("connection").toString()));
                break;
            default:
                break;
        }

        if (notification) {
            notification->setPixmap(QIcon::fromTheme("dialog-information").pixmap(64, 64));
            notification->sendEvent();
        }
    }

    watcher->deleteLater();
}
