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

#include "TabConnectionInfo.h"
#include "ui_TabConnectionInfo.h"

#include "uiutils.h"

#include <NetworkManagerQt/ActiveConnection>
#include <NetworkManagerQt/Manager>

#include <KGlobal>
#include <KLocalizedString>
#include <KLocale>
#include <KIconLoader>

#define CONNECTION_ICON_SIZE 64

using namespace NetworkManager;

TabConnectionInfo::TabConnectionInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabConnectionInfo)
{
    ui->setupUi(this);
}

TabConnectionInfo::~TabConnectionInfo()
{
    delete ui;
}

void TabConnectionInfo::setConnection(const NetworkManager::Connection::Ptr &connection)
{
    if (m_connection && m_connection->path() == connection->path()) {
        return;
    }

    if (m_connection) {
        m_connection->disconnect(this);
    }
    m_connection = connection;
    if (connection) {
        connect(connection.data(), SIGNAL(updated()),
                this, SLOT(updateState()));
        updateState();
    }
}

void TabConnectionInfo::updateState()
{
    QDateTime dateTime = m_connection->settings()->timestamp();
    if (dateTime.isValid()) {
        ui->lastUsedL->setText(KGlobal::locale()->formatDateTime(dateTime));
    } else {
        ui->lastUsedL->setText(i18n("never"));
    }
    ui->nameL->setText(m_connection->name());
    QStringList devices;
    foreach (const ActiveConnection::Ptr &activeConnection, NetworkManager::activeConnections()) {
        if (activeConnection->connection()->path() == m_connection->path()) {
            foreach (const QString &deviceUNI, activeConnection->devices()) {
                Device::Ptr device = NetworkManager::findNetworkInterface(deviceUNI);
                if (device) {
                    devices << UiUtils::prettyInterfaceName(device->type(), device->interfaceName());
                }
            }
        }
    }

    if (devices.isEmpty()) {
        ui->statusL->setText(i18n("not active"));
        ui->connectDisconnectPB->setText(i18n("Connect"));
    } else {
        ui->statusL->setText(i18n("active on %1", devices.join(QLatin1String(", "))));
        ui->connectDisconnectPB->setText(i18n("Disconnect"));
    }

    QString title;
    QString iconName;
    iconName = UiUtils::iconAndTitleForConnectionSettingsType(m_connection->settings()->connectionType(),
                                                              title);
    ui->typeL->setText(title);

    QPixmap icon = KIconLoader::global()->loadIcon(iconName,
                                                   KIconLoader::NoGroup,
                                                   CONNECTION_ICON_SIZE, // a not so huge icon
                                                   KIconLoader::DefaultState);
    ui->iconL->setPixmap(icon);
}

void TabConnectionInfo::on_connectDisconnectPB_clicked()
{
    bool disconnected = false;
    foreach (const ActiveConnection::Ptr &activeConnection, NetworkManager::activeConnections()) {
        if (activeConnection->connection()->path() == m_connection->path()) {
            NetworkManager::deactivateConnection(activeConnection->path());
            disconnected = true;
        }
    }

    if (disconnected) {
        // TODO find a device to activate this connection
    }
}
