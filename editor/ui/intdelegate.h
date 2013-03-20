/*
Copyright 2011 Ilia Kats <ilia-kats@gmx.net>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INTDELEGATE_H
#define INTDELEGATE_H

#include <QLineEdit>
#include <QStandardItem>
#include <QStandardItemModel>

#include "delegate.h"

class IntDelegate : public Delegate
{
    Q_OBJECT
public:
    IntDelegate(QObject * parent = 0);
    IntDelegate(int min, int max, QObject * parent = 0);
    virtual ~IntDelegate();

    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    int m_min;
    int m_max;
    bool m_boundary;
};

#endif
