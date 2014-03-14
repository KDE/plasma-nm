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

#include "intdelegate.h"
#include <QIntValidator>

#include <KLineEdit>

IntDelegate::IntDelegate(QObject * parent) : Delegate(parent), m_boundary(false) {}
IntDelegate::IntDelegate(int min, int max, QObject * parent) : Delegate(parent), m_min(min), m_max(max), m_boundary(true) {}
IntDelegate::~IntDelegate() {}

QWidget * IntDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    KLineEdit *editor = new KLineEdit(parent);
    if (m_boundary)
        editor->setValidator(new QIntValidator(m_min, m_max, editor));
    else
        editor->setValidator(new QIntValidator(editor));

    return editor;
}
