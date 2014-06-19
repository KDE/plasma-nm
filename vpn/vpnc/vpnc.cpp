/*
    Copyright 2008 Will Stephenson <wstephenson@kde.org>
    Copyright 2011-2012 Rajeesh K Nambiar <rajeeshknambiar@gmail.com>
    Copyright 2011-2012 Lamarque V. Souza <lamarque@kde.org>
    Copyright 2013 Lukas Tinkl <ltinkl@redhat.com>

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

#include "vpnc.h"

#include <QStandardPaths>
#include <QFile>

#include <KPluginFactory>
#include <KSharedConfig>
#include <KMessageBox>
#include <KLocalizedString>
#include <QDebug>
#include "nm-vpnc-service.h"

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/Ipv4Setting>

#include "vpncwidget.h"
#include "vpncauth.h"

VpncUiPluginPrivate::VpncUiPluginPrivate()
{
    decryptedPasswd.clear();
    ciscoDecrypt = 0;
}

VpncUiPluginPrivate::~VpncUiPluginPrivate()
{
}

QString VpncUiPluginPrivate::readStringKeyValue(const KConfigGroup & configGroup, const QString & key)
{
    const QString retValue = configGroup.readEntry(key);
    if (retValue.isEmpty()) {
        // String key can also start with "!" in CISCO pcf file.
        return configGroup.readEntry('!' + key);
    } else {
        return retValue;
    }
}

void VpncUiPluginPrivate::gotCiscoDecryptOutput()
{
    QByteArray output = ciscoDecrypt->readAll();
    if (!output.isEmpty()) {
        QList<QByteArray> lines = output.split('\n');
        if (!lines.isEmpty()) {
            decryptedPasswd = QString::fromUtf8(lines.first());
        }
    }
}

void VpncUiPluginPrivate::ciscoDecryptFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode || exitStatus != QProcess::NormalExit)
        decryptedPasswd.clear();
}

void VpncUiPluginPrivate::ciscoDecryptError(QProcess::ProcessError pError)
{
    if (!pError) {
        qDebug() << "Error in executing cisco-decrypt";
        KMessageBox::error(0, i18n("Error decrypting the obfuscated password"), i18n("Error"), KMessageBox::Notify);
    }
    decryptedPasswd.clear();
}


#define NM_VPNC_LOCAL_PORT_DEFAULT 500

K_PLUGIN_FACTORY_WITH_JSON(VpncUiPluginFactory, "plasmanetworkmanagement_vpncui.json", registerPlugin<VpncUiPlugin>();)

VpncUiPlugin::VpncUiPlugin(QObject * parent, const QVariantList &) : VpnUiPlugin(parent)
{

}

VpncUiPlugin::~VpncUiPlugin()
{

}

SettingWidget * VpncUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new VpncWidget(setting, parent);
}

SettingWidget *VpncUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting, QWidget * parent)
{
    return new VpncAuthDialog(setting, parent);
}

QString VpncUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    return connection->id() + ".pcf";
}

QString VpncUiPlugin::supportedFileExtensions() const
{
    return "*.pcf";
}

NMVariantMapMap VpncUiPlugin::importConnectionSettings(const QString &fileName)
{
    qDebug() << "Importing Cisco VPN connection from " << fileName;

    VpncUiPluginPrivate * decrPlugin = 0;
    NMVariantMapMap result;

    if (!fileName.endsWith(QLatin1String(".pcf"), Qt::CaseInsensitive)) {
        return result;
    }

    mError = VpnUiPlugin::Error;

    // NOTE: Cisco VPN pcf files follow ini style matching KConfig files
    // http://www.cisco.com/en/US/docs/security/vpn_client/cisco_vpn_client/vpn_client46/administration/guide/vcAch2.html#wp1155033
    KSharedConfig::Ptr config = KSharedConfig::openConfig(fileName);
    if (!config) {
        mErrorMessage = i18n("File %1 could not be opened.", fileName);
        return result;
    }

    KConfigGroup cg(config, "main");   // Keys&Values are stored under [main]
    if (cg.exists()) {
        // Setup cisco-decrypt binary to decrypt the passwords
        const QString ciscoDecryptBinary = QStandardPaths::findExecutable("cisco-decrypt", QStringList() << QFile::decodeName(qgetenv("PATH")) << "/usr/lib/vpnc");
        if (ciscoDecryptBinary.isEmpty()) {
            mErrorMessage = i18n("Needed executable cisco-decrypt could not be found.");
            return result;
        }

        decrPlugin = new VpncUiPluginPrivate();
        decrPlugin->ciscoDecrypt = new KProcess(decrPlugin);
        decrPlugin->ciscoDecrypt->setOutputChannelMode(KProcess::OnlyStdoutChannel);
        decrPlugin->ciscoDecrypt->setReadChannel(QProcess::StandardOutput);
        connect(decrPlugin->ciscoDecrypt, SIGNAL(error(QProcess::ProcessError)), decrPlugin, SLOT(ciscoDecryptError(QProcess::ProcessError)));
        connect(decrPlugin->ciscoDecrypt, SIGNAL(finished(int,QProcess::ExitStatus)), decrPlugin, SLOT(ciscoDecryptFinished(int,QProcess::ExitStatus)));
        connect(decrPlugin->ciscoDecrypt, SIGNAL(readyReadStandardOutput()), decrPlugin, SLOT(gotCiscoDecryptOutput()));

        NMStringMap data;
        NMStringMap secretData;
        QVariantMap ipv4Data;

        // gateway
        data.insert(NM_VPNC_KEY_GATEWAY, decrPlugin->readStringKeyValue(cg,"Host"));
        // group name
        data.insert(NM_VPNC_KEY_ID, decrPlugin->readStringKeyValue(cg,"GroupName"));
        // user password
        if (!decrPlugin->readStringKeyValue(cg,"UserPassword").isEmpty()) {
            secretData.insert(NM_VPNC_KEY_XAUTH_PASSWORD, decrPlugin->readStringKeyValue(cg,"UserPassword"));
        }
        else if (!decrPlugin->readStringKeyValue(cg,"enc_UserPassword").isEmpty() && !ciscoDecryptBinary.isEmpty()) {
            // Decrypt the password and insert into map
            decrPlugin->ciscoDecrypt->setProgram(ciscoDecryptBinary);
            decrPlugin->ciscoDecrypt->start();
            decrPlugin->ciscoDecrypt->waitForStarted();
            decrPlugin->ciscoDecrypt->write(decrPlugin->readStringKeyValue(cg,"enc_UserPassword").toUtf8());
            if (decrPlugin->ciscoDecrypt->waitForFinished()) {
                secretData.insert(NM_VPNC_KEY_XAUTH_PASSWORD, decrPlugin->decryptedPasswd);
            }
        }
        // Save user password
        switch (cg.readEntry("SaveUserPassword").toInt())
        {
            case 0:
                data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotSaved));
                break;
            case 1:
                data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::AgentOwned));
                break;
            case 2:
                data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(NetworkManager::Setting::NotRequired));
                break;
        }

        // group password
        if (!decrPlugin->readStringKeyValue(cg,"GroupPwd").isEmpty()) {
            secretData.insert(NM_VPNC_KEY_SECRET, decrPlugin->readStringKeyValue(cg,"GroupPwd"));
            data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(NetworkManager::Setting::AgentOwned));
        } else if (!decrPlugin->readStringKeyValue(cg,"enc_GroupPwd").isEmpty() && !ciscoDecryptBinary.isEmpty()) {
            //Decrypt the password and insert into map
            decrPlugin->ciscoDecrypt->setProgram(ciscoDecryptBinary);
            decrPlugin->ciscoDecrypt->start();
            decrPlugin->ciscoDecrypt->waitForStarted();
            decrPlugin->ciscoDecrypt->write(decrPlugin->readStringKeyValue(cg,"enc_GroupPwd").toUtf8());
            if (decrPlugin->ciscoDecrypt->waitForFinished()) {
                secretData.insert(NM_VPNC_KEY_SECRET, decrPlugin->decryptedPasswd);
                data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(NetworkManager::Setting::AgentOwned));
            }
        }

        // Auth Type
        if (!cg.readEntry("AuthType").isEmpty() && cg.readEntry("AuthType").toInt() == 5) {
            data.insert(NM_VPNC_KEY_AUTHMODE, QLatin1String("hybrid"));
        }

        // Optional settings
        // username
        if (!decrPlugin->readStringKeyValue(cg,"Username").isEmpty()) {
            data.insert(NM_VPNC_KEY_XAUTH_USER, decrPlugin->readStringKeyValue(cg,"Username"));
        }
        // domain
        if (!decrPlugin->readStringKeyValue(cg,"NTDomain").isEmpty()) {
            data.insert(NM_VPNC_KEY_DOMAIN, decrPlugin->readStringKeyValue(cg,"NTDomain"));
        }
        // encryption
        if (!cg.readEntry("SingleDES").isEmpty() && cg.readEntry("SingleDES").toInt() != 0) {
            data.insert(NM_VPNC_KEY_SINGLE_DES, QLatin1String("yes"));
        }
        /* Disable all NAT Traversal if explicit EnableNat=0 exists, otherwise
         * default to NAT-T which is newer and standardized.  If EnableNat=1, then
         * use Cisco-UDP like always; but if the key "X-NM-Use-NAT-T" is set, then
         * use NAT-T.  If the key "X-NM-Force-NAT-T" is set then force NAT-T always
         * on.  See vpnc documentation for more information on what the different
         * NAT modes are.
         */
        // enable NAT
        if (cg.readEntry("EnableNat").toInt() == 1) {
            data.insert(NM_VPNC_KEY_NAT_TRAVERSAL_MODE, QLatin1String(NM_VPNC_NATT_MODE_CISCO));
            // NAT traversal
            if (!cg.readEntry("X-NM-Use-NAT-T").isEmpty()) {
                if (cg.readEntry("X-NM-Use-NAT-T").toInt() == 1) {
                    data.insert(NM_VPNC_KEY_NAT_TRAVERSAL_MODE, QLatin1String(NM_VPNC_NATT_MODE_NATT));
                }
                if (cg.readEntry("X-NM-Force-NAT-T").toInt() == 1) {
                    data.insert(NM_VPNC_KEY_NAT_TRAVERSAL_MODE, QLatin1String(NM_VPNC_NATT_MODE_NATT_ALWAYS));
                }
            }
        }
        else {
            data.insert(NM_VPNC_KEY_NAT_TRAVERSAL_MODE, QLatin1String(NM_VPNC_NATT_MODE_NONE));
        }
        // dead peer detection
        data.insert(NM_VPNC_KEY_DPD_IDLE_TIMEOUT, cg.readEntry("PeerTimeout"));
        // UseLegacyIKEPort=0 uses dynamic source IKE port instead of 500.
        if (cg.readEntry("UseLegacyIKEPort").isEmpty() || cg.readEntry("UseLegacyIKEPort").toInt() != 0) {
            data.insert(NM_VPNC_KEY_LOCAL_PORT, QString::number(NM_VPNC_LOCAL_PORT_DEFAULT));
        }
        // DH Group
        data.insert(NM_VPNC_KEY_DHGROUP, decrPlugin->readStringKeyValue(cg,"DHGroup"));
        // Tunneling Mode - not supported by vpnc
        if (cg.readEntry("TunnelingMode").toInt() == 1) {
            KMessageBox::error(0, i18n("The VPN settings file '%1' specifies that VPN traffic should be tunneled through TCP which is currently not supported in the vpnc software.\n\nThe connection can still be created, with TCP tunneling disabled, however it may not work as expected.", fileName), i18n("Not supported"), KMessageBox::Notify);
        }
        // EnableLocalLAN and X-NM-Routes are to be added to IPv4Setting
        if (!cg.readEntry("EnableLocalLAN").isEmpty()) {
            ipv4Data.insert("never-default", cg.readEntry("EnableLocalLAN"));
        }
        if (!decrPlugin->readStringKeyValue(cg,"X-NM-Routes").isEmpty()) {
            ipv4Data.insert("X-NM-Routes", decrPlugin->readStringKeyValue(cg,"X-NM-Routes"));
        }

        // Set the '...-type' and '...-flags' value also
        NetworkManager::VpnSetting setting;
        setting.setServiceType("org.freedesktop.NetworkManager.vpnc");
        setting.setData(data);
        setting.setSecrets(secretData);

        QVariantMap conn;
        conn.insert("id", decrPlugin->readStringKeyValue(cg,"Description"));
        conn.insert("type", "vpn");
        result.insert("connection", conn);

        result.insert("vpn", setting.toMap());

        if (!ipv4Data.isEmpty()) {
            result.insert("ipv4", ipv4Data);
        }

        delete decrPlugin;
    } else {
        mErrorMessage = i18n("%1: file format error.", fileName);
        return result;
    }

    mError = VpncUiPlugin::NoError;
    return result;
}

