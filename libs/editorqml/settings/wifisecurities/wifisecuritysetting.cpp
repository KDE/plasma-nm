/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "wifisecuritysetting.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>

WifiSecuritySetting::WifiSecuritySetting(QObject *parent)
    : QObject(parent)
    , m_8021xSetting(new Security8021xSetting(this))
{
    connect(m_8021xSetting, &Security8021xSetting::validChanged, this, &WifiSecuritySetting::validChanged);
}

void WifiSecuritySetting::loadConfig(const NetworkManager::WirelessSecuritySetting::Ptr &setting)
{
    if (!setting) {
        return;
    }
    m_wifiSecurity = setting;

    const auto keyMgmt = setting->keyMgmt();
    const auto authAlg = setting->authAlg();

    if (keyMgmt == NetworkManager::WirelessSecuritySetting::Unknown) {
        m_securityType = None;

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Wep) {
        if (setting->wepKeyType() == NetworkManager::WirelessSecuritySetting::Hex
            || setting->wepKeyType() == NetworkManager::WirelessSecuritySetting::NotSpecified) {
            m_securityType = WepHex;
        } else
            m_securityType = WepPassphrase;

        m_wepKeyIndex = static_cast<int>(setting->wepTxKeyindex());
        m_wepAuth = (setting->authAlg() == NetworkManager::WirelessSecuritySetting::Open) ? 0 : 1;
        m_wepKeys[0] = setting->wepKey0();
        m_wepKeys[1] = setting->wepKey1();
        m_wepKeys[2] = setting->wepKey2();
        m_wepKeys[3] = setting->wepKey3();
        setWepKey(m_wepKeys[m_wepKeyIndex]);
        if (setting->wepKeyFlags().testFlag(NetworkManager::Setting::None)) {
            m_wepKeyOption = StoreForAllUsers;
        } else if (setting->wepKeyFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
            m_wepKeyOption = StoreForUser;
        } else {
            m_wepKeyOption = AlwaysAsk;
        }

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x && authAlg == NetworkManager::WirelessSecuritySetting::Leap) {
        m_securityType = Leap;
        m_leapUsername = setting->leapUsername();
        m_leapPassword = setting->leapPassword();

        if (setting->leapPasswordFlags().testFlag(NetworkManager::Setting::None)) {
            m_leapPasswordOption = StoreForAllUsers;
        } else if (setting->leapPasswordFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
            m_leapPasswordOption = StoreForUser;
        } else {
            m_leapPasswordOption = AlwaysAsk;
        }

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x) {
        m_securityType = DynamicWep;

    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaPsk) {
        m_securityType = WpaPsk;

        if (setting->pskFlags().testFlag(NetworkManager::Setting::None)) {
            m_pskOption = StoreForAllUsers;
        } else if (setting->pskFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
            m_pskOption = StoreForUser;
        } else {
            m_pskOption = AlwaysAsk;
        }
    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEap) {
        m_securityType = WpaEap;
    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::SAE) {
        m_securityType = SAE;

        if (setting->pskFlags().testFlag(NetworkManager::Setting::None)) {
            m_pskOption = StoreForAllUsers;
        } else if (setting->pskFlags().testFlag(NetworkManager::Setting::AgentOwned)) {
            m_pskOption = StoreForUser;
        } else {
            m_pskOption = AlwaysAsk;
        }
    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEapSuiteB192) {
        m_securityType = Wpa3SuiteB192;
    } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::OWE) {
        m_securityType = OWE;
    }

    if (keyMgmt != NetworkManager::WirelessSecuritySetting::Ieee8021x && keyMgmt != NetworkManager::WirelessSecuritySetting::WpaEap
        && keyMgmt != NetworkManager::WirelessSecuritySetting::WpaEapSuiteB192) {
        loadSecrets(setting, nullptr);
    }

    Q_EMIT securityTypeChanged();
    Q_EMIT validChanged();
}

