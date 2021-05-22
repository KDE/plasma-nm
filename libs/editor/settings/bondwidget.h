/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_BOND_WIDGET_H
#define PLASMA_NM_BOND_WIDGET_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QDBusPendingCallWatcher>

#include <NetworkManagerQt/BondSetting>

#include "settingwidget.h"

namespace Ui
{
class BondWidget;
}

class Q_DECL_EXPORT BondWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit BondWidget(const QString & masterUuid, const QString &masterId,
                        const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(),
               QWidget* parent = nullptr, Qt::WindowFlags f = {});
    ~BondWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

private Q_SLOTS:
    void addBond(QAction * action);
    void currentBondChanged(QListWidgetItem * current, QListWidgetItem * previous);
    void bondAddComplete(QDBusPendingCallWatcher * watcher);

    void editBond();
    void deleteBond();

    void populateBonds();

private:
    QString m_uuid;
    QString m_id;
    Ui::BondWidget * m_ui;
    QMenu * m_menu;
};

#endif // PLASMA_NM_BOND_WIDGET_H
