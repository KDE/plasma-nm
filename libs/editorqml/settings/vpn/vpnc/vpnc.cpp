/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "vpnc.h"

#include "nm-vpnc-service.h"

#include <QUrl>

namespace
{
const QLatin1String YesString("yes");

VpncSetting::PasswordOption optionFromFlags(const QString &rawFlags)
{
    const auto flags = static_cast<NetworkManager::Setting::SecretFlags>(rawFlags.toInt());

    if (flags.testFlag(NetworkManager::Setting::None)) {
        return VpncSetting::StoreForAllUsers;
    }
    if (flags.testFlag(NetworkManager::Setting::AgentOwned)) {
        return VpncSetting::StoreForUser;
    }
    if (flags.testFlag(NetworkManager::Setting::NotSaved)) {
        return VpncSetting::AlwaysAsk;
    }
    return VpncSetting::NotRequired;
}

QString flagsFromOption(VpncSetting::PasswordOption option)
{
    switch (option) {
    case VpncSetting::StoreForAllUsers:
        return QString::number(NetworkManager::Setting::None);
    case VpncSetting::StoreForUser:
        return QString::number(NetworkManager::Setting::AgentOwned);
    case VpncSetting::AlwaysAsk:
        return QString::number(NetworkManager::Setting::NotSaved);
    case VpncSetting::NotRequired:
        break;
    }
    return QString::number(NetworkManager::Setting::NotRequired);
}
}

VpncSetting::VpncSetting(QObject *parent)
    : QObject(parent)
{
}

VpncSetting::~VpncSetting() = default;

void VpncSetting::loadConfig(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    setUserPassword(QString());
    setGroupPassword(QString());

    const NMStringMap data = setting->data();

    // General
    setGateway(data.value(QLatin1String(NM_VPNC_KEY_GATEWAY)));

    setUser(data.value(QLatin1String(NM_VPNC_KEY_XAUTH_USER)));
    setUserPasswordOption(optionFromFlags(data.value(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD "-flags"))));

    setGroup(data.value(QLatin1String(NM_VPNC_KEY_ID)));
    setGroupPasswordOption(optionFromFlags(data.value(QLatin1String(NM_VPNC_KEY_SECRET "-flags"))));

    const bool hybrid = data.value(QLatin1String(NM_VPNC_KEY_AUTHMODE)) == QLatin1String("hybrid");
    setUseHybridAuth(hybrid);

    const QString caFile = data.value(QLatin1String(NM_VPNC_KEY_CA_FILE));
    setCaFile(hybrid && !caFile.isEmpty() ? QUrl::fromLocalFile(caFile).toString() : QString());

    // Advanced - Identification
    setDomain(data.value(QLatin1String(NM_VPNC_KEY_DOMAIN)));

    const QString vendor = data.value(QLatin1String(NM_VPNC_KEY_VENDOR));
    if (vendor == QLatin1String(NM_VPNC_VENDOR_NETSCREEN)) {
        setVendor(Netscreen);
    } else if (vendor == QLatin1String(NM_VPNC_VENDOR_FORTIGATE)) {
        setVendor(Fortigate);
    } else {
        setVendor(Cisco);
    }

    // Advanced - Transport and Security
    if (data.value(QLatin1String(NM_VPNC_KEY_SINGLE_DES)) == YesString) {
        setEncryption(Weak);
    } else if (data.value(QLatin1String(NM_VPNC_KEY_NO_ENCRYPTION)) == YesString) {
        setEncryption(NoEncryption);
    } else {
        setEncryption(Secure);
    }

    const QString nat = data.value(QLatin1String(NM_VPNC_KEY_NAT_TRAVERSAL_MODE));
    if (nat == QLatin1String(NM_VPNC_NATT_MODE_NATT_ALWAYS)) {
        setNatTraversal(NattAlways);
    } else if (nat == QLatin1String(NM_VPNC_NATT_MODE_CISCO)) {
        setNatTraversal(CiscoUdp);
    } else if (nat == QLatin1String(NM_VPNC_NATT_MODE_NONE)) {
        setNatTraversal(NattDisabled);
    } else {
        setNatTraversal(NattWhenAvailable);
    }

    const QString dhGroup = data.value(QLatin1String(NM_VPNC_KEY_DHGROUP));
    if (dhGroup == QLatin1String(NM_VPNC_DHGROUP_DH1)) {
        setDhGroup(DhGroup1);
    } else if (dhGroup == QLatin1String(NM_VPNC_DHGROUP_DH5)) {
        setDhGroup(DhGroup5);
    } else {
        setDhGroup(DhGroup2); // default
    }

    const QString pfs = data.value(QLatin1String(NM_VPNC_KEY_PERFECT_FORWARD));
    if (pfs == QLatin1String(NM_VPNC_PFS_NOPFS)) {
        setPerfectForwardSecrecy(PfsNone);
    } else if (pfs == QLatin1String(NM_VPNC_PFS_DH1)) {
        setPerfectForwardSecrecy(PfsDh1);
    } else if (pfs == QLatin1String(NM_VPNC_PFS_DH2)) {
        setPerfectForwardSecrecy(PfsDh2);
    } else if (pfs == QLatin1String(NM_VPNC_PFS_DH5)) {
        setPerfectForwardSecrecy(PfsDh5);
    } else {
        setPerfectForwardSecrecy(PfsServer);
    }

    bool ok = false;
    const uint localPort = data.value(QLatin1String(NM_VPNC_KEY_LOCAL_PORT)).toUInt(&ok);
    setLocalPort(ok && localPort <= 65535 ? static_cast<int>(localPort) : 0);

    const uint dpd = data.value(QLatin1String(NM_VPNC_KEY_DPD_IDLE_TIMEOUT)).toUInt(&ok);
    setDisableDeadPeerDetection(ok && dpd == 0);

    loadSecrets(setting);
}

