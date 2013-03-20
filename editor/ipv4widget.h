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

#ifndef IPV4_WIDGET_H
#define IPV4_WIDGET_H

#include <QtGui/QWidget>
#include <QtNetworkManager/settings/ipv4.h>

#include "settingwidget.h"
#include "ui/ipv4routeswidget.h"

namespace Ui
{
class IPv4Widget;
}

class IPv4Widget : public SettingWidget
{
    Q_OBJECT

public:
    IPv4Widget(NetworkManager::Settings::Setting* setting = 0, QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~IPv4Widget();

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
    Ui::IPv4Widget * m_ui;
    NetworkManager::Settings::Ipv4Setting* m_ipv4Setting;

    class Private;
    Private *d;
};

#endif // IPV4_WIDGET_H
