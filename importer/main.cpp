/*
 * SPDX-FileCopyrightText: 2024 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "handler.h"
#include "vpnuiplugin.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginMetaData>

#include <KIO/CommandLauncherJob>

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    KAboutData about(u"plasmanm-importer"_s,
                     i18n("Network Configuration Importer"),
                     QStringLiteral(PROJECT_VERSION),
                     QString(),
                     KAboutLicense::GPL,
                     i18n("(C) 2024 Kai Uwe Broulik"));
    about.addAuthor(u"Kai Uwe Broulik"_s, QString(), u"kde@broulik.de"_s);
    about.setDesktopFileName(u"org.kde.plasma.networkmanagement.importer"_s);
    KAboutData::setApplicationData(about);

    QCommandLineParser parser;
    parser.addPositionalArgument(u"url"_s, i18n("URL of the file describing the connection"));
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    if (parser.positionalArguments().count() != 1) {
        parser.showHelp();
    }

    const QUrl url = QUrl::fromUserInput(parser.positionalArguments().constFirst(), QDir::currentPath(), QUrl::AssumeLocalFile);
    if (!url.isValid() || !url.isLocalFile()) {
        qWarning() << "Invalid or non-local URL" << url;
        parser.showHelp(1);
    }

    const QString path = url.toLocalFile();
    QFileInfo fi(path);
    if (!fi.exists()) {
        qWarning() << "File" << path << "does not exist";
        return 1;
    }

    const QString ext = u"*."_s % fi.suffix();
    qDebug() << "Importing VPN connection" << path << "with extension" << ext;

    // Find matching VPN plug-in.
    QString vpnPluginName;
    std::unique_ptr<VpnUiPlugin> vpnPlugin;

    const QList<KPluginMetaData> services = KPluginMetaData::findPlugins(u"plasma/network/vpn"_s);
    for (const KPluginMetaData &service : services) {
        const auto result = KPluginFactory::instantiatePlugin<VpnUiPlugin>(service);
        if (!result) {
            continue;
        }

        std::unique_ptr<VpnUiPlugin> candidatePlugin(result.plugin);
        if (candidatePlugin->supportedFileExtensions().contains(ext)) {
            qDebug() << "Found VPN plugin" << service.name() << ", type:" << service.value("X-NetworkManager-Services");
            vpnPluginName = service.name();
            vpnPlugin = std::move(candidatePlugin);
            break;
        }
    }

    if (!vpnPlugin) {
        qWarning() << "Unknown or unsupported VPN type.";
        KMessageBox::error(nullptr, i18n("Unknown or unsupported VPN type."));
        return 2;
    }

    if (KMessageBox::questionTwoActions(nullptr,
                                        i18nc("Placeholders are file name and VPN plug-in name",
                                              "Would you like to import \"%1\" as \"%2\" connection?",
                                              fi.completeBaseName().toHtmlEscaped(),
                                              vpnPluginName),
                                        i18nc("@title:window", "Import VPN"),
                                        KGuiItem(i18nc("@action:button", "Import VPN"), u"network-vpn"_s),
                                        KStandardGuiItem::cancel())
        != KMessageBox::PrimaryAction) {
        return 0;
    }

    VpnUiPlugin::ImportResult result = vpnPlugin->importConnectionSettings(path);
    if (!result) {
        qWarning() << "Failed to import" << result.errorMessage();
        KMessageBox::error(nullptr, i18n("Failed to import VPN connection: %1", result.errorMessage()));
        return 2;
    }

    // Once the connection has been added, open System Settings to allow further adjustments be made.
    QObject::connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionAdded, &app, [&app](const QString &path) {
        NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnection(path);
        if (!newConnection) {
            qWarning() << "Failed to find newly added connection" << path;
            return;
        }

        const QString systemSettings = u"systemsettings"_s;
        const QStringList cmdline{u"kcm_networkmanagement"_s, u"--args"_s, u"Uuid=%1"_s.arg(newConnection->uuid())};

        auto *job = new KIO::CommandLauncherJob(systemSettings, cmdline);
        job->setDesktopName(systemSettings);
        QObject::connect(job, &KJob::result, &app, &QApplication::quit);
        job->start();
    });

    auto *handler = new Handler(&app);
    handler->addConnection(result.connection());

    // In case adding the connection fails in NM, quit eventually.
    QTimer::singleShot(10000, &app, [&app] {
        qWarning() << "Timeout while adding connection...";
        app.quit();
    });

    app.exec();
}
