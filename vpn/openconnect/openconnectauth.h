/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2013 Lukáš Tinkl <ltinkl@redhat.com>

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

#ifndef OPENCONNECTAUTH_H
#define OPENCONNECTAUTH_H

#include "settingwidget.h"

#include <NetworkManagerQt/VpnSetting>

#include <QString>

class QLayout;
struct openconnect_info;
struct oc_auth_form;

class OpenconnectAuthWidgetPrivate;

class OpenconnectAuthWidget : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OpenconnectAuthWidget)
public:
    explicit OpenconnectAuthWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    ~OpenconnectAuthWidget();
    virtual void readSecrets();
    void readConfig();
    virtual QVariantMap setting(bool agentOwned = false) const;

private:
    OpenconnectAuthWidgetPrivate * d_ptr;
    void acceptDialog();
    void addFormInfo(const QString &, const QString &);
    void deleteAllFromLayout(QLayout *);

private slots:
    void writeNewConfig(const QString &);
    void validatePeerCert(const QString &, const QString &, const QString &, bool*);
    void processAuthForm(struct oc_auth_form *);
    void updateLog(const QString &, const int &);
    void logLevelChanged(int);
    void formLoginClicked();
    void workerFinished(const int&);
    void viewServerLogToggled(bool);
    void passwordModeToggled(bool);
    void connectHost();
};

#endif // OPENCONNECTAUTH_H
