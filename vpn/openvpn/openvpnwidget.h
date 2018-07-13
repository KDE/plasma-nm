/*
    Copyright 2008 Will Stephenson <wstephenson@kde.org>
    Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENVPNWIDGET_H
#define OPENVPNWIDGET_H

#include "settingwidget.h"

#include <QProcess>

#include "ui_openvpn.h"

#include <NetworkManagerQt/VpnSetting>

class QUrl;
class QLineEdit;

class OpenVpnSettingWidget : public SettingWidget
{
    Q_OBJECT
public:
    explicit OpenVpnSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);
    ~OpenVpnSettingWidget() override;

    void loadConfig(const NetworkManager::Setting::Ptr &setting) override;
    void loadSecrets(const NetworkManager::Setting::Ptr &setting) override;

    QVariantMap setting() const override;

    bool isValid() const override;

private Q_SLOTS:
    void updateStartDir(const QUrl &);
    void showAdvanced();

private:
    class Private;
    Private *d;
    void setPasswordType(QLineEdit *, int);
    void fillOnePasswordCombo(PasswordField *, NetworkManager::Setting::SecretFlags);
    void handleOnePasswordType(const PasswordField *, const QString &, NMStringMap &) const;
};

#endif // OPENVPNWIDGET_H
