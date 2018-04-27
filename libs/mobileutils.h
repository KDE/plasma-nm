/*
 *   Copyright 2018 Martin Kacej <m.kacej@atlas.sk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MOBILEUTILS_H
#define MOBILEUTILS_H

#include <QObject>

class Q_DECL_EXPORT MobileUtils : public QObject
{
    Q_OBJECT
public:
    explicit MobileUtils(QObject *parent = nullptr);

    Q_INVOKABLE QVariantMap getConnectionSettings(const QString &connection, const QString &type);
    Q_INVOKABLE QVariantMap getActiveConnectionInfo(const QString &connection);
    Q_INVOKABLE void addConnectionFromQML(const QVariantMap &QMLmap);
    Q_INVOKABLE void updateConnectionFromQML(const QString &path, const QVariantMap &map);
    Q_INVOKABLE QString getAccessPointDevice();
    Q_INVOKABLE QString getAccesPointConnection();
    Q_INVOKABLE void startAccessPoint(const QString &uuid,const QString &device);

};

#endif // MOBILEUTILS_H
