/*
    SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mobileproviders.h"
#include <QTest>

class MobileProvidersTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testProviders();
};

void MobileProvidersTest::testProviders()
{
    MobileProviders providers;

    const QStringList germanProviders = providers.getProvidersList(QStringLiteral("DE"), NetworkManager::ConnectionSettings::Gsm);

    QVERIFY(germanProviders.contains(QStringLiteral("Vodafone")));
}

QTEST_GUILESS_MAIN(MobileProvidersTest)

#include "mobileproviderstest.moc"
