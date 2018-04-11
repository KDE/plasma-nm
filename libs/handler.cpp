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
#include <KWallet/Wallet>

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
            // get the list of supported VPN service types
            const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin",
                                                                              QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(vpnSetting->serviceType()));
            if (services.isEmpty()) {
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

void Handler::addAndActivateConnection(const QString& device, const QString& specificObject, const QString& password)
{
    NetworkManager::AccessPoint::Ptr ap;
    NetworkManager::WirelessDevice::Ptr wifiDev;
    Q_FOREACH (const NetworkManager::Device::Ptr & dev, NetworkManager::networkInterfaces()) {
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
    Q_FOREACH (const NetworkManager::ActiveConnection::Ptr & active, NetworkManager::activeConnections()) {
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
    Q_FOREACH (const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
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
                Q_FOREACH (const QDBusObjectPath &path, reply.value().keys()) {
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
    Q_FOREACH (const NetworkManager::Connection::Ptr &connection, NetworkManager::listConnections()) {
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

void Handler::requestScan()
{
    Q_FOREACH (NetworkManager::Device::Ptr device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
            if (wifiDevice) {
                QDBusPendingReply<> reply = wifiDevice->requestScan();
                QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
                watcher->setProperty("action", Handler::RequestScan);
                connect(watcher, &QDBusPendingCallWatcher::finished, this, &Handler::replyFinished);
            }
        }
    }
}

void Handler::initKdedModule()
{
    QDBusMessage initMsg = QDBusMessage::createMethodCall(QStringLiteral(AGENT_SERVICE),
                                                          QStringLiteral(AGENT_PATH),
                                                          QStringLiteral(AGENT_IFACE),
                                                          QStringLiteral("init"));
    QDBusConnection::sessionBus().send(initMsg);
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
            case Handler::RequestScan:
                /* INFO: Disabled for now as wifi scanning is now automatical
                    notification = new KNotification("FailedToRequestScan", KNotification::CloseOnTimeout, this);
                    notification->setTitle(i18n("Failed to request scan"));
                */
                break;
            case Handler::UpdateConnection:
                notification = new KNotification("FailedToUpdateConnection", KNotification::CloseOnTimeout, this);
                notification->setTitle(i18n("Failed to update connection %1", watcher->property("connection").toString()));
                break;
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
        KNotification *notification = 0;
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
QVariantMap Handler::getConnectionSettings(const QString &connection, const QString &type)
{
    if (type.isEmpty())
        return QVariantMap();
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    if (!con)
        return QVariantMap();
    QVariantMap map = con->settings()->toMap().value(type);
    //qCWarning(PLASMA_NM) << "Map:" <<con->settings()->toMap().value(type);
    return map;
}

QVariantMap Handler::getActiveConnectionInfo(const QString &connection)
{
    if (connection.isEmpty())
        return QVariantMap();
    NetworkManager::ActiveConnection::Ptr activeCon;
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);
    Q_FOREACH (const NetworkManager::ActiveConnection::Ptr &active, NetworkManager::activeConnections()) {
        if (active->uuid() == con->uuid())
            activeCon = active;
    }
    if(!activeCon){
        qWarning(PLASMA_NM) << "Active" << connection << "not found";
        return QVariantMap();
    }
    QVariantMap map;
    map.insert("address",QVariant(activeCon->ipV4Config().addresses().first().ip().toString()));
    map.insert("prefix",QVariant(activeCon->ipV4Config().addresses().first().netmask().toString()));
    map.insert("gateway",QVariant(activeCon->ipV4Config().gateway()));
    map.insert("dns",QVariant(activeCon->ipV4Config().nameservers().first().toString()));
    //qWarning() << map;
    return map;
}

void Handler::addConnectionFromQML(const QVariantMap &QMLmap)
{
    if (QMLmap.isEmpty())
        return;

    if (QMLmap["mode"].toString() == "infrastructure") {
        NetworkManager::WirelessSetting::Ptr wirelessSettings = NetworkManager::WirelessSetting::Ptr(new NetworkManager::WirelessSetting());
        wirelessSettings->setSsid(QMLmap.value(QLatin1String("id")).toString().toUtf8());
        wirelessSettings->setMode(NetworkManager::WirelessSetting::Infrastructure);

        NetworkManager::Ipv4Setting::Ptr ipSettings = NetworkManager::Ipv4Setting::Ptr(new NetworkManager::Ipv4Setting());
        if (QMLmap["method"] == QLatin1String("auto")) {
            ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Automatic);
        } else {
            ipSettings->setMethod(NetworkManager::Ipv4Setting::ConfigMethod::Manual);
            NetworkManager::IpAddress ipaddr;
            ipaddr.setIp(QHostAddress(QMLmap["address"].toString()));
            ipaddr.setPrefixLength(QMLmap["prefix"].toInt());
            ipaddr.setGateway(QHostAddress(QMLmap["gateway"].toString()));
            ipSettings->setAddresses(QList<NetworkManager::IpAddress>({ipaddr}));
            ipSettings->setDns(QList<QHostAddress>({QHostAddress(QMLmap["dns"].toString())}));
        }

        NetworkManager::ConnectionSettings::Ptr connectionSettings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
        connectionSettings->setId(QMLmap.value(QLatin1String("id")).toString());
        connectionSettings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());

        NMVariantMapMap map = connectionSettings->toMap();
        map.insert("802-11-wireless",wirelessSettings->toMap());
        map.insert("ipv4",ipSettings->toMap());

        if (QMLmap.contains("802-11-wireless-security")) {
            QVariantMap securMap = QMLmap["802-11-wireless-security"].toMap();
            int type = securMap["type"].toInt();
            if (!type == NetworkManager::NoneSecurity) {
                NetworkManager::WirelessSecuritySetting::Ptr securitySettings = NetworkManager::WirelessSecuritySetting::Ptr(new NetworkManager::WirelessSecuritySetting());
                if (type == NetworkManager::Wpa2Psk ) {
                    securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk);
                    securitySettings->setAuthAlg(NetworkManager::WirelessSecuritySetting::AuthAlg::Open);
                    securitySettings->setPskFlags(NetworkManager::Setting::SecretFlagType::AgentOwned);
                    securitySettings->setPsk(securMap["password"].toString());
                }
                if (type == NetworkManager::StaticWep) {
                    securitySettings->setKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep);
                    securitySettings->setAuthAlg(NetworkManager::WirelessSecuritySetting::AuthAlg::Open);
                    securitySettings->setWepKeyType(NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
                    securitySettings->setWepKeyFlags(NetworkManager::Setting::SecretFlagType::AgentOwned);
                    securitySettings->setWepKey0(securMap["password"].toString());
                }
                map.insert("802-11-wireless-security",securitySettings->toMap());
            }
        }
        qWarning() << map;
        this->addConnection(map);
    }
}

#if WITH_MODEMMANAGER_SUPPORT
void Handler::unlockRequiredChanged(MMModemLock modemLock)
{
    if (modemLock == MM_MODEM_LOCK_NONE) {
        activateConnection(m_tmpConnectionPath, m_tmpDevicePath, m_tmpSpecificPath);
    }
}
#endif

