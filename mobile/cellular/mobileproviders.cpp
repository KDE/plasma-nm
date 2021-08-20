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

// taken from libs/editor/mobileproviders.cpp, and adapted for mobile purposes

#include "debug.h"
#include "mobileproviders.h"

#include <QFile>
#include <QTextStream>
#include <QLocale>

const QString MobileProviders::ProvidersFile = "/usr/share/mobile-broadband-provider-info/serviceproviders.xml";

bool localeAwareCompare(const QString & one, const QString & two) {
    return one.localeAwareCompare(two) < 0;
}

MobileProviders::MobileProviders()
{
    mError = Success;

    QFile file2(ProvidersFile);

    if (file2.open(QIODevice::ReadOnly)) {
        if (mDocProviders.setContent(&file2)) {
            docElement = mDocProviders.documentElement();

            if (docElement.isNull()) {
                qWarning() << ProvidersFile << ": document is null";
                mError = ProvidersIsNull;
            } else {
                if (docElement.isNull() || docElement.tagName() != "serviceproviders") {
                    qWarning() << ProvidersFile << ": wrong format";
                    mError = ProvidersWrongFormat;
                } else {
                    if (docElement.attribute("format") != "2.0") {
                        qWarning() << ProvidersFile << ": mobile broadband provider database format '" << docElement.attribute("format") << "' not supported.";
                        mError = ProvidersFormatNotSupported;
                    } else {
                        // qCDebug(PLASMA_NM) << "Everything is alright so far";
                    }
                }
            }
        }

        file2.close();
    } else {
        qWarning() << "Error opening providers file" << ProvidersFile;
        mError = ProvidersMissing;
    }
}

MobileProviders::~MobileProviders()
{
}

void MobileProviders::fillProvidersList()
{
    mProvidersGsm.clear();
    mProvidersCdma.clear();
    QDomNode n = docElement.firstChild();

    QMap<QString, QString> sortedGsm;
    QMap<QString, QString> sortedCdma;
    while (!n.isNull()) {
        QDomElement e = n.toElement(); // <country ...>

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
                const QString name = getNameByLocale(localizedProviderNames);
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
        n = n.nextSibling();
    }
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
                            // qCDebug(PLASMA_NM) << "apn" << e2.attribute("value") << "ignored because of usage" << e3.attribute("type");
                            isInternet = false;
                            break;
                        }
                        n3 = n3.nextSibling();
                    }
                    if (isInternet) {
                        mApns.insert(e2.attribute("value"), e2.firstChild());
                    }
                } else if (!e2.isNull() && e2.tagName().toLower() == "network-id") {
                    mNetworkIds.insert(e2.attribute("mcc") + e2.attribute("mnc"), provider);
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


QString MobileProviders::getProvider(const QString &mccmnc)
{
    return mNetworkIds[mccmnc];
}

QVariantMap MobileProviders::getApnInfo(const QString &apn)
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
            } else if (e.tagName().toLower() == "usage") {
                temp.insert("usageType", e.attribute("type"));
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

QString MobileProviders::getNameByLocale(const QMap<QString, QString> &localizedNames) const
{
    QString name;
    const QStringList locales = QLocale().uiLanguages();
    for (const QString &locale : locales) {
        QString language = locale.split(QLatin1Char('-')).at(0);

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
