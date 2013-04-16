/*
Copyright 2009 Will Stephenson <wstephenson@kde.org>
Copyright 2013 by Daniel Nicoletti <dantti12@gmail.com>

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

#include "interfacenotification.h"

#include <uiutils.h>

#include <QtNetworkManager/manager.h>

#include <KLocalizedString>
#include <KNotification>
#include <KDebug>

InterfaceNotification::InterfaceNotification(QObject *parent) :
    QObject(parent)
{
    foreach (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        addDevice(device);
    }
}

void InterfaceNotification::deviceAdded(const QString &uni)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    addDevice(device);
}

void InterfaceNotification::addDevice(const NetworkManager::Device::Ptr &device)
{
    connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
            this, SLOT(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)));
}

void InterfaceNotification::stateChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    kDebug() << newstate << reason;
    NetworkManager::Device *device = qobject_cast<NetworkManager::Device*>(sender());
    QString title;
    QString text;
    KNotification::NotificationFlag flag = KNotification::Persistent;

    QString identifier = UiUtils::prettyInterfaceName(device->type(), device->interfaceName());
    QString stateString = UiUtils::connectionStateToString((NetworkManager::Device::State)newstate);
    kDebug() << stateString << reason;
    /*
    // need to keep the notification object around to reset it during connection cycles, but
    // delete it at the end of a connection cycle
    // keep a map of interface to KNotification
    // if not end of connection cycle, look for a
    // if set and not end of connection cycle, reuse this notification
    bool keepNotification = false;

    if (newstate == NetworkManager::Device::Preparing
    || newstate == NetworkManager::Device::Configuring
    || newstate == NetworkManager::Device::NeedAuth
    || newstate == NetworkManager::Device::IPConfig) {
    keepNotification = true;
    }
    */
    // if it's a connecting state, get the name of the InterfaceConnection that is activating
    // identify which connection is actually activating.  This is ugly because NM should signal
    // when it activates
//    Knm::InterfaceConnection * activatingConnection = 0;
//    if (!m_activating.isEmpty()) {
//        activatingConnection = *(m_activating.begin());
//    }

    //X     QSetIterator<Knm::InterfaceConnection*> it(m_interfaceConnections);
    //X
    //X     while (it.hasNext()) {
    //X         Knm::InterfaceConnection * ic = it.next();
    //X         if (ic->activationState() == Knm::InterfaceConnection::Activating) {
    //X             activatingConnection = ic;
    //X             break;
    //X         }

