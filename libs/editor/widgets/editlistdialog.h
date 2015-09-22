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

#ifndef PLASMA_NM_EDIT_LIST_DIALOG_H
#define PLASMA_NM_EDIT_LIST_DIALOG_H

#include <QDialog>
#include <KEditListWidget>

class QValidator;

class EditListDialog : public QDialog
{
Q_OBJECT
public:
    explicit EditListDialog(QWidget *parent = nullptr);
    virtual ~EditListDialog();

    void setItems(const QStringList &items);
    QStringList items() const;

    void setValidator(const QValidator *validator);
    const QValidator *validator() const;

    void setToolTip(const QString toolTip);

Q_SIGNALS:
    void itemsEdited(const QStringList &items);

protected Q_SLOTS:
    void dialogAccepted();

private:
    KEditListWidget *editListWidget;

    void removeEmptyItems(QStringList &list);
};

#endif // PLASMA_NM_EDIT_LIST_DIALOG_H

