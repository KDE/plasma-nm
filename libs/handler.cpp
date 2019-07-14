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
#include "connectioneditordialog.h"
#include "uiutils.h"
#include "debug.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Setting>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Ipv4Setting>

#include <libnm/nm-vpn-plugin-info.h>

#if WITH_MODEMMANAGER_SUPPORT
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/ModemDevice>
#endif

#include <QDBusError>
#include <QDBusPendingReply>
#include <QIcon>

#include <KNotification>
#include <KLocalizedString>
#include <KUser>
#include <KProcess>
#include <KService>
#include <KServiceTypeTrader>
#include <KWindowSystem>
#include <KIconLoader>
#include <KWallet>

#define AGENT_SERVICE "org.kde.kded5"
#define AGENT_PATH "/modules/networkmanagement"
#define AGENT_IFACE "org.kde.plasmanetworkmanagement"


Handler::Handler(QObject *parent)
    : QObject(parent)
    , m_tmpWirelessEnabled(NetworkManager::isWirelessEnabled())
    , m_tmpWwanEnabled(NetworkManager::isWwanEnabled())
{
    initKdedModule();
    QDBusConnection::sessionBus().connect(QStringLiteral(AGENT_SERVICE),
                                            QStringLiteral(AGENT_PATH),
                                            QStringLiteral(AGENT_IFACE),
                                            QStringLiteral("registered"),
                                            this, SLOT(initKdedModule()));

    QDBusConnection::sessionBus().connect(QStringLiteral(AGENT_SERVICE),
                                            QStringLiteral(AGENT_PATH),
                                            QStringLiteral(AGENT_IFACE),
                                            QStringLiteral("secretsError"),
                                            this, SLOT(secretAgentError(QString, QString)));

    // Interval (in ms) between attempts to scan for wifi networks
    m_wirelessScanRetryTimer.setInterval(2000);
    m_wirelessScanRetryTimer.setSingleShot(true);
}

Handler::~Handler()
{
}

void Handler::activateConnection(const QString& connection, const QString& device, const QString& specificObject)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        qCWarning(PLASMA_NM) << "Not possible to activate this connection";
        return;
    }

    if (con->settings()->connectionType() == NetworkManager::ConnectionSettings::Vpn) {
        NetworkManager::VpnSetting::Ptr vpnSetting = con->settings()->setting(NetworkManager::Setting::Vpn).staticCast<NetworkManager::VpnSetting>();
        if (vpnSetting) {
            qCDebug(PLASMA_NM) << "Checking VPN" << con->name() << "type:" << vpnSetting->serviceType();

            bool pluginMissing = false;

            // Check missing plasma-nm VPN plugin
            const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin",
                                                                              QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(vpnSetting->serviceType()));
            pluginMissing = services.isEmpty();

            // Check missing NetworkManager VPN plugin
            if (!pluginMissing) {
                GSList *plugins = nullptr;
                plugins = nm_vpn_plugin_info_list_load();

                NMVpnPluginInfo *plugin_info = nm_vpn_plugin_info_list_find_by_service(plugins, vpnSetting->serviceType().toStdString().c_str());
                pluginMissing = !plugin_info;
            }

            if (pluginMissing) {
                qCWarning(PLASMA_NM) << "VPN" << vpnSetting->serviceType() << "not found, skipping";
                KNotification *notification = new KNotification("MissingVpnPlugin", KNotification::CloseOnTimeout, this);
                notification->setComponentName("networkmanagement");
                notification->setTitle(con->name());
                notification->setText(i18n("Missing VPN plugin"));
                notification->setPixmap(QIcon::fromTheme("dialog-warning").pixmap(KIconLoader::SizeHuge));
                notification->sendEvent();
                return;
            }

        }
    }

