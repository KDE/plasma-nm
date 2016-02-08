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

#include "devicemodel.h"
#include "devicemodelitem.h"

#include "debug.h"
#include "uiutils.h"

#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>

DeviceModel::DeviceModel(QObject* parent)
    : QAbstractListModel(parent)
{
    QLoggingCategory::setFilterRules(QStringLiteral("plasma-nm.debug = false"));

    initialize();
}

DeviceModel::~DeviceModel()
{
}

QVariant DeviceModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_list.count()) {
        DeviceModelItem * item = m_list.itemAt(row);

        switch (role) {
            case ActiveConnectionRole:
                return item->activeConnection();
            case DeviceDetailsRole:
                return item->details();
            case DeviceIconRole:
                return item->icon();
                break;
            case DeviceLabelRole:
                return item->deviceLabel();
                break;
            case DeviceSubLabelRole:
                return item->deviceSubLabel();
                break;
            case DeviceNameRole:
                return item->deviceName();
                break;
            case DevicePathRole:
                return item->devicePath();
                break;
            case DeviceStateRole:
                return item->deviceState();
                break;
            case DeviceTypeRole:
                return item->deviceType();
                break;
            default:
                break;
        }
    }

    return QVariant();
}

int DeviceModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QHash< int, QByteArray > DeviceModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ActiveConnectionRole] = "ActiveConnection";
    roles[DeviceDetailsRole] = "DeviceDetails";
    roles[DeviceIconRole] = "DeviceIcon";
    roles[DeviceLabelRole] = "DeviceLabel";
    roles[DeviceSubLabelRole] = "DeviceSubLabel";
    roles[DeviceNameRole] = "DeviceName";
    roles[DevicePathRole] = "DevicePath";
    roles[DeviceStateRole] = "DeviceState";
    roles[DeviceTypeRole] = "DeviceType";

    return roles;
}

void DeviceModel::initialize()
{
    // Initialize existing devices
    Q_FOREACH (const NetworkManager::Device::Ptr& dev, NetworkManager::networkInterfaces()) {
        addDevice(dev);
    }

    disambiguateDeviceNames();

    initializeSignals();
}

void DeviceModel::initializeSignals()
{
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceAdded, this, &DeviceModel::deviceAdded, Qt::UniqueConnection);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceRemoved, this, &DeviceModel::deviceRemoved, Qt::UniqueConnection);
}

void DeviceModel::initializeSignals(const NetworkManager::Device::Ptr &device)
{
    connect(device.data(), &NetworkManager::Device::activeConnectionChanged, this, &DeviceModel::activeConnectionChanged);
    connect(device.data(), &NetworkManager::Device::ipInterfaceChanged, this, &DeviceModel::ipInterfaceChanged);
    connect(device.data(), &NetworkManager::Device::stateChanged, this, &DeviceModel::deviceStateChanged, Qt::UniqueConnection);
}

void DeviceModel::addDevice(const NetworkManager::Device::Ptr& device)
{
    initializeSignals(device);

    if (supportedDevice(device)) {
        if (!m_list.contains(DeviceItemsList::Path, device->uni())) {
            DeviceModelItem *item = new DeviceModelItem();
            if (!device->activeConnection().isNull()) {
                item->setActiveConnection(device->activeConnection()->id());
            }
            if (device->ipInterfaceName().isEmpty()) {
                item->setDeviceName(device->interfaceName());
            } else {
                item->setDeviceName(device->ipInterfaceName());
            }
            item->setDevicePath(device->uni());
            item->setDeviceState(device->state());
            item->setDeviceType(device->type());
            item->setDeviceUdi(device->udi());
            // TODO other properties

            item->updateDetails();

            const int index = m_list.count();
            beginInsertRows(QModelIndex(), index, index);
            m_list.insertItem(item);
            endInsertRows();
            qCDebug(PLASMA_NM) << "New device " << item->deviceName() << " added";
        }
    }
}

void DeviceModel::activeConnectionChanged()
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    if (device) {
        Q_FOREACH (DeviceModelItem* item, m_list.returnItems(DeviceItemsList::Path, device->uni())) {
            if (!device->activeConnection().isNull()) {
                qCDebug(PLASMA_NM) << "Active connection changed to " << device->activeConnection()->id();
                item->setActiveConnection(device->activeConnection()->id());
                updateItem(item);
            }
        }
    }
}

