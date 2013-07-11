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

#ifndef PLASMA_NM_SETTINGS_NETWORK_SETTINGS_MODEL_H
#define PLASMA_NM_SETTINGS_NETWORK_SETTINGS_MODEL_H

#include "networksettingmodelitem.h"

#include <QAbstractListModel>

#include <NetworkManagerQt/Device>

class NetworkSettingsModelItem;

class NetworkSettingsModel : public QAbstractListModel
{
Q_OBJECT
Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum ItemRole {TypeRole = Qt::UserRole + 1, NameRole, IconRole, PathRole};

    explicit NetworkSettingsModel(QObject *parent = 0);
    virtual ~NetworkSettingsModel();

    int count() const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private Q_SLOTS:

Q_SIGNALS:
    void countChanged();

private:
    void addConnection(const NetworkManager::Connection::Ptr &connection);
    void addDevice(const NetworkManager::Device::Ptr &device);

    QList<NetworkSettingModelItem*> m_networkItems;
};

#endif // PLASMA_NM_SETTINGS_NETWORK_SETTINGS_MODEL_H