void WifiSecuritySetting::loadSecrets(const NetworkManager::WirelessSecuritySetting::Ptr &setting,
                                      const NetworkManager::Security8021xSetting::Ptr &security8021xSetting)
{
    if (!setting) {
        return;
    }
    const auto keyMgmt = setting->keyMgmt();
    const auto authAlg = setting->authAlg();

    if ((keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x && authAlg != NetworkManager::WirelessSecuritySetting::Leap)
        || keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEap || keyMgmt == NetworkManager::WirelessSecuritySetting::WpaEapSuiteB192) {
        if (security8021xSetting) {
            m_8021xSetting->loadSecrets(security8021xSetting);
        }

    } else {
        if (setting) {
            if (keyMgmt == NetworkManager::WirelessSecuritySetting::Wep) {
                m_wifiSecurity->secretsFromMap(setting->secretsToMap());
                const int keyIndex = static_cast<int>(m_wifiSecurity->wepTxKeyindex());
                const QString updatedKey = (keyIndex == 0) ? m_wifiSecurity->wepKey0()
                    : (keyIndex == 1)                      ? m_wifiSecurity->wepKey1()
                    : (keyIndex == 2)                      ? m_wifiSecurity->wepKey2()
                                                           : m_wifiSecurity->wepKey3();
                m_wepKeys[keyIndex] = updatedKey;
                setWepKey(updatedKey);

            } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::Ieee8021x && authAlg == NetworkManager::WirelessSecuritySetting::Leap) {
                if (!setting->leapPassword().isEmpty()) {
                    m_leapPassword = setting->leapPassword();
                    Q_EMIT leapPasswordChanged();
                }

            } else if (keyMgmt == NetworkManager::WirelessSecuritySetting::WpaPsk || keyMgmt == NetworkManager::WirelessSecuritySetting::SAE) {
                if (!setting->psk().isEmpty()) {
                    m_psk = setting->psk();
                    Q_EMIT pskChanged();
                }
            }
        }
    }
}

QVariantMap WifiSecuritySetting::setting() const
{
    NetworkManager::WirelessSecuritySetting wifiSecurity;

    if (m_securityType == None) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Unknown);
    } else if (m_securityType == WepHex || m_securityType == WepPassphrase) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Wep);
        if (m_securityType == WepHex)
            wifiSecurity.setWepKeyType(NetworkManager::WirelessSecuritySetting::Hex);
        else
            wifiSecurity.setWepKeyType(NetworkManager::WirelessSecuritySetting::Passphrase);
        wifiSecurity.setWepTxKeyindex(m_wepKeyIndex);

        if (m_wepKeyIndex == 0)
            wifiSecurity.setWepKey0(m_wepKey);
        else if (m_wepKeyIndex == 1)
            wifiSecurity.setWepKey1(m_wepKey);
        else if (m_wepKeyIndex == 2)
            wifiSecurity.setWepKey2(m_wepKey);
        else
            wifiSecurity.setWepKey3(m_wepKey);
        if (m_wepKeyOption == StoreForAllUsers) {
            wifiSecurity.setWepKeyFlags(NetworkManager::Setting::None);

        } else if (m_wepKeyOption == StoreForUser) {
            wifiSecurity.setWepKeyFlags(NetworkManager::Setting::AgentOwned);

        } else {
            wifiSecurity.setWepKeyFlags(NetworkManager::Setting::NotSaved);
        }

        if (m_wepAuth == 0) {
            wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Open);

        } else {
            wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Shared);
        }
    } else if (m_securityType == Leap) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);
        wifiSecurity.setAuthAlg(NetworkManager::WirelessSecuritySetting::Leap);
        wifiSecurity.setLeapUsername(m_leapUsername);
        wifiSecurity.setLeapPassword(m_leapPassword);

        if (m_leapPasswordOption == PasswordOption::StoreForAllUsers) {
            wifiSecurity.setLeapPasswordFlags(NetworkManager::Setting::None);
        } else if (m_leapPasswordOption == PasswordOption::StoreForUser) {
            wifiSecurity.setLeapPasswordFlags(NetworkManager::Setting::AgentOwned);
        } else {
            wifiSecurity.setLeapPasswordFlags(NetworkManager::Setting::NotSaved);
        }

    } else if (m_securityType == DynamicWep) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::Ieee8021x);

    } else if (m_securityType == SAE) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::SAE);
        wifiSecurity.setPsk(m_psk);

        if (m_pskOption == StoreForAllUsers) {
            wifiSecurity.setPskFlags(NetworkManager::Setting::None);

        } else if (m_pskOption == StoreForUser) {
            wifiSecurity.setPskFlags(NetworkManager::Setting::AgentOwned);

        } else {
            wifiSecurity.setPskFlags(NetworkManager::Setting::NotSaved);
        }
    } else if (m_securityType == WpaPsk) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
        wifiSecurity.setPsk(m_psk);

        if (m_pskOption == StoreForAllUsers) {
            wifiSecurity.setPskFlags(NetworkManager::Setting::None);
        } else if (m_pskOption == StoreForUser) {
            wifiSecurity.setPskFlags(NetworkManager::Setting::AgentOwned);
        } else {
            wifiSecurity.setPskFlags(NetworkManager::Setting::NotSaved);
        }

    } else if (m_securityType == OWE) {
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::OWE);

    } else if (m_securityType == Wpa3SuiteB192) { // WPA3 Enterprise Suite B 192
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEapSuiteB192);
        wifiSecurity.setPmf(NetworkManager::WirelessSecuritySetting::RequiredPmf);
    } else if (m_securityType == WpaEap) { // WPA2 Enterprise
        wifiSecurity.setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
    }

    return wifiSecurity.toMap();
}

