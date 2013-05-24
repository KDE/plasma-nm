/*
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

#include "ssidcombobox.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WirelessDevice>
#include <kicon.h>

#include <QtAlgorithms>

bool signalCompare(const NetworkManager::WirelessNetwork::Ptr & one, const NetworkManager::WirelessNetwork::Ptr & two) {
    return one->signalStrength() > two->signalStrength();
}

SsidComboBox::SsidComboBox(QWidget *parent) :
    KComboBox(parent), m_dirty(false)
{
    setEditable(true);
    setInsertPolicy(QComboBox::NoInsert);

    connect(this, SIGNAL(editTextChanged(QString)), SLOT(editTextChanged(QString)));
    connect(this, SIGNAL(currentIndexChanged(int)), SLOT(currentIndexChanged(int)));
}

QString SsidComboBox::ssid() const
{
    QString result;
    if (!m_dirty)
        result = itemData(currentIndex()).toString();
    else
        result = currentText(); // FIXME validate

    //qDebug() << "Result:" << currentIndex() << result;

    return result;
}

void SsidComboBox::editTextChanged(const QString &)
{
    m_dirty = true;
    emit ssidChanged();
}

void SsidComboBox::currentIndexChanged(int)
{
    m_dirty = false;
    emit ssidChanged();
}

void SsidComboBox::init(const QString &ssid)
{
    m_initialSsid = ssid;

    //qDebug() << "Initial ssid:" << m_initialSsid;

    QList<NetworkManager::WirelessNetwork::Ptr> networks;

    foreach(const NetworkManager::Device::Ptr & device, NetworkManager::networkInterfaces()) {
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();

            foreach (const NetworkManager::WirelessNetwork::Ptr & newNetwork, wifiDevice->networks()) {
                bool found = false;
                foreach (const NetworkManager::WirelessNetwork::Ptr & existingNetwork, networks) {
                    if (newNetwork->ssid() == existingNetwork->ssid()) {
                        if (newNetwork->signalStrength() > existingNetwork->signalStrength()) {
                            networks.removeOne(existingNetwork);
                            break;
                        } else {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found) {
                    networks << newNetwork;
                }
            }
        }
    }

    qSort(networks.begin(), networks.end(), signalCompare);
    addSsidsToCombo(networks);

    int index = findData(m_initialSsid);
    if (index == -1) {
        insertItem(0, m_initialSsid, m_initialSsid);
        setCurrentIndex(0);
    } else {
        setCurrentIndex(index);
    }
}

void SsidComboBox::addSsidsToCombo(const QList<NetworkManager::WirelessNetwork::Ptr> &networks)
{
    foreach (const NetworkManager::WirelessNetwork::Ptr & network, networks) {
        NetworkManager::AccessPoint::Ptr accessPoint = network->referenceAccessPoint();

        if (!accessPoint) {
            continue;
        }

        QString text = QString("%1 (%2%)").arg(accessPoint->ssid()).arg(network->signalStrength());
        if (accessPoint->capabilities() & NetworkManager::AccessPoint::Privacy) {
            addItem(KIcon("object-locked"), text, QVariant::fromValue(accessPoint->ssid()));
        } else {
            addItem(KIcon("object-unlocked"), text, QVariant::fromValue(accessPoint->ssid()));
        }
    }
}
