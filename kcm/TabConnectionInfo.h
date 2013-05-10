/***************************************************************************
 *   Copyright (C) 2012 by Daniel Nicoletti                                *
 *   dantti12@gmail.com                                                    *
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

#ifndef TABCONNECTIONINFO_H
#define TABCONNECTIONINFO_H

#include <QWidget>

#include <NetworkManagerQt/Connection>

namespace Ui {
class TabConnectionInfo;
}

class TabConnectionInfo : public QWidget
{
    Q_OBJECT
public:
    explicit TabConnectionInfo(QWidget *parent = 0);
    ~TabConnectionInfo();

    void setConnection(const NetworkManager::Connection::Ptr &connection);

private:
    Ui::TabConnectionInfo *ui;
};

#endif // TABCONNECTIONINFO_H
