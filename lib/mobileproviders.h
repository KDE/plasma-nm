/*
Copyright 2010-2012 Lamarque Souza <lamarque@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MOBILEPROVIDERS_H
#define MOBILEPROVIDERS_H

#include <QStringList>
#include <QHash>
#include <QDomDocument>
#include <QVariantMap>

#include <QtNetworkManager/settings/connection.h>

class MobileProviders
{
public:
    static const QString CountryCodesFile;
    static const QString ProvidersFile;

    enum ErrorCodes { Success, CountryCodesMissing, ProvidersMissing, ProvidersIsNull, ProvidersWrongFormat, ProvidersFormatNotSupported };

    MobileProviders();
    ~MobileProviders();

    QStringList getCountryList() const;
    QString countryFromLocale() const;
    QString getCountryName(const QString & key) { return mCountries[key]; }
    QStringList getProvidersList(QString country, NetworkManager::Settings::ConnectionSettings::ConnectionType type);
    QStringList getApns(const QString & provider);
    QStringList getNetworkIds(const QString & provider);
    QVariantMap getApnInfo(const QString & apn);
    QVariantMap getCdmaInfo(const QString & provider);
    const QString getGsmNumber() { return QString("*99#"); }
    const QString getCdmaNumber() { return QString("#777"); }
    inline ErrorCodes getError() { return mError; }

private:
    QHash<QString, QString> mCountries;
    QMap<QString, QDomNode> mProvidersGsm;
    QMap<QString, QDomNode> mProvidersCdma;
    QMap<QString, QDomNode> mApns;
    QStringList mNetworkIds;
    QDomDocument mDocProviders;
    QDomElement docElement;
    ErrorCodes mError;
    QString getNameByLocale(const QMap<QString, QString> & names) const;
};

#endif
