/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_CONNECTION_STATUS_H
#define PLASMA_NM_CONNECTION_STATUS_H

#include "plasmanm_editorqml_export.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>
#include <qqmlregistration.h>

class ConnectionDetailsModel;
namespace ConnectionDetails
{
struct ConnectionDetailSection;
}

class PLASMANM_EDITORQML_EXPORT ConnectionStatus : public QObject
{
    Q_OBJECT
    QML_UNCREATABLE("")
    QML_ELEMENT

    Q_PROPERTY(ConnectionDetailsModel *detailsModel READ detailsModel CONSTANT)
    Q_PROPERTY(bool hasDetails READ hasDetails NOTIFY detailsChanged)

public:
    explicit ConnectionStatus(QObject *parent = nullptr);
    ~ConnectionStatus() override;

    ConnectionDetailsModel *detailsModel() const;
    bool hasDetails() const;

    void setConnectionAndDevice(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device, const QString &accessPointPath);

    Q_INVOKABLE void setConnectionUuid(const QString &uuid);
Q_SIGNALS:
    void detailsChanged();

private:
    void updateStatusWidget();
    void updateConnectionDetails();
    QList<ConnectionDetails::ConnectionDetailSection> getConnectionDetails() const;

    QString m_connectionUuid;

    NetworkManager::Connection::Ptr m_connection;
    NetworkManager::Device::Ptr m_device;
    QString m_accessPointPath;

    ConnectionDetailsModel *m_detailsModel = nullptr;
};

#endif // PLASMA_NM_CONNECTION_STATUS_WIDGET_H
