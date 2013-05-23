/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "hwaddrcombobox.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/BluetoothDevice>
#include <NetworkManagerQt/OlpcMeshDevice>
#include <NetworkManagerQt/WimaxDevice>
#include <NetworkManagerQt/InfinibandDevice>
#include <NetworkManagerQt/BondDevice>
#include <NetworkManagerQt/BridgeDevice>
#include <NetworkManagerQt/VlanDevice>

HwAddrComboBox::HwAddrComboBox(QWidget *parent) :
    KComboBox(parent), m_dirty(false)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);

    connect(this, SIGNAL(editTextChanged(QString)), SLOT(editTextChanged(QString)));
    connect(this, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
}

QString HwAddrComboBox::hwAddress() const
{
    QString result;
    if (!m_dirty)
        result = itemData(currentIndex()).toString();
    else
        result = currentText(); // FIXME validate

    //qDebug() << "Result:" << currentIndex() << result;

    return result;
}

void HwAddrComboBox::editTextChanged(const QString &)
{
    m_dirty = true;
    emit hwAddressChanged();
}

void HwAddrComboBox::currentIndexChanged(int)
{
    m_dirty = false;
    emit hwAddressChanged();
}

void HwAddrComboBox::init(const NetworkManager::Device::Type &deviceType, const QString &address)
{
    m_initialAddress = address;

    //qDebug() << "Initial address:" << m_initialAddress;

    foreach(const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        const NetworkManager::Device::Type type = device->type();
        if (type == deviceType) {
            addAddressToCombo(device);
        }
    }

    int index = findData(m_initialAddress);
    if (index == -1) {
        insertItem(0, m_initialAddress, m_initialAddress);
        setCurrentIndex(0);
    } else {
        setCurrentIndex(index);
    }
}

void HwAddrComboBox::addAddressToCombo(const NetworkManager::Device::Ptr &device)
{
    const NetworkManager::Device::Type type = device->type();

    //qDebug() << "Adding to combo, type:" << type;

    QVariant data;
    if (type == NetworkManager::Device::Ethernet) {
        data = device->as<NetworkManager::WiredDevice>()->hardwareAddress();
    } else if (type == NetworkManager::Device::Wifi) {
        data = device->as<NetworkManager::WirelessDevice>()->hardwareAddress();
    } else if (type == NetworkManager::Device::Bluetooth) {
        data = device->as<NetworkManager::BluetoothDevice>()->hardwareAddress();
    } else if (type == NetworkManager::Device::OlpcMesh) {
        data = device->as<NetworkManager::OlpcMeshDevice>()->hardwareAddress();
    } else if (type == NetworkManager::Device::Wimax) {
        data = device->as<NetworkManager::WimaxDevice>()->hardwareAddress();
    } else if (type == NetworkManager::Device::InfiniBand) {
        data = device->as<NetworkManager::InfinibandDevice>()->hwAddress();
    } else if (type == NetworkManager::Device::Bond) {
        data = device->as<NetworkManager::BondDevice>()->hwAddress();
    } else if (type == NetworkManager::Device::Bridge) {
        data = device->as<NetworkManager::BridgeDevice>()->hwAddress();
    } else if (type == NetworkManager::Device::Vlan) {
        data = device->as<NetworkManager::VlanDevice>()->hwAddress();
    }

    //qDebug() << "Data:" << data;

    QString name;
    if (device->state() == NetworkManager::Device::Activated)
        name = device->ipInterfaceName();
    else
        name = device->interfaceName();

    //qDebug() << "Name:" << name;

    if (!data.isNull()) {
        if (name == data.toString()) {
            addItem(data.toString(), data);
        }
        else {
            addItem(QString("%1 (%2)").arg(data.toString()).arg(name), data);
        }
    }
}
