/*
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf54321@gmail.com>
    SPDX-FileCopyrightText: 2009 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2011-2012 Lamarque V. Souza <lamarque@kde.org>
    SPDX-FileCopyrightText: 2013-2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "service.h"

#include <KPluginFactory>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDBusServiceWatcher>

K_PLUGIN_CLASS_WITH_JSON(NetworkManagementService, "networkmanagement.json")

NetworkManagementService::NetworkManagementService(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
{
    connect(this, &KDEDModule::moduleRegistered, this, &NetworkManagementService::init);

    agent = new SecretAgent(this);
    connect(agent, &SecretAgent::secretsError, this, &NetworkManagementService::secretsError);
}

void NetworkManagementService::init()
{
    if (!notification) {
        notification = new Notification(this);
    }

    if (!monitor) {
        monitor = new Monitor(this);
    }

    if (!connectivityMonitor) {
        connectivityMonitor = new ConnectivityMonitor(this);
    }
}

#include "service.moc"
