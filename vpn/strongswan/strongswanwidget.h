/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2010 Maurus Rohrer <maurus.rohrer@gmail.com>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#ifndef STRONGSWANWIDGET_H
#define STRONGSWANWIDGET_H

#include "settingwidget.h"

#include <NetworkManagerQt/VpnSetting>

class KComboBox;
class StrongswanSettingWidgetPrivate;

class StrongswanSettingWidget : public SettingWidget
{
Q_OBJECT
Q_DECLARE_PRIVATE(StrongswanSettingWidget)
public:
    explicit StrongswanSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    ~StrongswanSettingWidget();

    virtual void loadConfig(const NetworkManager::Setting::Ptr &setting);
    virtual QVariantMap setting(bool agentOwned = false) const;

    virtual bool isValid() const;

protected Q_SLOTS:
    void userPasswordTypeChanged(int);
    void privateKeyPasswordTypeChanged(int);
    void pinTypeChanged(int);
    void methodChanged(int);
    void showPasswordsChanged(bool show);
private:
    void fillOnePasswordCombo(KComboBox * combo, const QString & key, const NMStringMap & data, bool hasPassword);
    uint handleOnePasswordType(const KComboBox * combo, const QString & key, NMStringMap & data, bool agentOwned) const;
    StrongswanSettingWidgetPrivate * const d_ptr;
};

#endif // STRONGSWANWIDGET_H