void VpncSetting::loadSecrets(const NetworkManager::VpnSetting::Ptr &setting)
{
    if (!setting) {
        return;
    }

    const NMStringMap secrets = setting->secrets();

    const QString userPassword = secrets.value(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD));
    if (!userPassword.isEmpty()) {
        setUserPassword(userPassword);
    }

    const QString groupPassword = secrets.value(QLatin1String(NM_VPNC_KEY_SECRET));
    if (!groupPassword.isEmpty()) {
        setGroupPassword(groupPassword);
    }
}

QString VpncSetting::serviceType() const
{
    return QLatin1String(NM_DBUS_SERVICE_VPNC);
}

QVariantMap VpncSetting::setting() const
{
    NetworkManager::VpnSetting setting;
    setting.setServiceType(QLatin1String(NM_DBUS_SERVICE_VPNC));

    NMStringMap data;
    NMStringMap secrets;

    if (!m_gateway.isEmpty()) {
        data.insert(QLatin1String(NM_VPNC_KEY_GATEWAY), m_gateway);
    }

    if (!m_user.isEmpty()) {
        data.insert(QLatin1String(NM_VPNC_KEY_XAUTH_USER), m_user);
    }

    if (m_userPasswordOption != NotRequired && !m_userPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD), m_userPassword);
    }
    data.insert(QLatin1String(NM_VPNC_KEY_XAUTH_PASSWORD "-flags"), flagsFromOption(m_userPasswordOption));

    if (!m_group.isEmpty()) {
        data.insert(QLatin1String(NM_VPNC_KEY_ID), m_group);
    }

    if (m_groupPasswordOption != NotRequired && !m_groupPassword.isEmpty()) {
        secrets.insert(QLatin1String(NM_VPNC_KEY_SECRET), m_groupPassword);
    }
    data.insert(QLatin1String(NM_VPNC_KEY_SECRET "-flags"), flagsFromOption(m_groupPasswordOption));

    if (m_useHybridAuth && !m_caFile.isEmpty()) {
        data.insert(QLatin1String(NM_VPNC_KEY_AUTHMODE), QLatin1String("hybrid"));
        data.insert(QLatin1String(NM_VPNC_KEY_CA_FILE), QUrl(m_caFile).toLocalFile());
    }

    // Advanced - Identification
    if (!m_domain.isEmpty()) {
        data.insert(QLatin1String(NM_VPNC_KEY_DOMAIN), m_domain);
    }

    switch (m_vendor) {
    case Cisco:
        data.insert(QLatin1String(NM_VPNC_KEY_VENDOR), QLatin1String(NM_VPNC_VENDOR_CISCO));
        break;
    case Netscreen:
        data.insert(QLatin1String(NM_VPNC_KEY_VENDOR), QLatin1String(NM_VPNC_VENDOR_NETSCREEN));
        break;
    case Fortigate:
        data.insert(QLatin1String(NM_VPNC_KEY_VENDOR), QLatin1String(NM_VPNC_VENDOR_FORTIGATE));
        break;
    }

    // Advanced - Transport and Security
    switch (m_encryption) {
    case Secure:
        break;
    case Weak:
        data.insert(QLatin1String(NM_VPNC_KEY_SINGLE_DES), YesString);
        break;
    case NoEncryption:
        data.insert(QLatin1String(NM_VPNC_KEY_NO_ENCRYPTION), YesString);
        break;
    }

    switch (m_natTraversal) {
    case NattWhenAvailable:
        data.insert(QLatin1String(NM_VPNC_KEY_NAT_TRAVERSAL_MODE), QLatin1String(NM_VPNC_NATT_MODE_NATT));
        break;
    case NattAlways:
        data.insert(QLatin1String(NM_VPNC_KEY_NAT_TRAVERSAL_MODE), QLatin1String(NM_VPNC_NATT_MODE_NATT_ALWAYS));
        break;
    case CiscoUdp:
        data.insert(QLatin1String(NM_VPNC_KEY_NAT_TRAVERSAL_MODE), QLatin1String(NM_VPNC_NATT_MODE_CISCO));
        break;
    case NattDisabled:
        data.insert(QLatin1String(NM_VPNC_KEY_NAT_TRAVERSAL_MODE), QLatin1String(NM_VPNC_NATT_MODE_NONE));
        break;
    }

    switch (m_dhGroup) {
    case DhGroup1:
        data.insert(QLatin1String(NM_VPNC_KEY_DHGROUP), QLatin1String(NM_VPNC_DHGROUP_DH1));
        break;
    case DhGroup2:
        data.insert(QLatin1String(NM_VPNC_KEY_DHGROUP), QLatin1String(NM_VPNC_DHGROUP_DH2));
        break;
    case DhGroup5:
        data.insert(QLatin1String(NM_VPNC_KEY_DHGROUP), QLatin1String(NM_VPNC_DHGROUP_DH5));
        break;
    }

    switch (m_perfectForwardSecrecy) {
    case PfsServer:
        data.insert(QLatin1String(NM_VPNC_KEY_PERFECT_FORWARD), QLatin1String(NM_VPNC_PFS_SERVER));
        break;
    case PfsNone:
        data.insert(QLatin1String(NM_VPNC_KEY_PERFECT_FORWARD), QLatin1String(NM_VPNC_PFS_NOPFS));
        break;
    case PfsDh1:
        data.insert(QLatin1String(NM_VPNC_KEY_PERFECT_FORWARD), QLatin1String(NM_VPNC_PFS_DH1));
        break;
    case PfsDh2:
        data.insert(QLatin1String(NM_VPNC_KEY_PERFECT_FORWARD), QLatin1String(NM_VPNC_PFS_DH2));
        break;
    case PfsDh5:
        data.insert(QLatin1String(NM_VPNC_KEY_PERFECT_FORWARD), QLatin1String(NM_VPNC_PFS_DH5));
        break;
    }

    data.insert(QLatin1String(NM_VPNC_KEY_LOCAL_PORT), QString::number(m_localPort));

    if (m_disableDeadPeerDetection) {
        data.insert(QLatin1String(NM_VPNC_KEY_DPD_IDLE_TIMEOUT), QStringLiteral("0"));
    }

    setting.setData(data);
    setting.setSecrets(secrets);

    return setting.toMap();
}

