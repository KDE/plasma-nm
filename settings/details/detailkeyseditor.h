/*
    Copyright 2012 Lamarque V. Souza <lamarque@kde.org>
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

#ifndef PLASMA_NM_DETAIL_KEYS_EDITOR_H
#define PLASMA_NM_DETAIL_KEYS_EDITOR_H

#include <QWidget>
#include <QMap>
#include <QPair>

class QTreeWidgetItem;
class DetailKeysEditorPrivate;

class DetailKeysEditor : public QWidget
{
Q_OBJECT
Q_PROPERTY(QStringList currentDetails READ currentDetails WRITE loadSelectedElements NOTIFY currentDetailsChanged USER true)
Q_DECLARE_PRIVATE(DetailKeysEditor)
public:
    explicit DetailKeysEditor(QWidget * parent = 0);
    explicit DetailKeysEditor(const QStringList &, QWidget *parent = 0);
    ~DetailKeysEditor();

    QStringList currentDetails();

protected:
    DetailKeysEditorPrivate *d_ptr;

private:
    enum Columns {Name = 0, Key = 1};

    void loadSelectedElements(const QStringList & keys);
    void loadAllDetailElements();
    void setupCommon();
    QTreeWidgetItem * constructItem(const QString &);
    QMap<QString, QPair<QString, QString> > m_allDetailsElements;

Q_SIGNALS:
    void currentDetailsChanged(const QStringList & keys);
private Q_SLOTS:
    void upArrowClicked();
    void leftArrowClicked();
    void rightArrowClicked();
    void downArrowClicked();
};

#endif // PLASMA_NM_DETAIL_KEYS_EDITOR_H
