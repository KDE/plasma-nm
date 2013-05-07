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

#ifndef TABDEVICEADVANCED_H
#define TABDEVICEADVANCED_H

#include <QWidget>
#include <QStandardItemModel>

#include <NetworkManagerQt/Device>

namespace Ui {
class TabDeviceAdvanced;
}

class TabDeviceAdvanced : public QWidget
{
    Q_OBJECT
    
public:
    explicit TabDeviceAdvanced(QWidget *parent = 0);
    ~TabDeviceAdvanced();

    void setDevice(const NetworkManager::Device::Ptr &device);
    
private:
    void addItem(const QString &key, const QString &value);
    Ui::TabDeviceAdvanced *ui;
    QStandardItemModel *m_model;
};

#endif // TABDEVICEADVANCED_H
