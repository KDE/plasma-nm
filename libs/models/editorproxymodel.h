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

#ifndef PLASMA_NM_EDITOR_PROXY_MODEL_H
#define PLASMA_NM_EDITOR_PROXY_MODEL_H

#include <QSortFilterProxyModel>

#include "networkmodelitem.h"

class PLASMA_NM_EXPORT EditorProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
Q_PROPERTY(QAbstractItemModel * sourceModel READ sourceModel WRITE setSourceModel)
Q_PROPERTY(QByteArray sortRole READ sortRole WRITE setSortRole)
Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder)
Q_PROPERTY(QString filterString READ filterString WRITE setFilterString)

public:
    enum SortedConnectionType {Wired, Wireless, Wimax, Gsm, Cdma, Pppoe, Adsl, Infiniband, OLPCMesh, Bluetooth, Vpn, Vlan, Bridge, Bond, Unknown };

    static SortedConnectionType connectionTypeToSortedType(NetworkManager::ConnectionSettings::ConnectionType type);

    explicit EditorProxyModel(QObject* parent = 0);
    virtual ~EditorProxyModel();

    QByteArray sortRole() const;
    void setSortRole(const QByteArray& role);
    void setSortOrder(Qt::SortOrder order);

    QString filterString() const;
    void setFilterString(const QString& filter);

    Q_INVOKABLE QVariant get(int row, const QString& role);

protected:
    int roleKey(const QByteArray &role) const;
    QHash<int, QByteArray> roleNames() const;

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};


#endif // PLASMA_NM_EDITOR_PROXY_MODEL_H
