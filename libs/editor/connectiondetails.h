/*
    SPDX-FileCopyrightText: 2013-2018 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_CONNECTION_DETAILS_H
#define PLASMA_NM_CONNECTION_DETAILS_H

#include "plasmanm_editor_export.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>

#include <QList>
#include <QPair>
#include <QString>

namespace ConnectionDetails
{

struct ConnectionDetailSection {
    QString title;
    QList<QPair<QString, QString>> details;
};

/**
 * Extracts detailed information about a network connection.
 * @param connection The NetworkManager connection.
 * @param device The network device.
 * @param accessPointPath Optional access point path for disconnected Wi-Fi networks.
 * @return A list of sections, each containing a title and an ordered list of label-value pairs.
 *         The order of sections and details within each section is preserved as defined.
 */
PLASMANM_EDITOR_EXPORT QList<ConnectionDetailSection>
getConnectionDetails(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device, const QString &accessPointPath = QString());
}

#endif // PLASMA_NM_CONNECTION_DETAILS_H