QVariantMap WifiSecuritySetting::setting8021x() const
{
    if (m_8021xSetting && (m_securityType == DynamicWep || m_securityType == WpaEap || m_securityType == Wpa3SuiteB192)) {
        return m_8021xSetting->setting();
    }
    return {};
}

void WifiSecuritySetting::onSsidChanged(const QString &ssid)
{
    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
        if (device->type() != NetworkManager::Device::Wifi)
            continue;
        auto wifiDevice = device.staticCast<NetworkManager::WirelessDevice>();
        if (!wifiDevice)
            continue;
        for (const NetworkManager::WirelessNetwork::Ptr &network : wifiDevice->networks()) {
            if (!network || network->ssid() != ssid)
                continue;
            auto ap = network->referenceAccessPoint();
            auto secType = NetworkManager::findBestWirelessSecurity(wifiDevice->wirelessCapabilities(),
                                                                    true,
                                                                    wifiDevice->mode() == NetworkManager::WirelessDevice::Adhoc,
                                                                    ap->capabilities(),
                                                                    ap->wpaFlags(),
                                                                    ap->rsnFlags());

            switch (secType) {
            case NetworkManager::WirelessSecurityType::StaticWep:
                m_securityType = WepHex;
                break;
            case NetworkManager::WirelessSecurityType::DynamicWep:
                m_securityType = DynamicWep;
                break;
            case NetworkManager::WirelessSecurityType::Leap:
                m_securityType = Leap;
                break;
            case NetworkManager::WirelessSecurityType::WpaPsk:
            case NetworkManager::WirelessSecurityType::Wpa2Psk:
                m_securityType = WpaPsk;
                break;
            case NetworkManager::WirelessSecurityType::WpaEap:
            case NetworkManager::WirelessSecurityType::Wpa2Eap:
                m_securityType = WpaEap;
                break;
            case NetworkManager::WirelessSecurityType::SAE:
                m_securityType = SAE;
                break;
            case NetworkManager::WirelessSecurityType::Wpa3SuiteB192:
                m_securityType = Wpa3SuiteB192;
                break;
            case NetworkManager::WirelessSecurityType::OWE:
                m_securityType = OWE;
                break;
            default:
                m_securityType = None;
                break;
            }
            Q_EMIT securityTypeChanged();
            return;
        }
    }
    m_securityType = None;
    Q_EMIT securityTypeChanged();
}

bool WifiSecuritySetting::isValid() const
{
    switch (m_securityType) {
    case WepHex:
        return NetworkManager::wepKeyIsValid(m_wepKey, NetworkManager::WirelessSecuritySetting::Hex) || m_wepKeyOption == AlwaysAsk;
    case WepPassphrase:
        return NetworkManager::wepKeyIsValid(m_wepKey, NetworkManager::WirelessSecuritySetting::Passphrase) || m_wepKeyOption == AlwaysAsk;
    case Leap:
        return !m_leapUsername.isEmpty() && (!m_leapPassword.isEmpty() || m_leapPasswordOption == AlwaysAsk);
    case WpaPsk:
        return NetworkManager::wpaPskIsValid(m_psk) || m_pskOption == AlwaysAsk;
    case SAE:
        return !m_psk.isEmpty() || m_pskOption == AlwaysAsk;
    case DynamicWep:
        return m_8021xSetting->isValid();
    case WpaEap:
        return m_8021xSetting->isValid();
    case Wpa3SuiteB192:
        return m_8021xSetting->isValid();
    default:
        return true;
    }
}

