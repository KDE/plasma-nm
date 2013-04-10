/***************************************************************************
 *   Copyright (C) 2012 by Daniel Nicoletti <dantti12@gmail.com>           *
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

#include <QStringBuilder>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

#include <KGlobal>
#include <KLocale>
#include <KDateTime>
#include <KToolInvocation>
#include <KDebug>
#include <KMessageWidget>

#include <QtNetworkManager/manager.h>
#include <QtNetworkManager/ipconfig.h>
#include <QtNetworkManager/settings/connection.h>
#include <QtNetworkManager/activeconnection.h>

typedef QList<QDBusObjectPath> ObjectPathList;
typedef QMap<QString, QString>  StringStringMap;

using namespace NetworkManager;

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
    while (ui->tabWidget->count() > 1) {
        ui->tabWidget->removeTab(1);
    }

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(uni);
    m_device = device;
    if (device) {
        QString state;
        switch (device->state()) {
        case Device::Unavailable:
            state = i18n("Unavailable");
            break;
        case Device::Disconnected:
            state = i18n("Disconnected");
            break;
        case Device::Activated:
            state = i18n("Connected");
            break;
        default:
            break;
        }
        ui->statusL->setText(state);
        ui->disconnectPB->setEnabled(device->state() == Device::Activated);

        ui->connectionCB->clear();
        foreach (const Settings::Connection::Ptr &connection, device->availableConnections()) {
            ui->connectionCB->addItem(connection->name(), connection->uuid());
        }

        int active = -1;
        if (device->activeConnection()) {
            active = ui->connectionCB->findData(device->activeConnection()->connection()->uuid());
        }
        ui->connectionCB->setCurrentIndex(active);


        QStringList routers;
        QStringList addresses;
        foreach (const NetworkManager::IpAddress &address, device->ipV4Config().addresses()) {
            addresses << address.ip().toString() % QLatin1Char('/') % QString::number(address.prefixLength());
            routers << address.gateway().toString();
        }
        ui->ipv4AddressL->setText(addresses.join(QLatin1String("\n")));


        foreach (const NetworkManager::IpRoute &route, device->ipV4Config().routes()) {
            routers << i18n("%1/%2, nexthop %3 metric %4",
                            route.ip().toString(),
                            QString::number(route.prefixLength()),
                            route.nextHop().toString(),
                            QString::number(route.metric()));
        }
        ui->ipv4RouterL->setText(routers.join(QLatin1String("\n")));

        QStringList nameservers;
        foreach (const QHostAddress &nameserver, device->ipV4Config().nameservers()) {
            nameservers << nameserver.toString();
        }
        ui->ipv4DnsServerL->setText(nameservers.join(QLatin1String(", ")));

        ui->ipv4SearchDomainsL->setText(device->ipV4Config().domains().join(QLatin1String("\n")));
    }
}

void Description::on_disconnectPB_clicked()
{
    kDebug() << m_device;
    if (m_device) {
        kDebug() << m_device->state() << NetworkManager::Device::Activated;
        if (m_device->state() == NetworkManager::Device::Activated) {
            m_device->disconnectInterface();
        } else {
            ui->disconnectPB->setEnabled(false);
        }
    }
}

#include "Description.moc"
