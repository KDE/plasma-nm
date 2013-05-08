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

#include <QWidget>

#include <NetworkManagerQt/settings/Security8021xSetting>

namespace Ui
{
class Security8021x;
}

class Security8021x: public QWidget
{
    Q_OBJECT
public:
    Security8021x(const NetworkManager::Settings::Security8021xSetting::Ptr &setting, bool wifiMode, QWidget *parent = 0);
    virtual ~Security8021x();
    QVariantMap setting(bool agentOwned = false) const;

private slots:
    void setShowMD5Password(bool on);
    void setShowTlsPrivateKeyPassword(bool on);
    void setShowLeapPassword(bool on);
    void setShowFastPassword(bool on);
    void setShowTtlsPassword(bool on);
    void setShowPeapPassword(bool on);

private:
    void loadConfig();
    NetworkManager::Settings::Security8021xSetting::Ptr m_setting;
    Ui::Security8021x * m_ui;
};

#endif // SECURITY8021X_H
