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

#include "deviceitemslist.h"
#include "devicemodelitem.h"

DeviceItemsList::DeviceItemsList(QObject* parent)
    : QObject(parent)
{
}

DeviceItemsList::~DeviceItemsList()
{
    qDeleteAll(m_items);
}

bool DeviceItemsList::contains(const DeviceItemsList::FilterType type, const QString& parameter) const
{
    Q_FOREACH (DeviceModelItem * item, m_items) {
        switch (type) {
            case DeviceItemsList::Name:
                if (item->deviceName() == parameter) {
                    return true;
                }
                break;
            case DeviceItemsList::Path:
                if (item->devicePath() == parameter) {
                    return true;
                }
                break;
            case DeviceItemsList::Type:
                break;
            default: break;
        }
    }

    return false;
}

int DeviceItemsList::count() const
{
    return m_items.count();
}

int DeviceItemsList::indexOf(DeviceModelItem* item) const
{
    return m_items.indexOf(item);
}

void DeviceItemsList::insertItem(DeviceModelItem* item)
{
    m_items << item;
}

DeviceModelItem* DeviceItemsList::itemAt(int index) const
{
    return m_items.at(index);
}

QList< DeviceModelItem* > DeviceItemsList::items() const
{
    return m_items;
}

void DeviceItemsList::removeItem(DeviceModelItem* item)
{
    m_items.removeAll(item);
}

QList< DeviceModelItem* > DeviceItemsList::returnItems(const DeviceItemsList::FilterType type, const QString& parameter, const QString& additionalParameter) const
{
    QList<DeviceModelItem*> result;

    Q_FOREACH (DeviceModelItem * item, m_items) {
        switch (type) {
            case DeviceItemsList::Name:
                if (item->deviceName() == parameter) {
                    result << item;
                }
                break;
            case DeviceItemsList::Path:
                if (item->devicePath() == parameter) {
                    result << item;
                }
                break;
            case DeviceItemsList::Type:
                break;
        }
    }

    return result;
}

QList< DeviceModelItem* > DeviceItemsList::returnItems(const DeviceItemsList::FilterType type, NetworkManager::Device::Type typeParameter) const
{
    QList<DeviceModelItem*> result;

    Q_FOREACH (DeviceModelItem * item, m_items) {
        if (type == DeviceItemsList::Type) {
            if (item->deviceType() == typeParameter) {
                result << item;
            }
        }
    }
    return result;
}
