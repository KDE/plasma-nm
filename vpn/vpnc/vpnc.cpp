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

#include <KPluginFactory>
#include <KSharedConfig>
#include <KStandardDirs>
#include <KMessageBox>
#include <KLocale>
#include "nm-vpnc-service.h"

#include <QtNetworkManager/connection.h>
#include <QtNetworkManager/settings/vpn.h>
#include <QtNetworkManager/settings/ipv4.h>

#include "vpncwidget.h"

#define NM_VPNC_LOCAL_PORT_DEFAULT 500

K_PLUGIN_FACTORY(VpncUiPluginFactory, registerPlugin<VpncUiPlugin>();)
K_EXPORT_PLUGIN(VpncUiPluginFactory("plasmanm_vpncui", "plasmanm_vpncui"))

VpncUiPlugin::VpncUiPlugin(QObject * parent, const QVariantList &) : VpnUiPlugin(parent)
{

}

VpncUiPlugin::~VpncUiPlugin()
{

}

SettingWidget * VpncUiPlugin::widget(NetworkManager::Settings::Setting *setting, QWidget * parent)
{
    return new VpncWidget(setting, parent);
}

QDialog * VpncUiPlugin::askUser(NetworkManager::Settings::Setting *setting, QWidget * parent)
{
    // TODO
    return 0;
}

#if 0
QString VpncUiPlugin::suggestedFileName(Knm::Connection *connection) const
{
    return connection->name() + ".pcf";
}

QString VpncUiPlugin::supportedFileExtensions() const
{
    return "*.pcf";
}

QVariantList VpncUiPlugin::importConnectionSettings(const QString &fileName)
{
    kDebug() << "Importing Cisco VPN connection from " << fileName;

    VpncUiPluginPrivate * decrPlugin = 0;
    QVariantList conSetting;

    if (!fileName.endsWith(QLatin1String(".pcf"), Qt::CaseInsensitive)) {
        return conSetting;
    }

    mError = VpnUiPlugin::Error;

    // NOTE: Cisco VPN pcf files follow ini style matching KConfig files
    // http://www.cisco.com/en/US/docs/security/vpn_client/cisco_vpn_client/vpn_client46/administration/guide/vcAch2.html#wp1155033
    KSharedConfig::Ptr config = KSharedConfig::openConfig(fileName);
    if (!config) {
        mErrorMessage = i18n("File %1 could not be opened.", fileName);
        return conSetting;
    }

    KConfigGroup cg(config, "main");   // Keys&Values are stored under [main]
    if (cg.exists()) {
        // Setup cisco-decrypt binary to decrypt the passwords
        QStringList decrArgs;
        QString ciscoDecryptBinary = KStandardDirs::findExe("cisco-decrypt", QString::fromLocal8Bit(qgetenv("PATH")) + ":/usr/lib/vpnc");
        if (ciscoDecryptBinary.isEmpty()) {
            mErrorMessage = i18n("Needed executable cisco-decrypt could not be found.");
            return QVariantList();
        }

        decrPlugin = new VpncUiPluginPrivate();
        decrPlugin->ciscoDecrypt = new KProcess(decrPlugin);
        decrPlugin->ciscoDecrypt->setOutputChannelMode(KProcess::OnlyStdoutChannel);
        decrPlugin->ciscoDecrypt->setReadChannel(QProcess::StandardOutput);
        connect(decrPlugin->ciscoDecrypt, SIGNAL(error(QProcess::ProcessError)), decrPlugin, SLOT(ciscoDecryptError(QProcess::ProcessError)));
        connect(decrPlugin->ciscoDecrypt, SIGNAL(finished(int,QProcess::ExitStatus)), decrPlugin, SLOT(ciscoDecryptFinished(int,QProcess::ExitStatus)));
        connect(decrPlugin->ciscoDecrypt, SIGNAL(readyReadStandardOutput()), decrPlugin, SLOT(gotciscoDecryptOutput()));

        QStringMap data;
        QStringMap secretData;
        QStringMap ipv4Data;

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
            decrArgs.clear();
            decrArgs << decrPlugin->readStringKeyValue(cg,"enc_UserPassword");
            decrPlugin->ciscoDecrypt->setProgram(ciscoDecryptBinary, decrArgs);
            decrPlugin->ciscoDecrypt->start();
            if (decrPlugin->ciscoDecrypt->waitForStarted() && decrPlugin->ciscoDecrypt->waitForFinished()) {
                secretData.insert(NM_VPNC_KEY_XAUTH_PASSWORD, decrPlugin->decryptedPasswd);
            }
        }
        // Save user password
        switch (cg.readEntry("SaveUserPassword").toInt())
        {
            case 0:
                data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(Knm::Setting::NotSaved));
                break;
            case 1:
                data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(Knm::Setting::AgentOwned));
                break;
            case 2:
                data.insert(NM_VPNC_KEY_XAUTH_PASSWORD"-flags", QString::number(Knm::Setting::NotRequired));
                break;
        }

        // group password
        if (!decrPlugin->readStringKeyValue(cg,"GroupPwd").isEmpty()) {
            secretData.insert(NM_VPNC_KEY_SECRET, decrPlugin->readStringKeyValue(cg,"GroupPwd"));
            data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(Knm::Setting::AgentOwned));
        }
        else if (!decrPlugin->readStringKeyValue(cg,"enc_GroupPwd").isEmpty() && !ciscoDecryptBinary.isEmpty()) {
            //Decrypt the password and insert into map
            decrArgs.clear();
            decrArgs << decrPlugin->readStringKeyValue(cg,"enc_GroupPwd");
            decrPlugin->ciscoDecrypt->setProgram(ciscoDecryptBinary, decrArgs);
            decrPlugin->ciscoDecrypt->start();
            if (decrPlugin->ciscoDecrypt->waitForStarted() && decrPlugin->ciscoDecrypt->waitForFinished()) {
                secretData.insert(NM_VPNC_KEY_SECRET, decrPlugin->decryptedPasswd);
                data.insert(NM_VPNC_KEY_SECRET"-flags", QString::number(Knm::Setting::AgentOwned));
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
            data.insert(NM_VPNC_KEY_LOCAL_PORT, QString(NM_VPNC_LOCAL_PORT_DEFAULT));
        }
        // DH Group
        data.insert(NM_VPNC_KEY_DHGROUP, decrPlugin->readStringKeyValue(cg,"DHGroup"));
        // Tunneling Mode - not supported by vpnc
        if (cg.readEntry("TunnelingMode").toInt() == 1) {
            KMessageBox::error(0, i18n("The VPN settings file '%1' specifies that VPN traffic should be tunneled through TCP which is currently not supported in the vpnc software.\n\nThe connection can still be created, with TCP tunneling disabled, however it may not work as expected.", fileName), i18n("Not supported"), KMessageBox::Notify);
        }
        // EnableLocalLAN and X-NM-Routes are to be added to IPv4Setting
        if (!cg.readEntry("EnableLocalLAN").isEmpty()) {
            ipv4Data.insert(NM_SETTING_IP4_CONFIG_NEVER_DEFAULT, cg.readEntry("EnableLocalLAN"));
        }
        if (!decrPlugin->readStringKeyValue(cg,"X-NM-Routes").isEmpty()) {
            ipv4Data.insert(NM_SETTING_IP4_CONFIG_ROUTES, decrPlugin->readStringKeyValue(cg,"X-NM-Routes"));
        }

        // Set the '...-type' and '...-flags' value also
        Knm::VpnSetting setting;
        setting.setData(data);
        // get the filled data() back
        data.clear();
        data = setting.data();

        conSetting << Knm::VpnSetting::variantMapFromStringList(Knm::VpnSetting::stringMapToStringList(data));
        conSetting << Knm::VpnSetting::variantMapFromStringList(Knm::VpnSetting::stringMapToStringList(secretData));
        conSetting << decrPlugin->readStringKeyValue(cg,"Description");
        if (!ipv4Data.isEmpty()) {
            conSetting << Knm::VpnSetting::variantMapFromStringList(Knm::VpnSetting::stringMapToStringList(ipv4Data));
        }

        delete decrPlugin;
    } else {
        mErrorMessage = i18n("%1: file format error.", fileName);
        return conSetting;
    }

    mError = VpncUiPlugin::NoError;
    return conSetting;
}

