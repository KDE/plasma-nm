/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2013 by Daniel Nicoletti <dantti12@gmail.com>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
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

#include "debug.h"
#include "notification.h"

#include <uiutils.h>

#include <NetworkManagerQt/Manager>

#include <KLocalizedString>
#include <KNotification>
#include <KIconLoader>

#include <QDBusConnection>
#include <QIcon>
#include <QTimer>

Notification::Notification(QObject *parent) :
    QObject(parent)
{
    // devices
    Q_FOREACH (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        addDevice(device);
    }

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceAdded, this, &Notification::deviceAdded);

    // connections
    Q_FOREACH (const NetworkManager::ActiveConnection::Ptr &ac, NetworkManager::activeConnections()) {
        addActiveConnection(ac);
    }

    connect(NetworkManager::notifier(), &NetworkManager::Notifier::activeConnectionAdded, this, QOverload<const QString&>::of(&Notification::addActiveConnection));

    QDBusConnection::systemBus().connect(QStringLiteral("org.freedesktop.login1"),
                                         QStringLiteral("/org/freedesktop/login1"),
                                         QStringLiteral("org.freedesktop.login1.Manager"),
                                         QStringLiteral("PrepareForSleep"),
                                         this,
                                         SLOT(onPrepareForSleep(bool)));
}

void Notification::deviceAdded(const QString &uni)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    addDevice(device);
}

void Notification::addDevice(const NetworkManager::Device::Ptr &device)
{
    connect(device.data(), &NetworkManager::Device::stateChanged, this, &Notification::stateChanged);
}

