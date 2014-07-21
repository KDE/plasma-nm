/*
    Copyright 2009 Dario Freddi <drf54321@gmail.com>
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2011-2012 Lamarque V. Souza <lamarque@kde.org>
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "service.h"

#include <KPluginFactory>

#include "secretagent.h"
#include "notification.h"
#if WITH_MODEMMANAGER_SUPPORT
#include "modemmonitor.h"
#endif
#include "bluetoothmonitor.h"

#include <QDBusMetaType>
#include <QDBusServiceWatcher>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>

K_PLUGIN_FACTORY(NetworkManagementServiceFactory, registerPlugin<NetworkManagementService>();)
K_EXPORT_PLUGIN(NetworkManagementServiceFactory("networkmanagement", "plasmanetworkmanagement-kded"))

class NetworkManagementServicePrivate
{
public:
    SecretAgent * agent;
    BluetoothMonitor * bluetoothMonitor;
    ModemMonitor * modemMonitor;
    Notification * notification;
};

NetworkManagementService::NetworkManagementService(QObject * parent, const QVariantList&)
        : KDEDModule(parent), d_ptr(new NetworkManagementServicePrivate)
{
    Q_D(NetworkManagementService);

    QDBusReply<bool> notificationsReply = QDBusConnection::sessionBus().interface()->isServiceRegistered("org.freedesktop.Notifications");
    if (notificationsReply.value()) {
        initializeNotifications();
    } else {
        QDBusServiceWatcher * watcher = new QDBusServiceWatcher("org.freedesktop.Notifications", QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
        connect(watcher, SIGNAL(serviceRegistered()), this, SLOT(doInitializeNotifications()));
    }

#if WITH_MODEMMANAGER_SUPPORT
    d->modemMonitor = new ModemMonitor(this);
#endif
    d->bluetoothMonitor = new BluetoothMonitor(this);
    d->agent = new SecretAgent(this);
}

NetworkManagementService::~NetworkManagementService()
{
    delete d_ptr;
}

void NetworkManagementService::doInitializeNotifications()
{
    QDBusServiceWatcher * watcher = static_cast<QDBusServiceWatcher*>(sender());
    watcher->deleteLater();

    initializeNotifications();
}

void NetworkManagementService::initializeNotifications()
{
    Q_D(NetworkManagementService);

    d->notification = new Notification(this);
}
