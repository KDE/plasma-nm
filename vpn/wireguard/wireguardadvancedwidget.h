/*
    Copyright 2018 Bruce Anderson <banderson19com@san.rr.com>

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

#ifndef PLASMA_NM_WIREGUARD_ADVANCED_WIDGET_H
#define PLASMA_NM_WIREGUARD_ADVANCED_WIDGET_H

#include "passwordfield.h"

#include <QDialog>

#include <NetworkManagerQt/VpnSetting>

namespace Ui
{
class WireGuardAdvancedWidget;
}

class QLineEdit;
class WireGuardAdvancedWidget : public QDialog
{
    Q_OBJECT

public:
    explicit WireGuardAdvancedWidget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent = nullptr);
    ~WireGuardAdvancedWidget() override;
    NetworkManager::VpnSetting::Ptr setting() const;

private:
    void loadConfig();
    void setProperty(NMStringMap &data, const QLatin1String &key, const QString &value) const;
    void setProperty(NMStringMap &data, const QLatin1String &key, const int value) const;
    void checkPresharedKey();
    void checkTable();
    void checkFwMark();
    void setBackground(QWidget *w, bool result) const;
    void slotWidgetChanged();
    class Private;
    Private *const d;
};

#endif // PLASMA_NM_WIREGUARD_ADVANCED_WIDGET_H
