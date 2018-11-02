/*
    Copyright (c) 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef PLASMA_NM_SECURITY8021X_H
#define PLASMA_NM_SECURITY8021X_H

#include <QRegExpValidator>
#include <QWidget>

#include <NetworkManagerQt/Security8021xSetting>

#include "settingwidget.h"

namespace Ui
{
class Security8021x;
}

class Q_DECL_EXPORT Security8021x: public SettingWidget
{
    Q_OBJECT
public:
    explicit Security8021x(const NetworkManager::Setting::Ptr &setting = NetworkManager::Setting::Ptr(), bool wifiMode = true, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~Security8021x() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

private Q_SLOTS:
    void altSubjectMatchesButtonClicked();
    void connectToServersButtonClicked();
    void currentAuthChanged(int index);

private:
    NetworkManager::Security8021xSetting::Ptr m_setting;
    Ui::Security8021x *m_ui;
    QRegExpValidator *altSubjectValidator;
    QRegExpValidator *serversValidator;
};

#endif // SECURITY8021X_H
