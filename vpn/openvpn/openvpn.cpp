/*
    SPDX-FileCopyrightText: 2008 Will Stephenson <wstephenson@kde.org>
    SPDX-FileCopyrightText: 2011-2012 Rajeesh K Nambiar <rajeeshknambiar@gmail.com>
    SPDX-FileCopyrightText: 2011 Ilia Kats <ilia-kats@gmx.net>
    SPDX-FileCopyrightText: 2012-2016 Lamarque V. Souza <lamarque@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "openvpn.h"

#include <QLatin1Char>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStringBuilder>

#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>
#include <kwidgetsaddons_version.h>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/VpnSetting>

#include "openvpnauth.h"
#include "openvpnwidget.h"

#include <arpa/inet.h>

#include "nm-openvpn-service.h"

K_PLUGIN_CLASS_WITH_JSON(OpenVpnUiPlugin, "plasmanetworkmanagement_openvpnui.json")

#define AUTH_TAG "auth"
#define AUTH_USER_PASS_TAG "auth-user-pass"
#define CA_TAG "ca"
#define CERT_TAG "cert"
#define CIPHER_TAG "cipher"
#define CLIENT_TAG "client"
#define COMPRESS_TAG "compress"
#define COMP_TAG "comp-lzo"
#define DEV_TAG "dev"
#define FRAGMENT_TAG "fragment"
#define IFCONFIG_TAG "ifconfig"
#define KEY_TAG "key"
#define MSSFIX_TAG "mssfix"
#define PKCS12_TAG "pkcs12"
#define PORT_TAG "port"
#define PROTO_TAG "proto"
#define HTTP_PROXY_TAG "http-proxy"
#define HTTP_PROXY_RETRY_TAG "http-proxy-retry"
#define SOCKS_PROXY_TAG "socks-proxy"
#define SOCKS_PROXY_RETRY_TAG "socks-proxy-retry"
#define REMOTE_TAG "remote"
#define RENEG_SEC_TAG "reneg-sec"
#define RPORT_TAG "rport"
#define SECRET_TAG "secret"
#define TLS_AUTH_TAG "tls-auth"
#define TLS_CRYPT_TAG "tls-crypt"
#define TLS_CLIENT_TAG "tls-client"
#define TLS_REMOTE_TAG "tls-remote"
#define TUNMTU_TAG "tun-mtu"
#define KEY_DIRECTION_TAG "key-direction"

#define BEGIN_KEY_CA_TAG "<ca>"
#define END_KEY_CA_TAG "</ca>"
#define BEGIN_KEY_CERT_TAG "<cert>"
#define END_KEY_CERT_TAG "</cert>"
#define BEGIN_KEY_KEY_TAG "<key>"
#define END_KEY_KEY_TAG "</key>"
#define BEGIN_KEY_SECRET_TAG "<secret>"
#define END_KEY_SECRET_TAG "</secret>"
#define BEGIN_TLS_AUTH_TAG "<tls-auth>"
#define END_TLS_AUTH_TAG "</tls-auth>"
#define BEGIN_TLS_CRYPT_TAG "<tls-crypt>"
#define END_TLS_CRYPT_TAG "</tls-crypt>"

#define PROC_TYPE_TAG "Proc-Type: 4,ENCRYPTED"
#define PKCS8_TAG "-----BEGIN ENCRYPTED PRIVATE KEY-----"

QString localCertPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/networkmanagement/certificates/");
}

QString unQuote(QString &certVal, const QString &fileName)
{
    /* Unquote according to openvpn rules
     * Unquoted filename is returned, and @certVal is modified
     * to the leftover string
     */
    int nextSep;
    QString certFile = certVal.trimmed();
    if (certFile.startsWith('"') || certFile.startsWith('\'')) { // Quoted
        certFile.remove(0, 1); // Remove the starting quote
        nextSep = 0;
        while ((nextSep = certFile.indexOf(QRegularExpression(QStringLiteral("\"|'")), nextSep)) != -1) {
            if (nextSep > 0 && certFile.at(nextSep - 1) != '\\') { // Quote not escaped
                certVal = certFile.right(certFile.length() - nextSep - 1); // Leftover string
                certFile.truncate(nextSep); // Quoted string
                break;
            }
        }
    } else {
        nextSep = certFile.indexOf(QRegularExpression(QStringLiteral("\\s"))); // First whitespace
        if (nextSep != -1) {
            certVal = certFile.right(certFile.length() - nextSep - 1); // Leftover
            certFile = certFile.left(nextSep); // value
        } else {
            certVal.clear();
        }
    }
    certFile.replace("\\\\", "\\"); // Replace '\\' with '\'
    certFile.replace("\\ ", " "); // Replace escaped space with space
    if (QFileInfo(certFile).isRelative()) {
        certFile = QFileInfo(fileName).dir().absolutePath() + QLatin1Char('/') + certFile;
    }
    return certFile;
}

bool isEncrypted(const QString &fileName)
{
    bool encrypted = false;
    // TODO: if is_pkcs12(fileName) return true;
    // NOTE: will have to use SEC_PKCS12DecoderStart and friends from <p12.h>, which will
    //       build a new dependency on nss-devel. See NetworkManager/libnm-util/crypto_nss.c+453

    QFile inFile(fileName);
    if (!inFile.open(QFile::ReadOnly)) {
        return false;
    }
    QTextStream in(&inFile);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (!line.isEmpty() && (line.startsWith(PROC_TYPE_TAG) || line.startsWith(PKCS8_TAG))) {
            encrypted = true;
            break;
        }
    }
    inFile.close();
    return encrypted;
}

