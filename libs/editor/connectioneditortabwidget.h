/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_CONNECTION_EDITOR_TAB_WIDGET_H
#define PLASMA_NM_CONNECTION_EDITOR_TAB_WIDGET_H

#include "connectioneditorbase.h"

#include <QDBusPendingCallWatcher>
#include <QDialog>

namespace Ui
{
class ConnectionEditorTabWidget;
}

class Q_DECL_EXPORT ConnectionEditorTabWidget : public ConnectionEditorBase
{
Q_OBJECT
public:
    explicit ConnectionEditorTabWidget(const NetworkManager::ConnectionSettings::Ptr &connection,
                                       QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~ConnectionEditorTabWidget();

    void setConnection(const NetworkManager::ConnectionSettings::Ptr &connection) override;

protected:
    void addWidget(QWidget *widget, const QString &text) override;
    QString connectionName() const override;

private:
    Ui::ConnectionEditorTabWidget *m_ui;

    void initializeTabWidget(const NetworkManager::ConnectionSettings::Ptr &connection);
};

#endif // PLASMA_NM_CONNECTION_EDITOR_TAB_WIDGET_H

