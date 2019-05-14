/*
    Copyright 2019 Bruce Anderson <banderson19com@san.rr.com>

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

#ifndef PLASMA_NM_WIREGUARD_INTERFACE_WIDGET_H
#define PLASMA_NM_WIREGUARD_INTERFACE_WIDGET_H

#include <QWidget>

#include "settingwidget.h"
#include <NetworkManagerQt/WireguardSetting>
#include "ui_wireguardinterfacewidget.h"

class Q_DECL_EXPORT WireGuardInterfaceWidget : public SettingWidget
{
Q_OBJECT

public:
    explicit WireGuardInterfaceWidget(const NetworkManager::Setting::Ptr &setting, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~WireGuardInterfaceWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;
    static QString supportedFileExtensions();
    static NMVariantMapMap importConnectionSettings(const QString &fileName);

private Q_SLOTS:
    void showPeers();

private:
    void setBackground(QWidget *w, bool result) const;
    void checkPrivateKeyValid();
    void checkFwmarkValid();
    void checkListenPortValid();
    class Private;
    Private *d;
};

#endif // PLASMA_NM_WIREGUARD_INTERFACE_WIDGET_H
