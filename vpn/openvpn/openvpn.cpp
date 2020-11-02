/*
    Copyright 2008 Will Stephenson <wstephenson@kde.org>
    Copyright 2011-2012 Rajeesh K Nambiar <rajeeshknambiar@gmail.com>
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>
    Copyright 2012-2016 Lamarque V. Souza <lamarque@kde.org>

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

#include "openvpn.h"

#include <QLatin1Char>
#include <QStringBuilder>
#include <QStandardPaths>

#include <KPluginFactory>
#include <KLocalizedString>
#include <KMessageBox>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/Ipv4Setting>

#include "openvpnwidget.h"
#include "openvpnauth.h"

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

#define PROC_TYPE_TAG "Proc-Type: 4,ENCRYPTED"
#define PKCS8_TAG "-----BEGIN ENCRYPTED PRIVATE KEY-----"

QString localCertPath()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
        QLatin1String("/networkmanagement/certificates/");
}

QString unQuote(QString &certVal, const QString &fileName)
{
    /* Unquote according to openvpn rules
     * Unquoted filename is returned, and @certVal is modified
     * to the leftover string
     */
    int nextSep;
    QString certFile = certVal.trimmed();
    if (certFile.startsWith('"') || certFile.startsWith('\'')) {  // Quoted
        certFile.remove(0,1);   // Remove the starting quote
        nextSep = 0;
        while ((nextSep = certFile.indexOf(QRegExp("\"|'"), nextSep)) != -1) {
            if (nextSep > 0 && certFile.at(nextSep - 1) != '\\')  {  // Quote not escaped
                certVal = certFile.right(certFile.length() - nextSep - 1);  // Leftover string
                certFile.truncate(nextSep);           // Quoted string
                break;
            }
        }
    } else {
        nextSep = certFile.indexOf(QRegExp("\\s"));   // First whitespace
        if (nextSep != -1) {
            certVal = certFile.right(certFile.length() - nextSep - 1);  // Leftover
            certFile = certFile.left(nextSep);        // value
        } else {
            certVal.clear();
        }
    }
    certFile.replace("\\\\", "\\");     // Replace '\\' with '\'
    certFile.replace("\\ ", " ");        // Replace escaped space with space
    if (QFileInfo(certFile).isRelative()) {
        certFile = QFileInfo(fileName).dir().absolutePath() + '/' + certFile;
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
        QString line = in.readLine();
        if (!line.isEmpty() && (line.startsWith(PROC_TYPE_TAG) || line.startsWith(PKCS8_TAG))) {
            encrypted = true;
            break;
        }
    }
    inFile.close();
    return encrypted;
}

OpenVpnUiPlugin::OpenVpnUiPlugin(QObject * parent, const QVariantList &) : VpnUiPlugin(parent)
{
}

OpenVpnUiPlugin::~OpenVpnUiPlugin()
{
}

SettingWidget * OpenVpnUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    OpenVpnSettingWidget * wid = new OpenVpnSettingWidget(setting, parent);
    return wid;
}

SettingWidget * OpenVpnUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new OpenVpnAuthWidget(setting, parent);
}

QString OpenVpnUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    return connection->id() + "_openvpn.conf";
}

QString OpenVpnUiPlugin::supportedFileExtensions() const
{
    return "*.ovpn *.conf";
}

