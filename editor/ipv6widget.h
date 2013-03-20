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

#ifndef IPV6_WIDGET_H
#define IPV6_WIDGET_H

#include <QtGui/QWidget>
#include <QtNetworkManager/settings/ipv6.h>

#include "settingwidget.h"
#include "ui/ipv6routeswidget.h"

namespace Ui
{
class IPv6Widget;
}

class IPv6Widget : public SettingWidget
{
    Q_OBJECT

public:
    IPv6Widget(NetworkManager::Settings::Setting* setting = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~IPv6Widget();

    void loadConfig(NetworkManager::Settings::Setting * setting);

    QVariantMap setting() const;

private slots:
    void slotModeComboChanged(int index);
    void slotRoutesDialog();

    void slotAddIPAddress();
    void slotRemoveIPAddress();

    void selectionChanged(const QItemSelection & selected);
    void tableViewItemChanged(QStandardItem * item);

private:
    Ui::IPv6Widget * m_ui;
    NetworkManager::Settings::Ipv6Setting* m_ipv6Setting;

    class Private;
    Private *d;
};

#endif // IPV4_WIDGET_H
