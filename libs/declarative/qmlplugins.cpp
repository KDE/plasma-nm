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

#include "qmlplugins.h"

#include <QtQml>

#include "availabledevices.h"
#include "connectionicon.h"
#include "enabledconnections.h"
#include "globalconfig.h"
#include "networkstatus.h"
// #include "trafficmonitor.h"

#include "appletproxymodel.h"
#include "networkmodel.h"

#include "handler.h"
#include "enums.h"

void QmlPlugins::registerTypes(const char* uri)
{
    // @uri org.kde.plasma.networkmanagement.AvailableDevices
    qmlRegisterType<AvailableDevices>(uri, 0, 1, "AvailableDevices");
    // @uri org.kde.plasma.networkmanagement.ConnectionIcon
    qmlRegisterType<ConnectionIcon>(uri, 0, 1, "ConnectionIcon");
    // @uri org.kde.plasma.networkmanagement.EnabledConnections
    qmlRegisterType<EnabledConnections>(uri, 0, 1, "EnabledConnections");
    // @uri org.kde.plasma.networkmanagement.Enums
    qmlRegisterUncreatableType<Enums>(uri, 0, 1, "Enums", "You cannot create Enums on yourself");
    // @uri org.kde.plasma.networkmanagement.GlobalConfig
    qmlRegisterType<GlobalConfig>(uri, 0, 1, "GlobalConfig");
    // @uri org.kde.plasma.networkmanagement.NetworkStatus
    qmlRegisterType<NetworkStatus>(uri, 0, 1, "NetworkStatus");
    // @uri org.kde.plasma.networkmanagement.Handler
    qmlRegisterType<Handler>(uri, 0, 1, "Handler");
    // @uri org.kde.plasma.networkmanagement.NetworkModel
    qmlRegisterType<NetworkModel>(uri, 0, 1, "NetworkModel");
    // @uri org.kde.plasma.networkmanagement.AppletProxyModel
    qmlRegisterType<AppletProxyModel>(uri, 0, 1, "AppletProxyModel");
    // @uri org.kde.plasma.networkmanagement.TrafficMonitor
//     qmlRegisterType<TrafficMonitor>(uri, 0, 1, "TrafficMonitor");
}

//Q_EXPORT_PLUGIN2(plasmanetworkmanagement, QmlPlugins)