NMVariantMapMap OpenVpnUiPlugin::importConnectionSettings(const QString &fileName)
{
    NMVariantMapMap result;

    QFile impFile(fileName);
    if (!impFile.open(QFile::ReadOnly|QFile::Text)) {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("Could not open file");
        return result;
    }

    bool copyCertificates;
    KMessageBox::ButtonCode buttonCode;
    if (KMessageBox::shouldBeShownYesNo(QLatin1String("copyCertificatesDialog"), buttonCode)) {
        copyCertificates = KMessageBox::questionYesNo(nullptr, i18n("Do you want to copy your certificates to %1?", localCertPath()),
                                   i18n("Copy certificates"), KStandardGuiItem::yes(), KStandardGuiItem::no(), QLatin1String("copyCertificatesDialog")) == KMessageBox::Yes;
    } else {
        copyCertificates = buttonCode == KMessageBox::Yes;
    }

    const QString connectionName = QFileInfo(fileName).completeBaseName();
    NMStringMap dataMap;
    NMStringMap secretData;
    QVariantMap ipv4Data;

    QString proxy_type;
    QString proxy_user;
    QString proxy_passwd;
    bool have_client = false;
    bool have_remote = false;
    bool proxy_set = false;
    bool have_pass = false;
    bool have_sk = false;
    int key_direction = -1;

    QTextStream in(&impFile);
    while (!in.atEnd()) {
        QStringList key_value;
        QString line = in.readLine();
        // Skip comments
        if (line.indexOf('#') >= 0) {
            line.truncate(line.indexOf('#'));
        }
        if (line.indexOf(';') >= 0) {
            line.truncate(line.indexOf(';'));
        }
        if (line.isEmpty()) {
            continue;
        }
        key_value.clear();
        key_value << line.split(QRegExp("\\s+")); // Split at one or more whitespaces

        if (key_value[0] == CLIENT_TAG || key_value[0] == TLS_CLIENT_TAG) {
            have_client = true;
            continue;
        }
        if (key_value[0] == DEV_TAG) {
            if (key_value.count() == 2) {
                if (key_value[1].startsWith(QLatin1String("tun"))) {
                    // ignore; default is tun
                } else if (key_value[1].startsWith(QLatin1String("tap"))) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TAP_DEV), "yes");
                } else {
                    KMessageBox::information(nullptr, i18n("Unknown option: %1", line));
                }
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == PROTO_TAG) {
            if (key_value.count() == 2) {
                /* Valid parameters are "udp", "tcp-client" and "tcp-server".
                 * 'tcp' isn't technically valid, but it used to be accepted so
                 * we'll handle it here anyway.
                 */
                if (key_value[1] == "udp") {
                    // ignore; default is udp
                } else if (key_value[1] == "tcp-client" || key_value[1] == "tcp-server" || key_value[1] == "tcp") {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PROTO_TCP), "yes");
                } else {
                    KMessageBox::information(nullptr, i18n("Unknown option: %1", line));
                }
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == MSSFIX_TAG) {
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_MSSFIX), "yes");
            continue;
        }
        if (key_value[0] == TUNMTU_TAG) {
            if (key_value.count() == 2) {
                if (key_value[1].toLong() >= 0 && key_value[1].toLong() < 0xFFFF ) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TUNNEL_MTU), key_value[1]);
                } else {
                    KMessageBox::information(nullptr, i18n("Invalid size (should be between 0 and 0xFFFF) in option: %1", line));
                }
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == FRAGMENT_TAG) {
            if (key_value.count() == 2) {
                if (key_value[1].toLong() >= 0 && key_value[1].toLong() < 0xFFFF ) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_FRAGMENT_SIZE), key_value[1]);
                } else {
                    KMessageBox::information(nullptr, i18n("Invalid size (should be between 0 and 0xFFFF) in option: %1", line));
                }
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == COMP_TAG) {
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_COMP_LZO), "yes");
            continue;
        }
        if (key_value[0] == COMPRESS_TAG) {
            if (key_value.count() > 1) {
                if (key_value[1] == "yes") {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), "yes");
                    continue;
                } else if (key_value[1] == "lzo") {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), "lzo");
                    continue;
                } else if (key_value[1] == "lz4") {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), "lz4");
                    continue;
                } else if (key_value[1] == "lz4-v2") {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), "lz4-v2");
                    continue;
                }
            } else {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_COMPRESS), "yes");
                continue;
            }
        }
        if (key_value[0] == RENEG_SEC_TAG) {
            if (key_value.count() == 2) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_RENEG_SECONDS), key_value[1]);
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == HTTP_PROXY_RETRY_TAG || key_value[0] == SOCKS_PROXY_RETRY_TAG) {
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_RETRY), "yes");
            continue;
        }
        if (key_value[0] == HTTP_PROXY_TAG) {
            proxy_type = "http";
        }
        if (key_value[0] == SOCKS_PROXY_TAG) {
            proxy_type = "socks";
        }
        if (!proxy_type.isEmpty() && !proxy_set && key_value.count() >= 3) {
            bool success = true;
            if (proxy_type == "http" && key_value.count() >= 4) {
                // Parse the HTTP proxy file
                QFile httpProxyFile(QFileInfo(fileName).dir().absolutePath() + '/' + key_value[3]);
                if (httpProxyFile.open(QFile::ReadOnly|QFile::Text)) {
                    QTextStream httpProxyIn(&httpProxyFile);
                    while (!httpProxyIn.atEnd()) {
                        QString httpProxyLine = httpProxyIn.readLine();
                        if (httpProxyLine.isEmpty()) {
                            continue;
                        }
                        if (proxy_user.isEmpty()) {
                            proxy_user = httpProxyLine;
                        }
                        if (proxy_passwd.isEmpty()) {
                            proxy_passwd = httpProxyLine;
                            break;
                        }
                    }
                    if (proxy_user.isEmpty()||proxy_passwd.isEmpty()) {
                        success = false;
                    }
                }
            }
            if (success && !proxy_type.isEmpty() && key_value[2].toLong() > 0 // Port
                                                 && key_value[2].toLong() < 65536) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_TYPE), proxy_type);
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_SERVER), key_value[1]);  // Proxy server
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PROXY_PORT), key_value[2]);    // Port
                if (!proxy_user.isEmpty()) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_USERNAME), proxy_user);
                }
                if (!proxy_passwd.isEmpty()) {
                    secretData.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD), proxy_passwd);
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD"-flags"), QString::number(NetworkManager::Setting::NotSaved));
                }
                proxy_set = true;
            }
            if (!success) {
                KMessageBox::information(nullptr, i18n("Invalid proxy option: %1", line));
            }
            continue;
        }
        if (key_value[0] == REMOTE_TAG) {
            if (have_remote) {
                continue;
            }
            if (key_value.count() >= 2 && key_value.count() <= 4) {
                QString remote = key_value[1];
                if (remote.startsWith(QLatin1Char('\'')) || remote.startsWith(QLatin1Char('"'))) {
                    remote.remove(0, 1); // Remove first quote
                    remote.remove(remote.size() - 1, 1); // Remove last quote
                }
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE), remote);
                have_remote = true;
                if (key_value.count() >= 3 && key_value[2].toLong() > 0
                                           && key_value[2].toLong() < 65536) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PORT), key_value[2]);
                    if (key_value.count() == 4) {
                        QString protocol = key_value[3];
                        if(protocol.startsWith(QLatin1String("tcp"))) {
                            dataMap[QLatin1String(NM_OPENVPN_KEY_PROTO_TCP)] = QLatin1String("yes");
                        }
                    }
                }
            }
        }
        if (key_value[0] == PORT_TAG || key_value[0] == RPORT_TAG) {
            // Port specified in 'remote' always takes precedence
            if (!dataMap.contains(NM_OPENVPN_KEY_PORT)) {
                if (key_value.count() == 2 ) {
                    if (key_value[1].toLong() > 0 && key_value[1].toLong() < 65536) {
                        dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PORT), key_value[1]);
                    } else {
                        KMessageBox::information(nullptr, i18n("Invalid port (should be between 1 and 65535) in option: %1", line));
                    }
                } else
                    KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == PKCS12_TAG && key_value.count() > 1) {
            key_value[1] = line.right(line.length() - line.indexOf(QRegExp("\\s"))); // Get whole string after key
            QString certFile = unQuote(key_value[1], fileName);
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CA), certFile);
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CERT), certFile);
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_KEY), certFile);
            continue;
        }
        if (key_value[0] == CA_TAG && key_value.count() > 1) {
            key_value[1] = line.right(line.length() - line.indexOf(QRegExp("\\s"))); // Get whole string after key
            if (key_value[1].trimmed() == QLatin1String("[inline]")) {
                // No data or file to copy for now, it will be available later when we reach <ca> tag.
                continue;
            }
            if (copyCertificates) {
                const QString absoluteFilePath = tryToCopyToCertificatesDirectory(connectionName, unQuote(key_value[1], fileName));
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CA), absoluteFilePath);
            } else {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CA), unQuote(key_value[1], fileName));
            }
            continue;
        }
        if (key_value[0] == CERT_TAG && key_value.count() > 1) {
            key_value[1] = line.right(line.length() - line.indexOf(QRegExp("\\s"))); // Get whole string after key
            if (key_value[1].trimmed() == QLatin1String("[inline]")) {
                // No data or file to copy for now, it will be available later when we reach <cert> tag.
                continue;
            }
            if (copyCertificates) {
                const QString absoluteFilePath = tryToCopyToCertificatesDirectory(connectionName, unQuote(key_value[1], fileName));
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CERT), absoluteFilePath);
            } else {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CERT), unQuote(key_value[1], fileName));
            }
            continue;
        }
        if (key_value[0] == KEY_TAG && key_value.count() > 1) {
            key_value[1] = line.right(line.length() - line.indexOf(QRegExp("\\s"))); // Get whole string after key
            if (key_value[1].trimmed() == QLatin1String("[inline]")) {
                // No data or file to copy for now, it will be available later when we reach <key> tag.
                continue;
            }
            if (copyCertificates) {
                const QString absoluteFilePath = tryToCopyToCertificatesDirectory(connectionName, unQuote(key_value[1], fileName));
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_KEY), absoluteFilePath);
            } else {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_KEY), unQuote(key_value[1], fileName));
            }
            continue;
        }
        if (key_value[0] == SECRET_TAG && key_value.count() > 1) {
            key_value[1] = line.right(line.length() - line.indexOf(QRegExp("\\s"))); // Get whole string after key
            if (copyCertificates) {
                const QString absoluteFilePath = tryToCopyToCertificatesDirectory(connectionName, unQuote(key_value[1], fileName));
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY), absoluteFilePath);
            } else {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY), unQuote(key_value[1], fileName));
            }
            if (key_value.count() > 2) {
                key_value[2] = key_value[1];
                if (!key_value[2].isEmpty() && (key_value[2].toLong() == 0 ||key_value[2].toLong() == 1)) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION), key_value[2]);
                }
            }
            have_sk = true;
            continue;
        }
        if (key_value[0] == TLS_AUTH_TAG && key_value.count() >1) {
            key_value[1] = line.right(line.length() - line.indexOf(QRegExp("\\s"))); // Get whole string after key

            // We will copy inline certificate later when we reach <tls-auth> tag.
            if (key_value[1].trimmed() != QLatin1String("[inline]")) {
                if (copyCertificates) {
                    const QString absoluteFilePath = tryToCopyToCertificatesDirectory(connectionName, unQuote(key_value[1], fileName));
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TA), absoluteFilePath);
                } else {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TA), unQuote(key_value[1], fileName));
                }
            }

            if (key_value.count() > 2) {
                key_value[2] = key_value[1];
                if (!key_value[2].isEmpty() && (key_value[2].toLong() == 0 ||key_value[2].toLong() == 1)) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TA_DIR), key_value[2]);
                }
            }
            continue;
        }
        if (key_value[0] == CIPHER_TAG) {
            if (key_value.count() == 2) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CIPHER), key_value[1]);
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == TLS_REMOTE_TAG) {
            if (!unQuote(key_value[1], fileName).isEmpty()) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TLS_REMOTE), key_value[1]);
            } else {
                KMessageBox::information(nullptr, i18n("Unknown option: %1", line));
            }
            continue;
        }
        if (key_value[0] == IFCONFIG_TAG) {
            if (key_value.count() == 3) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_LOCAL_IP), key_value[1]);
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_REMOTE_IP), key_value[2]);
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 2) in option: %1", line));
            }
            continue;
        }
        if (key_value[0] == AUTH_USER_PASS_TAG) {
            have_pass = true;
        }
        if (key_value[0] == AUTH_TAG) {
            if (key_value.count() == 2) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_AUTH), key_value[1]);
            } else {
                KMessageBox::information(nullptr, i18n("Invalid number of arguments (expected 1) in option: %1", line));
            }
            continue;
        }

        if (key_value[0] == KEY_DIRECTION_TAG) {
            if (key_value.count() == 2) {
                key_direction = key_value[1].toInt();
            }

            if (key_direction != 0 && key_direction != 1) {
                KMessageBox::information(nullptr, i18n("Invalid argument in option: %1", line));
                key_direction = -1;
            }

            // `key-direction` may also be specified *after* the <tls-auth> etc. block in an *.ovpn file
            if (key_direction > -1) {
                if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_TA))
                    && !dataMap.contains(QLatin1String(NM_OPENVPN_KEY_TA_DIR))) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TA_DIR), QString().setNum(key_direction));
                }
                if (dataMap.contains(QLatin1String(NM_OPENVPN_KEY_KEY))
                    && !dataMap.contains(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION))) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION), QString().setNum(key_direction));
                }
            }

            continue;
        }

        if (key_value[0] == BEGIN_KEY_CA_TAG) {
            const QString caAbsolutePath = saveFile(in, QLatin1String(END_KEY_CA_TAG), connectionName, "ca.crt");
            if (!caAbsolutePath.isEmpty()) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CA), caAbsolutePath);
            }
            continue;
        } else if (key_value[0] == BEGIN_KEY_CERT_TAG) {
            const QString certAbsolutePath = saveFile(in, QLatin1String(END_KEY_CERT_TAG), connectionName, "cert.crt");
            if (!certAbsolutePath.isEmpty()) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CERT), certAbsolutePath);
            }
            continue;
        } else if (key_value[0] == BEGIN_KEY_KEY_TAG) {
            const QString keyAbsolutePath = saveFile(in, QLatin1String(END_KEY_KEY_TAG), connectionName, "private.key");
            if (!keyAbsolutePath.isEmpty()) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_KEY), keyAbsolutePath);
            }
            continue;
        } else if (key_value[0] == BEGIN_KEY_SECRET_TAG) {
            const QString secretAbsolutePath = saveFile(in, QLatin1String(END_KEY_SECRET_TAG), connectionName, "secret.key");
            if (!secretAbsolutePath.isEmpty()) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_KEY), secretAbsolutePath);
                have_sk = true;

                if (key_direction > -1) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_STATIC_KEY_DIRECTION), QString().setNum(key_direction));
                }
            }
            continue;
        } else if (key_value[0] == BEGIN_TLS_AUTH_TAG) {
            const QString tlsAuthAbsolutePath = saveFile(in, QLatin1String(END_TLS_AUTH_TAG), connectionName, "tls_auth.key");
            if (!tlsAuthAbsolutePath.isEmpty()) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TA), tlsAuthAbsolutePath);

                if (key_direction > -1) {
                    dataMap.insert(QLatin1String(NM_OPENVPN_KEY_TA_DIR), QString().setNum(key_direction));
                }
            }
            continue;
        }

        // Import X-NM-Routes if present
        if (key_value[0] == "X-NM-Routes") {
            QList<NetworkManager::IpRoute> list;
            for (int i = 1; i < key_value.count(); i++) {
                NetworkManager::IpRoute ipRoute;
                ipRoute.setIp(QHostAddress(key_value[1].split('/').first()));
                ipRoute.setPrefixLength(key_value[1].split('/').at(1).toInt());
                list << ipRoute;
            }
            QList<QList<uint> > dbusRoutes;
            for (const NetworkManager::IpRoute &route : list) {
                QList<uint> dbusRoute;
                dbusRoute << htonl(route.ip().toIPv4Address())
                        << route.prefixLength()
                        << htonl(route.nextHop().toIPv4Address())
                        << route.metric();
                dbusRoutes << dbusRoute;
            }
            ipv4Data.insert("routes", QVariant::fromValue(dbusRoutes));
            continue;
        }
    }
    if (!have_client && !have_sk) {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("File %1 is not a valid OpenVPN's client configuration file", fileName);
        return result;
    } else if (!have_remote) {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("File %1 is not a valid OpenVPN configuration (no remote).", fileName);
        return result;
    } else {
        QString conType;
        bool have_certs = false;
        bool have_ca = false;

        if (dataMap.contains(NM_OPENVPN_KEY_CA)) {
            have_ca = true;
        }
        if (have_ca && dataMap.contains(NM_OPENVPN_KEY_CERT) && dataMap.contains(NM_OPENVPN_KEY_KEY)) {
            have_certs = true;
        }
        // Determine connection type
        if (have_pass) {
            if (have_certs) {
                conType = NM_OPENVPN_CONTYPE_PASSWORD_TLS;
            } else if (have_ca) {
                conType = NM_OPENVPN_CONTYPE_PASSWORD;
            }
        } else if (have_certs) {
            conType = NM_OPENVPN_CONTYPE_TLS;
        } else if (have_sk) {
            conType = NM_OPENVPN_CONTYPE_STATIC_KEY;
        }
        if (conType.isEmpty()) {
            conType = NM_OPENVPN_CONTYPE_TLS;
        }
        dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CONNECTION_TYPE), conType);
        // Default secret flags to be agent-owned
        if (have_pass) {
            dataMap.insert(QLatin1String(NM_OPENVPN_KEY_PASSWORD"-flags"), QString::number(NetworkManager::Setting::AgentOwned));
        }
        if (have_certs) {
            if (dataMap.contains(NM_OPENVPN_KEY_KEY) && isEncrypted(dataMap[NM_OPENVPN_KEY_KEY])) {
                dataMap.insert(QLatin1String(NM_OPENVPN_KEY_CERTPASS"-flags"), QString::number(NetworkManager::Setting::AgentOwned));
            }
        }
    }

    // Set the '...-type' and '...-flags' value also
    NetworkManager::VpnSetting setting;
    setting.setServiceType("org.freedesktop.NetworkManager.openvpn");
    setting.setData(dataMap);
    setting.setSecrets(secretData);

    QVariantMap conn;
    conn.insert("id", connectionName);
    conn.insert("type", "vpn");
    result.insert("connection", conn);

    result.insert("vpn", setting.toMap());

    if (!ipv4Data.isEmpty()) {
        result.insert("ipv4", ipv4Data);
    }

    impFile.close();
    return result;
}