#if WITH_MODEMMANAGER_SUPPORT
    if (con->settings()->connectionType() == NetworkManager::ConnectionSettings::Gsm) {
        NetworkManager::ModemDevice::Ptr nmModemDevice = NetworkManager::findNetworkInterface(device).objectCast<NetworkManager::ModemDevice>();
        if (nmModemDevice) {
            ModemManager::ModemDevice::Ptr mmModemDevice = ModemManager::findModemDevice(nmModemDevice->udi());
            if (mmModemDevice) {
                ModemManager::Modem::Ptr modem = mmModemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
                NetworkManager::GsmSetting::Ptr gsmSetting = con->settings()->setting(NetworkManager::Setting::Gsm).staticCast<NetworkManager::GsmSetting>();
                if (gsmSetting && gsmSetting->pinFlags() == NetworkManager::Setting::NotSaved &&
                    modem && modem->unlockRequired() > MM_MODEM_LOCK_NONE) {
                    QDBusInterface managerIface("org.kde.plasmanetworkmanagement", "/org/kde/plasmanetworkmanagement", "org.kde.plasmanetworkmanagement", QDBusConnection::sessionBus(), this);
                    managerIface.call("unlockModem", mmModemDevice->uni());
                    connect(modem.data(), &ModemManager::Modem::unlockRequiredChanged, this, &Handler::unlockRequiredChanged);
                    m_tmpConnectionPath = connection;
                    m_tmpDevicePath = device;
                    m_tmpSpecificPath = specificObject;
                    return;
                }
            }
        }
    }
#endif

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::activateConnection(connection, device, specificObject);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", Handler::ActivateConnection);
    watcher->setProperty("connection", con->name());
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
}

QString Handler::wifiCode(const QString& connectionPath, const QString& ssid, int _securityType) const
{
    NetworkManager::WirelessSecurityType securityType = static_cast<NetworkManager::WirelessSecurityType>(_securityType);

    QString ret = QStringLiteral("WIFI:S:") + ssid + QLatin1Char(';');
    if (securityType != NetworkManager::NoneSecurity) {
        switch (securityType) {
            case NetworkManager::NoneSecurity:
                break;
            case NetworkManager::StaticWep:
                ret += "T:WEP;";
                break;
            case NetworkManager::WpaPsk:
            case NetworkManager::Wpa2Psk:
                ret += "T:WPA;";
                break;
            default:
            case NetworkManager::DynamicWep:
            case NetworkManager::WpaEap:
            case NetworkManager::Wpa2Eap:
            case NetworkManager::Leap:
                return {};
        }
    }

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(connectionPath);
    if(!connection)
        return {};

    const auto key = QStringLiteral("802-11-wireless-security");
    auto reply = connection->secrets(key);

    const auto secret = reply.argumentAt<0>()[key];
    QString pass;
    switch (securityType) {
        case NetworkManager::NoneSecurity:
            break;
        case NetworkManager::WpaPsk:
        case NetworkManager::Wpa2Psk:
            pass = secret["psk"].toString();
            break;
        default:
            return {};
    }
    if (!pass.isEmpty())
        ret += QStringLiteral("P:") + pass + QLatin1Char(';');

    return ret + QLatin1Char(';');
}

