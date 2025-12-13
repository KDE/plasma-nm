/*
    SPDX-FileCopyrightText: 2025 Alexander Wilms <f.alexander.wilms@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef PLASMA_NM_CONNECTION_STATUS_WIDGET_H
#define PLASMA_NM_CONNECTION_STATUS_WIDGET_H

#include <QWidget>
#include <QList>
#include <QPair>
#include <QString>
#include <QPointer>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Device>

#include <QStackedLayout>

class QFormLayout;
class QObject;
class QVBoxLayout;
class QLabel;

namespace ConnectionDetails
{
struct ConnectionDetailSection;
}

class ConnectionStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionStatusWidget(const QString &connectionUuid, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~ConnectionStatusWidget() override;

    void setConnectionUuid(const QString &uuid);

    // Set a NetworkModelItem that provides connection details (used by applet)
    // The KCM can call this with the actual NetworkModel-Item pointer
    // We use QObject* to avoid including NetworkModelItem header (avoid circular dependency)
    void setDetailsSource(QObject *networkModelItem);

    // Set connection and device directly (used by KCM)
    void setConnectionAndDevice(const NetworkManager::Connection::Ptr &connection, const NetworkManager::Device::Ptr &device);

private Q_SLOTS:
    void updateConnectionDetails();

private:
    QList<ConnectionDetails::ConnectionDetailSection> getConnectionDetails() const;

    QString m_connectionUuid;

    QStackedLayout *m_stackedLayout = nullptr;
    QWidget *m_formContainer = nullptr;
    QVBoxLayout *m_containerLayout = nullptr;
    QFormLayout *m_detailsLayout = nullptr;
    QLabel *m_disconnectedLabel = nullptr;

    QPointer<QObject> m_detailsSource;  // Actually a NetworkModelItem*, but using QObject* to avoid dependency
    NetworkManager::Connection::Ptr m_connection;
    NetworkManager::Device::Ptr m_device;
};

#endif // PLASMA_NM_CONNECTION_STATUS_WIDGET_H
