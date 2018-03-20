/*
    Copyright 2016-2018 Jan Grulich <jgrulich@redhat.com>

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
    License along with this library.  If not, see <htitp://www.gnu.org/licenses/>.
*/

#ifndef PLASMA_NM_CREATABLE_CONNECTIONS_MODEL_H
#define PLASMA_NM_CREATABLE_CONNECTIONS_MODEL_H

#include <QAbstractListModel>

#include <NetworkManagerQt/ConnectionSettings>

class Q_DECL_EXPORT CreatableConnectionItem : public QObject
{
Q_OBJECT
public:
    explicit CreatableConnectionItem(const QString &typeName, const QString &typeSection,             // Visible properties
                                     const QString &description, const QString &icon,                 // Visible properties
                                     NetworkManager::ConnectionSettings::ConnectionType type,         // Properties needed for creating
                                     const QString &vpnType = QString(),                              // Properties needed for creating
                                     const QString &specificType = QString(),                         // Properties needed for creating
                                     bool shared = false,                                             // Properties needed for creating
                                     QObject *parent = nullptr);
    explicit CreatableConnectionItem(QObject *parent = nullptr);
    ~CreatableConnectionItem() override;

    NetworkManager::ConnectionSettings::ConnectionType connectionType() const;
    void setConnectionType(NetworkManager::ConnectionSettings::ConnectionType type);

    QString description() const;
    void setDescription(const QString &description);

    QString icon() const;
    void setIcon(const QString &icon);

    QString specificType() const;
    void setSpecificType(const QString &specificType);

    bool shared() const;
    void setShared(bool shared);

    QString typeName() const;
    void setTypeName(const QString &typeName);

    QString typeSection() const;
    void setTypeSection(const QString &typeSection);

    QString vpnType() const;
    void setVpnType(const QString &vpnType);

private:
    bool m_shared;
    NetworkManager::ConnectionSettings::ConnectionType m_connectionType;
    QString m_description;
    QString m_icon;
    QString m_specificType;
    QString m_typeName;
    QString m_typeSection;
    QString m_vpnType;
};


class Q_DECL_EXPORT CreatableConnectionsModel : public QAbstractListModel
{
Q_OBJECT
public:
    explicit CreatableConnectionsModel(QObject *parent = nullptr);
    ~CreatableConnectionsModel() override;

    enum ItemRole {
        ConnectionDescription = Qt::UserRole + 1,
        ConnectionIcon,
        ConnectionSpeficType,
        ConnectionShared,
        ConnectionType,
        ConnectionTypeName ,
        ConnectionTypeSection,
        ConnectionVpnType
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash< int, QByteArray > roleNames() const override;

private:
    QList<CreatableConnectionItem*> m_list;
};

#endif // PLASMA_NM_CREATABLE_CONNECTIONS_MODEL_H