void Handler::addAndActivateConnection(const QString& device, const QString& specificObject, const QString& password)
{
    NetworkManager::AccessPoint::Ptr ap;
    NetworkManager::WirelessDevice::Ptr wifiDev;
    for (const NetworkManager::Device::Ptr &dev : NetworkManager::networkInterfaces()) {
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

    NetworkManager::WirelessSecurityType securityType = NetworkManager::findBestWirelessSecurity(wifiDev->wirelessCapabilities(), true, (ap->mode() == NetworkManager::AccessPoint::Adhoc), ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());

    if (securityType != NetworkManager::NoneSecurity) {
        wifiSecurity->setInitialized(true);
        wifiSetting->setSecurity("802-11-wireless-security");
    }

    if (securityType == NetworkManager::Leap ||
        securityType == NetworkManager::DynamicWep ||
        securityType == NetworkManager::Wpa2Eap ||
        securityType == NetworkManager::WpaEap) {
        if (securityType == NetworkManager::DynamicWep || securityType == NetworkManager::Leap) {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
            if (securityType == NetworkManager::Leap) {
                wifiSecurity->setAuthAlg(NetworkManager::WirelessSecuritySetting::Leap);
            }
        } else {
            wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
        }
        m_tmpConnectionUuid = settings->uuid();
        m_tmpDevicePath = device;
        m_tmpSpecificPath = specificObject;

        QPointer<ConnectionEditorDialog> editor = new ConnectionEditorDialog(settings);
        editor->show();
        KWindowSystem::setState(editor->winId(), NET::KeepAbove);
        KWindowSystem::forceActiveWindow(editor->winId());
        connect(editor.data(), &ConnectionEditorDialog::accepted,
                [editor, this] () {
                    addConnection(editor->setting());
                });
        connect(editor.data(), &ConnectionEditorDialog::finished,
                [editor] () {
                    if (editor) {
                        editor->deleteLater();
                    }
                });
        editor->setModal(true);
        editor->show();
    } else {
        if (securityType == NetworkManager::StaticWep) {
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
        connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
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
        qCWarning(PLASMA_NM) << "Not possible to deactivate this connection";
        return;
    }

    QDBusPendingReply<> reply;
    for (const NetworkManager::ActiveConnection::Ptr &active : NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid() && ((!active->devices().isEmpty() && active->devices().first() == device) ||
                                               active->vpn())) {
            if (active->vpn()) {
                reply = NetworkManager::deactivateConnection(active->path());
            } else {
                NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(active->devices().first());
                if (device) {
                    reply = device->disconnectInterface();
                }
            }
        }
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", Handler::DeactivateConnection);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
}

void Handler::disconnectAll()
{
    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
        device->disconnectInterface();
    }
}

void Handler::enableAirplaneMode(bool enable)
{
    if (enable) {
        m_tmpWirelessEnabled = NetworkManager::isWirelessEnabled();
        m_tmpWwanEnabled = NetworkManager::isWwanEnabled();
        enableBluetooth(false);
        enableWireless(false);
        enableWwan(false);
    } else {
        enableBluetooth(true);
        if (m_tmpWirelessEnabled) {
            enableWireless(true);
        }
        if (m_tmpWwanEnabled) {
            enableWwan(true);
        }
    }
}

void Handler::enableBluetooth(bool enable)
{
    qDBusRegisterMetaType< QMap<QDBusObjectPath, NMVariantMapMap > >();

    QDBusMessage message = QDBusMessage::createMethodCall("org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects");
    QDBusPendingReply<QMap<QDBusObjectPath, NMVariantMapMap> > reply = QDBusConnection::systemBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
        [this, enable] (QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QMap<QDBusObjectPath, NMVariantMapMap> > reply = *watcher;
            if (reply.isValid()) {
                for (const QDBusObjectPath &path : reply.value().keys()) {
                    const QString objPath = path.path();
                    qCDebug(PLASMA_NM) << "inspecting path" << objPath;
                    const QStringList interfaces = reply.value().value(path).keys();
                    qCDebug(PLASMA_NM) << "interfaces:" << interfaces;
                    if (interfaces.contains("org.bluez.Adapter1")) {
                        // We need to check previous state first
                        if (!enable) {
                            QDBusMessage message = QDBusMessage::createMethodCall("org.bluez", objPath, "org.freedesktop.DBus.Properties", "Get");
                            QList<QVariant> arguments;
                            arguments << QLatin1Literal("org.bluez.Adapter1");
                            arguments << QLatin1Literal("Powered");
                            message.setArguments(arguments);
                            QDBusPendingReply<QVariant> getReply = QDBusConnection::systemBus().asyncCall(message);
                            QDBusPendingCallWatcher *getWatcher = new QDBusPendingCallWatcher(getReply, this);
                                connect(getWatcher, &QDBusPendingCallWatcher::finished,
                                    [this, objPath] (QDBusPendingCallWatcher *watcher) {
                                        QDBusPendingReply<QVariant> reply = *watcher;
                                        if (reply.isValid()) {
                                            m_bluetoothAdapters.insert(objPath, reply.value().toBool());
                                            QDBusMessage message = QDBusMessage::createMethodCall("org.bluez", objPath, "org.freedesktop.DBus.Properties", "Set");
                                            QList<QVariant> arguments;
                                            arguments << QLatin1Literal("org.bluez.Adapter1");
                                            arguments << QLatin1Literal("Powered");
                                            arguments << QVariant::fromValue(QDBusVariant(QVariant(false)));
                                            message.setArguments(arguments);
                                            QDBusConnection::systemBus().asyncCall(message);
                                        }

                                    });
                            getWatcher->deleteLater();
                        } else if (enable && m_bluetoothAdapters.value(objPath)) {
                            QDBusMessage message = QDBusMessage::createMethodCall("org.bluez", objPath, "org.freedesktop.DBus.Properties", "Set");
                            QList<QVariant> arguments;
                            arguments << QLatin1Literal("org.bluez.Adapter1");
                            arguments << QLatin1Literal("Powered");
                            arguments << QVariant::fromValue(QDBusVariant(QVariant(enable)));
                            message.setArguments(arguments);
                            QDBusConnection::systemBus().asyncCall(message);
                        }
                    }
                }
            }
        });
    watcher->deleteLater();
}

