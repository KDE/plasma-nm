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

#ifndef PLASMA_NM_CONNECTION_DETAIL_EDITOR_H
#define PLASMA_NM_CONNECTION_DETAIL_EDITOR_H

#include <QtGui/QDialog>

#include <NetworkManagerQt/ConnectionSettings>

#include "plasmanm_export.h"

namespace Ui
{
class ConnectionDetailEditor;
}

class PLASMA_NM_EXPORT ConnectionDetailEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionDetailEditor(NetworkManager::ConnectionSettings::ConnectionType type,
                                    QWidget* parent = 0,
                                    const QString &masterUuid = QString(),
                                    const QString &slaveType = QString(),
                                    Qt::WindowFlags f = 0);
    explicit ConnectionDetailEditor(NetworkManager::ConnectionSettings::ConnectionType type,
                                    QWidget* parent = 0,
                                    const QString &vpnType = QString(),
                                    bool shared = false,
                                    Qt::WindowFlags f = 0);
    explicit ConnectionDetailEditor(const NetworkManager::ConnectionSettings::Ptr &connection,
                                    QWidget* parent = 0, Qt::WindowFlags f = 0, bool newConnection = false);
    explicit ConnectionDetailEditor(NetworkManager::ConnectionSettings::ConnectionType type,
                                    const QVariantList &args,
                                    QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~ConnectionDetailEditor();

    bool isSlave() const { return !m_masterUuid.isEmpty() && !m_slaveType.isEmpty(); }

private Q_SLOTS:
    void connectionAddComplete(const QString & id, bool success, const QString & msg);
    void disconnectSignals();
    void gotSecrets(const QString & id, bool success, const NMVariantMapMap & secrets, const QString & msg);
    void validChanged(bool valid);
    void saveSetting();

private:
    void enableOKButton(bool enabled);

    Ui::ConnectionDetailEditor * m_ui;
    NetworkManager::ConnectionSettings::Ptr m_connection;
    bool m_new;
    QString m_vpnType;
    QString m_masterUuid;
    QString m_slaveType;

    void initEditor();
    void initTabs();
};

#endif // PLASMA_NM_CONNECTION_DETAIL_EDITOR_H