QString OpenVpnUiPlugin::saveFile(QTextStream &in, const QString &endTag, const QString &connectionName, const QString &fileName)
{
    const QString certificatesDirectory = localCertPath() + connectionName;
    const QString absoluteFilePath = certificatesDirectory + '/' + fileName;
    QFile outFile(absoluteFilePath);

    QDir().mkpath(certificatesDirectory);
    if (!outFile.open(QFile::WriteOnly | QFile::Text)) {
        KMessageBox::information(nullptr, i18n("Error saving file %1: %2", absoluteFilePath, outFile.errorString()));
        return QString();
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

bool OpenVpnUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    QFile expFile(fileName);
    if (! expFile.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("Could not open file for writing");
        return false;
    }

    NMStringMap dataMap;
    NMStringMap secretData;

    NetworkManager::VpnSetting::Ptr vpnSetting = connection->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
    dataMap = vpnSetting->data();
    secretData = vpnSetting->secrets();

    QString line;
    QString cacert, user_cert, private_key;

    line = QString(CLIENT_TAG) + '\n';
    expFile.write(line.toLatin1());
    line = QString(REMOTE_TAG) + ' ' + dataMap[NM_OPENVPN_KEY_REMOTE] +
           (dataMap[NM_OPENVPN_KEY_PORT].isEmpty() ? "\n" : (' ' + dataMap[NM_OPENVPN_KEY_PORT]) + '\n');
    expFile.write(line.toLatin1());
    if (dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_TLS ||
            dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_PASSWORD ||
            dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_PASSWORD_TLS) {
        if (!dataMap[NM_OPENVPN_KEY_CA].isEmpty()) {
            cacert = dataMap[NM_OPENVPN_KEY_CA];
        }
    }
    if (dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_TLS ||
            dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_PASSWORD_TLS) {
        if (!dataMap[NM_OPENVPN_KEY_CERT].isEmpty()) {
            user_cert = dataMap[NM_OPENVPN_KEY_CERT];
        } if (!dataMap[NM_OPENVPN_KEY_KEY].isEmpty()) {
            private_key = dataMap[NM_OPENVPN_KEY_KEY];
        }

    }
    // Handle PKCS#12 (all certs are the same file)
    if (!cacert.isEmpty() && !user_cert.isEmpty() && !private_key.isEmpty()
                          && cacert == user_cert && cacert == private_key) {
        line = QString("%1 \"%2\"\n").arg(PKCS12_TAG, cacert);
        expFile.write(line.toLatin1());
    } else {
        if (!cacert.isEmpty()) {
            line = QString("%1 \"%2\"\n").arg(CA_TAG, cacert);
            expFile.write(line.toLatin1());
        }
        if (!user_cert.isEmpty()) {
            line = QString("%1 \"%2\"\n").arg(CERT_TAG, user_cert);
            expFile.write(line.toLatin1());
        }
        if (!private_key.isEmpty()) {
            line = QString("%1 \"%2\"\n").arg(KEY_TAG, private_key);
            expFile.write(line.toLatin1());
        }
    }
    if (dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_TLS ||
        dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_STATIC_KEY ||
        dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_PASSWORD ||
        dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_PASSWORD_TLS) {
        line = QString(AUTH_USER_PASS_TAG) + '\n';
        expFile.write(line.toLatin1());
        if (!dataMap[NM_OPENVPN_KEY_TLS_REMOTE].isEmpty()) {
            line = QString(TLS_REMOTE_TAG) + " \"" + dataMap[NM_OPENVPN_KEY_TLS_REMOTE] + "\"\n";
            expFile.write(line.toLatin1());
        }
        if (!dataMap[NM_OPENVPN_KEY_TA].isEmpty()) {
            line = QString(TLS_AUTH_TAG) + " \"" + dataMap[NM_OPENVPN_KEY_TA] + '\"' + (dataMap[NM_OPENVPN_KEY_TA_DIR].isEmpty() ?
                                "\n" : (' ' + dataMap[NM_OPENVPN_KEY_TA_DIR]) + '\n');
            expFile.write(line.toLatin1());
        }
    }
    if (dataMap[NM_OPENVPN_KEY_CONNECTION_TYPE] == NM_OPENVPN_CONTYPE_STATIC_KEY) {
        line = QString(SECRET_TAG) + " \"" + dataMap[NM_OPENVPN_KEY_STATIC_KEY] + '\"' + (dataMap[NM_OPENVPN_KEY_STATIC_KEY_DIRECTION].isEmpty() ?
                          "\n" : (' ' + dataMap[NM_OPENVPN_KEY_STATIC_KEY_DIRECTION]) + '\n');
        expFile.write(line.toLatin1());
    }
    if (dataMap.contains(NM_OPENVPN_KEY_RENEG_SECONDS) && !dataMap[NM_OPENVPN_KEY_RENEG_SECONDS].isEmpty()) {
        line = QString(RENEG_SEC_TAG) + ' ' + dataMap[NM_OPENVPN_KEY_RENEG_SECONDS] + '\n';
        expFile.write(line.toLatin1());
    }
    if (!dataMap[NM_OPENVPN_KEY_CIPHER].isEmpty()) {
        line = QString(CIPHER_TAG) + ' ' + dataMap[NM_OPENVPN_KEY_CIPHER] + '\n';
        expFile.write(line.toLatin1());
    }
    if (dataMap[NM_OPENVPN_KEY_COMP_LZO] == "adaptive") {
        line = QString(COMP_TAG) + " adaptive\n";
        expFile.write(line.toLatin1());
    }
    if (dataMap[NM_OPENVPN_KEY_COMPRESS] == "yes") {
        line = QString(COMPRESS_TAG) + " yes\n";
        expFile.write(line.toLatin1());
    }
    if (dataMap[NM_OPENVPN_KEY_COMPRESS] == "lzo") {
        line = QString(COMPRESS_TAG) + " lzo\n";
        expFile.write(line.toLatin1());
    }
    if (dataMap[NM_OPENVPN_KEY_COMPRESS] == "lz4") {
        line = QString(COMPRESS_TAG) + " lz4\n";
        expFile.write(line.toLatin1());
    }
    if (dataMap[NM_OPENVPN_KEY_COMPRESS] == "lz4-v2") {
        line = QString(COMPRESS_TAG) + " lz4-v2\n";
        expFile.write(line.toLatin1());
    }
    if (dataMap[NM_OPENVPN_KEY_MSSFIX] == "yes") {
        line = QString(MSSFIX_TAG) + '\n';
        expFile.write(line.toLatin1());
    }
    if (!dataMap[NM_OPENVPN_KEY_TUNNEL_MTU].isEmpty()) {
        line = QString(TUNMTU_TAG) + ' ' + dataMap[NM_OPENVPN_KEY_TUNNEL_MTU] + '\n';
        expFile.write(line.toLatin1());
    }
    if (!dataMap[NM_OPENVPN_KEY_FRAGMENT_SIZE].isEmpty()) {
        line = QString(FRAGMENT_TAG) + ' ' + dataMap[NM_OPENVPN_KEY_FRAGMENT_SIZE] + '\n';
        expFile.write(line.toLatin1());
    }
    line = QString(DEV_TAG) + (dataMap[NM_OPENVPN_KEY_TAP_DEV] == "yes" ? " tap\n" : " tun\n");
    expFile.write(line.toLatin1());
    line = QString(PROTO_TAG) + (dataMap[NM_OPENVPN_KEY_PROTO_TCP] == "yes" ? " tcp\n" : " udp\n");
    expFile.write(line.toLatin1());
    // Proxy stuff
    if (!dataMap[NM_OPENVPN_KEY_PROXY_TYPE].isEmpty()) {
        QString proxy_port = dataMap[NM_OPENVPN_KEY_PROXY_PORT];
        if (dataMap[NM_OPENVPN_KEY_PROXY_TYPE] == "http" && !dataMap[NM_OPENVPN_KEY_PROXY_SERVER].isEmpty()
                                                         && dataMap.contains(NM_OPENVPN_KEY_PROXY_PORT)) {
            if (proxy_port.toInt() == 0) {
                proxy_port = "8080";
            }
            line = QString(HTTP_PROXY_TAG) + ' ' + dataMap[NM_OPENVPN_KEY_PROXY_SERVER] + ' ' + proxy_port +
                    (dataMap[NM_OPENVPN_KEY_HTTP_PROXY_USERNAME].isEmpty() ? "\n" : (' ' + fileName + "-httpauthfile") + '\n');
            expFile.write(line.toLatin1());
            if (dataMap[NM_OPENVPN_KEY_PROXY_RETRY] == "yes") {
                line = QString(HTTP_PROXY_RETRY_TAG) + '\n';
                expFile.write(line.toLatin1());
            }
            // If there is a username, need to write an authfile
            if (!dataMap[NM_OPENVPN_KEY_HTTP_PROXY_USERNAME].isEmpty()) {
                QFile authFile(fileName + "-httpauthfile");
                if (authFile.open(QFile::WriteOnly | QFile::Text)) {
                    line = dataMap[NM_OPENVPN_KEY_HTTP_PROXY_USERNAME] + (dataMap[NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD].isEmpty()?
                                                                         "\n" : (dataMap[NM_OPENVPN_KEY_HTTP_PROXY_PASSWORD] + '\n'));
                    authFile.write(line.toLatin1());
                    authFile.close();
                }
            }
        } else if (dataMap[NM_OPENVPN_KEY_PROXY_TYPE] == "socks" && !dataMap[NM_OPENVPN_KEY_PROXY_SERVER].isEmpty() && dataMap.contains(NM_OPENVPN_KEY_PROXY_PORT)) {
            if (proxy_port.toInt() == 0) {
                proxy_port = "1080";
            }
            line = QString(SOCKS_PROXY_TAG) + dataMap[NM_OPENVPN_KEY_PROXY_SERVER] + ' ' + proxy_port + '\n';
            expFile.write(line.toLatin1());
            if (dataMap[NM_OPENVPN_KEY_PROXY_RETRY] == "yes") {
                line = QString(SOCKS_PROXY_RETRY_TAG) + '\n';
                expFile.write(line.toLatin1());
            }
        }
    }

    NetworkManager::Ipv4Setting::Ptr ipv4Setting = connection->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
    // Export X-NM-Routes
    if (!ipv4Setting->routes().isEmpty()) {
        QString routes;
        for (const NetworkManager::IpRoute &route : ipv4Setting->routes()) {
            routes += route.ip().toString() % QLatin1Char('/') % QString::number(route.prefixLength()) % QLatin1Char(' ');
        }
        if (!routes.isEmpty()) {
            routes = "X-NM-Routes " + routes.trimmed();
            expFile.write(routes.toLatin1() + '\n');
        }
    }
    // Add hard-coded stuff
    expFile.write("nobind\n"
                  "auth-nocache\n"
                  "script-security 2\n"
                  "persist-key\n"
                  "persist-tun\n"
                  "user nobody\n"
                  "group nobody\n");
    expFile.close();
    return true;
}

#include "openvpn.moc"
