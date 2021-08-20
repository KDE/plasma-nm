/*
    Copyright 2010-2012 Lamarque Souza <lamarque@kde.org>
              2020 Devin Lin <espidev@gmail.com>

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

// taken from libs/editor/mobileproviders.h, and adapted for mobile purposes

#pragma once

#include <QStringList>
#include <QHash>
#include <QDomDocument>
#include <QVariantMap>

#include <NetworkManagerQt/ConnectionSettings>

class MobileProviders
{
public:
    static const QString ProvidersFile;

    enum ErrorCodes { Success, CountryCodesMissing, ProvidersMissing, ProvidersIsNull, ProvidersWrongFormat, ProvidersFormatNotSupported };

    MobileProviders();
    ~MobileProviders();

    void fillProvidersList();
    QStringList getApns(const QString &provider);
    QString getProvider(const QString &mccmnc);
    QVariantMap getApnInfo(const QString &apn);
    QVariantMap getCdmaInfo(const QString &provider);
    QString getGsmNumber() const { return QString("*99#"); }
    QString getCdmaNumber() const { return QString("#777"); }
    inline ErrorCodes getError() { return mError; }

private:
    QMap<QString, QDomNode> mProvidersGsm;
    QMap<QString, QDomNode> mProvidersCdma;
    QMap<QString, QDomNode> mApns;
    QMap<QString, QString> mNetworkIds; // <mcc-mnc, provider name>
    QDomDocument mDocProviders;
    QDomElement docElement;
    ErrorCodes mError;
    QString getNameByLocale(const QMap<QString, QString> & names) const;
};
