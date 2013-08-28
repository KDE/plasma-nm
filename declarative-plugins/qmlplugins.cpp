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

#include <QtDeclarative/QDeclarativeItem>

#include "applet/availabledevices.h"
#include "applet/connectionicon.h"
#include "applet/enabledconnections.h"
#include "applet/globalconfig.h"
#include "applet/networkstatus.h"
#include "applet/trafficmonitor.h"

#include "model/modelitem.h"
#include "model/monitor.h"
#include "model/model.h"
#include "model/sortmodel.h"

#include "handler.h"
#include "enums.h"

void QmlPlugins::registerTypes(const char* uri)
{
    qmlRegisterType<AvailableDevices>(uri, 0, 1, "AvailableDevices");
    qmlRegisterType<ConnectionIcon>(uri, 0, 1, "ConnectionIcon");
    qmlRegisterType<EnabledConnections>(uri, 0, 1, "EnabledConnections");
    qmlRegisterType<Enums>(uri, 0, 1, "Enums");
    qmlRegisterType<GlobalConfig>(uri, 0, 1, "GlobalConfig");
    qmlRegisterType<NetworkStatus>(uri, 0, 1, "NetworkStatus");
    qmlRegisterType<Handler>(uri, 0, 1, "Handler");
    qmlRegisterType<Model>(uri, 0, 1, "Model");
    qmlRegisterType<SortModel>(uri, 0, 1, "SortModel");
    qmlRegisterType<TrafficMonitor>(uri, 0, 1, "TrafficMonitor");
}

Q_EXPORT_PLUGIN2(plasmanm, QmlPlugins)
