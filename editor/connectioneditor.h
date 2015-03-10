/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>
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

#ifndef PLASMA_NM_CONNECTION_EDITOR_H
#define PLASMA_NM_CONNECTION_EDITOR_H

#include "handler.h"

#include <QMenu>
#include <QModelIndex>

#include <KXmlGuiWindow>
#include <KActionMenu>

#include <NetworkManagerQt/Connection>

namespace Ui
{
class ConnectionEditor;
}

class QTreeWidgetItem;

class ConnectionEditor : public KXmlGuiWindow
{
Q_OBJECT

public:
    explicit ConnectionEditor(QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~ConnectionEditor();

public Q_SLOTS:
    void activateAndRaise();

private Q_SLOTS:
    void addConnection(QAction * action);
    void connectionAdded(const QString & connection);
    void connectConnection();
    void disconnectConnection();
    void editConnection();
    void exportVpn();
    void importVpn();
    void initializeConnections();
    void removeConnection();
    void slotContextMenuRequested(const QPoint& point);
    void slotSelectionChanged(const QModelIndex& index, const QModelIndex &previous);
    void slotItemDoubleClicked(const QModelIndex& index);

private:
    Ui::ConnectionEditor * m_editor;
    Handler * m_handler;
    KActionMenu * m_menu;

    void addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void initializeMenu();
    void importSecretsFromPlainTextFiles();
    void storeSecrets(const QMap<QString, QMap<QString, QString> > & map);
};

#endif // PLASMA_NM_CONNECTION_EDITOR_H