bool VpncSetting::isValid() const
{
    return !m_gateway.isEmpty();
}

QString VpncSetting::gateway() const
{
    return m_gateway;
}

void VpncSetting::setGateway(const QString &gateway)
{
    if (m_gateway == gateway) {
        return;
    }
    m_gateway = gateway;
    Q_EMIT gatewayChanged();
    Q_EMIT validChanged();
}

QString VpncSetting::user() const
{
    return m_user;
}

void VpncSetting::setUser(const QString &user)
{
    if (m_user == user) {
        return;
    }
    m_user = user;
    Q_EMIT userChanged();
    Q_EMIT validChanged();
}

QString VpncSetting::userPassword() const
{
    return m_userPassword;
}

void VpncSetting::setUserPassword(const QString &password)
{
    if (m_userPassword == password) {
        return;
    }
    m_userPassword = password;
    Q_EMIT userPasswordChanged();
    Q_EMIT validChanged();
}

VpncSetting::PasswordOption VpncSetting::userPasswordOption() const
{
    return m_userPasswordOption;
}

void VpncSetting::setUserPasswordOption(PasswordOption option)
{
    if (m_userPasswordOption == option) {
        return;
    }
    m_userPasswordOption = option;
    Q_EMIT userPasswordOptionChanged();
    Q_EMIT validChanged();
}