bool VpncUiPlugin::exportConnectionSettings(Knm::Connection * connection, const QString &fileName)
{
    QStringMap data;
    QStringMap secretData;

    Knm::VpnSetting * vpnSetting = static_cast<Knm::VpnSetting*>(connection->setting(Knm::Setting::Vpn));
    data = vpnSetting->data();
    secretData = vpnSetting->vpnSecrets();

    KSharedConfig::Ptr config = KSharedConfig::openConfig(fileName);
    if (!config) {
        mErrorMessage = i18n("%1: file could not be created", fileName);
        return false;
    }
    KConfigGroup cg(config,"main");

    cg.writeEntry("Description", connection->name());
    cg.writeEntry("Host", data.value(NM_VPNC_KEY_GATEWAY));
    if (data.value(NM_VPNC_KEY_AUTHMODE) == QLatin1String("hybrid"))
        cg.writeEntry("AuthType", "5");
    else
        cg.writeEntry("AuthType", "1");
    cg.writeEntry("GroupName", data.value(NM_VPNC_KEY_ID));
    //cg.writeEntry("GroupPwd", secretData.value(NM_VPNC_KEY_SECRET));
    //cg.writeEntry("UserPassword", secretData.value(NM_VPNC_KEY_XAUTH_PASSWORD));
    cg.writeEntry("GroupPwd", "");
    cg.writeEntry("UserPassword", "");
    cg.writeEntry("enc_GroupPwd", "");
    cg.writeEntry("enc_UserPassword", "");
    if ((Knm::Setting::secretsTypes)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt() & Knm::Setting::NotSaved) {
        cg.writeEntry("SaveUserPassword", "0");
    }
    if ((Knm::Setting::secretsTypes)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt() & Knm::Setting::AgentOwned) {
        cg.writeEntry("SaveUserPassword", "1");
    }
    if ((Knm::Setting::secretsTypes)data.value(NM_VPNC_KEY_XAUTH_PASSWORD"-flags").toInt() & Knm::Setting::NotRequired) {
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
    Knm::Ipv4Setting *ipv4Setting = static_cast<Knm::Ipv4Setting*>(connection->setting(Knm::Setting::Ipv4));
    if (!ipv4Setting->routes().isEmpty()) {
        QString routes;
        foreach(const NetworkManager::IPv4Route &oneRoute, ipv4Setting->routes()) {
            routes += QHostAddress(oneRoute.route()).toString() + '/' + QString::number(oneRoute.prefix()) + ' ';
        }
        cg.writeEntry("X-NM-Routes", routes.trimmed());
    }

    mError = VpncUiPlugin::NoError;
    return true;
}
#endif

