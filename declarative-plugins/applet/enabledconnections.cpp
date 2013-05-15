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

#include <NetworkManagerQt/Manager>

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
    connect(NetworkManager::notifier(), SIGNAL(wirelessHardwareEnabledChanged(bool)),
            SIGNAL(wirelessHwEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wimaxEnabledChanged(bool)),
            SIGNAL(wimaxEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wimaxHardwareEnabledChanged(bool)),
            SLOT(wimaxHwEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanEnabledChanged(bool)),
            SIGNAL(wwanEnabled(bool)));
    connect(NetworkManager::notifier(), SIGNAL(wwanHardwareEnabledChanged(bool)),
            SIGNAL(wwanHwEnabled(bool)));

    NMAppletDebug() << "Emit signal networkingEnabled(" << NetworkManager::isNetworkingEnabled() << ")";
    Q_EMIT networkingEnabled(NetworkManager::isNetworkingEnabled());
    NMAppletDebug() << "Emit signal wirelessEnabled(" << NetworkManager::isWirelessEnabled() << ")";
    Q_EMIT wirelessEnabled(NetworkManager::isWirelessEnabled());
    NMAppletDebug() << "Emit signal wirelessHwEnabled(" << NetworkManager::isWirelessHardwareEnabled() << ")";
    Q_EMIT wirelessHwEnabled(NetworkManager::isWirelessHardwareEnabled());
    NMAppletDebug() << "Emit signal wimaxEnabled(" << NetworkManager::isWimaxEnabled() << ")";
    Q_EMIT wimaxEnabled(NetworkManager::isWimaxEnabled());
    NMAppletDebug() << "Emit signal wimaxHwEnabled(" << NetworkManager::isWimaxHardwareEnabled() << ")";
    Q_EMIT wimaxHwEnabled(NetworkManager::isWimaxHardwareEnabled());
    NMAppletDebug() << "Emit signal wwanEnabled(" << NetworkManager::isWwanEnabled() << ")";
    Q_EMIT wwanEnabled(NetworkManager::isWwanEnabled());
    NMAppletDebug() << "Emit signal wwanHWEnabled(" << NetworkManager::isWwanHardwareEnabled() << ")";
    Q_EMIT wwanHwEnabled(NetworkManager::isWwanHardwareEnabled());
}