OpenVpnUiPlugin::OpenVpnUiPlugin(QObject *parent, const QVariantList &)
    : VpnUiPlugin(parent)
{
}

OpenVpnUiPlugin::~OpenVpnUiPlugin() = default;

SettingWidget *OpenVpnUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget *parent)
{
    auto wid = new OpenVpnSettingWidget(setting, parent);
    return wid;
}

SettingWidget *OpenVpnUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, const QStringList &hints, QWidget *parent)
{
    return new OpenVpnAuthWidget(setting, hints, parent);
}

QString OpenVpnUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    return connection->id() + QStringLiteral("_openvpn.conf");
}

QStringList OpenVpnUiPlugin::supportedFileExtensions() const
{
    return {QStringLiteral("*.ovpn"), QStringLiteral("*.conf")};
}

VpnUiPlugin::ImportResult OpenVpnUiPlugin::importConnectionSettings(const QString &fileName)
{
    GError *error = nullptr;

    GSList *plugins = nm_vpn_plugin_info_list_load();

    NMVpnPluginInfo *plugin_info = nm_vpn_plugin_info_list_find_by_service(plugins, "org.freedesktop.NetworkManager.openvpn");

    if (!plugin_info) {
        return VpnUiPlugin::ImportResult::fail(i18n("NetworkManager is missing support for OpenVPN"));
    }

    NMVpnEditorPlugin *plugin = nm_vpn_plugin_info_load_editor_plugin(plugin_info, &error);

    NMConnection *connection = nm_vpn_editor_plugin_import(plugin, fileName.toUtf8().constData(), &error);

    if (!connection) {
        const QString errorMessage = QString::fromUtf8(error->message);
        g_error_free(error);

        return VpnUiPlugin::ImportResult::fail(errorMessage);
    }

    return VpnUiPlugin::ImportResult::pass(connection);
}

QString OpenVpnUiPlugin::saveFile(QTextStream &in, const QString &endTag, const QString &connectionName, const QString &fileName)
{
    const QString certificatesDirectory = localCertPath() + connectionName;
    const QString absoluteFilePath = certificatesDirectory + '/' + fileName;
    QFile outFile(absoluteFilePath);

    QDir().mkpath(certificatesDirectory);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        KMessageBox::information(nullptr, i18n("Error saving file %1: %2", absoluteFilePath, outFile.errorString()));
        return {};
    }

    QTextStream out(&outFile);
    while (!in.atEnd()) {
        const QString line = in.readLine();

        if (line.indexOf(endTag) >= 0) {
            break;
        }

        out << line << "\n";
    }

    outFile.close();
    return absoluteFilePath;
}

QString OpenVpnUiPlugin::tryToCopyToCertificatesDirectory(const QString &connectionName, const QString &sourceFilePath)
{
    const QString certificatesDirectory = localCertPath();
    const QString absoluteFilePath = certificatesDirectory + connectionName + '_' + QFileInfo(sourceFilePath).fileName();

    QFile sourceFile(sourceFilePath);

    QDir().mkpath(certificatesDirectory);
    if (!sourceFile.copy(absoluteFilePath)) {
        KMessageBox::information(nullptr, i18n("Error copying certificate to %1: %2", absoluteFilePath, sourceFile.errorString()));
        return sourceFilePath;
    }

    return absoluteFilePath;
}

VpnUiPlugin::ExportResult OpenVpnUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    QFile expFile(fileName);
    if (!expFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return VpnUiPlugin::ExportResult::fail("Could not open file for writing");
    }

    NMClient *client = nm_client_new(NULL, NULL);
    const GPtrArray *connections = nm_client_get_connections(client);
    NMConnection *found = NULL;

    for (uint i = 0; i < connections->len; i++) {
        auto conn = static_cast<NMConnection *>(g_ptr_array_index(connections, i));
        if (nm_connection_get_id(conn) == connection->id()) {
            found = conn;
            break;
        }
    }

    if (!found) {
        return VpnUiPlugin::ExportResult::fail(QString());
    }

    GError *error = nullptr;
    GSList *plugins = nm_vpn_plugin_info_list_load();
    NMVpnPluginInfo *plugin_info = nm_vpn_plugin_info_list_find_by_service(plugins, "org.freedesktop.NetworkManager.openvpn");
    if (!plugin_info) {
        return VpnUiPlugin::ExportResult::fail(i18n("NetworkManager is missing support for OpenVPN"));
    }

    NMVpnEditorPlugin *plugin = nm_vpn_plugin_info_load_editor_plugin(plugin_info, &error);

    nm_vpn_editor_plugin_export(plugin, fileName.toUtf8().constData(), found, &error);
    if (!connection) {
        const QString errorMessage = QString::fromUtf8(error->message);
        g_error_free(error);

        return VpnUiPlugin::ExportResult::fail(errorMessage);
    }

    return VpnUiPlugin::ExportResult::pass();
}

#include "openvpn.moc"

#include "moc_openvpn.cpp"
