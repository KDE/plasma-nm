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
#ifndef DEVICE_CONNECTION_SORT_FILTER_MODEL_H
#define DEVICE_CONNECTION_SORT_FILTER_MODEL_H

#include <QSortFilterProxyModel>

#include <QDeclarativeItem>

#include <kdemacros.h>

class KDE_EXPORT DeviceConnectionSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DeviceConnectionSortFilterModel(QObject *parent = 0);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

public slots:
    void setShowInactiveConnections(bool show);

private:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

    bool m_showInactiveConnections;
};

#endif // DEVICE_CONNECTION_SORT_FILTER_MODEL_H
