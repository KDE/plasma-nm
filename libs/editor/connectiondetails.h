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

#include <QVariantList>

namespace ConnectionDetails
{
/**
 * Extracts detailed information about an active network connection
 * @param connection The NetworkManager connection
 * @param device The network device
 * @return List of QVariantMaps with "label" and "value" keys
 */
PLASMANM_EDITOR_EXPORT QVariantList getConnectionDetails(const NetworkManager::Connection::Ptr &connection,
                                                          const NetworkManager::Device::Ptr &device);

}

#endif // PLASMA_NM_CONNECTION_DETAILS_H
