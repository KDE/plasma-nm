/*
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
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

#ifndef PPTPWIDGET_H
#define PPTPWIDGET_H

#include "settingwidget.h"

#include <NetworkManagerQt/VpnSetting>

class KComboBox;
class PptpSettingWidgetPrivate;

class PptpSettingWidget : public SettingWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PptpSettingWidget)
public:
    explicit PptpSettingWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent = 0);
    ~PptpSettingWidget();
    virtual void loadConfig(const NetworkManager::Setting::Ptr &setting);
    virtual QVariantMap setting(bool agentOwned = false) const;
    virtual bool isValid() const;

private Q_SLOTS:
    void doAdvancedDialog();
    void passwordTypeChanged(int);
    void setShowPassword(bool);

private:
    PptpSettingWidgetPrivate * d_ptr;
    void fillOnePasswordCombo(KComboBox *, NetworkManager::Setting::SecretFlags);
    uint handleOnePasswordType(const KComboBox *, const QString &, NMStringMap &, bool agentOwned) const;
};

#endif // PPTPWIDGET_H
