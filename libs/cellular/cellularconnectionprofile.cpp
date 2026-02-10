/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "cellularconnectionprofile.h"
#include "plasma_nm_cellular.h"

#include <KLocalizedString>
#include <NetworkManagerQt/Settings>
#include <QDBusReply>

using namespace Qt::Literals::StringLiterals;

CellularConnectionProfile::CellularConnectionProfile(QObject *parent)
    : QAbstractListModel{parent}
    , m_nmModem{nullptr}
{
}

CellularConnectionProfile::~CellularConnectionProfile() = default;

int CellularConnectionProfile::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

QVariant CellularConnectionProfile::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row < 0 || row >= m_list.count()) {
        return {};
    }

    const ProfileData &item = m_list.at(row);

    switch (role) {
    case ConnectionName:
        return item.name;
    case ConnectionAPN:
        return item.apn;
    case ConnectionUser:
        return item.user;
    case ConnectionPassword:
        return item.password;
    case ConnectionNetworkType:
        return item.networkType;
    case ConnectionUni:
        return item.connectionUni;
    case RoamingAllowed:
        return item.roamingAllowed;
    default:
        break;
    }

    return {};
}

QHash<int, QByteArray> CellularConnectionProfile::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ConnectionName] = "connectionName";
    roles[ConnectionAPN] = "connectionAPN";
    roles[ConnectionUser] = "connectionUser";
    roles[ConnectionPassword] = "connectionPassword";
    roles[ConnectionNetworkType] = "connectionNetworkType";
    roles[ConnectionUni] = "connectionUni";
    roles[RoamingAllowed] = "roamingAllowed";
    return roles;
}

void CellularConnectionProfile::setNmModem(NetworkManager::ModemDevice::Ptr nmModem)
{
    m_nmModem = nmModem;
}

void CellularConnectionProfile::refreshProfiles()
{
    beginResetModel();
    m_list.clear();

    if (!m_nmModem) {
        endResetModel();
        Q_EMIT profilesChanged();
        return;
    }

    for (auto connection : m_nmModem->availableConnections()) {
        for (auto setting : connection->settings()->settings()) {
            if (auto gsmSetting = setting.dynamicCast<NetworkManager::GsmSetting>()) {
                ProfileData profile;
                profile.name = connection->name();
                profile.apn = gsmSetting->apn();
                profile.user = gsmSetting->username();
                profile.password = gsmSetting->password();
                profile.networkType = networkTypeStr(gsmSetting->networkType());
                profile.connectionUni = connection->uuid();
                profile.roamingAllowed = !gsmSetting->homeOnly();
                m_list.append(std::move(profile));
            }
        }
    }
    endResetModel();
    Q_EMIT profilesChanged();
}

int CellularConnectionProfile::indexOfConnection(const QString &connectionUni) const
{
    for (int i = 0; i < m_list.count(); ++i) {
        if (m_list[i].connectionUni == connectionUni) {
            return i;
        }
    }
    return -1;
}

bool CellularConnectionProfile::roamingAllowedForConnection(const QString &connectionUni) const
{
    int idx = indexOfConnection(connectionUni);
    if (idx < 0) {
        return false;
    }
    return m_list[idx].roamingAllowed;
}

QCoro::Task<void> CellularConnectionProfile::setRoamingAllowed(int index, bool allowed)
{
    if (index < 0 || index >= m_list.count()) {
        co_return;
    }

    const QString &connectionUni = m_list[index].connectionUni;
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connectionUni);
    if (!con) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << u"Could not find connection"_s << connectionUni << u"to update roaming!"_s;
        co_return;
    }

    NetworkManager::GsmSetting::Ptr gsmSetting = con->settings()->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
    if (!gsmSetting) {
        co_return;
    }

    gsmSetting->setHomeOnly(!allowed);

    QDBusReply<void> reply = co_await con->update(con->settings()->toMap());
    if (!reply.isValid()) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << u"Error updating roaming for"_s << connectionUni << u":"_s << reply.error().message();
    } else {
        m_list[index].roamingAllowed = allowed;
        Q_EMIT dataChanged(createIndex(index, 0), createIndex(index, 0), {RoamingAllowed});
    }
}

QString CellularConnectionProfile::networkTypeStr(NetworkManager::GsmSetting::NetworkType networkType)
{
    if (networkType == NetworkManager::GsmSetting::NetworkType::Any) {
        return u"Any"_s;
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::GprsEdgeOnly) {
        return u"Only 2G"_s;
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Only3G) {
        return u"Only 3G"_s;
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Only4GLte) {
        return u"Only 4G"_s;
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer2G) {
        return u"2G"_s;
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer3G) {
        return u"3G/2G"_s;
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer4GLte) {
        return u"4G/3G/2G"_s;
    }
    return u"Any"_s;
}

NetworkManager::GsmSetting::NetworkType CellularConnectionProfile::networkTypeFlag(const QString &networkType)
{
    if (networkType == u"Any"_s) {
        return NetworkManager::GsmSetting::NetworkType::Any;
    } else if (networkType == u"Only 2G"_s) {
        return NetworkManager::GsmSetting::NetworkType::GprsEdgeOnly;
    } else if (networkType == u"Only 3G"_s) {
        return NetworkManager::GsmSetting::NetworkType::Only3G;
    } else if (networkType == u"Only 4G"_s) {
        return NetworkManager::GsmSetting::NetworkType::Only4GLte;
    } else if (networkType == u"2G"_s) {
        return NetworkManager::GsmSetting::NetworkType::Prefer2G;
    } else if (networkType == u"3G/2G"_s) {
        return NetworkManager::GsmSetting::NetworkType::Prefer3G;
    } else if (networkType == u"4G/3G/2G"_s) {
        return NetworkManager::GsmSetting::NetworkType::Prefer4GLte;
    }
    return NetworkManager::GsmSetting::NetworkType::Any;
}
