/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_OPENVPN_HELPERS_P_H
#define PLASMA_NM_OPENVPN_HELPERS_P_H

#include <NetworkManagerQt/Setting>

#include <QLatin1String>
#include <QString>
#include <QUrl>

// Shared between the main page and the advanced dialog, which carry the same
// four-way password storage choice and the same file pickers.
namespace OpenvpnHelpers
{
inline constexpr QLatin1String YesString("yes");
inline constexpr QLatin1String NoString("no");

// Both classes spell the storage choice as an enum of 0..3 in this order, so
// the mapping is shared as plain ints and cast at the call site.
enum StorageOption {
    StoreForUser = 0,
    StoreForAllUsers,
    AlwaysAsk,
    NotRequired
};

inline int optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return StoreForUser;
    }
    if (flags.testFlag(NetworkManager::Setting::NotSaved)) {
        return AlwaysAsk;
    }
    return NotRequired;
}

inline QString flagsFromOption(int option)
{
    switch (option) {
    case StoreForAllUsers:
        return QString::number(NetworkManager::Setting::None);
    case StoreForUser:
        return QString::number(NetworkManager::Setting::AgentOwned);
    case AlwaysAsk:
        return QString::number(NetworkManager::Setting::NotSaved);
    default:
        break;
    }
    return QString::number(NetworkManager::Setting::NotRequired);
}

// The file pickers hand out URLs, NetworkManager stores plain paths.
inline QString pathToUrl(const QString &path)
{
    return path.isEmpty() ? QString() : QUrl::fromLocalFile(path).toString();
}

inline QString urlToPath(const QString &url)
{
    return url.isEmpty() ? QString() : QUrl(url).toLocalFile();
}
}

#endif // PLASMA_NM_OPENVPN_HELPERS_P_H
