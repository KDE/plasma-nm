/*
 * SPDX-FileCopyrightText: 2010-2012 Lamarque Souza <lamarque@kde.org>
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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