void Notification::stateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    Q_UNUSED(oldstate)

    NetworkManager::Device *device = qobject_cast<NetworkManager::Device*>(sender());
    if (newstate == NetworkManager::Device::Activated && m_notifications.contains(device->uni())) {
        KNotification *notify = m_notifications.value(device->uni());
        notify->deleteLater();
        m_notifications.remove(device->uni());
        return;
    } else if (newstate != NetworkManager::Device::Failed) {
        return;
    }

    const QString identifier = UiUtils::prettyInterfaceName(device->type(), device->interfaceName());
    QString text;
    switch (reason) {
    case NetworkManager::Device::NoReason:
    case NetworkManager::Device::UnknownReason:
    case NetworkManager::Device::NowManagedReason:
    case NetworkManager::Device::NowUnmanagedReason:
        return;
    case NetworkManager::Device::ConfigFailedReason:
        text = i18nc("@info:status Notification when the device failed due to ConfigFailedReason",
                     "The device could not be configured");
        break;
    case NetworkManager::Device::ConfigUnavailableReason:
        text = i18nc("@info:status Notification when the device failed due to ConfigUnavailableReason",
                     "IP configuration was unavailable");
        break;
    case NetworkManager::Device::ConfigExpiredReason:
        text = i18nc("@info:status Notification when the device failed due to ConfigExpiredReason",
                     "IP configuration expired");
        break;
    case NetworkManager::Device::NoSecretsReason:
        text = i18nc("@info:status Notification when the device failed due to NoSecretsReason",
                     "No secrets were provided");
        break;
    case NetworkManager::Device::AuthSupplicantDisconnectReason:
        text = i18nc("@info:status Notification when the device failed due to AuthSupplicantDisconnectReason",
                     "Authorization supplicant disconnected");
        break;
    case NetworkManager::Device::AuthSupplicantConfigFailedReason:
        text = i18nc("@info:status Notification when the device failed due to AuthSupplicantConfigFailedReason",
                     "Authorization supplicant's configuration failed");
        break;
    case NetworkManager::Device::AuthSupplicantFailedReason:
        text = i18nc("@info:status Notification when the device failed due to AuthSupplicantFailedReason",
                     "Authorization supplicant failed");
        break;
    case NetworkManager::Device::AuthSupplicantTimeoutReason:
        text = i18nc("@info:status Notification when the device failed due to AuthSupplicantTimeoutReason",
                     "Authorization supplicant timed out");
        break;
    case NetworkManager::Device::PppStartFailedReason:
        text = i18nc("@info:status Notification when the device failed due to PppStartFailedReason",
                     "PPP failed to start");
        break;
    case NetworkManager::Device::PppDisconnectReason:
        text = i18nc("@info:status Notification when the device failed due to PppDisconnectReason",
                     "PPP disconnected");
        break;
    case NetworkManager::Device::PppFailedReason:
        text = i18nc("@info:status Notification when the device failed due to PppFailedReason",
                     "PPP failed");
        break;
    case NetworkManager::Device::DhcpStartFailedReason:
        text = i18nc("@info:status Notification when the device failed due to DhcpStartFailedReason",
                     "DHCP failed to start");
        break;
    case NetworkManager::Device::DhcpErrorReason:
        text = i18nc("@info:status Notification when the device failed due to DhcpErrorReason",
                     "A DHCP error occurred");
        break;
    case NetworkManager::Device::DhcpFailedReason:
        text = i18nc("@info:status Notification when the device failed due to DhcpFailedReason",
                     "DHCP failed ");
        break;
    case NetworkManager::Device::SharedStartFailedReason:
        text = i18nc("@info:status Notification when the device failed due to SharedStartFailedReason",
                     "The shared service failed to start");
        break;
    case NetworkManager::Device::SharedFailedReason:
        text = i18nc("@info:status Notification when the device failed due to SharedFailedReason",
                     "The shared service failed");
        break;
    case NetworkManager::Device::AutoIpStartFailedReason:
        text = i18nc("@info:status Notification when the device failed due to AutoIpStartFailedReason",
                     "The auto IP service failed to start");
        break;
    case NetworkManager::Device::AutoIpErrorReason:
        text = i18nc("@info:status Notification when the device failed due to AutoIpErrorReason",
                     "The auto IP service reported an error");
        break;
    case NetworkManager::Device::AutoIpFailedReason:
        text = i18nc("@info:status Notification when the device failed due to AutoIpFailedReason",
                     "The auto IP service failed");
        break;
    case NetworkManager::Device::ModemBusyReason:
        text = i18nc("@info:status Notification when the device failed due to ModemBusyReason",
                     "The modem is busy");
        break;
    case NetworkManager::Device::ModemNoDialToneReason:
        text = i18nc("@info:status Notification when the device failed due to ModemNoDialToneReason",
                     "The modem has no dial tone");
        break;
    case NetworkManager::Device::ModemNoCarrierReason:
        text = i18nc("@info:status Notification when the device failed due to ModemNoCarrierReason",
                     "The modem shows no carrier");
        break;
    case NetworkManager::Device::ModemDialTimeoutReason:
        text = i18nc("@info:status Notification when the device failed due to ModemDialTimeoutReason",
                     "The modem dial timed out");
        break;
    case NetworkManager::Device::ModemDialFailedReason:
        text = i18nc("@info:status Notification when the device failed due to ModemDialFailedReason",
                     "The modem dial failed");
        break;
    case NetworkManager::Device::ModemInitFailedReason:
        text = i18nc("@info:status Notification when the device failed due to ModemInitFailedReason",
                     "The modem could not be initialized");
        break;
    case NetworkManager::Device::GsmApnSelectFailedReason:
        text = i18nc("@info:status Notification when the device failed due to GsmApnSelectFailedReason",
                     "The GSM APN could not be selected");
        break;
    case NetworkManager::Device::GsmNotSearchingReason:
        text = i18nc("@info:status Notification when the device failed due to GsmNotSearchingReason",
                     "The GSM modem is not searching");
        break;
    case NetworkManager::Device::GsmRegistrationDeniedReason:
        text = i18nc("@info:status Notification when the device failed due to GsmRegistrationDeniedReason",
                     "GSM network registration was denied");
        break;
    case NetworkManager::Device::GsmRegistrationTimeoutReason:
        text = i18nc("@info:status Notification when the device failed due to GsmRegistrationTimeoutReason",
                     "GSM network registration timed out");
        break;
    case NetworkManager::Device::GsmRegistrationFailedReason:
        text = i18nc("@info:status Notification when the device failed due to GsmRegistrationFailedReason",
                     "GSM registration failed");
        break;
    case NetworkManager::Device::GsmPinCheckFailedReason:
        text = i18nc("@info:status Notification when the device failed due to GsmPinCheckFailedReason",
                     "The GSM PIN check failed");
        break;
    case NetworkManager::Device::FirmwareMissingReason:
        text = i18nc("@info:status Notification when the device failed due to FirmwareMissingReason",
                     "Device firmware is missing");
        break;
    case NetworkManager::Device::DeviceRemovedReason:
        text = i18nc("@info:status Notification when the device failed due to DeviceRemovedReason",
                     "The device was removed");
        break;
    case NetworkManager::Device::SleepingReason:
        text = i18nc("@info:status Notification when the device failed due to SleepingReason",
                     "The networking system is now sleeping");
        break;
    case NetworkManager::Device::ConnectionRemovedReason:
        text = i18nc("@info:status Notification when the device failed due to ConnectionRemovedReason",
                     "The connection was removed");
        break;
    case NetworkManager::Device::UserRequestedReason:
        return;
    case NetworkManager::Device::CarrierReason:
        text = i18nc("@info:status Notification when the device failed due to CarrierReason",
                     "The cable was disconnected");
        break;
    case NetworkManager::Device::ConnectionAssumedReason:
    case NetworkManager::Device::SupplicantAvailableReason:
        return;
    case NetworkManager::Device::ModemNotFoundReason:
        text = i18nc("@info:status Notification when the device failed due to ModemNotFoundReason",
                     "The modem could not be found");
        break;
    case NetworkManager::Device::BluetoothFailedReason:
        text = i18nc("@info:status Notification when the device failed due to BluetoothFailedReason",
                     "The bluetooth connection failed or timed out");
        break;
    case NetworkManager::Device::GsmSimNotInserted:
        text = i18nc("@info:status Notification when the device failed due to GsmSimNotInserted",
                     "GSM Modem's SIM Card not inserted");
        break;
    case NetworkManager::Device::GsmSimPinRequired:
        text = i18nc("@info:status Notification when the device failed due to GsmSimPinRequired",
                     "GSM Modem's SIM Pin required");
        break;
    case NetworkManager::Device::GsmSimPukRequired:
        text = i18nc("@info:status Notification when the device failed due to GsmSimPukRequired",
                     "GSM Modem's SIM Puk required");
        break;
    case NetworkManager::Device::GsmSimWrong:
        text = i18nc("@info:status Notification when the device failed due to GsmSimWrong",
                     "GSM Modem's SIM wrong");
        break;
    case NetworkManager::Device::InfiniBandMode:
        text = i18nc("@info:status Notification when the device failed due to InfiniBandMode",
                     "InfiniBand device does not support connected mode");
        break;
    case NetworkManager::Device::DependencyFailed:
        text = i18nc("@info:status Notification when the device failed due to DependencyFailed",
                     "A dependency of the connection failed");
        break;
    case NetworkManager::Device::Br2684Failed:
        text = i18nc("@info:status Notification when the device failed due to Br2684Failed",
                     "Problem with the RFC 2684 Ethernet over ADSL bridge");
        break;
    case NetworkManager::Device::ModemManagerUnavailable:
        text = i18nc("@info:status Notification when the device failed due to ModemManagerUnavailable",
                     "ModemManager not running");
        break;
    case NetworkManager::Device::SsidNotFound:
        text = i18nc("@info:status Notification when the device failed due to SsidNotFound",
                     "The WiFi network could not be found");
        break;
    case NetworkManager::Device::SecondaryConnectionFailed:
        text = i18nc("@info:status Notification when the device failed due to SecondaryConnectionFailed",
                     "A secondary connection of the base connection failed");
        break;
#if NM_CHECK_VERSION(0, 9, 10)
    case NetworkManager::Device::DcbFcoeFailed:
        text = i18nc("@info:status Notification when the device failed due to DcbFcoeFailed",
                     "DCB or FCoE setup failed");
        break;
    case NetworkManager::Device::TeamdControlFailed:
        text = i18nc("@info:status Notification when the device failed due to TeamdControlFailed",
                     "teamd control failed");
        break;
    case NetworkManager::Device::ModemFailed:
        text = i18nc("@info:status Notification when the device failed due to ModemFailed",
                     "Modem failed or no longer available");
        break;
    case NetworkManager::Device::ModemAvailable:
        text = i18nc("@info:status Notification when the device failed due to ModemAvailable",
                     "Modem now ready and available");
        break;
    case NetworkManager::Device::SimPinIncorrect:
        text = i18nc("@info:status Notification when the device failed due to SimPinIncorrect",
                     "The SIM PIN was incorrect");
        break;
#endif
#if NM_CHECK_VERSION(1, 0, 4)
    case NetworkManager::Device::NewActivation:
        text = i18nc("@info:status Notification when the device failed due to NewActivation",
                     "A new connection activation was enqueued");
        break;
    case NetworkManager::Device::ParentChanged:
        text = i18nc("@info:status Notification when the device failed due to ParentChanged",
                     "The device's parent changed");
        break;
    case NetworkManager::Device::ParentManagedChanged:
        text = i18nc("@info:status Notification when the device failed due to ParentManagedChanged",
                     "The device parent's management changed");
        break;
#endif
    case NetworkManager::Device::Reserved:
        return;
    }

    if (m_notifications.contains(device->uni())) {
        KNotification *notify = m_notifications.value(device->uni());
        notify->setText(text);
        notify->update();
    } else {
        KNotification *notify = new KNotification(QStringLiteral("DeviceFailed"), KNotification::CloseOnTimeout, this);
        connect(notify, &KNotification::closed, this, &Notification::notificationClosed);
        notify->setProperty("uni", device->uni());
        notify->setComponentName(QStringLiteral("networkmanagement"));
        notify->setPixmap(QIcon::fromTheme(QStringLiteral("dialog-warning")).pixmap(KIconLoader::SizeHuge));
        notify->setTitle(identifier);
        notify->setText(text);
        notify->sendEvent();
        if (notify->id() != -1) {
                m_notifications[device->uni()] = notify;
        }
    }
}

