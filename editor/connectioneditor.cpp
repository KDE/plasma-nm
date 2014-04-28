/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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
#include "mobileconnectionwizard.h"
#include "vpnuiplugin.h"

#include <KLocale>
#include <KService>
#include <KServiceTypeTrader>

#include <QFileDialog>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>

ConnectionEditor::ConnectionEditor(QObject* parent)
    : QObject(parent)
{
    const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");
    foreach (const KService::Ptr & service, services) {
        m_vpnPlugins.insert(service->name(), service->property("X-NetworkManager-Services", QVariant::String).toString());
    }

    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionAdded(QString)),
            SLOT(connectionAdded(QString)));
}

ConnectionEditor::~ConnectionEditor()
{
}

void ConnectionEditor::addConnection(uint connectionType, bool shared)
{
    NetworkManager::ConnectionSettings::ConnectionType type = (NetworkManager::ConnectionSettings::ConnectionType) connectionType;

    if (type == NetworkManager::ConnectionSettings::Gsm) { // launch the mobile broadband wizard, both gsm/cdma
#if WITH_MODEMMANAGER_SUPPORT
        QWeakPointer<MobileConnectionWizard> wizard = new MobileConnectionWizard(NetworkManager::ConnectionSettings::Unknown);
        if (wizard.data()->exec() == QDialog::Accepted && wizard.data()->getError() == MobileProviders::Success) {
            qDebug() << "Mobile broadband wizard finished:" << wizard.data()->type() << wizard.data()->args();
            QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(wizard.data()->type(), wizard.data()->args());
            editor->exec();

            if (editor) {
                editor->deleteLater();
            }
        }
        if (wizard) {
            wizard.data()->deleteLater();
        }
#endif
    } else {
        if (type == NetworkManager::ConnectionSettings::Wired || type == NetworkManager::ConnectionSettings::Wireless) {
        }

        QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(type, QString(), shared);
        editor->exec();

        if (editor) {
            editor->deleteLater();
        }
    }
}

void ConnectionEditor::addVpnConnection(const QString& plugin)
{
    QPointer<ConnectionDetailEditor> editor = new ConnectionDetailEditor(NetworkManager::ConnectionSettings::Vpn, m_vpnPlugins[plugin], false);
    editor->exec();

    if (editor) {
        editor->deleteLater();
    }
}

QStringList ConnectionEditor::availableVpnPlugins() const
{
    return m_vpnPlugins.keys();
}

QString ConnectionEditor::statusBarText() const
{
    return m_statusBarText;
}

// void ConnectionEditor::importSecretsFromPlainTextFiles()
// {
//     const QString secretsDirectory = QStandardPaths::locate(QStandardPaths::DataLocation, "networkmanagement/secrets/", QStandardPaths::LocateDirectory);
//     QDir dir(secretsDirectory);
//     if (dir.exists() && !dir.entryList(QDir::Files).isEmpty()) {
//         QMap<QString, QMap<QString, QString > > resultingMap;
//         foreach (const QString & file, dir.entryList(QDir::Files)) {
//             KConfig config(secretsDirectory % file, KConfig::SimpleConfig);
//             foreach (const QString & groupName, config.groupList()) {
//                 KConfigGroup group = config.group(groupName);
//                 QMap<QString, QString> map = group.entryMap();
//                 if (!map.isEmpty()) {
//                     const QString entry = file % ';' % groupName;
//                     resultingMap.insert(entry, map);
//                 }
//             }
//         }
//
//         storeSecrets(resultingMap);
//     }
// }

// void ConnectionEditor::storeSecrets(const QMap< QString, QMap< QString, QString > >& map)
// {
//     if (KWallet::Wallet::isEnabled()) {
//         KWallet::Wallet * wallet = KWallet::Wallet::openWallet(KWallet::Wallet::LocalWallet(), 0, KWallet::Wallet::Synchronous);
//
//         if (!wallet || !wallet->isOpen()) {
//             return;
//         }
//
//         if (!wallet->hasFolder("Network Management")) {
//             wallet->createFolder("Network Management");
//         }
//
//         if (wallet->hasFolder("Network Management") && wallet->setFolder("Network Management")) {
//             int count = 0;
//             foreach (const QString & entry, map.keys()) {
//                 QString connectionUuid = entry.split(';').first();
//                 connectionUuid.replace('{',"").replace('}',"");
//                 NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(connectionUuid);
//
//                 if (connection) {
//                     wallet->writeMap(entry, map.value(entry));
//                     ++count;
//                 }
//             }
//         }
//     } else {
//         KConfig config("plasma-networkmanagement");
//         foreach (const QString & groupName, map.keys()) {
//             KConfigGroup secretsGroup = config.group(groupName);
//             NMStringMap secretsMap = map.value(groupName);
//             NMStringMap::ConstIterator i = secretsMap.constBegin();
//             while (i != secretsMap.constEnd()) {
//                 secretsGroup.writeEntry(i.key(), i.value());
//                 ++i;
//             }
//         }
//     }
// }