void Handler::enableNetworking(bool enable)
{
    NetworkManager::setNetworkingEnabled(enable);
}

void Handler::enableWireless(bool enable)
{
    NetworkManager::setWirelessEnabled(enable);
}

void Handler::enableWwan(bool enable)
{
    NetworkManager::setWwanEnabled(enable);
}

void Handler::removeConnection(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con || con->uuid().isEmpty()) {
        qCWarning(PLASMA_NM) << "Not possible to remove connection " << connection;
        return;
    }

    // Remove slave connections
    for (const NetworkManager::Connection::Ptr &connection : NetworkManager::listConnections()) {
        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->master() == con->uuid()) {
            connection->remove();
        }
    }

    QDBusPendingReply<> reply = con->remove();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", Handler::RemoveConnection);
    watcher->setProperty("connection", con->name());
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
}

void Handler::updateConnection(const NetworkManager::Connection::Ptr& connection, const NMVariantMapMap& map)
{
    QDBusPendingReply<> reply = connection->update(map);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("action", UpdateConnection);
    watcher->setProperty("connection", connection->name());
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
}

void Handler::requestScan(const QString &interface)
{
    for (NetworkManager::Device::Ptr device : NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();

            if (wifiDevice && wifiDevice->state() != NetworkManager::WirelessDevice::Unavailable) {
                if (!interface.isEmpty() && interface != wifiDevice->interfaceName()) {
                    continue;
                }

                qCDebug(PLASMA_NM) << "Requesting wifi scan on device" << wifiDevice->interfaceName();
                QDBusPendingReply<> reply = wifiDevice->requestScan();
                QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
                watcher->setProperty("action", Handler::RequestScan);
                watcher->setProperty("interface", wifiDevice->interfaceName());
                connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
            }
        }
    }
}

void Handler::scanRequestFailed(const QString &interface)
{
    if (m_wirelessScanRetryTimer.isActive()) {
        return;
    }
    qCDebug(PLASMA_NM) << "Trying soon a new scan on" << interface;

    emit wirelessScanTimerEnabled(false);

    auto retryAction = [this,interface]() {
        m_wirelessScanRetryTimer.disconnect();
        requestScan(interface);
    };
    connect(&m_wirelessScanRetryTimer, &QTimer::timeout, this, retryAction);
    m_wirelessScanRetryTimer.start();
}

