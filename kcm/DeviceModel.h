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

#ifndef DEVICE_MODEL_H
#define DEVICE_MODEL_H

#include <QStandardItemModel>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusMessage>

#include <QtNetworkManager/manager.h>

typedef QMap<QString, QString>  StringStringMap;
typedef QList<QDBusObjectPath> ObjectPathList;

class DeviceModel : public QStandardItemModel
{
    Q_OBJECT
public:
    typedef enum {
        DeviceUNI = Qt::UserRole + 1,
        ConectionUNI,
        ParentObjectPathRole,
        IsDeviceRole,
        SortRole,
        FilenameRole,
        ColorspaceRole,
        ProfileKindRole,
        CanRemoveProfileRole
    } DeviceRoles;
    explicit DeviceModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

public slots:
    void serviceOwnerChanged(const QString &serviceName, const QString &oldOwner, const QString &newOwner);

signals:
    void changed();

private slots:
    void deviceAdded(const QString &uni);
    void deviceChanged(const QString &uni);
    void deviceRemoved(const QString &uni);
    void addDevice(const NetworkManager::Device::Ptr &device);
    void changeDevice(QStandardItem *stdItem, const NetworkManager::Device::Ptr &device);

private:
    QStandardItem* createProfileItem(const QDBusObjectPath &objectPath,
                                     const QDBusObjectPath &parentObjectPath,
                                     bool checked);
    QStandardItem* findProfile(QStandardItem *parent, const QDBusObjectPath &objectPath);
    void removeProfilesNotInList(QStandardItem *parent, const ObjectPathList &profiles);
    int findDeviceItem(const QString &uni);
};

#endif // DEVICE_MODEL_H
