/*
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

#ifndef PLASMA_NM_CONNECTION_DETAIL_EDITOR_H
#define PLASMA_NM_CONNECTION_DETAIL_EDITOR_H

#include <QDialog>
#include <QDBusPendingCallWatcher>

#include <NetworkManagerQt/ConnectionSettings>

namespace Ui
{
class ConnectionDetailEditor;
}

class Q_DECL_EXPORT ConnectionDetailEditor : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionDetailEditor(const NetworkManager::ConnectionSettings::Ptr &connection,
                                    bool newConnection = false, QWidget* parent = 0, Qt::WindowFlags f = 0);

    virtual ~ConnectionDetailEditor();

    NMVariantMapMap setting();

private Q_SLOTS:
    void replyFinished(QDBusPendingCallWatcher *watcher);
    void validChanged(bool valid);
private:
    NetworkManager::ConnectionSettings::Ptr m_connection;
    bool m_new;
    Ui::ConnectionDetailEditor * m_ui;

    void enableOKButton(bool enabled);
    void initEditor();
    void initTabs();
};

#endif // PLASMA_NM_CONNECTION_DETAIL_EDITOR_H
