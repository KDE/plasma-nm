/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "connectioneditor.h"
#include "connectiondetaileditor.h"
#include <config.h>

#include <KApplication>
#include <KGlobal>
#include <k4aboutdata.h>
#include <KCmdLineArgs>
#include <KMainWindow>
#include <QUrl>
#include <KIcon>

#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>

int main(int argc, char *argv[])
{
    K4AboutData about("kde-nm-connection-editor", 0, ki18n("Connection editor"),
                     PLASMA_NM_VERSION_STRING, ki18n("Manage your network connections"),
                     K4AboutData::License_GPL, ki18n("(C) 2013-2014 Jan Grulich and Lukáš Tinkl"),
                     ki18n("This application allows you to create, edit and delete network connections.\n\nUsing NM version: %1")
                     .subs(NetworkManager::version()));
    about.addAuthor(ki18n("Jan Grulich"), ki18n("Developer"), "jgrulich@redhat.com");
    about.addAuthor(ki18n("Lukáš Tinkl"), ki18n("Developer"), "ltinkl@redhat.com");
    about.addCredit(ki18n("Lamarque Souza"), ki18n("libnm-qt author"), "lamarque@kde.org");
    about.addCredit(ki18n("Daniel Nicoletti"), ki18n("various bugfixes"), "dantti12@gmail.com");
    about.addCredit(ki18n("Will Stephenson"), ki18n("VPN plugins"), "wstephenson@kde.org");
    about.addCredit(ki18n("Ilia Kats"), ki18n("VPN plugins"), "ilia-kats@gmx.net");
    about.setProductName("plasma-nm/editor");

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("+[uuid]", ki18n("Edit connection"));
    KCmdLineArgs::addCmdLineOptions(options);

    QApplication app(KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv());
    app.setApplicationName("kde-nm-connection-editor");
    app.setApplicationVersion(PLASMA_NM_VERSION_STRING);

#warning "Translations for kde-nm-connection-editor disabled"
    KGlobal::locale()->insertCatalog("libplasmanetworkmanagement-editor");  // setting widgets
    KGlobal::locale()->insertCatalog("plasma_applet_org.kde.plasma.networkmanagement");  // mobile wizard, UiUtils, ...

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if(args->count()) {
        NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(args->arg(0));

        if (connection) {
            NetworkManager::ConnectionSettings::Ptr connectionSetting = connection->settings();

            ConnectionDetailEditor * editor = new ConnectionDetailEditor(connectionSetting);
            editor->setWindowIcon(KIcon("network-defaultroute"));
            editor->show();
        } else {
            return 1;
        }
    } else {
        ConnectionEditor * editor = new ConnectionEditor();
        editor->setWindowIcon(KIcon("network-defaultroute"));
        editor->show();
    }

    return app.exec();
}
