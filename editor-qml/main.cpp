/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>

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

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QCommandLineParser>
#include <QtQml/QtQml>

#include <KGlobal>

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>

#include "connectioneditor.h"
#include "connectiondetaileditor.h"

void prependEnv(const char *env, const QByteArray &value)
{
    QByteArray values = qgetenv(env);
    if (!values.contains(value + ":")) {
        values = value + ":" + values;
    }
    qputenv(env, values);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    prependEnv("XDG_DATA_DIRS", "/opt/kde5/share");
    // FIXME: OUCH!
    prependEnv("QML2_IMPORT_PATH", "/usr/lib64/qml");
    prependEnv("QML2_IMPORT_PATH", "/opt/kde5/lib64/qml");
    // FIXME:

    QCommandLineParser parser;
    parser.addPositionalArgument("uuid", i18n("Edit connection"), "[uuid]");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (!args.isEmpty()) {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(args.first());

        if (connection) {
            NetworkManager::ConnectionSettings::Ptr connectionSetting = connection->settings();

            ConnectionDetailEditor * editor = new ConnectionDetailEditor(connectionSetting);
            editor->show();
        } else {
            return 1;
        }
    } else {
        qmlRegisterType<ConnectionEditor>("ConnectionEditor", 0, 1, "ConnectionEditor");
        // FIXME
        QQmlApplicationEngine * engine = new QQmlApplicationEngine("/opt/kde5/share/kde-nm-connection-editor/qml/main.qml");

    }

    return app.exec();
}
