/*
    Copyright 2011 Lamarque Souza <lamarque@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
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

#include "bluetoothmonitor.h"
#include "bluetoothdbustype.h"
#include "connectiondetaileditor.h"
#include "dbusproperties.h"
#include "debug.h"
#include <config.h>

#include <QDBusPendingCall>
#include <QUuid>

#include <KLocalizedString>
#include <KMessageBox>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/BluetoothSetting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Utils>

#define BLUEZ_DBUS_SERVICE_NAME "org.bluez"
#define BLUEZ_DEVICE_DBUS_INTERFACE_NAME "org.bluez.Device1"
#define BLUEZ_ADAPTER_DBUS_INTERFACE_NAME "org.bluez.Adapter1"

BluetoothMonitor::BluetoothMonitor(QObject * parent)
    : QObject(parent)
     ,mServiceWatcher(new QDBusServiceWatcher(BLUEZ_DBUS_SERVICE_NAME, QDBusConnection::systemBus(), QDBusServiceWatcher::WatchForOwnerChange, this))
     ,mBluezInterface(nullptr)
{
    registerBluetoohDBusType();

    connect(mServiceWatcher, &QDBusServiceWatcher::serviceOwnerChanged, [this](const QString &, const QString &oldOwner, const QString &newOwner){
        /* old die */
        if (oldOwner.length() > 0 || newOwner.length() > 0) {
            delete mBluezInterface;
            mBluezInterface = nullptr;
        }

        /* new rise */
        if (newOwner.length() > 0) {
            mBluezInterface = new org::freedesktop::DBus::ObjectManager(BLUEZ_DBUS_SERVICE_NAME, "/", QDBusConnection::systemBus(), this);
        }
    });
    if (QDBusConnection::systemBus().interface()->isServiceRegistered(BLUEZ_DBUS_SERVICE_NAME)) {
        mBluezInterface = new org::freedesktop::DBus::ObjectManager(BLUEZ_DBUS_SERVICE_NAME, "/", QDBusConnection::systemBus(), this);
    }
}

BluetoothMonitor::~BluetoothMonitor()
{
}

