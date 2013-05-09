/*
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

#include "modelitems.h"
#include "modelitem.h"

ModelItems::ModelItems(QObject* parent): QObject(parent)
{
}

ModelItems::~ModelItems()
{
    qDeleteAll(m_items);
}

ModelItem* ModelItems::itemAt(int index) const
{
    return m_items.at(index);
}

ModelItem* ModelItems::itemByActiveConnection(const QString& active) const
{
    foreach (ModelItem * item, m_items) {
        if (item->activeConnectionPath() == active) {
            return item;
        }
    }

    return 0;
}

QList< ModelItem* > ModelItems::itemsByActiveConnection(const QString& active) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->activeConnectionPath() == active) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::itemsByConnection(const QString& connection) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->connectionPath() == connection) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::itemsByDevice(const QString& device) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->devicePath() == device) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::itemsBySsid(const QString& ssid) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->ssid() == ssid) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::itemsBySsid(const QString& ssid, const QString& device) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->ssid() == ssid && item->devicePath() == device) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::itemsByUuid(const QString& uuid) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->uuid() == uuid) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::itemsByType(NetworkManager::ConnectionSettings::ConnectionType type) const
{
    QList<ModelItem*> result;

    foreach (ModelItem * item, m_items) {
        if (item->type() == type) {
            result << item;
        }
    }

    return result;
}

QList< ModelItem* > ModelItems::items() const
{
    return m_items;
}

int ModelItems::count() const
{
    return m_items.count();
}

int ModelItems::indexOf(ModelItem* item)
{
    return m_items.indexOf(item);
}

void ModelItems::insertItem(ModelItem* item)
{
    m_items << item;
}

void ModelItems::removeItem(ModelItem* item)
{
    m_items.removeAll(item);
}