bool WifiSecuritySetting::showPsk() const
{
    return m_securityType == WpaPsk || m_securityType == SAE;
}
bool WifiSecuritySetting::showWep() const
{
    return m_securityType == WepHex || m_securityType == WepPassphrase;
}
bool WifiSecuritySetting::showLeap() const
{
    return m_securityType == Leap;
}
bool WifiSecuritySetting::showEnterprise() const
{
    return m_securityType == DynamicWep || m_securityType == WpaEap;
}
bool WifiSecuritySetting::showWpa3Enterprise192() const
{
    return m_securityType == Wpa3SuiteB192;
}
bool WifiSecuritySetting::enabled() const
{
    return m_securityType != None;
}
bool WifiSecuritySetting::enabled8021x() const
{
    return m_securityType == DynamicWep || m_securityType == WpaEap || m_securityType == Wpa3SuiteB192;
}

Security8021xSetting *WifiSecuritySetting::setting8021x_pointer() const
{
    return m_8021xSetting;
}
WifiSecuritySetting::SecurityType WifiSecuritySetting::securityType() const
{
    return m_securityType;
}
void WifiSecuritySetting::setSecurityType(SecurityType t)
{
    if (m_securityType == t)
        return;
    m_securityType = t;
    Q_EMIT securityTypeChanged();
    Q_EMIT validChanged();
}

QString WifiSecuritySetting::psk() const
{
    return m_psk;
}
void WifiSecuritySetting::setPsk(const QString &v)
{
    if (m_psk == v)
        return;
    m_psk = v;
    Q_EMIT pskChanged();
    Q_EMIT validChanged();
}

WifiSecuritySetting::PasswordOption WifiSecuritySetting::pskOption() const
{
    return m_pskOption;
}
void WifiSecuritySetting::setPskOption(PasswordOption v)
{
    if (m_pskOption == v)
        return;
    m_pskOption = v;
    Q_EMIT pskOptionChanged();
    Q_EMIT validChanged();
}

QString WifiSecuritySetting::wepKey() const
{
    return m_wepKey;
}
void WifiSecuritySetting::setWepKey(const QString &key)
{
    if (m_wepKey == key)
        return;
    m_wepKey = key;
    m_wepKeys[m_wepKeyIndex] = key;
    Q_EMIT wepKeyChanged();
    Q_EMIT validChanged();
}
int WifiSecuritySetting::wepKeyIndex() const
{
    return m_wepKeyIndex;
}
void WifiSecuritySetting::setWepKeyIndex(int v)
{
    if (m_wepKeyIndex == v || v < 0 || v > 3)
        return;
    m_wepKeyIndex = v;
    m_wepKey = m_wepKeys[v];
    Q_EMIT wepKeyIndexChanged();
    Q_EMIT wepKeyChanged();
    Q_EMIT validChanged();
}

int WifiSecuritySetting::wepAuth() const
{
    return m_wepAuth;
}
void WifiSecuritySetting::setWepAuth(int v)
{
    if (m_wepAuth == v)
        return;
    m_wepAuth = v;
    Q_EMIT wepAuthChanged();
    Q_EMIT validChanged();
}

WifiSecuritySetting::PasswordOption WifiSecuritySetting::wepKeyOption() const
{
    return m_wepKeyOption;
}
void WifiSecuritySetting::setWepKeyOption(PasswordOption v)
{
    if (m_wepKeyOption == v)
        return;
    m_wepKeyOption = v;
    Q_EMIT wepKeyOptionChanged();
    Q_EMIT validChanged();
}

QString WifiSecuritySetting::leapUsername() const
{
    return m_leapUsername;
}
void WifiSecuritySetting::setLeapUsername(const QString &v)
{
    if (m_leapUsername == v)
        return;
    m_leapUsername = v;
    Q_EMIT leapUsernameChanged();
    Q_EMIT validChanged();
}

QString WifiSecuritySetting::leapPassword() const
{
    return m_leapPassword;
}
void WifiSecuritySetting::setLeapPassword(const QString &v)
{
    if (m_leapPassword == v)
        return;
    m_leapPassword = v;
    Q_EMIT leapPasswordChanged();
    Q_EMIT validChanged();
}

WifiSecuritySetting::PasswordOption WifiSecuritySetting::leapPasswordOption() const
{
    return m_leapPasswordOption;
}
void WifiSecuritySetting::setLeapPasswordOption(PasswordOption v)
{
    if (m_leapPasswordOption == v)
        return;
    m_leapPasswordOption = v;
    Q_EMIT leapPasswordOptionChanged();
    Q_EMIT validChanged();
}

#include "moc_wifisecuritysetting.cpp"
