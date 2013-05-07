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

#include "Description.h"
#include "ui_Description.h"

#include "TabDeviceInfo.h"

#include <QStringBuilder>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <KGlobal>
#include <KLocale>
#include <KDateTime>
#include <KToolInvocation>
#include <KDebug>
#include <KMessageWidget>
#include <KMessageBox>

Description::Description(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Description)
{
    ui->setupUi(this);
}

Description::~Description()
{
    delete ui;
}

int Description::innerHeight() const
{
    return ui->tabWidget->currentWidget()->height();
}

void Description::setDevice(const QString &uni)
{
    int i = 0;
    while (i < ui->tabWidget->count()) {
        ui->tabWidget->removeTab(i);
    }

    TabDeviceInfo *tabDeviceInfo = new TabDeviceInfo(this);
    tabDeviceInfo->setDevice(uni);
    ui->tabWidget->addTab(tabDeviceInfo, i18n("Information"));
}

#include "Description.moc"
