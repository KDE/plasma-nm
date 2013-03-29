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


#include "enabledconnections.h"

#include <QtNetworkManager/manager.h>

#include "debug.h"

EnabledConnections::EnabledConnections(QObject* parent):
    QObject(parent)
{
}

EnabledConnections::~EnabledConnections()
{
}

void EnabledConnections::init()
{
    connect(NetworkManager::notifier(), SIGNAL(networkingEnabledChanged(bool)),
            SIGNAL(networkingEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wirelessEnabledChanged(bool)),
            SIGNAL(wirelessEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wimaxHardwareEnabledChanged(bool)),
            SIGNAL(wirelessHwEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanEnabledChanged(bool)),
            SIGNAL(wwanEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanHardwareEnabledChanged(bool)),
            SIGNAL(wwanHwEnabled(bool)));

    NMAppletDebug() << "Emig signal networkingEnabled(" << NetworkManager::isNetworkingEnabled() << ")";
    networkingEnabled(NetworkManager::isNetworkingEnabled());
    NMAppletDebug() << "Emig signal wirelessEnabled(" << NetworkManager::isWirelessEnabled() << ")";
    wirelessEnabled(NetworkManager::isWirelessEnabled());
    NMAppletDebug() << "Emig signal wirelessHwEnabled(" << NetworkManager::isWirelessHardwareEnabled() << ")";
    wirelessHwEnabled(NetworkManager::isWirelessHardwareEnabled());
    NMAppletDebug() << "Emig signal wwanEnabled(" << NetworkManager::isWwanEnabled() << ")";
    wwanEnabled(NetworkManager::isWwanEnabled());
    NMAppletDebug() << "Emig signal wwanEnabled(" << NetworkManager::isWwanHardwareEnabled() << ")";
    wwanEnabled(NetworkManager::isWwanHardwareEnabled());
}
