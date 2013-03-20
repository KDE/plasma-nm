/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#ifndef CONNECTION_EDITOR_H
#define CONNECTION_EDITOR_H

#include <QtGui/QMainWindow>
#include <QtGui/QMenu>

#include <QtNetworkManager/settings/connection.h>

namespace Ui
{
class ConnectionEditor;
}

class QTreeWidgetItem;

class ConnectionEditor : public QMainWindow
{
Q_OBJECT

public:
    explicit ConnectionEditor(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~ConnectionEditor();

private Q_SLOTS:
    void initializeConnections();
    void currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void addConnection(QAction * action);
    void editConnection();

private:
    Ui::ConnectionEditor * m_editor;
    QMenu * m_menu;

    QString formatDateRelative(const QDateTime & lastUsed) const;
    QTreeWidgetItem * findTopLevelItem(const QString & type);
};

#endif // CONNECTION_EDITOR_H
