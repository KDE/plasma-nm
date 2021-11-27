/*
    SPDX-FileCopyrightText: 2016 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_KCM_H
#define PLASMA_NM_KCM_H

#include "connectioneditortabwidget.h"
#include "handler.h"

#include <KCModule>
#include <ui_kcm.h>


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
    Handler *const m_handler;
    ConnectionEditorTabWidget *m_tabWidget = nullptr;
    QTimer *m_timer = nullptr;
    Ui::KCMForm *const m_ui;
};

#endif
