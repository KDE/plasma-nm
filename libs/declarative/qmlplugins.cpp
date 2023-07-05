/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "qmlplugins.h"

#include <QQmlEngine>

#include "availabledevices.h"
#include "connectionicon.h"
#include "enabledconnections.h"
#include "networkstatus.h"

#include "appletproxymodel.h"
#include "configurationproxy.h"
#include "creatableconnectionsmodel.h"
#include "editorproxymodel.h"
#include "kcmidentitymodel.h"
#include "mobileproxymodel.h"
#include "networkmodel.h"

#include "enums.h"
#include "handler.h"
#include "wirelessstatus.h"

void QmlPlugins::registerTypes(const char *uri)
{
    // @uri org.kde.plasma.networkmanagement.AvailableDevices
    qmlRegisterType<AvailableDevices>(uri, 0, 2, "AvailableDevices");
    // @uri org.kde.plasma.networkmanagement.ConnectionIcon
    qmlRegisterType<ConnectionIcon>(uri, 0, 2, "ConnectionIcon");
    // @uri org.kde.plasma.networkmanagement.EnabledConnections
    qmlRegisterType<EnabledConnections>(uri, 0, 2, "EnabledConnections");
    // @uri org.kde.plasma.networkmanagement.Enums
    qmlRegisterUncreatableType<Enums>(uri, 0, 2, "Enums", "You cannot create Enums on yourself");
    // @uri org.kde.plasma.networkmanagement.NetworkStatus
    qmlRegisterType<NetworkStatus>(uri, 0, 2, "NetworkStatus");
    // @uri org.kde.plasma.networkmanagement.WirelessStatus
    qmlRegisterType<WirelessStatus>(uri, 0, 2, "WirelessStatus");
    // @uri org.kde.plasma.networkmanagement.Handler
    qmlRegisterType<Handler>(uri, 0, 2, "Handler");
    // @uri org.kde.plasma.networkmanagement.NetworkModel
    qmlRegisterType<NetworkModel>(uri, 0, 2, "NetworkModel");
    // @uri org.kde.plasma.networkmanagement.AppletProxyModel
    qmlRegisterType<AppletProxyModel>(uri, 0, 2, "AppletProxyModel");
    // @uri org.kde.plasma.networkmanagement.EditorProxyModel
    qmlRegisterType<EditorProxyModel>(uri, 0, 2, "EditorProxyModel");
    // @uri org.kde.plasma.networkmanagement.KcmIdentityModel
    qmlRegisterType<KcmIdentityModel>(uri, 0, 2, "KcmIdentityModel");
    // @uri org.kde.plasma.networkmanagement.CreatableConnectionsModel
    qmlRegisterType<CreatableConnectionsModel>(uri, 0, 2, "CreatableConnectionsModel");
    // @uri org.kde.plasma.networkmanagement.MobileProxyModel
    qmlRegisterType<MobileProxyModel>(uri, 0, 2, "MobileProxyModel");

    qmlRegisterSingletonType<ConfigurationProxy>(uri, 0, 2, "Configuration", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new ConfigurationProxy;
    });
}

#include "moc_qmlplugins.cpp"