void DeviceModel::deviceAdded(const QString& device)
{
    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);
    if (dev) {
        addDevice(dev);
        disambiguateDeviceNames();
    }
}

void DeviceModel::deviceRemoved(const QString& device)
{
    Q_FOREACH (DeviceModelItem * item, m_list.returnItems(DeviceItemsList::Path, device)) {
        const int row = m_list.indexOf(item);
        if (row >= 0) {
            qCDebug(PLASMA_NM) << "Device " << item->deviceName() << " removed completely";
            beginRemoveRows(QModelIndex(), row, row);
            m_list.removeItem(item);
            item->deleteLater();
            endRemoveRows();
        }
    }
}

void DeviceModel::deviceStateChanged(NetworkManager::Device::State state, NetworkManager::Device::State oldState, NetworkManager::Device::StateChangeReason reason)
{
    Q_UNUSED(oldState);
    Q_UNUSED(reason);

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    if (device) {
        Q_FOREACH (DeviceModelItem* item, m_list.returnItems(DeviceItemsList::Path, device->uni())) {
            item->setDeviceState(state);
            updateItem(item);
        }
    }
}

void DeviceModel::ipInterfaceChanged()
{
    NetworkManager::Device * device = qobject_cast<NetworkManager::Device*>(sender());
    if (device) {
        Q_FOREACH (DeviceModelItem * item, m_list.returnItems(DeviceItemsList::Path, device->uni())) {
            if (device->ipInterfaceName().isEmpty()) {
                item->setDeviceName(device->interfaceName());
            } else {
                item->setDeviceName(device->ipInterfaceName());
            }
            disambiguateDeviceNames();
        }
    }
}

void DeviceModel::disambiguateDeviceNames()
{
    QList<DeviceModelItem*> duplicates;
    Q_FOREACH (DeviceModelItem * item, m_list.items()) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(item->devicePath());
        item->setDeviceLabel(UiUtils::interfaceTypeLabel(item->deviceType(), device));
        updateItem(item);
    }

    duplicates = findDuplicates();
    if (duplicates.isEmpty()) {
        return;
    }

    /* Try prefixing bus name (eg, "Ethernet (PCI)" vs "Ethernet (USB)"  */
    Q_FOREACH (DeviceModelItem * item, duplicates) {
        if (item->deviceUdi().contains(QLatin1String("usb"))) {
            item->setDeviceLabel(QString("%1 (USB)").arg(item->deviceLabel()));
        } else if (item->deviceUdi().contains(QLatin1String("pci"))) {
            item->setDeviceLabel(QString("%1 (PCI)").arg(item->deviceLabel()));
        }
        updateItem(item);
    }

    duplicates = findDuplicates();
    if (duplicates.isEmpty()) {
        return;
    }

    /* If we still have duplicates, try to distinguish them by device name */
    Q_FOREACH (DeviceModelItem * item, duplicates) {
        item->setDeviceLabel(QString("%1 (%2)").arg(item->deviceLabel()).arg(item->deviceName()));
        updateItem(item);
    }
}

void DeviceModel::updateItem(DeviceModelItem* item)
{
    const int row = m_list.indexOf(item);

    if (row >= 0) {
        item->updateDetails();
        QModelIndex index = createIndex(row, 0);
        Q_EMIT dataChanged(index, index);
    }
}

bool DeviceModel::supportedDevice(const NetworkManager::Device::Ptr& device) const
{
    return device->type() == NetworkManager::Device::Ethernet ||
           device->type() == NetworkManager::Device::Wifi ||
           device->type() == NetworkManager::Device::Modem ||
           device->type() == NetworkManager::Device::Bluetooth;
}

QList< DeviceModelItem* > DeviceModel::findDuplicates()
{
    QList<DeviceModelItem*> duplicates;
    Q_FOREACH (DeviceModelItem * item, m_list.items()) {
        Q_FOREACH (DeviceModelItem * item2, m_list.items()) {
            if (!duplicates.contains(item) && item != item2 && item->deviceLabel() == item2->deviceLabel()) {
                duplicates << item << item2;
            }
        }
    }

    return duplicates;
}
