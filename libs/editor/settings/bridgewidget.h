/*
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_BRIDGE_WIDGET_H
#define PLASMA_NM_BRIDGE_WIDGET_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>

#include <NetworkManagerQt/BridgeSetting>

#include "settingwidget.h"
#include "plasmanm_export.h"

namespace Ui
{
class BridgeWidget;
}

class PLASMA_NM_EXPORT BridgeWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit BridgeWidget(const QString & masterUuid, const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(),
                 QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~BridgeWidget();

    void loadConfig(const NetworkManager::Setting::Ptr &setting);

    QVariantMap setting(bool agentOwned = false) const;

    virtual bool isValid() const;

private slots:
    void addBridge(QAction * action);
    void currentBridgeChanged(QListWidgetItem * current, QListWidgetItem * previous);
    void bridgeAddComplete(const QString &uuid, bool success, const QString & msg);

    void editBridge();
    void deleteBridge();

    void populateBridges();

private:
    QString m_uuid;
    Ui::BridgeWidget * m_ui;
    QMenu * m_menu;
};

#endif // PLASMA_NM_BRIDGE_WIDGET_H
