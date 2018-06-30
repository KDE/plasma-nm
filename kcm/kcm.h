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

#ifndef PLASMA_NM_KCM_H
#define PLASMA_NM_KCM_H

#include "connectioneditortabwidget.h"
#include "handler.h"

#include <KCModule>
#include <ui_kcm.h>

class QQuickView;

class KCMNetworkmanagement : public KCModule
{
    Q_OBJECT
public:
    explicit KCMNetworkmanagement(QWidget *parent = nullptr, const QVariantList &args = QVariantList());
    ~KCMNetworkmanagement() override;

public Q_SLOTS:
    void defaults() override;
    void load() override;
    void save() override;

private Q_SLOTS:
    void onConnectionAdded(const QString &connection);
    void onSelectedConnectionChanged(const QString &connectionPath);
    void onRequestCreateConnection(int connectionType, const QString &vpnType, const QString &specificType, bool shared);
    void onRequestExportConnection(const QString &connectionPath);
    void onRequestToChangeConnection(const QString &connectionName, const QString &connectionPath);

private:
    void addConnection(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void importVpn();
    void kcmChanged(bool kcmChanged);
    void loadConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connectionSettings);
    void resetSelection();

    QString m_currentConnectionPath;
    QString m_createdConnectionUuid;
    Handler *m_handler;
    ConnectionEditorTabWidget *m_tabWidget;
    QTimer *m_timer;
    Ui::KCMForm *m_ui;
    QQuickView *m_quickView;
};

#endif
