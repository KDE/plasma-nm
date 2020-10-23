/*
    Copyright 2009 Andrey Batyiev <batyiev@gmail.com>
    Copyright 2015 Jan Grulich <jgrulich@redhat.com>

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

#include "editlistdialog.h"

#include <QLayout>
#include <QDialogButtonBox>

#include <KLineEdit>


EditListDialog::EditListDialog(QWidget *parent)
    : QDialog(parent)
{
    editListWidget = new KEditListWidget(this);
    editListWidget->setCheckAtEntering(true);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::accepted, this, &EditListDialog::dialogAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    setLayout(new QVBoxLayout);
    layout()->addWidget(editListWidget);
    layout()->addWidget(buttons);
}

EditListDialog::~EditListDialog()
{
}

void EditListDialog::setItems(const QStringList &items)
{
    editListWidget->setItems(items);
}

QStringList EditListDialog::items() const
{
    return editListWidget->items();
}

void EditListDialog::removeEmptyItems(QStringList &list)
{
    QStringList::iterator it = list.begin();
    const QStringList::iterator end = list.end();
    while (it != end) {
        if ((*it).trimmed().isEmpty()) {
            it = list.erase(it);
        } else {
            it++;
        }
    }
}

void EditListDialog::dialogAccepted()
{
    QStringList list = items();
    removeEmptyItems(list);
    Q_EMIT itemsEdited(list);
}

void EditListDialog::setValidator(const QValidator *validator)
{
    editListWidget->lineEdit()->setValidator(validator);
}

const QValidator * EditListDialog::validator() const
{
    return editListWidget->lineEdit()->validator();
}

void EditListDialog::setToolTip(const QString toolTip)
{
    editListWidget->lineEdit()->setToolTip(toolTip);
}
