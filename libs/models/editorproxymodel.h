/*
    Copyright 2013-2018 Jan Grulich <jgrulich@redhat.com>

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

#include "networkmodelitem.h"

#include <QSortFilterProxyModel>

class Q_DECL_EXPORT EditorProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
Q_PROPERTY(QAbstractItemModel * sourceModel READ sourceModel WRITE setSourceModel)
public:
    explicit EditorProxyModel(QObject *parent = 0);
    ~EditorProxyModel() override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};


#endif // PLASMA_NM_EDITOR_PROXY_MODEL_H
