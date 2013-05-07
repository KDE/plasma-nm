/***************************************************************************
 *   Copyright (C) 2013 by Daniel Nicoletti <dantti12@gmail.com>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; see the file COPYING. If not, write to       *
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,  *
 *   Boston, MA 02110-1301, USA.                                           *
 ***************************************************************************/

#ifndef TABDEVICEINFO_H
#define TABDEVICEINFO_H

#include <QWidget>

#include <NetworkManagerQt/Device>

namespace Ui {
class TabDeviceInfo;
}

class AvailableConnectionsModel;
class AvailableConnectionsSortModel;
class TabDeviceInfo : public QWidget
{
    Q_OBJECT
    
public:
    explicit TabDeviceInfo(QWidget *parent = 0);
    ~TabDeviceInfo();

    void setDevice(const NetworkManager::Device::Ptr &device);

private slots:
    void updateState();
    void updateActiveConnection();
    void updateIpV4Config();

    void on_disconnectPB_clicked();
    void on_connectionCB_activated(int index);

private:
    Ui::TabDeviceInfo *ui;
    NetworkManager::Device::Ptr m_device;
    AvailableConnectionsModel *m_availableConnectionsModel;
    AvailableConnectionsSortModel *m_availableConnectionsSortModel;

};

#endif // TABDEVICEINFO_H