bool VpncUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection, const QString &fileName)
{
    NMStringMap data;
    NMStringMap secretData;

    NetworkManager::VpnSetting::Ptr vpnSetting = connection->setting(NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
    data = vpnSetting->data();
    secretData = vpnSetting->secrets();

    KSharedConfig::Ptr config = KSharedConfig::openConfig(fileName);
    if (!config) {
        mErrorMessage = i18n("%1: file could not be created", fileName);
        return false;
    }
    KConfigGroup cg(config,"main");

    cg.writeEntry("Description", connection->id());
    cg.writeEntry("Host", data.value(NM_VPNC_KEY_GATEWAY));
    if (data.value(NM_VPNC_KEY_AUTHMODE) == QLatin1String("hybrid"))
        cg.writeEntry("AuthType", "5");
    else
        cg.writeEntry("AuthType", "1");
    cg.writeEntry("GroupName", data.value(NM_VPNC_KEY_ID));
    cg.writeEntry("GroupPwd", secretData.value(NM_VPNC_KEY_SECRET));
    cg.writeEntry("UserPassword", secretData.value(NM_VPNC_KEY_XAUTH_PASSWORD));
    cg.writeEntry("enc_GroupPwd", "");
    cg.writeEntry("enc_UserPassword", "");
    if ((NetworkManager::Setting::SecretFlags)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt() & NetworkManager::Setting::NotSaved) {
        cg.writeEntry("SaveUserPassword", "0");
    }
    if ((NetworkManager::Setting::SecretFlags)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt() & NetworkManager::Setting::AgentOwned) {
        cg.writeEntry("SaveUserPassword", "1");
    }
    if ((NetworkManager::Setting::SecretFlags)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt() & NetworkManager::Setting::NotRequired) {
        cg.writeEntry("SaveUserPassword", "2");
    }
    cg.writeEntry("Username", data.value(NM_VPNC_KEY_XAUTH_USER));
    cg.writeEntry("EnableISPConnect", "0");
    cg.writeEntry("ISPConnectType", "0");
    cg.writeEntry("ISPConnect", "");
    cg.writeEntry("ISPCommand", "");
    cg.writeEntry("EnableBackup", "0");
    cg.writeEntry("BackupServer", "");
    cg.writeEntry("CertStore", "0");
    cg.writeEntry("CertName", "");
    cg.writeEntry("CertPath", "");
    cg.writeEntry("CertSubjectName", "");
    cg.writeEntry("CertSerialHash", "");
    cg.writeEntry("DHGroup", data.value(NM_VPNC_KEY_DHGROUP));
    cg.writeEntry("ForceKeepAlives", "0");
    cg.writeEntry("NTDomain", data.value(NM_VPNC_KEY_DOMAIN));
    cg.writeEntry("EnableMSLogon", "0");
    cg.writeEntry("MSLogonType", "0");
    cg.writeEntry("TunnelingMode", "0");
    cg.writeEntry("TcpTunnelingPort", "10000");
    cg.writeEntry("PeerTimeout", data.value(NM_VPNC_KEY_DPD_IDLE_TIMEOUT));
    cg.writeEntry("EnableLocalLAN", "1");
    cg.writeEntry("SendCertChain", "0");
    cg.writeEntry("VerifyCertDN", "");
    cg.writeEntry("EnableSplitDNS", "1");
    cg.writeEntry("SPPhonebook", "");
    if (data.value(NM_VPNC_KEY_SINGLE_DES) == "yes") {
        cg.writeEntry("SingleDES", "1");
    }
    if (data.value(NM_VPNC_KEY_NAT_TRAVERSAL_MODE) == NM_VPNC_NATT_MODE_CISCO) {
        cg.writeEntry("EnableNat", "1");
    }
    if (data.value(NM_VPNC_KEY_NAT_TRAVERSAL_MODE) == NM_VPNC_NATT_MODE_NATT) {
        cg.writeEntry("EnableNat", "1");
        cg.writeEntry("X-NM-Use-NAT-T", "1");
    }
    if (data.value(NM_VPNC_KEY_NAT_TRAVERSAL_MODE) == NM_VPNC_NATT_MODE_NATT_ALWAYS) {
        cg.writeEntry("EnableNat", "1");
        cg.writeEntry("X-NM-Force-NAT-T", "1");
    }
    // Export X-NM-Routes
    NetworkManager::Ipv4Setting::Ptr ipv4Setting = connection->setting(NetworkManager::Setting::Ipv4).dynamicCast<NetworkManager::Ipv4Setting>();
    if (!ipv4Setting->routes().isEmpty()) {
        QString routes;
        foreach(const NetworkManager::IpRoute &route, ipv4Setting->routes()) {
            routes += route.ip().toString() + QLatin1Char('/') + QString::number(route.prefixLength()) + QLatin1Char(' ');
        }
        cg.writeEntry("X-NM-Routes", routes.trimmed());
    }

    cg.sync();

    mError = VpncUiPlugin::NoError;
    return true;
}

#include "vpnc.moc"