void Notification::addActiveConnection(const QString &path)
{
    NetworkManager::ActiveConnection::Ptr ac = NetworkManager::findActiveConnection(path);
    if (ac && ac->isValid()) {
        addActiveConnection(ac);
    }
}

void Notification::addActiveConnection(const NetworkManager::ActiveConnection::Ptr &ac)
{
    if (ac->vpn()) {
        NetworkManager::VpnConnection::Ptr vpnConnection = ac.objectCast<NetworkManager::VpnConnection>();
        connect(vpnConnection.data(), &NetworkManager::VpnConnection::stateChanged, this, &Notification::onVpnConnectionStateChanged);
#if NM_CHECK_VERSION(0, 9, 10)
    } else if (ac->type() != NetworkManager::ConnectionSettings::Bond &&
               ac->type() != NetworkManager::ConnectionSettings::Bridge &&
               ac->type() != NetworkManager::ConnectionSettings::Generic &&
               ac->type() != NetworkManager::ConnectionSettings::Infiniband &&
               ac->type() != NetworkManager::ConnectionSettings::Team &&
#if NM_CHECK_VERSION(1, 1, 92)
               ac->type() != NetworkManager::ConnectionSettings::Vlan &&
               ac->type() != NetworkManager::ConnectionSettings::Tun) {
#else
               ac->type() != NetworkManager::ConnectionSettings::Vlan) {
#endif

#else
    } else if (ac->type() != NetworkManager::ConnectionSettings::Bond &&
               ac->type() != NetworkManager::ConnectionSettings::Bridge &&
               ac->type() != NetworkManager::ConnectionSettings::Infiniband &&
               ac->type() != NetworkManager::ConnectionSettings::Vlan) {
#endif
        connect(ac.data(), &NetworkManager::ActiveConnection::stateChanged, this, &Notification::onActiveConnectionStateChanged);
    }
}

