/*
    SPDX-FileCopyrightText: 2023 David Redondo <kde@david-redondo.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KDBusService>
#include <KLocalizedContext>
#include <KWindowSystem>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtWebEngineQuick>

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    QGuiApplication::setDesktopFileName(u"org.kde.captiveportalview"_qs);
    QCoreApplication::setOrganizationDomain(u"kde.org"_qs);
    QCoreApplication::setApplicationName(u"captiveportalview"_qs);
    QtWebEngineQuick::initialize();
    QApplication app(argc, argv);
    KDBusService service(KDBusService::Unique);
    QObject::connect(&service, &KDBusService::activateRequested, &app, [] {
        if (auto window = qGuiApp->topLevelWindows().first()) {
            window->show();
            KWindowSystem::updateStartupId(window);
            window->raise();
            KWindowSystem::activateWindow(window);
        }
    });
    QQmlApplicationEngine engine;
    KLocalizedContext context;
    context.setTranslationDomain(TRANSLATION_DOMAIN u""_qs);
    engine.rootContext()->setContextObject(&context);
    engine.load(QUrl(u"qrc:///main.qml"_qs));
    app.exec();
}
