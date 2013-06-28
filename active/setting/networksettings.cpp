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

#include "networksettings.h"
#include "networkmodel.h"
#include "uiutils.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>

#include <KDebug>
#include <KLocale>

class NetworkSettingsPrivate {
public:
    NetworkSettings *q;
    QObject *networkModel;
    QString name;
    QString icon;
    QString status;

    QString path;
    NetworkModelItem::NetworkType type;

    void initNetwork();
};

NetworkSettings::NetworkSettings()
{
    d = new NetworkSettingsPrivate;
    d->q = this;
    d->networkModel = 0;

    d->initNetwork();
    kDebug() << "NetworkSettings module loaded.";
}

NetworkSettings::~NetworkSettings()
{
    kDebug() << "========================== NetworkSettings destroyed";
    delete d;
}

void NetworkSettingsPrivate::initNetwork()
{
    QAbstractItemModel * _networkModel = new NetworkModel(q);
    networkModel = _networkModel;
}

QObject* NetworkSettings::networkModel()
{
    return d->networkModel;
}

void NetworkSettings::setNetworkModel(QObject* networkModel)
{
    if ( d->networkModel != networkModel) {
        d->networkModel = networkModel;
        emit networkModelChanged();
    }
}

QString NetworkSettings::icon() const
{
    return d->icon;
}

void NetworkSettings::setIcon(const QString& icon)
{
    if (d->icon != icon) {
        d->icon = icon;
        emit iconChanged();
    }
}

QString NetworkSettings::name() const
{
    return d->name;
}

void NetworkSettings::setName(const QString& name)
{
    if (d->name != name) {
        d->name = name;
        emit nameChanged();
    }
}

QString NetworkSettings::status() const
{
    return d->status;
}

void NetworkSettings::setStatus(const QString& status)
{
    if (d->status != status) {
        d->status = status;
        emit statusChanged();
    }
}

void NetworkSettings::setNetwork(uint type, const QString& path)
{
    d->type = (NetworkModelItem::NetworkType) type;
    d->path = path;

    // TODO: disconnect previous

    if (d->type == NetworkModelItem::Ethernet ||
        d->type == NetworkModelItem::Wifi) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(path);

        if (device) {
            connect(device.data(), SIGNAL(connectionStateChanged()),
                    SLOT(updateStatus()), Qt::UniqueConnection);
            connect(device.data(), SIGNAL(activeConnectionChanged()),
                    SLOT(updateStatus()), Qt::UniqueConnection);
            //TODO: icon changes
        }
    }

    if (d->type == NetworkModelItem::Vpn) {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(path);

        if (connection) {
            foreach (const NetworkManager::ActiveConnection::Ptr & activeConnection, NetworkManager::activeConnections()) {
                if (activeConnection && activeConnection->connection()->path() == connection->path()) {
                    connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                            SLOT(updateStatus()), Qt::UniqueConnection);
                }
            }
        }

        connect(NetworkManager::notifier(), SIGNAL(activeConnectionAdded(QString)),
                SLOT(activeConnectionAdded(QString)), Qt::UniqueConnection);
    }

    updateIcon();
    updateName();
    updateStatus();
}

void NetworkSettings::activeConnectionAdded(const QString& active)
{
    NetworkManager::ActiveConnection::Ptr activeConnection = NetworkManager::findActiveConnection(active);

    // TODO: disconnect previous
    if (activeConnection && activeConnection->connection()->path() == d->path) {
        connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(updateStatus()), Qt::UniqueConnection);

        updateStatus();
    }
}

void NetworkSettings::updateIcon()
{
    if (d->type != NetworkModelItem::Vpn) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(d->path);
        if (device) {
            setIcon(UiUtils::iconName(device));
        }
    } else {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(d->path);
        if (connection) {
            QString title;
            setIcon(UiUtils::iconAndTitleForConnectionSettingsType(connection->settings()->connectionType(), title));
        }
    }
}

void NetworkSettings::updateName()
{
    if (d->type != NetworkModelItem::Vpn) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(d->path);
        if (device) {
            setName(UiUtils::interfaceTypeLabel(device->type(), device));
        }
    } else {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(d->path);
        if (connection) {
            setName(i18n("VPN %1").arg(connection->name()));
        } else {
            setName(i18n("VPN"));
        }
    }
}

void NetworkSettings::updateStatus()
{
    if (d->type != NetworkModelItem::Vpn) {
        NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(d->path);
        if (device) {
            NetworkManager::ActiveConnection::Ptr activeConnection = device->activeConnection();
            if (activeConnection) {
                setStatus(UiUtils::connectionStateToString(device->state(), activeConnection->connection()->name()));
            } else {
                setStatus(UiUtils::connectionStateToString(device->state()));
            }
        }
    } else if (d->type == NetworkModelItem::Vpn) {
        NetworkManager::Connection::Ptr vpnConnection = NetworkManager::findConnection(d->path);
        if (vpnConnection) {
            foreach (const NetworkManager::ActiveConnection::Ptr & activeConnection, NetworkManager::activeConnections()) {
                if (activeConnection && activeConnection->connection()->path() == vpnConnection->path()) {
                    if (activeConnection->state() == NetworkManager::ActiveConnection::Unknown) {
                        setStatus(UiUtils::connectionStateToString(NetworkManager::Device::UnknownState));
                    } else if (activeConnection->state() == NetworkManager::ActiveConnection::Activating) {
                        setStatus(i18n("Connecting"));
                    } else if (activeConnection->state() == NetworkManager::ActiveConnection::Activated) {
                        setStatus(UiUtils::connectionStateToString(NetworkManager::Device::Activated));
                    } else if (activeConnection->state() == NetworkManager::ActiveConnection::Deactivating) {
                        setStatus(UiUtils::connectionStateToString(NetworkManager::Device::Deactivating));
                    } else if (activeConnection->state() == NetworkManager::ActiveConnection::Deactivated) {
                        setStatus(UiUtils::connectionStateToString(NetworkManager::Device::Disconnected));
                    }
                }
            }
        }
    } else {
        setStatus(UiUtils::connectionStateToString(NetworkManager::Device::UnknownState));
    }
}


#include "networksettings.moc"