void BluetoothMonitor::addBluetoothConnection(const QString& bdAddr, const QString& service)
{
    qCDebug(PLASMA_NM) << "Adding BT connection:" << bdAddr << service;

    if (bdAddr.isEmpty() || service.isEmpty()) {
        return;
    }

    QRegExp rx("dun|rfcomm?|nap");

    if (rx.indexIn(service) < 0) {
        KMessageBox::sorry(0, i18n("Only 'dun' and 'nap' services are supported."));
        return;
    }
    qCDebug(PLASMA_NM) << "Bdaddr == " << bdAddr;

    if (!mBluezInterface) {
        KMessageBox::error(0, i18n("Could not contact Bluetooth manager (BlueZ)."));
        return;
    }

    QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(mBluezInterface->GetManagedObjects(), this);
    connect(callWatcher, &QDBusPendingCallWatcher::finished, [this, bdAddr, service](QDBusPendingCallWatcher *callWatcher){
        callWatcher->deleteLater();
        QDBusPendingReply<DBusManagedObjectMap> reply = *callWatcher;

        if (reply.isError()) {
            return;
        }

        // Every org.freedesktop.DBus.Properties.GetAll callback hold a reference on getPropertiesWatchers.
        QSharedPointer<QSet<QDBusPendingCallWatcher *>> getPropertiesWatchers(new QSet<QDBusPendingCallWatcher *>());
        auto managedObjects = reply.value();
        for(auto managedObjectIt = managedObjects.constBegin(); managedObjectIt != managedObjects.constEnd(); ++managedObjectIt) {
            const QString path = managedObjectIt.key().path();
            const DBusManagedObject &interfaces = managedObjectIt.value();
            if (interfaces.contains(BLUEZ_DEVICE_DBUS_INTERFACE_NAME)) {
                org::freedesktop::DBus::Properties deviceInterface(BLUEZ_DBUS_SERVICE_NAME, path, QDBusConnection::systemBus());
                *getPropertiesWatchers << new QDBusPendingCallWatcher(deviceInterface.GetAll(BLUEZ_DEVICE_DBUS_INTERFACE_NAME), this);
            }
        }

        Q_FOREACH (QDBusPendingCallWatcher *getPropertiesWatcher, *getPropertiesWatchers) {
            connect(getPropertiesWatcher, &QDBusPendingCallWatcher::finished, [this, bdAddr, service, getPropertiesWatchers](QDBusPendingCallWatcher *watcher) {
                // remove it from the shared set
                getPropertiesWatchers->remove(watcher);
                watcher->deleteLater();
                QDBusPendingReply<QVariantMap> reply = *watcher;

                if (reply.isError()) {
                    return;
                }

                const auto properties = reply.value();
                // Check required properties
                if (!properties.contains("Name") ||
                    !properties.contains("Adapter") ||
                    !properties.contains("Address") ||
                    !properties.contains("UUIDs")) {
                    return;
                }
                if (!properties["Address"].canConvert<QString>() ||
                    properties["Address"].toString() != bdAddr) {
                    return;
                }

                // Delete all other watcher since we already find the first match address
                Q_FOREACH (QDBusPendingCallWatcher *getPropertiesWatcher, *getPropertiesWatchers) {
                    delete getPropertiesWatcher;
                }
                getPropertiesWatchers->clear();

                if (!properties["UUIDs"].canConvert<QStringList>()) {
                    return;
                }

                // check support on dun and nap
                bool dun = false, nap = false;
                Q_FOREACH (const QString &u, properties["UUIDs"].toStringList()) {
                    QUuid uuid(u);
                    if (uuid.data1 == 0x1103) {
                        dun = true;
                    } else if (uuid.data1 == 0x1116) {
                        nap = true;
                    }
                }

                const auto deviceName = properties["Name"].toString();
                if (service != QLatin1String("nap") && !dun) {
                    KMessageBox::error(0, i18n("%1 (%2) does not support Dialup Networking (DUN).", deviceName, bdAddr));
                    return;
                }

                if (service == QLatin1String("nap") && !nap) {
                    KMessageBox::error(0, i18n("%1 (%2) does not support Network Access Point (NAP).", deviceName, bdAddr));
                    return;
                }

                org::freedesktop::DBus::Properties adapterInterface(BLUEZ_DBUS_SERVICE_NAME, properties["Adapter"].value<QDBusObjectPath>().path(), QDBusConnection::systemBus());
                QDBusPendingCallWatcher *adapterPoweredCall = new QDBusPendingCallWatcher(adapterInterface.Get(BLUEZ_ADAPTER_DBUS_INTERFACE_NAME, "Powered"), this);
                connect(adapterPoweredCall, &QDBusPendingCallWatcher::finished, [this, bdAddr, service, deviceName](QDBusPendingCallWatcher *watcher) {
                    watcher->deleteLater();
                    QDBusPendingReply<QDBusVariant> reply = *watcher;
                    if (reply.isError()) {
                        return;
                    }

                    auto value = reply.value();
                    if (!value.variant().canConvert<bool>() || !value.variant().toBool()) {
                        return;
                    }

                    bool exists = false;
                    Q_FOREACH (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
                        if (con && con->settings() && con->settings()->connectionType() == NetworkManager::ConnectionSettings::Bluetooth) {
                            NetworkManager::BluetoothSetting::Ptr btSetting = con->settings()->setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
                            if (btSetting->bluetoothAddress() == NetworkManager::macAddressFromString(bdAddr)) {
                                exists = true;
                                break;
                            }
                        }
                    }

                    if (!exists) {
                        if (service == QLatin1String("nap")) {
                            NetworkManager::ConnectionSettings connectionSettings(NetworkManager::ConnectionSettings::Bluetooth, NM_BT_CAPABILITY_NAP);
                            connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
                            connectionSettings.setId(deviceName);
                            NetworkManager::BluetoothSetting::Ptr btSetting = connectionSettings.setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
                            btSetting->setBluetoothAddress(NetworkManager::macAddressFromString(bdAddr));
                            btSetting->setProfileType(NetworkManager::BluetoothSetting::Panu);
                            btSetting->setInitialized(true);

                            NetworkManager::addConnection(connectionSettings.toMap());
                        }
#if WITH_MODEMMANAGER_SUPPORT
                        else if (service == QLatin1String("dun")) {
                            QPointer<MobileConnectionWizard> mobileConnectionWizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Bluetooth);
                            if (mobileConnectionWizard->exec() == QDialog::Accepted && mobileConnectionWizard->getError() == MobileProviders::Success) {
                                qCDebug(PLASMA_NM) << "Mobile broadband wizard finished:" << mobileConnectionWizard->type() << mobileConnectionWizard->args();
                                if (mobileConnectionWizard->args().count() == 2) { //GSM or CDMA
                                    qCDebug(PLASMA_NM) << "Creating new DUN connection for BT device:" << bdAddr;

                                    QVariantMap tmp = qdbus_cast<QVariantMap>(mobileConnectionWizard->args().value(1));
                                    NetworkManager::ConnectionSettings connectionSettings(NetworkManager::ConnectionSettings::Bluetooth, NM_BT_CAPABILITY_DUN);
                                    connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
                                    connectionSettings.setId(deviceName);
                                    NetworkManager::BluetoothSetting::Ptr btSetting = connectionSettings.setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
                                    btSetting->setBluetoothAddress(NetworkManager::macAddressFromString(bdAddr));
                                    btSetting->setProfileType(NetworkManager::BluetoothSetting::Dun);
                                    btSetting->setInitialized(true);

                                    if (mobileConnectionWizard->type() == NetworkManager::ConnectionSettings::Gsm) {
                                        connectionSettings.setting(NetworkManager::Setting::Gsm)->fromMap(tmp);
                                        connectionSettings.setting(NetworkManager::Setting::Gsm)->setInitialized(true);
                                    } else if (mobileConnectionWizard->type() == NetworkManager::ConnectionSettings::Cdma) {
                                        connectionSettings.setting(NetworkManager::Setting::Cdma)->fromMap(tmp);
                                        connectionSettings.setting(NetworkManager::Setting::Cdma)->setInitialized(true);
                                    }

                                    qCDebug(PLASMA_NM) << "Adding DUN connection" << connectionSettings;

                                    NetworkManager::addConnection(connectionSettings.toMap());
                                }
                            }

                            if (mobileConnectionWizard) {
                                delete mobileConnectionWizard;
                            }
                        }
#endif
                    }
                });
            });
        }
    });
}
