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

#include "mobileproviders.h"

#include <QFile>
#include <QTextStream>

#include <KDebug>
#include <KLocale>
#include <KGlobal>

const QString MobileProviders::CountryCodesFile = "/usr/share/zoneinfo/iso3166.tab";
const QString MobileProviders::ProvidersFile = "/usr/share/mobile-broadband-provider-info/serviceproviders.xml";

MobileProviders::MobileProviders()
{
    QFile file(CountryCodesFile);
    mError = Success;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith('#')) {
                continue;
            }
            QStringList pieces = line.split('\t');
            mCountries.insert(pieces.at(0), pieces.at(1));
        }
        file.close();
    } else {
        mError = CountryCodesMissing;
    }

    QFile file2(ProvidersFile);

    if (file2.open(QIODevice::ReadOnly)) {
        if (mDocProviders.setContent(&file2)) {
            docElement = mDocProviders.documentElement();

            if (docElement.isNull()) {
                kDebug() << ProvidersFile << ": document is null";
                mError = ProvidersIsNull;
            } else {
                if (docElement.isNull() || docElement.tagName() != "serviceproviders") {
                    kDebug() << ProvidersFile << ": wrong format";
                    mError = ProvidersWrongFormat;
                } else {
                    if (docElement.attribute("format") != "2.0") {
                        kDebug() << ProvidersFile << ": mobile broadband provider database format '" << docElement.attribute("format") << "' not supported.";
                        mError = ProvidersFormatNotSupported;
                    } else {
                        //kDebug() << "Everything is alright so far";
                    }
                }
            }
        }

        file2.close();
    } else {
        kDebug() << "Error opening providers file" << ProvidersFile;
        mError = ProvidersMissing;
    }
}

MobileProviders::~MobileProviders()
{
}

QStringList MobileProviders::getCountryList() const
{
    QStringList temp = mCountries.values();
    temp.sort();
    return temp;
}

QString MobileProviders::countryFromLocale() const
{
    QString lang(getenv("LC_ALL"));

    if (lang.isEmpty()) {
        lang = QString(getenv("LANG"));
    }
    if (lang.contains('_')) {
        lang = lang.section('_', 1);
    }
    if (lang.contains('.')) {
        lang = lang.section('.', 0, 0);
    }
    return lang.toUpper();
}

QStringList MobileProviders::getProvidersList(QString country, NetworkManager::Settings::ConnectionSettings::ConnectionType type)
{
    mProvidersGsm.clear();
    mProvidersCdma.clear();
    QDomNode n = docElement.firstChild();

    // country is a country name and we parse country codes.
    if (country.length() > 2) {
        country = mCountries.key(country);
    }
    QMap<QString, QString> sortedGsm;
    QMap<QString, QString> sortedCdma;
    while (!n.isNull()) {
        QDomElement e = n.toElement(); // <country ...>

        if (!e.isNull() && e.attribute("code").toUpper() == country) {
            QDomNode n2 = e.firstChild();
            while (!n2.isNull()) {
                QDomElement e2 = n2.toElement(); // <provider ...>

                if (!e2.isNull() && e2.tagName().toLower() == "provider") {
                    QDomNode n3 = e2.firstChild();
                    bool hasGsm = false;
                    bool hasCdma = false;
                    QMap<QString, QString> localizedProviderNames;

                    while (!n3.isNull()) {
                        QDomElement e3 = n3.toElement(); // <name | gsm | cdma>

                        if (!e3.isNull()) {
                            if (e3.tagName().toLower() == "gsm") {
                                hasGsm = true;
                            } else if (e3.tagName().toLower() == "cdma") {
                                hasCdma = true;
                            } else if (e3.tagName().toLower() == "name") {
                                QString lang = e3.attribute("xml:lang");
                                if (lang.isEmpty()) {
                                    lang = "en";     // English is default
                                } else {
                                    lang = lang.toLower();
                                    lang.remove(QRegExp("\\-.*$"));  // Remove everything after '-' in xml:lang attribute.
                                }
                                localizedProviderNames.insert(lang, e3.text());
                            }
                        }
                        n3 = n3.nextSibling();
                    }
                    QString name = getNameByLocale(localizedProviderNames);
                    if (hasGsm) {
                        mProvidersGsm.insert(name, e2.firstChild());
                        sortedGsm.insert(name.toLower(), name);
                    }
                    if (hasCdma) {
                        mProvidersCdma.insert(name, e2.firstChild());
                        sortedCdma.insert(name.toLower(), name);
                    }
                }
                n2 = n2.nextSibling();
            }
            break;
        }
        n = n.nextSibling();
    }

    if (type == NetworkManager::Settings::ConnectionSettings::Gsm) {
        return sortedGsm.values();
    }
    return sortedCdma.values();
}