void Notification::onActiveConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    NetworkManager::ActiveConnection *ac = qobject_cast<NetworkManager::ActiveConnection*>(sender());

    QString eventId, text, iconName;
    const QString acName = ac->id();
    const QString connectionId = ac->path();

    if (state == NetworkManager::ActiveConnection::Activated) {
        auto foundConnection = std::find_if(m_activeConnectionsBeforeSleep.constBegin(),
                                            m_activeConnectionsBeforeSleep.constEnd(),
                                            [ac](const QString &uuid) {
            return uuid == ac->uuid();
        });

        if (foundConnection != m_activeConnectionsBeforeSleep.constEnd()) {
            qCDebug(PLASMA_NM) << "Not emitting conection activated notification as the connection was active prior to suspend";
            return;
        }

        eventId = QStringLiteral("ConnectionActivated");
        text = i18n("Connection '%1' activated.", acName);

        switch (ac->type()) {
        case NetworkManager::ConnectionSettings::Wireless:
            iconName = QStringLiteral("network-wireless-on");
            break;
        case NetworkManager::ConnectionSettings::Wired:
            iconName = QStringLiteral("network-wired-activated");
            break;
        default: // silence warning
            break;
        }
    } else if (state == NetworkManager::ActiveConnection::Deactivated) {
        if (m_preparingForSleep) {
            qCDebug(PLASMA_NM) << "Not emitting connection deactivated notification as we're about to suspend";
            return;
        }

        eventId = QStringLiteral("ConnectionDeactivated");
        text = i18n("Connection '%1' deactivated.", acName);

        switch (ac->type()) {
        case NetworkManager::ConnectionSettings::Wireless:
            iconName = QStringLiteral("network-wireless-disconnected");
            break;
        case NetworkManager::ConnectionSettings::Wired:
            iconName = QStringLiteral("network-unavailable");
            break;
        default: // silence warning
            break;
        }
    } else {
        qCWarning(PLASMA_NM) << "Unhandled active connection state change: " << state;
        return;
    }

    KNotification *notify = new KNotification(eventId, KNotification::CloseOnTimeout, this);
    connect(notify, &KNotification::closed, this, &Notification::notificationClosed);
    notify->setProperty("uni", connectionId);
    notify->setComponentName(QStringLiteral("networkmanagement"));
    if (!iconName.isEmpty()) {
        notify->setIconName(iconName);
    } else {
        if (state == NetworkManager::ActiveConnection::Activated) {
            notify->setIconName(QStringLiteral("dialog-information"));
        } else {
            notify->setIconName(QStringLiteral("dialog-warning"));
        }
    }
    notify->setTitle(acName);
    notify->setText(text);
    notify->sendEvent();
    if (notify->id() != -1) {
        m_notifications[connectionId] = notify;
    }
}

