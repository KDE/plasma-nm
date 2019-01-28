/*
    Copyright 2018 Jan Grulich <jgrulich@redhat.com>

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

#ifndef PLASMA_NM_CONNECTION_SETTING_H
#define PLASMA_NM_CONNECTION_SETTING_H

class QObject;
class QDBusPendingCallWatcher;

class ConnectionSettingsPrivate;

#include <NetworkManagerQt/ConnectionSettings>

class Setting;

class Q_DECL_EXPORT ConnectionSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid)
    Q_PROPERTY(uint connectionType READ connectionType)
    Q_PROPERTY(bool autoconnect READ autoconnect WRITE setAutoconnect)
    Q_PROPERTY(QStringList permissions READ permissions WRITE setPermissions)
    Q_PROPERTY(QString secondaryConnection READ secondaryConnection WRITE setSecondaryConnection)
    Q_PROPERTY(QString zone READ zone WRITE setZone)
    Q_PROPERTY(int priority READ priority WRITE setPriority)
    Q_PROPERTY(int metered READ metered WRITE setMetered)
public:
    explicit ConnectionSettings(QObject *parent = nullptr);
    ConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &settings = NetworkManager::ConnectionSettings::Ptr(), QObject *parent = nullptr);

    virtual ~ConnectionSettings();

    void loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings);
    NMVariantMapMap settingMap() const;

    QString id() const;
    void setId(const QString &id);

    QString uuid() const;
    void setUuid(const QString &uuid);

    uint connectionType() const;

    bool autoconnect() const;
    void setAutoconnect(bool autoconnect);

    QStringList permissions() const;
    void setPermissions(const QStringList &permissions);

    QString secondaryConnection() const;
    void setSecondaryConnection(const QString &secondaryConnection);

    QString zone() const;
    void setZone(const QString &zone);

    int priority() const;
    void setPriority(int priority);

    int metered() const;
    void setMetered(int metered);

    Q_INVOKABLE QObject * setting(uint type) const;

    QList<NetworkManager::Setting::SettingType> settingTypes() const;
    void addSetting(NetworkManager::Setting::SettingType type, Setting *setting);

    bool isInitialized() const;
    bool isValid() const;

Q_SIGNALS:
    // The default value is supposed to be false, watch this property for validity change after
    // proper initialization with secrets
    void validityChanged(bool valid);

private Q_SLOTS:
    void onValidityChanged(bool valid);
    void onReplyFinished(QDBusPendingCallWatcher *watcher);

protected:
    ConnectionSettingsPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(ConnectionSettings)

};

#endif // PLASMA_NM_CONNECTION_SETTING_H
