/*
    Copyright 2018 Bruce Anderson <banderson19com@san.rr.com>

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

#include "wireguard.h"

#include <QLatin1Char>
#include <QFileInfo>
#include <KPluginFactory>
#include <KConfig>
#include <KConfigGroup>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/VpnSetting>
#include <NetworkManagerQt/Ipv4Setting>

#include "wireguardwidget.h"
#include "wireguardauth.h"
#include "simpleipv4addressvalidator.h"
#include "simpleipv6addressvalidator.h"
#include "simpleiplistvalidator.h"
#include "wireguardkeyvalidator.h"

#include "nm-wireguard-service.h"

K_PLUGIN_FACTORY_WITH_JSON(WireGuardUiPluginFactory,
                           "plasmanetworkmanagement_wireguardui.json",
                           registerPlugin<WireGuardUiPlugin>();)

#define NMV_WG_TAG_INTERFACE             "Interface"
#define NMV_WG_TAG_PRIVATE_KEY           "PrivateKey"
#define NMV_WG_TAG_LISTEN_PORT           "ListenPort"
#define NMV_WG_TAG_ADDRESS               "Address"
#define NMV_WG_TAG_DNS                   "DNS"
#define NMV_WG_TAG_MTU                   "MTU"
#define NMV_WG_TAG_TABLE                 "Table"
#define NMV_WG_TAG_PRE_UP                "PreUp"
#define NMV_WG_TAG_POST_UP               "PostUp"
#define NMV_WG_TAG_PRE_DOWN              "PreDown"
#define NMV_WG_TAG_POST_DOWN             "PostDown"
#define NMV_WG_TAG_FWMARK                "FwMark"

#define NMV_WG_TAG_PEER                  "Peer"
#define NMV_WG_TAG_PUBLIC_KEY            "PublicKey"
#define NMV_WG_TAG_ALLOWED_IPS           "AllowedIPs"
#define NMV_WG_TAG_ENDPOINT              "Endpoint"
#define NMV_WG_TAG_PRESHARED_KEY         "PresharedKey"
#define NMV_WG_TAG_PERSISTENT_KEEPALIVE  "PersistentKeepalive"


WireGuardUiPlugin::WireGuardUiPlugin(QObject *parent, const QVariantList &) : VpnUiPlugin(parent)
{
}

WireGuardUiPlugin::~WireGuardUiPlugin()
{
}

SettingWidget *WireGuardUiPlugin::widget(const NetworkManager::VpnSetting::Ptr &setting,
                                         QWidget *parent)
{
    return new WireGuardSettingWidget(setting, parent);
}

SettingWidget *WireGuardUiPlugin::askUser(const NetworkManager::VpnSetting::Ptr &setting,
                                          QWidget *parent)
{
    return new WireGuardAuthWidget(setting, parent);
}

QString WireGuardUiPlugin::suggestedFileName(const NetworkManager::ConnectionSettings::Ptr &connection) const
{
    return connection->id() + "_wireguard.conf";
}

QString WireGuardUiPlugin::supportedFileExtensions() const
{
    return "*.conf";
}

NMVariantMapMap WireGuardUiPlugin::importConnectionSettings(const QString &fileName)
{
    NMVariantMapMap result;

    const KConfig importFile(fileName, KConfig::NoGlobals);
    const KConfigGroup interfaceGroup = importFile.group(NMV_WG_TAG_INTERFACE);
    const KConfigGroup peerGroup = importFile.group(NMV_WG_TAG_PEER);

    // The config file must have both [Interface] and [Peer] sections
    if (!interfaceGroup.exists()
        || !peerGroup.exists()) {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("Config file needs both [Peer] and [Interface]");
        return result;
    }

    const QString connectionName = QFileInfo(fileName).completeBaseName();
    NMStringMap dataMap;
    QVariantMap ipv4Data;

    QString value;
    QStringList valueList;
    int intValue;

    // Do the required fields first and fail (i.e. return an empty result) if not present

    // Addresses
    valueList = interfaceGroup.readEntry(NMV_WG_TAG_ADDRESS, QStringList());
    if (valueList.isEmpty()) {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("No address in config file");
        return result;
    }

    for (const QString &address : valueList) {
        const QPair<QHostAddress, int> addressIn = QHostAddress::parseSubnet(address.trimmed());
        if (addressIn.first.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) {
            dataMap.insert(QLatin1String(NM_WG_KEY_ADDR_IP4), address);
        } else if (addressIn.first.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv6Protocol) {
            dataMap.insert(QLatin1String(NM_WG_KEY_ADDR_IP6), address);
        } else { // Error condition
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("No valid address in config file");
            return result;
        }
    }

    WireGuardKeyValidator keyValidator(nullptr);
    int pos = 0;

    // Private Key
    value = interfaceGroup.readEntry(NMV_WG_TAG_PRIVATE_KEY);
    if (!value.isEmpty()) {
        if (keyValidator.validate(value, pos) == QValidator::State::Acceptable) {
            dataMap.insert(QLatin1String(NM_WG_KEY_PRIVATE_KEY), value);
        } else {
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("No valid Private Key in config file");
            return result;
        }
    } else {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("No Private Key in config file");
        return result;
    }

    // Public Key
    value = peerGroup.readEntry(NMV_WG_TAG_PUBLIC_KEY);
    if (!value.isEmpty()) {
        if (keyValidator.validate(value, pos) == QValidator::State::Acceptable) {
            dataMap.insert(QLatin1String(NM_WG_KEY_PUBLIC_KEY), value);
        } else {
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("No valid Public Key in config file");
            return result;
        }
    } else {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("No Public Key in config file");
        return result;
    }

    // Allowed IPs
    value = peerGroup.readEntry(NMV_WG_TAG_ALLOWED_IPS);
    if (!value.isEmpty()) {
        SimpleIpListValidator validator(this,
                                        SimpleIpListValidator::WithCidr,
                                        SimpleIpListValidator::Both);

        if (validator.validate(value, pos) != QValidator::State::Invalid) {
            dataMap.insert(QLatin1String(NM_WG_KEY_ALLOWED_IPS), value);
        } else {
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("Invalid Allowed IP in config file");
            return result;
        }
    } else {
        mError = VpnUiPlugin::Error;
        mErrorMessage = i18n("No Allowed IPs in config file");
        return result;
    }

    // Now the optional ones

    // Listen Port
    intValue = interfaceGroup.readEntry(NMV_WG_TAG_LISTEN_PORT, 0);
    if (intValue > 0) {
        if (intValue < 65536) {
            dataMap.insert(QLatin1String(NM_WG_KEY_LISTEN_PORT), QString::number(intValue));
        } else {
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("Invalid Listen Port in config file");
            return result;
        }
    }

    // DNS
    value = interfaceGroup.readEntry(NMV_WG_TAG_DNS);
    if (!value.isEmpty()) {
        const QHostAddress testAddress(value);
        if (testAddress.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol
            || testAddress.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv6Protocol) {
            dataMap.insert(QLatin1String(NM_WG_KEY_DNS), value);
        } else {
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("Invalid DNS in config file");
            return result;
        }
    }

    // MTU
    intValue = interfaceGroup.readEntry(NMV_WG_TAG_MTU, 0);
    if (intValue > 0)
        dataMap.insert(QLatin1String(NM_WG_KEY_LISTEN_PORT), QString::number(intValue));

    // Table
    value = interfaceGroup.readEntry(NMV_WG_TAG_TABLE);
    if (!value.isEmpty())
        dataMap.insert(QLatin1String(NM_WG_KEY_TABLE), value);

    // PreUp
    value = interfaceGroup.readEntry(NMV_WG_TAG_PRE_UP);
    if (!value.isEmpty())
        dataMap.insert(QLatin1String(NM_WG_KEY_PRE_UP), value);

    // PostUp
    value = interfaceGroup.readEntry(NMV_WG_TAG_POST_UP);
    if (!value.isEmpty())
        dataMap.insert(QLatin1String(NM_WG_KEY_POST_UP), value);

    // PreDown
    value = interfaceGroup.readEntry(NMV_WG_TAG_PRE_DOWN);
    if (!value.isEmpty())
        dataMap.insert(QLatin1String(NM_WG_KEY_PRE_DOWN), value);

    // PostDown
    value = interfaceGroup.readEntry(NMV_WG_TAG_POST_DOWN);
    if (!value.isEmpty())
        dataMap.insert(QLatin1String(NM_WG_KEY_POST_DOWN), value);

    // Endpoint
    value = peerGroup.readEntry(NMV_WG_TAG_ENDPOINT);
    if (!value.isEmpty())
        dataMap.insert(QLatin1String(NM_WG_KEY_ENDPOINT), value);

    // Preshared Key
    value = peerGroup.readEntry(NMV_WG_TAG_PRESHARED_KEY);
    if (!value.isEmpty()) {
        if (keyValidator.validate(value, pos) == QValidator::State::Acceptable) {
            dataMap.insert(QLatin1String(NM_WG_KEY_PRESHARED_KEY), value);
        } else {
            mError = VpnUiPlugin::Error;
            mErrorMessage = i18n("Invalid Preshared Key in config file");
            return result;
        }
    }

    // Persistent Keepalive
    intValue = peerGroup.readEntry(NMV_WG_TAG_PERSISTENT_KEEPALIVE, -1);
    if (intValue > -1)
        dataMap.insert(QLatin1String(NM_WG_KEY_PERSISTENT_KEEPALIVE), QString::number(intValue));

    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_WIREGUARD));
    setting.setData(dataMap);

    QVariantMap conn;
    conn.insert("id", connectionName);
    conn.insert("type", "vpn");
    result.insert("connection", conn);
    result.insert("vpn", setting.toMap());

    return result;
}

bool WireGuardUiPlugin::exportConnectionSettings(const NetworkManager::ConnectionSettings::Ptr &connection,
                                                 const QString &fileName)
{
    NetworkManager::VpnSetting::Ptr vpnSetting = connection->setting(
                                                    NetworkManager::Setting::Vpn).dynamicCast<NetworkManager::VpnSetting>();
    NMStringMap dataMap = vpnSetting->data();

    // Make sure all the required fields are present
    if (!(dataMap.contains(QLatin1String(NM_WG_KEY_ADDR_IP4))
        || dataMap.contains(QLatin1String(NM_WG_KEY_ADDR_IP6)))
        || !dataMap.contains(QLatin1String(NM_WG_KEY_PRIVATE_KEY))
        || !dataMap.contains(QLatin1String(NM_WG_KEY_PUBLIC_KEY))
        || !dataMap.contains(QLatin1String(NM_WG_KEY_ALLOWED_IPS)))
        return false;

    // Open the output file
    KConfig exportFile(fileName, KConfig::NoGlobals);
    KConfigGroup interfaceGroup = exportFile.group(NMV_WG_TAG_INTERFACE);
    KConfigGroup peerGroup = exportFile.group(NMV_WG_TAG_PEER);

    QStringList outputList;

    if (dataMap.contains(QLatin1String(NM_WG_KEY_ADDR_IP4)))
        outputList.append(dataMap[NM_WG_KEY_ADDR_IP4]);
    if (dataMap.contains(QLatin1String(NM_WG_KEY_ADDR_IP6)))
        outputList.append(dataMap[NM_WG_KEY_ADDR_IP6]);
    interfaceGroup.writeEntry(NMV_WG_TAG_ADDRESS, outputList);

    // Do Private Key
    interfaceGroup.writeEntry(NMV_WG_TAG_PRIVATE_KEY, dataMap[NM_WG_KEY_PRIVATE_KEY]);

    // Do DNS (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_DNS)))
        interfaceGroup.writeEntry(NMV_WG_TAG_DNS, dataMap[NM_WG_KEY_DNS]);

    // Do MTU (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_MTU)))
        interfaceGroup.writeEntry(NMV_WG_TAG_MTU, dataMap[NM_WG_KEY_MTU]);

    // Do Table number (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_TABLE)))
        interfaceGroup.writeEntry(NMV_WG_TAG_TABLE, dataMap[NM_WG_KEY_TABLE]);

    // Do Listen Port (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_LISTEN_PORT)))
        interfaceGroup.writeEntry(NMV_WG_TAG_LISTEN_PORT, dataMap[NM_WG_KEY_LISTEN_PORT]);

    // Do FwMark (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_FWMARK)))
        interfaceGroup.writeEntry(NMV_WG_TAG_FWMARK, dataMap[NM_WG_KEY_FWMARK]);

    // Do PreUp (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_PRE_UP)))
        interfaceGroup.writeEntry(NMV_WG_TAG_PRE_UP, dataMap[NM_WG_KEY_PRE_UP]);

    // Do PostUp (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_POST_UP)))
        interfaceGroup.writeEntry(NMV_WG_TAG_POST_UP, dataMap[NM_WG_KEY_POST_UP]);

    // Do PreDown (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_PRE_DOWN)))
        interfaceGroup.writeEntry(NMV_WG_TAG_PRE_DOWN, dataMap[NM_WG_KEY_PRE_DOWN]);

    // Do PostDown (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_POST_DOWN)))
        interfaceGroup.writeEntry(NMV_WG_TAG_POST_DOWN, dataMap[NM_WG_KEY_POST_DOWN]);

    // Do Pupblic key (required)
    peerGroup.writeEntry(NMV_WG_TAG_PUBLIC_KEY, dataMap[NM_WG_KEY_PUBLIC_KEY]);

    // Do Allowed IP list (Required)
    peerGroup.writeEntry(NMV_WG_TAG_ALLOWED_IPS, dataMap[NM_WG_KEY_ALLOWED_IPS]);

    // Do Endpoint (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_ENDPOINT)))
        peerGroup.writeEntry(NMV_WG_TAG_ENDPOINT, dataMap[NM_WG_KEY_ENDPOINT]);

    // Do Preshared Key (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_PRESHARED_KEY)))
        peerGroup.writeEntry(NMV_WG_TAG_PRESHARED_KEY, dataMap[NM_WG_KEY_PRESHARED_KEY]);

    // Do Persistent Keepalive (Not required)
    if (dataMap.contains(QLatin1String(NM_WG_KEY_PERSISTENT_KEEPALIVE)))
        peerGroup.writeEntry(NMV_WG_TAG_PERSISTENT_KEEPALIVE, dataMap[NM_WG_KEY_PERSISTENT_KEEPALIVE]);

    exportFile.sync();
    return true;
}

#include "wireguard.moc"
