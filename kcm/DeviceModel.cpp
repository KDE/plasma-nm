/***************************************************************************
 *   Copyright (C) 2012 by Daniel Nicoletti                                *
 *   dantti12@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#include "DeviceModel.h"

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusReply>
#include <QStringBuilder>

#include <KDebug>
#include <KLocale>
#include <KDateTime>
#include <KIcon>

DeviceModel::DeviceModel(QObject *parent) :
    QStandardItemModel(parent)
{
    qDBusRegisterMetaType<ObjectPathList>();

    // Creates a ColorD interface, it must be created with new
    // otherwise the object will be deleted when this block ends
    QDBusInterface *interface;
    interface = new QDBusInterface(QLatin1String("org.freedesktop.ColorManager"),
                                   QLatin1String("/org/freedesktop/ColorManager"),
                                   QLatin1String("org.freedesktop.ColorManager"),
                                   QDBusConnection::systemBus(),
                                   this);
    // listen to colord for events
    connect(interface, SIGNAL(DeviceAdded(QDBusObjectPath)),
            this, SLOT(deviceAdded(QDBusObjectPath)));
    connect(interface, SIGNAL(DeviceRemoved(QDBusObjectPath)),
            this, SLOT(deviceRemoved(QDBusObjectPath)));
    connect(interface, SIGNAL(DeviceChanged(QDBusObjectPath)),
            this, SLOT(deviceChanged(QDBusObjectPath)));

    foreach (const NetworkManager::Device::Ptr &device, NetworkManager::networkInterfaces()) {
        addDevice(device);
    }
}

void DeviceModel::deviceAdded(const QString &uni)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    if (device) {
        addDevice(device);
    }
}

void DeviceModel::deviceChanged(const QString &uni)
{
    int row = findDeviceItem(uni);
    if (row == -1) {
        kWarning() << "Device not found" << uni;
        return;
    }

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    if (device) {
        changeDevice(item(row), device);
    } else {
        deviceRemoved(uni);
    }
}

void DeviceModel::deviceRemoved(const QString &uni)
{
    int row = findDeviceItem(uni);
    if (row != -1) {
        removeRow(row);
    }
}

void DeviceModel::addDevice(const NetworkManager::Device::Ptr &device)
{
    int row = findDeviceItem(device->uni());
    if (row != -1) {
        return;
    }

    QStandardItem *stdItem = new QStandardItem;
    stdItem->setData(device->uni(), DeviceUNI);
    changeDevice(stdItem, device);
    appendRow(stdItem);
}

void DeviceModel::changeDevice(QStandardItem *stdItem, const NetworkManager::Device::Ptr &device)
{
    switch (device->type()) {
    case NetworkManager::Device::Wifi:
        stdItem->setIcon(KIcon(QLatin1String("network-wireless")));
        stdItem->setText(i18n("Wireless Interface (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Ethernet:
        stdItem->setIcon(KIcon(QLatin1String("network-wired")));
        stdItem->setText(i18n("Wired Interface (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Bluetooth:
        stdItem->setIcon(KIcon(QLatin1String("bluetooth")));
        stdItem->setText(i18n("Bluetooth (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Modem:
        stdItem->setIcon(KIcon(QLatin1String("bluetooth")));
        stdItem->setText(i18n("Modem (%1)", device->interfaceName()));
        break;
    case NetworkManager::Device::Adsl:
        stdItem->setIcon(KIcon(QLatin1String("bluetooth")));
        stdItem->setText(i18n("ADSL (%1)", device->interfaceName()));
        break;
    default:
        stdItem->setText(device->interfaceName());
    }
}

void DeviceModel::serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(serviceName)
    if (newOwner.isEmpty() || oldOwner != newOwner) {
        // colord has quit or restarted
        removeRows(0, rowCount());
        emit changed();
    }
}

QStandardItem* DeviceModel::createProfileItem(const QDBusObjectPath &objectPath,
                                              const QDBusObjectPath &parentObjectPath,
                                              bool checked)
{
    QDBusInterface interface(QLatin1String("org.freedesktop.ColorManager"),
                             objectPath.path(),
                             QLatin1String("org.freedesktop.ColorManager.Profile"),
                             QDBusConnection::systemBus(),
                             this);
    if (!interface.isValid()) {
        return 0;
    }

    QStandardItem *stdItem = new QStandardItem;
    QString kind = interface.property("Kind").toString();
    QString filename = interface.property("Filename").toString();
    QString title = interface.property("Title").toString();
    qulonglong created = interface.property("Created").toULongLong();

    // Sets the profile title
    bool canRemoveProfile = true;
    if (title.isEmpty()) {
        QString colorspace = interface.property("Colorspace").toString();
        if (colorspace == QLatin1String("rgb")) {
            title = i18nc("colorspace", "Default RGB");
        } else if (colorspace == QLatin1String("cmyk")) {
            title = i18nc("colorspace", "Default CMYK");
        } else if (colorspace == QLatin1String("gray")) {
            title = i18nc("colorspace", "Default Gray");
        }
        canRemoveProfile = false;
    } else {

    }
    stdItem->setText(title);
    stdItem->setData(canRemoveProfile, CanRemoveProfileRole);

    stdItem->setData(qVariantFromValue(parentObjectPath), ParentObjectPathRole);
    stdItem->setData(filename, FilenameRole);
    stdItem->setData(kind, ProfileKindRole);
    stdItem->setCheckable(true);
    stdItem->setCheckState(checked ? Qt::Checked : Qt::Unchecked);

    return stdItem;
}

QStandardItem *DeviceModel::findProfile(QStandardItem *parent, const QDBusObjectPath &objectPath)
{
    return 0;
}

void DeviceModel::removeProfilesNotInList(QStandardItem *parent, const ObjectPathList &profiles)
{
}

int DeviceModel::findDeviceItem(const QString &uni)
{

    return -1;
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section == 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return i18n("Devices");
    }
    return QVariant();
}

bool DeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value)
    Q_UNUSED(role)



    // We return false since colord will emit a DeviceChanged signal telling us about this change
    return false;
}

Qt::ItemFlags DeviceModel::flags(const QModelIndex &index) const
{
    QStandardItem *stdItem = itemFromIndex(index);
    if (stdItem && stdItem->isCheckable() && stdItem->checkState() == Qt::Unchecked) {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
