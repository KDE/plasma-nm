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

#ifndef PLASMA_NM_MODEL_ITEMS_H
#define PLASMA_NM_MODEL_ITEMS_H

#include <QtCore/QAbstractListModel>

class ModelItem;

#include "monitor.h"

class ModelItems : public QObject
{
Q_OBJECT
public:
    explicit ModelItems(QObject* parent = 0);
    virtual ~ModelItems();

    ModelItem * itemAt(int index) const;
    ModelItem * itemByActiveConnection(const QString& active) const;
    QList<ModelItem*> itemsByActiveConnection(const QString& active) const;
    QList<ModelItem*> itemsByConnection(const QString& connection) const;
    QList<ModelItem*> itemsByDevice(const QString& device) const;
    QList<ModelItem*> itemsByName(const QString& name) const;
    QList<ModelItem*> itemsByNsp(const QString& nsp) const;
    QList<ModelItem*> itemsByNsp(const QString& nsp, const QString& device) const;
    QList<ModelItem*> itemsBySsid(const QString& ssid) const;
    QList<ModelItem*> itemsBySsid(const QString& ssid, const QString& device) const;
    QList<ModelItem*> itemsByUuid(const QString& uuid) const;
    QList<ModelItem*> itemsByType(NetworkManager::ConnectionSettings::ConnectionType type) const;

    QList<ModelItem*> items() const;

    int count() const;
    int indexOf(ModelItem * item) const;

    void insertItem(ModelItem * item);
    void removeItem(ModelItem * item);

private:
    QList<ModelItem*> m_items;
};

#endif // PLASMA_NM_MODEL_ITEMS_H
