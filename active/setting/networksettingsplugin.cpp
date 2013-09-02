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

#include "networksettingsplugin.h"
#include "networksettings.h"
#include "connectionsettingshandler.h"

#include <QDeclarativeComponent>

#include <KDebug>
#include <KPluginFactory>

K_PLUGIN_FACTORY(NetworkSettingsFactory, registerPlugin<NetworkSettingsPlugin>();)
K_EXPORT_PLUGIN(NetworkSettingsFactory("active_settings_network"))

NetworkSettingsPlugin::NetworkSettingsPlugin(QObject *parent, const QVariantList &list) :
    QObject(parent)
{
    Q_UNUSED(list)

    kDebug() << "NetworkSettingsPlugin created:)";
    qmlRegisterType<NetworkSettings>();
    qmlRegisterType<NetworkSettings>("org.kde.active.settings", 0, 1, "NetworkSettings");
    qmlRegisterType<ConnectionSettingsHandler>("org.kde.active.settings", 0, 1, "ConnectionSettingsHandler");
}

NetworkSettingsPlugin::~NetworkSettingsPlugin()
{
}

#include "networksettingsplugin.moc"