void Notification::onVpnConnectionStateChanged(NetworkManager::VpnConnection::State state, NetworkManager::VpnConnection::StateChangeReason reason)
{
    NetworkManager::VpnConnection *vpn = qobject_cast<NetworkManager::VpnConnection*>(sender());

    QString eventId, text;
    const QString vpnName = vpn->connection()->name();
    const QString connectionId = vpn->path();

    if (state == NetworkManager::VpnConnection::Activated) {
        eventId = QStringLiteral("ConnectionActivated");
        text = i18n("VPN connection '%1' activated.", vpnName);
    } else if (state == NetworkManager::VpnConnection::Failed) {
        eventId = QStringLiteral("FailedToActivateConnection");
        text = i18n("VPN connection '%1' failed.", vpnName);
    } else if (state == NetworkManager::VpnConnection::Disconnected) {
        eventId = QStringLiteral("ConnectionDeactivated");
        text = i18n("VPN connection '%1' disconnected.", vpnName);
    } else {
        qCWarning(PLASMA_NM) << "Unhandled VPN connection state change: " << state;
        return;
    }

    switch (reason) {
    case NetworkManager::VpnConnection::UserDisconnectedReason:
        text = i18n("The VPN connection changed state because the user disconnected it.");
        break;
    case NetworkManager::VpnConnection::DeviceDisconnectedReason:
        text = i18n("The VPN connection changed state because the device it was using was disconnected.");
        break;
    case NetworkManager::VpnConnection::ServiceStoppedReason:
        text = i18n("The service providing the VPN connection was stopped.");
        break;
    case NetworkManager::VpnConnection::IpConfigInvalidReason:
        text = i18n("The IP config of the VPN connection was invalid.");
        break;
    case NetworkManager::VpnConnection::ConnectTimeoutReason:
        text = i18n("The connection attempt to the VPN service timed out.");
        break;
    case NetworkManager::VpnConnection::ServiceStartTimeoutReason:
        text = i18n("A timeout occurred while starting the service providing the VPN connection.");
        break;
    case NetworkManager::VpnConnection::ServiceStartFailedReason:
        text = i18n("Starting the service providing the VPN connection failed.");
        break;
    case NetworkManager::VpnConnection::NoSecretsReason:
        text = i18n("Necessary secrets for the VPN connection were not provided.");
        break;
    case NetworkManager::VpnConnection::LoginFailedReason:
        text = i18n("Authentication to the VPN server failed.");
        break;
    case NetworkManager::VpnConnection::ConnectionRemovedReason:
        text = i18n("The connection was deleted from settings.");
        break;
    default:
    case NetworkManager::VpnConnection::UnknownReason:
    case NetworkManager::VpnConnection::NoneReason:
        break;
    }

    KNotification *notify = new KNotification(eventId, KNotification::CloseOnTimeout, this);
    connect(notify, &KNotification::closed, this, &Notification::notificationClosed);
    notify->setProperty("uni", connectionId);
    notify->setComponentName("networkmanagement");
    if (state == NetworkManager::VpnConnection::Activated) {
        notify->setIconName(QStringLiteral("dialog-information"));
    } else {
        notify->setIconName(QStringLiteral("dialog-warning"));
    }
    notify->setTitle(vpnName);
    notify->setText(text);
    notify->sendEvent();
    if (notify->id() != -1) {
        m_notifications[connectionId] = notify;
    }
}

