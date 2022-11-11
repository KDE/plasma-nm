/*
    SPDX-FileCopyrightText: 2009 Dario Freddi <drf54321@gmail.com>
    SPDX-FileCopyrightText: 2009 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2012 Lamarque V. Souza <lamarque@kde.org>
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2013-2014 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PLASMANM_KDED_SERVICE_H
#define PLASMANM_KDED_SERVICE_H

#include <KDEDModule>

#include <QVariant>

#include "connectivitymonitor.h"
#include "monitor.h"
#include "notification.h"
#include "secretagent.h"

class Q_DECL_EXPORT NetworkManagementService : public KDEDModule
{
    Q_CLASSINFO("D-Bus Interface", "org.kde.plasmanetworkmanagement")
    Q_OBJECT
public:
    explicit NetworkManagementService(QObject *parent, const QVariantList &);

public Q_SLOTS:
    Q_SCRIPTABLE void init();

Q_SIGNALS:
    Q_SCRIPTABLE
    void secretsError(const QString &connectionPath, const QString &message);

private:
    SecretAgent *agent = nullptr;
    Notification *notification = nullptr;
    Monitor *monitor = nullptr;
    ConnectivityMonitor *connectivityMonitor = nullptr;
};

#endif // PLASMANM_KDED_SERVICE_H
