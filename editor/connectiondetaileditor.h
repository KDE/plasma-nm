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

#ifndef CONNECTION_DETAIL_EDITOR_H
#define CONNECTION_DETAIL_EDITOR_H

#include <QtGui/QDialog>

#include <QtNetworkManager/settings/connection.h>

namespace Ui
{
class ConnectionDetailEditor;
}

class ConnectionDetailEditor : public QDialog
{
    Q_OBJECT

public:
    ConnectionDetailEditor(NetworkManager::Settings::ConnectionSettings * connection, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~ConnectionDetailEditor();

private Q_SLOTS:
    void saveSetting();

private:
    Ui::ConnectionDetailEditor * m_detailEditor;
    NetworkManager::Settings::ConnectionSettings * m_connection;

    void initTabs();
    void addTab(NetworkManager::Settings::Setting::SettingType type);
};

#endif // CONNECTION_DETAIL_EDITOR_H