void ConnectionEditor::importVpn()
{
    // get the list of supported extensions
    const KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");
    QString extensions;
    foreach (const KService::Ptr &service, services) {
        VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
        if (vpnPlugin) {
            extensions += vpnPlugin->supportedFileExtensions() % QLatin1Literal(" ");
            delete vpnPlugin;
        }
    }

    const QString filename = QFileDialog::getOpenFileName(0, i18n("Import VPN Connection"), QDir::homePath(), extensions.simplified());
    if (!filename.isEmpty()) {
        QFileInfo fi(filename);
        const QString ext = QLatin1Literal("*.") % fi.suffix();
        qDebug() << "Importing VPN connection" << filename << "extension:" << ext;

        foreach (const KService::Ptr &service, services) {
            VpnUiPlugin * vpnPlugin = service->createInstance<VpnUiPlugin>(this);
            if (vpnPlugin && vpnPlugin->supportedFileExtensions().contains(ext)) {
                qDebug() << "Found VPN plugin" << service->name() << ", type:" << service->property("X-NetworkManager-Services", QVariant::String).toString();

                NMVariantMapMap connection = vpnPlugin->importConnectionSettings(filename);

                //qDebug() << "Raw connection:" << connection;

                NetworkManager::ConnectionSettings connectionSettings;
                connectionSettings.fromMap(connection);
                connectionSettings.setUuid(NetworkManager::ConnectionSettings::createNewUuid());

                //qDebug() << "Converted connection:" << connectionSettings;

                const QString conId = NetworkManager::addConnection(connectionSettings.toMap());
                qDebug() << "Adding imported connection under id:" << conId;

                if (connection.isEmpty()) { // the "positive" part will arrive with connectionAdded
                    m_statusBarText = i18n("Importing VPN connection %1 failed\n%2", fi.fileName(), vpnPlugin->lastErrorMessage());
                    QTimer::singleShot(5000, this, SLOT(resetStatusBarText()));
                    Q_EMIT statusBarTextChanged();
                } else {
                    delete vpnPlugin;
                    break; // stop iterating over the plugins if the import produced at least some output
                }

                delete vpnPlugin;
            }
        }
    }
}

void ConnectionEditor::exportVpn(const QString& connectionUuid)
{
    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(connectionUuid);
    if (!connection) {
        return;
    }

    NetworkManager::ConnectionSettings::Ptr connSettings = connection->settings();

    if (connSettings->connectionType() != NetworkManager::ConnectionSettings::Vpn)
        return;

    NetworkManager::VpnSetting::Ptr vpnSetting = connSettings->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();

    qDebug() << "Exporting VPN connection" << connection->name() << "type:" << vpnSetting->serviceType();

    QString error;
    VpnUiPlugin * vpnPlugin = KServiceTypeTrader::createInstanceFromQuery<VpnUiPlugin>(QString::fromLatin1("PlasmaNetworkManagement/VpnUiPlugin"),
                                                                                       QString::fromLatin1("[X-NetworkManager-Services]=='%1'").arg(vpnSetting->serviceType()),
                                                                                       this, QVariantList(), &error);

    if (vpnPlugin) {
        if (vpnPlugin->suggestedFileName(connSettings).isEmpty()) { // this VPN doesn't support export
            m_statusBarText = i18n("Export is not supported by this VPN type");
            QTimer::singleShot(5000, this, SLOT(resetStatusBarText()));
            Q_EMIT statusBarTextChanged();
            return;
        }

        const QString url = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + vpnPlugin->suggestedFileName(connSettings);
        const QString filename = QFileDialog::getSaveFileName(0, i18n("Export VPN Connection"), url, vpnPlugin->supportedFileExtensions());
        if (!filename.isEmpty()) {
            // TODO display error/success
            if (!vpnPlugin->exportConnectionSettings(connSettings, filename)) {
                m_statusBarText = i18n("Exporting VPN connection %1 failed\n%2", connection->name(), vpnPlugin->lastErrorMessage());
                QTimer::singleShot(5000, this, SLOT(resetStatusBarText()));
                Q_EMIT statusBarTextChanged();
            } else {
                m_statusBarText = i18n("VPN connection %1 exported successfully", connection->name());
                QTimer::singleShot(5000, this, SLOT(resetStatusBarText()));
                Q_EMIT statusBarTextChanged();
            }
        }
        delete vpnPlugin;
    } else {
        qWarning() << "Error getting VpnUiPlugin for export:" << error;
    }
}

void ConnectionEditor::connectionAdded(const QString& connection)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnection(connection);

    if (!con) {
        return;
    }

    if (con->settings()->isSlave())
        return;

    m_statusBarText = i18n("Connection %1 has been added", con->name());
    QTimer::singleShot(5000, this, SLOT(resetStatusBarText()));
    Q_EMIT statusBarTextChanged();
}

void ConnectionEditor::resetStatusBarText()
{
    m_statusBarText = QString();
    Q_EMIT statusBarTextChanged();
}
