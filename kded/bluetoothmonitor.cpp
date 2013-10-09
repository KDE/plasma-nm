/*
    Copyright 2011 Lamarque Souza <lamarque@kde.org>
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

#include "bluetoothmonitor.h"
#include "connectiondetaileditor.h"
#include "config.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QUuid>

#include <QDebug>
#include <KStandardDirs>
#include <KLocalizedString>
#include <KMessageBox>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/BluetoothSetting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Utils>

#ifdef MODEMMANAGERQT_ONE
#include <ModemManagerQt/modem.h>
#include <ModemManagerQt/modemdevice.h>
#endif

BluetoothMonitor::BluetoothMonitor(QObject * parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().registerService("org.kde.plasmanetworkmanagement");
    QDBusConnection::sessionBus().registerObject("/org/kde/plasmanetworkmanagement", this, QDBusConnection::ExportScriptableContents);
}

BluetoothMonitor::~BluetoothMonitor()
{
}

void BluetoothMonitor::addBluetoothConnection(const QString& bdAddr, const QString& service)
{
    qDebug() << "Adding BT connection:" << bdAddr << service;

    if (bdAddr.isEmpty() || service.isEmpty()) {
        return;
    }

    mBdaddr = bdAddr;
    mService = service.toLower();
    if (mService == "dun") {
#if WITH_MODEMMANAGER_SUPPORT
        connect(ModemManager::notifier(), SIGNAL(modemAdded(QString)),
                SLOT(modemAdded(QString)));
#endif
    }
    init();
}

void BluetoothMonitor::init()
{
    QRegExp rx("dun|rfcomm?|nap");

    if (rx.indexIn(mService) < 0) {
        KMessageBox::sorry(0, i18n("We support 'dun' and 'nap' services only."));
        return;
    }
    qDebug() << "Bdaddr == " << mBdaddr;

    /*
     * Find default bluetooth adapter registered in BlueZ.
     */

    QDBusInterface bluez(QLatin1String("org.bluez"), QLatin1String("/"),
                         QLatin1String("org.bluez.Manager"), QDBusConnection::systemBus());

    if (!bluez.isValid()) {
        KMessageBox::error(0, i18n("Could not contact bluetooth manager (BlueZ)."));
        return;
    }

    qDebug() << "Querying default adapter";
    QDBusReply<QDBusObjectPath> adapterPath = bluez.call(QLatin1String("DefaultAdapter"));

    if (!adapterPath.isValid()) {
        KMessageBox::error(0, i18n("Default bluetooth adapter not found: %1", adapterPath.error().message()));
        return;
    }

    qDebug() << "Default adapter path is " << adapterPath.value().path();

    /*
     * Find device path registered in BlueZ.
     */

    QDBusInterface adapter(QLatin1String("org.bluez"), adapterPath.value().path(),
                           QLatin1String("org.bluez.Adapter"), QDBusConnection::systemBus());

    QDBusReply<QDBusObjectPath> devicePath = adapter.call(QLatin1String("FindDevice"), mBdaddr);

    if (!devicePath.isValid()) {
        qWarning() << mBdaddr << " is not registered in default bluetooth adapter, it may be in another adapter.";
        qWarning() << mBdaddr << " waiting for it to be registered in ModemManager";
        return;
    }

    mDevicePath = devicePath.value().path();
    qDebug() << "Device path for " << mBdaddr << " is " << mDevicePath;

    /*
     * Find name registered in BlueZ.
     */

    // get device properties
    QDBusInterface device(QLatin1String("org.bluez"), mDevicePath,
                          QLatin1String("org.bluez.Device"), QDBusConnection::systemBus());

    QDBusReply<QMap<QString, QVariant> > deviceProperties = device.call(QLatin1String("GetProperties"));

    if (!deviceProperties.isValid()) {
        return;
    }

    QMap<QString, QVariant> properties = deviceProperties.value();
    qDebug() << "Device properties == " << properties;

    if (properties.contains("Name")) {
        qDebug() << "Name for" << mBdaddr << "is" << properties["Name"].toString();
        mDeviceName = properties["Name"].toString();
    }

    /*
     * Check if phone supports the requested service.
     */
    bool dun = false, nap = false;
    if (properties.contains("UUIDs")) {
        foreach (const QString &u, properties["UUIDs"].toStringList()) {
            QUuid uuid(u);
            if (uuid.data1 == 0x1103) {
                dun = true;
            } else if (uuid.data1 == 0x1116) {
                nap = true;
            }
        }
    }

    if (mService != QLatin1String("nap") && !dun) {
        KMessageBox::error(0, i18n("%1 (%2) does not support Dialup Networking (DUN).", mDeviceName, mBdaddr));
        return;
    }

    if (mService == QLatin1String("nap") && !nap) {
        KMessageBox::error(0, i18n("%1 (%2) does not support Network Access Point (NAP).", mDeviceName, mBdaddr));
        return;
    }

    if (mService == QLatin1String("nap")) {
        bool exists = false;

        foreach (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
            if (con && con->settings() && con->settings()->connectionType() == NetworkManager::ConnectionSettings::Bluetooth) {
                NetworkManager::BluetoothSetting::Ptr btSetting = con->settings()->setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
                if (NetworkManager::Utils::macAddressFromString(btSetting->bluetoothAddress()) == mBdaddr) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            NetworkManager::ConnectionSettings connectionSettings(NetworkManager::ConnectionSettings::Bluetooth, NM_BT_CAPABILITY_NAP);
            connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
            connectionSettings.setId(mDeviceName);
            NetworkManager::BluetoothSetting::Ptr btSetting = connectionSettings.setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
            btSetting->setBluetoothAddress(NetworkManager::Utils::macAddressFromString(mBdaddr));
            btSetting->setProfileType(NetworkManager::BluetoothSetting::Panu);
            btSetting->setInitialized(true);
            qDebug() << "Adding PAN connection" << connectionSettings;

            NetworkManager::addConnection(connectionSettings.toMap());
        }
        return;
    } else if (mService != QLatin1String("dun")) {
        mDunDevice = mService;
        qWarning() << "device(" << mDunDevice << ") for" << mBdaddr << " passed as argument";
        qWarning() << "waiting for it to be registered in ModemManager";
        return;
    }

    qDebug() << "Connecting to modem's" << mDevicePath << "serial DUN port with" << mService;

    /*
     * Contact BlueZ to connect phone's service.
     */
    QDBusInterface serial(QLatin1String("org.bluez"), mDevicePath,
                          QLatin1String("org.bluez.Serial"), QDBusConnection::systemBus());

    QDBusReply<QString> reply = serial.call(QLatin1String("Connect"), mService);
    if (!reply.isValid()) {
        KMessageBox::error(0, i18n("Error activating devices's serial port: %1", reply.error().message()));
        return;
    }

    mDunDevice = reply.value();
}
#if WITH_MODEMMANAGER_SUPPORT
void BluetoothMonitor::modemAdded(const QString &udi)
{
    qDebug() << "Modem added" << udi;

#ifdef MODEMMANAGERQT_ONE
    ModemManager::ModemDevice::Ptr modemDevice = ModemManager::findModemDevice(udi);
    ModemManager::Modem::Ptr modem = modemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();

#else
    ModemManager::ModemInterface::Ptr modem = ModemManager::findModemInterface(udi, ModemManager::ModemInterface::GsmCard);

    if (!modem) {
        // Try CDMA if no GSM device has been found.
        modem = ModemManager::findModemInterface(udi, ModemManager::ModemInterface::NotGsm);
    }
#endif
#ifdef MODEMMANAGERQT_ONE
    qDebug() << "Found suitable modem:" << modemDevice->uni();
#else
    qDebug() << "Found suitable modem:" << modem->udi();
#endif
    qDebug() << "DUN device:" << mDunDevice;

    QStringList temp = mDunDevice.split('/');
    if (temp.count() == 3) {
        mDunDevice = temp[2];
    }

    if (!modem || modem->device() != mDunDevice) {

        if (modem) {
            KMessageBox::error(0, i18n("Device %1 is not the one we want (%2)", modem->device(), mDunDevice));
        } else {
            KMessageBox::error(0, i18n("Device for serial port %1 (%2) not found.", mDunDevice, udi));
        }
        return;
    }

    NetworkManager::ConnectionSettings::ConnectionType type;
#ifdef MODEMMANAGERQT_ONE
    if (modemDevice->isGsmModem())
        type = NetworkManager::ConnectionSettings::Gsm;
    else if (modemDevice->isCdmaModem())
        type = NetworkManager::ConnectionSettings::Cdma;
    else
        type = NetworkManager::ConnectionSettings::Unknown;
#else
    switch (modem->type()) {
        case ModemManager::ModemInterface::GsmType:
            type = NetworkManager::ConnectionSettings::Gsm;
            break;
        case ModemManager::ModemInterface::CdmaType:
            type = NetworkManager::ConnectionSettings::Cdma;
            break;
        default:
            type = NetworkManager::ConnectionSettings::Unknown;
    }
#endif

    if (type == NetworkManager::ConnectionSettings::Unknown) {
        return;
    }

    bool exists = false;

    foreach (const NetworkManager::Connection::Ptr &con, NetworkManager::listConnections()) {
        if (con && con->settings() && con->settings()->connectionType() == NetworkManager::ConnectionSettings::Bluetooth) {
            NetworkManager::BluetoothSetting::Ptr btSetting = con->settings()->setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
            if (btSetting->bluetoothAddress() == NetworkManager::Utils::macAddressFromString(mBdaddr)) {
                exists = true;
                break;
            }
        }
    }

    if (!exists) {
        mobileConnectionWizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Bluetooth);
        if (mobileConnectionWizard.data()->exec() == QDialog::Accepted && mobileConnectionWizard.data()->getError() == MobileProviders::Success) {
            qDebug() << "Mobile broadband wizard finished:" << mobileConnectionWizard.data()->type() << mobileConnectionWizard.data()->args();
            if (mobileConnectionWizard.data()->args().count() == 2) { //GSM or CDMA
                qDebug() << "Creating new PAN connection for BT device:" << mBdaddr;

                QVariantMap tmp = qdbus_cast<QVariantMap>(mobileConnectionWizard.data()->args().value(1));
                NetworkManager::ConnectionSettings connectionSettings(NetworkManager::ConnectionSettings::Bluetooth, NM_BT_CAPABILITY_DUN);
                connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());
                connectionSettings.setId(mDeviceName);
                NetworkManager::BluetoothSetting::Ptr btSetting = connectionSettings.setting(NetworkManager::Setting::Bluetooth).staticCast<NetworkManager::BluetoothSetting>();
                btSetting->setBluetoothAddress(NetworkManager::Utils::macAddressFromString(mBdaddr));
                btSetting->setProfileType(NetworkManager::BluetoothSetting::Dun);
                btSetting->setInitialized(true);

                if (mobileConnectionWizard.data()->type() == NetworkManager::ConnectionSettings::Gsm) {
                    connectionSettings.setting(NetworkManager::Setting::Gsm)->fromMap(tmp);
                    connectionSettings.setting(NetworkManager::Setting::Gsm)->setInitialized(true);
                } else if (mobileConnectionWizard.data()->type() == NetworkManager::ConnectionSettings::Cdma) {
                    connectionSettings.setting(NetworkManager::Setting::Cdma)->fromMap(tmp);
                    connectionSettings.setting(NetworkManager::Setting::Cdma)->setInitialized(true);
                }

                qDebug() << "Adding DUN connection" << connectionSettings;

                NetworkManager::addConnection(connectionSettings.toMap());
            }
        }

        if (mobileConnectionWizard) {
            mobileConnectionWizard.clear();
        }
    }
}
#endif
