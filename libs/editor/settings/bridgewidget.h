/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_BRIDGE_WIDGET_H
#define PLASMA_NM_BRIDGE_WIDGET_H

#include <QDBusPendingCallWatcher>
#include <QListWidgetItem>
#include <QMenu>
#include <QWidget>

#include <NetworkManagerQt/BridgeSetting>

#include "settingwidget.h"

namespace Ui
{
class BridgeWidget;
}

class Q_DECL_EXPORT BridgeWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit BridgeWidget(const QString &masterUuid,
                          const QString &masterId,
                          const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(),
                          QWidget *parent = nullptr,
                          Qt::WindowFlags f = {});
    ~BridgeWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

private Q_SLOTS:
    void addBridge(QAction *action);
    void currentBridgeChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void bridgeAddComplete(QDBusPendingCallWatcher *watcher);

    void editBridge();
    void deleteBridge();

    void populateBridges();

private:
    QString m_uuid;
    QString m_id;
    Ui::BridgeWidget *const m_ui;
    QMenu *m_menu = nullptr;
};

#endif // PLASMA_NM_BRIDGE_WIDGET_H