void Notification::notificationClosed()
{
    KNotification *notify = qobject_cast<KNotification*>(sender());
    m_notifications.remove(notify->property("uni").toString());
    notify->deleteLater();
}

void Notification::onPrepareForSleep(bool sleep)
{
    m_preparingForSleep = sleep;

    if (m_checkActiveConnectionOnResumeTimer) {
        m_checkActiveConnectionOnResumeTimer->stop();
    }

    if (sleep) {
        // store all active notifications so we don't show a "is connected" notification
        // on resume if we were connected previously
        m_activeConnectionsBeforeSleep.clear();
        const auto &connections = NetworkManager::activeConnections();
        for (const auto &connection : connections) {
            if (!connection->vpn() && connection->state() == NetworkManager::ActiveConnection::State::Activated) {
                m_activeConnectionsBeforeSleep << connection->uuid();
            }
        }
    } else {
        if (!m_checkActiveConnectionOnResumeTimer) {
            m_checkActiveConnectionOnResumeTimer = new QTimer(this);
            m_checkActiveConnectionOnResumeTimer->setInterval(10000);
            m_checkActiveConnectionOnResumeTimer->setSingleShot(true);
            connect(m_checkActiveConnectionOnResumeTimer, &QTimer::timeout, this, &Notification::onCheckActiveConnectionOnResume);
        }

        m_checkActiveConnectionOnResumeTimer->start();
    }
}

void Notification::onCheckActiveConnectionOnResume()
{
    if (m_activeConnectionsBeforeSleep.isEmpty()) {
        // if we weren't connected before, don't bother telling us now :)
        return;
    }

    m_activeConnectionsBeforeSleep.clear();

    const auto &connections = NetworkManager::activeConnections();
    for (const auto &connection : connections) {
        if (connection->state() == NetworkManager::ActiveConnection::State::Activated ||
            connection->state() == NetworkManager::ActiveConnection::State::Activating) {
            // we have an active or activating connection, don't tell the user we're no longer connected
            return;
        }
    }

    KNotification *notify = new KNotification(QStringLiteral("NoLongerConnected"), KNotification::CloseOnTimeout, this);
    connect(notify, &KNotification::closed, this, &Notification::notificationClosed);
    const QString uni = QStringLiteral("offlineNotification");
    notify->setProperty("uni", uni);
    notify->setComponentName("networkmanagement");
    notify->setIconName(QStringLiteral("dialog-warning"));
    notify->setTitle(i18n("No Network Connection"));
    notify->setText(i18n("You are no longer connected to a network."));
    notify->sendEvent();
    if (notify->id() != -1) {
        m_notifications[uni] = notify;
    }
}