//    switch (newstate) {
//        case NetworkManager::Device::Preparing:
//        case NetworkManager::Device::ConfiguringHardware:
//        case NetworkManager::Device::NeedAuth:
//        case NetworkManager::Device::ConfiguringIp:
//        case NetworkManager::Device::CheckingIp:
//        case NetworkManager::Device::WaitingForSecondaries:
//            if (activatingConnection) {
//                title = i18nc("@info:status interface (%2) status notification title when a connection (%1) is activating",
//                        "Activating %1 on %2", activatingConnection->connectionName(), identifier);
//            } else {
//                title = identifier;
//            }
//            flag = KNotification::Persistent;
//            break;
//        case NetworkManager::Device::Activated:
//        default:
//            if (activatingConnection) {
//                title = i18nc("@info:status interface (%2) status notification title when a connection (%1) has successfully activated",
//                        "%1 on %2", activatingConnection->connectionName(), identifier);
//            } else {
//                title = identifier;
//            }
//            flag = KNotification::CloseOnTimeout;
//            break;
//    }

    if (newstate != NetworkManager::Device::Failed) {
        return;
    }

    switch (reason) {
        case NetworkManager::Device::NoReason:
        case NetworkManager::Device::UnknownReason:
            text = stateString;
            break;
        case NetworkManager::Device::NowManagedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to NowManagedReason",
                         "%1 because it is now being managed", stateString);
            break;
        case NetworkManager::Device::NowUnmanagedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to NowUnmanagedReason","%1 because it is no longer being managed", stateString );
            break;
        case NetworkManager::Device::ConfigFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ConfigFailedReason","%1 because configuration failed", stateString);
            break;
        case NetworkManager::Device::ConfigUnavailableReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ConfigUnavailableReason","%1 because the configuration is unavailable", stateString);
            break;
        case NetworkManager::Device::ConfigExpiredReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ConfigExpiredReason","%1 because the configuration has expired", stateString);
            break;
        case NetworkManager::Device::NoSecretsReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to NoSecretsReason","%1 because secrets were not provided", stateString);
            break;
        case NetworkManager::Device::AuthSupplicantDisconnectReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AuthSupplicantDisconnectReason","%1 because the authorization supplicant disconnected", stateString);
            break;
        case NetworkManager::Device::AuthSupplicantConfigFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AuthSupplicantConfigFailedReason","%1 because the authorization supplicant's configuration failed", stateString);
            break;
        case NetworkManager::Device::AuthSupplicantFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AuthSupplicantFailedReason","%1 because the authorization supplicant failed", stateString);
            break;
        case NetworkManager::Device::AuthSupplicantTimeoutReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AuthSupplicantTimeoutReason","%1 because the authorization supplicant timed out", stateString);
            break;
        case NetworkManager::Device::PppStartFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to PppStartFailedReason","%1 because PPP failed to start", stateString);
            break;
        case NetworkManager::Device::PppDisconnectReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to PppDisconnectReason","%1 because PPP disconnected", stateString);
            break;
        case NetworkManager::Device::PppFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to PppFailedReason","%1 because PPP failed", stateString);
            break;
        case NetworkManager::Device::DhcpStartFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to DhcpStartFailedReason","%1 because DHCP failed to start", stateString);
            break;
        case NetworkManager::Device::DhcpErrorReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to DhcpErrorReason","%1 because a DHCP error occurred", stateString);
            break;
        case NetworkManager::Device::DhcpFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to DhcpFailedReason","%1 because DHCP failed ", stateString);
            break;
        case NetworkManager::Device::SharedStartFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to SharedStartFailedReason","%1 because the shared service failed to start", stateString);
            break;
        case NetworkManager::Device::SharedFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to SharedFailedReason","%1 because the shared service failed", stateString);
            break;
        case NetworkManager::Device::AutoIpStartFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AutoIpStartFailedReason","%1 because the auto IP service failed to start", stateString);
            break;
        case NetworkManager::Device::AutoIpErrorReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AutoIpErrorReason","%1 because the auto IP service reported an error", stateString);
            break;
        case NetworkManager::Device::AutoIpFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to AutoIpFailedReason","%1 because the auto IP service failed", stateString);
            break;
        case NetworkManager::Device::ModemBusyReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemBusyReason","%1 because the modem is busy", stateString);
            break;
        case NetworkManager::Device::ModemNoDialToneReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemNoDialToneReason","%1 because the modem has no dial tone", stateString);
            break;
        case NetworkManager::Device::ModemNoCarrierReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemNoCarrierReason","%1 because the modem shows no carrier", stateString);
            break;
        case NetworkManager::Device::ModemDialTimeoutReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemDialTimeoutReason","%1 because the modem dial timed out", stateString);
            break;
        case NetworkManager::Device::ModemDialFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemDialFailedReason","%1 because the modem dial failed", stateString);
            break;
        case NetworkManager::Device::ModemInitFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemInitFailedReason","%1 because the modem could not be initialized", stateString);
            break;
        case NetworkManager::Device::GsmApnSelectFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to GsmApnSelectFailedReason","%1 because the GSM APN could not be selected", stateString);
            break;
        case NetworkManager::Device::GsmNotSearchingReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to GsmNotSearchingReason","%1 because the GSM modem is not searching", stateString);
            break;
        case NetworkManager::Device::GsmRegistrationDeniedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to GsmRegistrationDeniedReason","%1 because GSM network registration was denied", stateString);
            break;
        case NetworkManager::Device::GsmRegistrationTimeoutReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to GsmRegistrationTimeoutReason","%1 because GSM network registration timed out", stateString);
            break;
        case NetworkManager::Device::GsmRegistrationFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to GsmRegistrationFailedReason","%1 because GSM registration failed", stateString);
            break;
        case NetworkManager::Device::GsmPinCheckFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to GsmPinCheckFailedReason","%1 because the GSM PIN check failed", stateString);
            break;
        case NetworkManager::Device::FirmwareMissingReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to FirmwareMissingReason","%1 because firmware is missing", stateString);
            break;
        case NetworkManager::Device::DeviceRemovedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to DeviceRemovedReason","%1 because the device was removed", stateString);
            break;
        case NetworkManager::Device::SleepingReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to SleepingReason","%1 because the networking system is now sleeping", stateString);
            break;
        case NetworkManager::Device::ConnectionRemovedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ConnectionRemovedReason","%1 because the connection was removed", stateString);
            break;
        case NetworkManager::Device::UserRequestedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to UserRequestedReason","%1 by request", stateString);
            break;
        case NetworkManager::Device::CarrierReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to CarrierReason","%1 because the cable was disconnected", stateString);
            break;
        case NetworkManager::Device::ConnectionAssumedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ConnectionAssumedReason","%1 because the device's existing connection was assumed", stateString);
            break;
        case NetworkManager::Device::SupplicantAvailableReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to SupplicantAvailableReason","%1 because the supplicant is now available", stateString);
            break;
        case NetworkManager::Device::ModemNotFoundReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to ModemNotFoundReason","%1 because the modem could not be found", stateString);
            break;
        case NetworkManager::Device::BluetoothFailedReason:
            text = i18nc("@info:status Notification when an interface changes state (%1) due to BluetoothFailedReason","%1 because the bluetooth connection failed or timed out", stateString);
            break;
        case NetworkManager::Device::Reserved:
            break;
    }
    kDebug() << stateString << text;
    if (!text.isEmpty()) {
//        performInterfaceNotification(title, text, KIcon(Knm::Connection::iconName(m_type)).pixmap(QSize(iconSize,iconSize)), flag);
    }
}
