/*
    Copyright 2013-2016 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_CONNECTION_EDITOR_BASE_H
#define PLASMA_NM_CONNECTION_EDITOR_BASE_H

#include <QDBusPendingCallWatcher>
#include <QWidget>

#include <NetworkManagerQt/ConnectionSettings>

class ConnectionWidget;
class SettingWidget;

class Q_DECL_EXPORT ConnectionEditorBase : public QWidget
{
Q_OBJECT
public:
    explicit ConnectionEditorBase(const NetworkManager::ConnectionSettings::Ptr &connection,
                                  QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit ConnectionEditorBase(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~ConnectionEditorBase();

    // When reimplementing do not forget to call the base method as well to initialize widgets
    virtual void setConnection(const NetworkManager::ConnectionSettings::Ptr &connection);

    NMVariantMapMap setting() const;

Q_SIGNALS:
    // The default value is supposed to be false, watch this property for validity change after
    // proper initialization with secrets
    void validityChanged(bool valid);

private Q_SLOTS:
    void replyFinished(QDBusPendingCallWatcher *watcher);
    void validChanged(bool valid);

protected:
    // Subclassed widget is supposed to take care of layouting for setting widgets
    virtual void addWidget(QWidget *widget, const QString &text) = 0;

    // Subclassed widget is supposed to provide an UI (input label) for editing connection name separately
    virtual QString connectionName() const = 0;

    // Subclassed widget is supposed to call initialization after the UI is initialized
    void initialize();

private:
    NetworkManager::ConnectionSettings::Ptr m_connection;
    ConnectionWidget *m_connectionWidget;
    QList<SettingWidget *> m_settingWidgets;

    void addConnectionWidget(ConnectionWidget *widget, const QString &text);
    void addSettingWidget(SettingWidget *widget, const QString &text);

};

#endif // PLASMA_NM_CONNECTION_EDITOR_BASE_H
