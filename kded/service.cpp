/*
    Copyright 2009 Dario Freddi <drf54321@gmail.com>
    Copyright 2009 Will Stephenson <wstephenson@kde.org>
    Copyright 2011-2012 Lamarque V. Souza <lamarque@kde.org>

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
};

NetworkManagementService::NetworkManagementService(QObject * parent, const QVariantList&)
        : KDEDModule(parent), d_ptr(new NetworkManagementServicePrivate)
{
    KGlobal::insertCatalog("plasma_applet_org.kde.networkmanagement");  // mobile wizard

    QDBusReply<bool> reply = QDBusConnection::sessionBus().interface()->isServiceRegistered("org.kde.plasma-desktop");
    if (reply.value()) {
        doInitialization();
    } else {
        QDBusServiceWatcher * watcher = new QDBusServiceWatcher("org.kde.plasma-desktop", QDBusConnection::sessionBus(), QDBusServiceWatcher::WatchForOwnerChange, this);
        connect(watcher, SIGNAL(serviceRegistered(QString)), SLOT(finishInitialization()));
    }
}

NetworkManagementService::~NetworkManagementService()
{
    delete d_ptr;
}

void NetworkManagementService::finishInitialization()
{
    QDBusServiceWatcher * watcher = static_cast<QDBusServiceWatcher*>(sender());
    disconnect(watcher, SIGNAL(serviceRegistered(QString)), this,  SLOT(finishInitialization()));

    doInitialization();
}

void NetworkManagementService::doInitialization()
{
    Q_D(NetworkManagementService);

    d->agent = new SecretAgent(this);
    new Notification(this);
#if WITH_MODEMMANAGER_SUPPORT
    new ModemMonitor(this);
#endif
    new BluetoothMonitor(this);
}
