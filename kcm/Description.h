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

#ifndef DESCRIPTION_H
#define DESCRIPTION_H

#include <QWidget>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>

namespace Ui {
    class Description;
}
class Description : public QWidget
{
    Q_OBJECT
public:
    explicit Description(QWidget *parent = 0);
    ~Description();

    int innerHeight() const;

    void setDevice(const QString &uni);
    void setConnection(const QString &path);

private:
    Ui::Description *ui;
    NetworkManager::Device::Ptr m_device;
    NetworkManager::Connection::Ptr m_connection;
};

#endif // DESCRIPTION_H