QString VpncSetting::group() const
{
    return m_group;
}

void VpncSetting::setGroup(const QString &group)
{
    if (m_group == group) {
        return;
    }
    m_group = group;
    Q_EMIT groupChanged();
    Q_EMIT validChanged();
}

QString VpncSetting::groupPassword() const
{
    return m_groupPassword;
}

void VpncSetting::setGroupPassword(const QString &password)
{
    if (m_groupPassword == password) {
        return;
    }
    m_groupPassword = password;
    Q_EMIT groupPasswordChanged();
    Q_EMIT validChanged();
}

VpncSetting::PasswordOption VpncSetting::groupPasswordOption() const
{
    return m_groupPasswordOption;
}

void VpncSetting::setGroupPasswordOption(PasswordOption option)
{
    if (m_groupPasswordOption == option) {
        return;
    }
    m_groupPasswordOption = option;
    Q_EMIT groupPasswordOptionChanged();
    Q_EMIT validChanged();
}

bool VpncSetting::useHybridAuth() const
{
    return m_useHybridAuth;
}

void VpncSetting::setUseHybridAuth(bool use)
{
    if (m_useHybridAuth == use) {
        return;
    }
    m_useHybridAuth = use;
    Q_EMIT useHybridAuthChanged();
    Q_EMIT validChanged();
}

QString VpncSetting::caFile() const
{
    return m_caFile;
}

void VpncSetting::setCaFile(const QString &caFile)
{
    if (m_caFile == caFile) {
        return;
    }
    m_caFile = caFile;
    Q_EMIT caFileChanged();
    Q_EMIT validChanged();
}

QString VpncSetting::domain() const
{
    return m_domain;
}

void VpncSetting::setDomain(const QString &domain)
{
    if (m_domain == domain) {
        return;
    }
    m_domain = domain;
    Q_EMIT domainChanged();
    Q_EMIT validChanged();
}

VpncSetting::Vendor VpncSetting::vendor() const
{
    return m_vendor;
}

void VpncSetting::setVendor(Vendor vendor)
{
    if (m_vendor == vendor) {
        return;
    }
    m_vendor = vendor;
    Q_EMIT vendorChanged();
    Q_EMIT validChanged();
}

VpncSetting::Encryption VpncSetting::encryption() const
{
    return m_encryption;
}

void VpncSetting::setEncryption(Encryption encryption)
{
    if (m_encryption == encryption) {
        return;
    }
    m_encryption = encryption;
    Q_EMIT encryptionChanged();
    Q_EMIT validChanged();
}

VpncSetting::NatTraversal VpncSetting::natTraversal() const
{
    return m_natTraversal;
}

void VpncSetting::setNatTraversal(NatTraversal natTraversal)
{
    if (m_natTraversal == natTraversal) {
        return;
    }
    m_natTraversal = natTraversal;
    Q_EMIT natTraversalChanged();
    Q_EMIT validChanged();
}

VpncSetting::DhGroup VpncSetting::dhGroup() const
{
    return m_dhGroup;
}

void VpncSetting::setDhGroup(DhGroup dhGroup)
{
    if (m_dhGroup == dhGroup) {
        return;
    }
    m_dhGroup = dhGroup;
    Q_EMIT dhGroupChanged();
    Q_EMIT validChanged();
}

VpncSetting::Pfs VpncSetting::perfectForwardSecrecy() const
{
    return m_perfectForwardSecrecy;
}

void VpncSetting::setPerfectForwardSecrecy(Pfs pfs)
{
    if (m_perfectForwardSecrecy == pfs) {
        return;
    }
    m_perfectForwardSecrecy = pfs;
    Q_EMIT perfectForwardSecrecyChanged();
    Q_EMIT validChanged();
}

int VpncSetting::localPort() const
{
    return m_localPort;
}

void VpncSetting::setLocalPort(int port)
{
    if (m_localPort == port) {
        return;
    }
    m_localPort = port;
    Q_EMIT localPortChanged();
    Q_EMIT validChanged();
}

bool VpncSetting::disableDeadPeerDetection() const
{
    return m_disableDeadPeerDetection;
}

void VpncSetting::setDisableDeadPeerDetection(bool disable)
{
    if (m_disableDeadPeerDetection == disable) {
        return;
    }
    m_disableDeadPeerDetection = disable;
    Q_EMIT disableDeadPeerDetectionChanged();
    Q_EMIT validChanged();
}

#include "moc_vpnc.cpp"
