/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "plasmanm_cellular_export.h"

#include <QAbstractListModel>
#include <QtQmlIntegration>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/ModemDevice>

#include <QCoroDBusPendingReply>

class CellularModem;

class PLASMANM_CELLULAR_EXPORT CellularConnectionProfile : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum ItemRole {
        ConnectionName = Qt::UserRole + 1,
        ConnectionAPN,
        ConnectionUser,
        ConnectionPassword,
        ConnectionNetworkType,
        ConnectionUni,
        RoamingAllowed,
    };
    Q_ENUM(ItemRole)

    explicit CellularConnectionProfile(QObject *parent = nullptr);
    ~CellularConnectionProfile() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QCoro::Task<void> setRoamingAllowed(int index, bool allowed);
    Q_INVOKABLE int indexOfConnection(const QString &connectionUni) const;
    Q_INVOKABLE bool roamingAllowedForConnection(const QString &connectionUni) const;

    void setNmModem(NetworkManager::ModemDevice::Ptr nmModem);
    void refreshProfiles();

    static QString networkTypeStr(NetworkManager::GsmSetting::NetworkType networkType);
    static NetworkManager::GsmSetting::NetworkType networkTypeFlag(const QString &networkType);

Q_SIGNALS:
    void profilesChanged();

private:
    struct ProfileData {
        QString name;
        QString apn;
        QString user;
        QString password;
        QString networkType;
        QString connectionUni;
        bool roamingAllowed;
    };

    QList<ProfileData> m_list;
    NetworkManager::ModemDevice::Ptr m_nmModem;
};
