/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_CONNECTION_WIDGET_H
#define PLASMA_NM_CONNECTION_WIDGET_H

#include <QtGui/QWidget>

#include <NetworkManagerQt/ConnectionSettings>

namespace Ui
{
class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
Q_OBJECT

public:
    explicit ConnectionWidget(const NetworkManager::ConnectionSettings::Ptr &settings = NetworkManager::ConnectionSettings::Ptr(),
                     QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~ConnectionWidget();

    void loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings);

    NMVariantMapMap setting() const;

private:
    // list of VPN: UUID, name
    NMStringMap vpnConnections() const;
    // list of firewalld zones
    QStringList firewallZones() const;

    void populateVpnConnections();
    Ui::ConnectionWidget * m_widget;
    NetworkManager::ConnectionSettings::ConnectionType m_type;
    QString m_masterUuid;
    QString m_slaveType;
};

#endif // PLASMA_NM_CONNECTION_WIDGET_H