void Handler::initKdedModule()
{
    QDBusMessage initMsg = QDBusMessage::createMethodCall(QStringLiteral(AGENT_SERVICE),
                                                          QStringLiteral(AGENT_PATH),
                                                          QStringLiteral(AGENT_IFACE),
                                                          QStringLiteral("init"));
    QDBusConnection::sessionBus().send(initMsg);
}

void Handler::secretAgentError(const QString &connectionPath, const QString &message)
{
    // If the password was wrong, forget it
    removeConnection(connectionPath);
    emit connectionActivationFailed(connectionPath, message);
}

void Handler::replyFinished(QDBusPendingCallWatcher * watcher)
{
    QDBusPendingReply<> reply = *watcher;
    if (reply.isError() || !reply.isValid()) {
        KNotification *notification = nullptr;
        QString error = reply.error().message();
        Handler::HandlerAction action = (Handler::HandlerAction)watcher->property("action").toUInt();
        switch (action) {
            case Handler::ActivateConnection:
                notification = new KNotification("FailedToActivateConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to activate %1", watcher->property("connection").toString()));
                break;
            case Handler::AddAndActivateConnection:
                notification = new KNotification("FailedToAddConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to add %1", watcher->property("connection").toString()));
                break;
            case Handler::AddConnection:
                notification = new KNotification("FailedToAddConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to add connection %1", watcher->property("connection").toString()));
                break;
            case Handler::DeactivateConnection:
                notification = new KNotification("FailedToDeactivateConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to deactivate %1", watcher->property("connection").toString()));
                break;
            case Handler::RemoveConnection:
                notification = new KNotification("FailedToRemoveConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to remove %1", watcher->property("connection").toString()));
                break;
            case Handler::UpdateConnection:
                notification = new KNotification("FailedToUpdateConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to update connection %1", watcher->property("connection").toString()));
                break;
            case Handler::RequestScan:
            {
                const QString interface = watcher->property("interface").toString();
                qCWarning(PLASMA_NM) << "Wireless scan on" << interface << "failed:" << error;
                scanRequestFailed(interface);
                break;
            }
            default:
                break;
        }

        if (notification) {
            notification->setComponentName("networkmanagement");
            notification->setText(error);
            notification->setPixmap(QIcon::fromTheme("dialog-warning").pixmap(KIconLoader::SizeHuge));
            notification->sendEvent();
        }
    } else {
        KNotification *notification = nullptr;
        Handler::HandlerAction action = (Handler::HandlerAction)watcher->property("action").toUInt();

        switch (action) {
            case Handler::AddConnection:
                notification = new KNotification("ConnectionAdded", KNotification::CloseOnTimeout, this);
                notification->setText(i18n("Connection %1 has been added", watcher->property("connection").toString()));
                break;
            case Handler::RemoveConnection:
                notification = new KNotification("ConnectionRemoved", KNotification::CloseOnTimeout, this);
                notification->setText(i18n("Connection %1 has been removed", watcher->property("connection").toString()));
                break;
            case Handler::UpdateConnection:
                notification = new KNotification("ConnectionUpdated", KNotification::CloseOnTimeout, this);
                notification->setText(i18n("Connection %1 has been updated", watcher->property("connection").toString()));
                break;
            case Handler::RequestScan:
                qCDebug(PLASMA_NM) << "Wireless scan on" << watcher->property("interface").toString() << "succeeded";
                emit wirelessScanTimerEnabled(true);
                break;
            default:
                break;
        }

        if (notification) {
            notification->setComponentName("networkmanagement");
            notification->setTitle(watcher->property("connection").toString());
            notification->setPixmap(QIcon::fromTheme("dialog-information").pixmap(KIconLoader::SizeHuge));
            notification->sendEvent();
        }
    }

    watcher->deleteLater();
}

#if WITH_MODEMMANAGER_SUPPORT
void Handler::unlockRequiredChanged(MMModemLock modemLock)
{
    if (modemLock == MM_MODEM_LOCK_NONE) {
        activateConnection(m_tmpConnectionPath, m_tmpDevicePath, m_tmpSpecificPath);
    }
}
#endif