QStringList MobileProviders::getApns(const QString & provider)
{
    mApns.clear();
    mNetworkIds.clear();
    if (!mProvidersGsm.contains(provider)) {
        return QStringList();
    }

    QDomNode n = mProvidersGsm[provider];

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // <gsm | cdma>

        if (!e.isNull() && e.tagName().toLower() == "gsm") {
            QDomNode n2 = e.firstChild();
            while (!n2.isNull()) {
                QDomElement e2 = n2.toElement(); // <apn | network-id>

                if (!e2.isNull() && e2.tagName().toLower() == "apn") {
                    bool isInternet = true;
                    QDomNode n3 = e2.firstChild();
                    while (!n3.isNull()) {
                        QDomElement e3 = n3.toElement(); // <usage>
                        if (!e3.isNull() &&
                            e3.tagName().toLower() == "usage" &&
                            !e3.attribute("type").isNull() &&
                            e3.attribute("type").toLower() != "internet") {
                            //kDebug() << "apn" << e2.attribute("value") << "ignored because of usage" << e3.attribute("type");
                            isInternet = false;
                            break;
                        }
                        n3 = n3.nextSibling();
                    }
                    if (isInternet) {
                        mApns.insert(e2.attribute("value"), e2.firstChild());
                    }
                } else if (!e2.isNull() && e2.tagName().toLower() == "network-id") {
                    mNetworkIds.append(e2.attribute("mcc") + '-' + e2.attribute("mnc"));
                }

                n2 = n2.nextSibling();
            }
        }
        n = n.nextSibling();
    }

    QStringList temp = mApns.keys();
    temp.sort();
    return temp;
}


QStringList MobileProviders::getNetworkIds(const QString & provider)
{
    if (mNetworkIds.isEmpty()) {
        getApns(provider);
    }
    return mNetworkIds;
}

QVariantMap MobileProviders::getApnInfo(const QString & apn)
{
    QVariantMap temp;
    QDomNode n = mApns[apn];
    QStringList dnsList;
    QMap<QString, QString> localizedPlanNames;

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // <name|username|password|dns(*)>

        if (!e.isNull()) {
            if (e.tagName().toLower() == "name") {
                QString lang = e.attribute("xml:lang");
                if (lang.isEmpty()) {
                    lang = "en";     // English is default
                } else {
                    lang = lang.toLower();
                    lang.remove(QRegExp("\\-.*$"));  // Remove everything after '-' in xml:lang attribute.
                }
                localizedPlanNames.insert(lang, e.text());
            } else if (e.tagName().toLower() == "username") {
                temp.insert("username", e.text());
            } else if (e.tagName().toLower() == "password") {
                temp.insert("password", e.text());
            } else if (e.tagName().toLower() == "dns") {
                dnsList.append(e.text());
            }
        }

        n = n.nextSibling();
    }

    QString name = getNameByLocale(localizedPlanNames);
    if (!name.isEmpty()) {
        temp.insert("name", QVariant::fromValue(name));
    }
    temp.insert("number", getGsmNumber());
    temp.insert("apn", apn);
    temp.insert("dnsList", dnsList);

    return temp;
}

QVariantMap MobileProviders::getCdmaInfo(const QString & provider)
{
    if (!mProvidersCdma.contains(provider)) {
        return QVariantMap();
    }

    QVariantMap temp;
    QDomNode n = mProvidersCdma[provider];
    QStringList sidList;

    while (!n.isNull()) {
        QDomElement e = n.toElement(); // <gsm or cdma ...>

        if (!e.isNull() && e.tagName().toLower() == "cdma") {
            QDomNode n2 = e.firstChild();
            while (!n2.isNull()) {
                QDomElement e2 = n2.toElement(); // <name | username | password | sid>

                if (!e2.isNull()) {
                    if (e2.tagName().toLower() == "username") {
                        temp.insert("username", e2.text());
                    } else if (e2.tagName().toLower() == "password") {
                        temp.insert("password", e2.text());
                    } else if (e2.tagName().toLower() == "sid") {
                        sidList.append(e2.text());
                    }
                }

                n2 = n2.nextSibling();
            }
        }
        n = n.nextSibling();
    }

    temp.insert("number", getCdmaNumber());
    temp.insert("sidList", sidList);
    return temp;
}

QString MobileProviders::getNameByLocale(const QMap<QString, QString> & localizedNames) const
{
    QString name;
    QStringList locales = KGlobal::locale()->languageList();
    foreach(const QString & locale, locales) {
        QString language, country, modifier, charset;
        KLocale::splitLocale(locale, language, country, modifier, charset);

        if (localizedNames.contains(language)) {
            return localizedNames[language];
        }
    }

    name = localizedNames["en"];

    // Use any language if no proper localized name were found.
    if (name.isEmpty() && !localizedNames.isEmpty()) {
        name = localizedNames.constBegin().value();
    }
    return name;
}
