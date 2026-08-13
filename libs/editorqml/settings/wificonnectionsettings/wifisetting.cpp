/*
    SPDX-FileCopyrightText: 2013 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "wifisetting.h"
#include "plasma_nm_editorqml.h"

#include <NetworkManagerQt/AccessPoint>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessNetwork>

#include <KLocalizedString>

#include <QRandomGenerator>

#include <algorithm>
#include <networkmanagerqt/utils.h>
#include <networkmanagerqt/wirelesssetting.h>

WifiSetting::WifiSetting(QObject *parent)
    : QObject(parent)
{
}

QString WifiSetting::ssid() const
{
    return m_ssid;
}

void WifiSetting::setSsid(const QString &ssid)
{
    if (m_ssid == ssid) {
        return;
    }
    m_ssid = ssid;
    Q_EMIT ssidChanged();
    Q_EMIT availableBssidsChanged();
    Q_EMIT validChanged();
}

int WifiSetting::mode() const
{
    return m_mode;
}

void WifiSetting::setMode(int mode)
{
    const auto newMode = static_cast<NetworkManager::WirelessSetting::NetworkMode>(mode);
    if (m_mode == newMode) {
        return;
    }
    m_mode = newMode;
    Q_EMIT modeChanged();
    Q_EMIT validChanged();
}

QString WifiSetting::bssid() const
{
    return m_bssid;
}

void WifiSetting::setBssid(const QString &bssid)
{
    if (m_bssid == bssid) {
        return;
    }
    m_bssid = bssid;
    Q_EMIT bssidChanged();
    Q_EMIT validChanged();
}

int WifiSetting::band() const
{
    return m_band;
}

void WifiSetting::setBand(int band)
{
    const auto newBand = static_cast<NetworkManager::WirelessSetting::FrequencyBand>(band);

    if (m_band == newBand) {
        return;
    }

    m_band = newBand;

    if (m_channel != 0) {
        m_channel = 0;
        Q_EMIT channelChanged();
    }

    Q_EMIT bandChanged();
    Q_EMIT channelsChanged();
    Q_EMIT validChanged();
}

int WifiSetting::channel() const
{
    return m_channel;
}

void WifiSetting::setChannel(int channel)
{
    if (m_channel == channel) {
        return;
    }
    m_channel = channel;
    Q_EMIT channelChanged();
    Q_EMIT validChanged();
}

QString WifiSetting::macAddress() const
{
    return m_macAddress;
}

void WifiSetting::setMacAddress(const QString &macAddress)
{
    if (m_macAddress == macAddress) {
        return;
    }
    m_macAddress = macAddress;
    Q_EMIT macAddressChanged();
    Q_EMIT validChanged();
}

QString WifiSetting::clonedMacAddress() const
{
    return m_clonedMacAddress;
}

void WifiSetting::setClonedMacAddress(const QString &clonedMacAddress)
{
    if (m_clonedMacAddress == clonedMacAddress) {
        return;
    }
    m_clonedMacAddress = clonedMacAddress;
    Q_EMIT clonedMacAddressChanged();
    Q_EMIT validChanged();
}

int WifiSetting::mtu() const
{
    return m_mtu;
}

void WifiSetting::setMtu(int mtu)
{
    if (m_mtu == mtu) {
        return;
    }
    m_mtu = mtu;
    Q_EMIT mtuChanged();
    Q_EMIT validChanged();
}

bool WifiSetting::hidden() const
{
    return m_hidden;
}

void WifiSetting::setHidden(bool hidden)
{
    if (m_hidden == hidden) {
        return;
    }
    m_hidden = hidden;
    Q_EMIT hiddenChanged();
    Q_EMIT validChanged();
}

QStringList WifiSetting::availableSsids() const
{
    QList<NetworkManager::WirelessNetwork::Ptr> networks;

    for (const auto &device : NetworkManager::networkInterfaces()) {
        // Ignore Non-wifi devices
        if (device->type() != NetworkManager::Device::Wifi)
            continue;

        auto wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();

        for (const auto &network : wifiDevice->networks()) {
            bool found = false;

            // Look for the same SSID
            for (int i = 0; i < networks.size(); i++) {
                if (networks[i]->ssid() == network->ssid()) {
                    if (network->signalStrength() > networks[i]->signalStrength())
                        networks[i] = network;

                    found = true;
                    break;
                }
            }

            if (!found)
                networks.append(network);
        }
    }

    std::sort(networks.begin(), networks.end(), [](const auto &a, const auto &b) {
        return a->signalStrength() > b->signalStrength();
    });

    QStringList ssids;
    for (const auto &network : networks)
        ssids.append(network->ssid());

    return ssids;
}

QVariantList WifiSetting::availableBssids() const
{
    if (m_ssid.isEmpty())
        return {};

    QList<NetworkManager::AccessPoint::Ptr> aps;

    for (const auto &device : NetworkManager::networkInterfaces()) {
        if (device->type() != NetworkManager::Device::Wifi)
            continue;

        auto wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
        auto network = wifiDevice->findNetwork(m_ssid);

        if (!network)
            continue;

        for (const auto &ap : network->accessPoints()) {
            bool found = false;

            for (int i = 0; i < aps.size(); ++i) {
                if (aps[i]->hardwareAddress() == ap->hardwareAddress()) {
                    if (ap->signalStrength() > aps[i]->signalStrength())
                        aps[i] = ap;

                    found = true;
                    break;
                }
            }

            if (!found)
                aps.append(ap);
        }
    }

    std::sort(aps.begin(), aps.end(), [](const auto &a, const auto &b) {
        return a->signalStrength() > b->signalStrength();
    });

    QVariantList result;

    for (const auto &ap : std::as_const(aps)) {
        result.append(QVariantMap{
            {QStringLiteral("bssid"), ap->hardwareAddress()},
            {QStringLiteral("signal"), ap->signalStrength()},
            {QStringLiteral("frequency"), ap->frequency()},
            {QStringLiteral("channel"), NetworkManager::findChannel(ap->frequency())},
        });
    }

    return result;
}

QStringList WifiSetting::macAddresses() const
{
    QStringList result;
    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
        if (device->type() != NetworkManager::Device::Wifi) {
            continue;
        }
        NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
        const QString mac = wifiDevice->permanentHardwareAddress();
        if (!mac.isEmpty()) {
            result << mac;
        }
    }
    return result;
}

QVariantList WifiSetting::channels() const
{
    QVariantList result;

    result.append(QVariantMap{
        {QStringLiteral("text"), i18nc("@item automatic wireless channel selection", "Automatic")},
        {QStringLiteral("value"), 0},
    });

    QList<QPair<int, int>> freqs;

    switch (m_band) {
    case NetworkManager::WirelessSetting::Automatic:
        // No band-specific channel list; only "Automatic" applies.
        return result;
    case NetworkManager::WirelessSetting::A:
        freqs = NetworkManager::getAFreqs();
        break;
    case NetworkManager::WirelessSetting::Bg:
        freqs = NetworkManager::getBFreqs();
        break;
    default:
        qCWarning(PLASMA_NM_EDITORQML_LOG) << Q_FUNC_INFO << "Unhandled band number" << m_band;
        return result;
    }

    for (const auto &channel : std::as_const(freqs)) {
        result.append(QVariantMap{
            {QStringLiteral("text"), i18n("%1 (%2 MHz)", channel.first, channel.second)},
            {QStringLiteral("value"), channel.first},
        });
    }

    return result;
}

bool WifiSetting::isValid() const
{
    const bool ssidValid = !m_ssid.isEmpty();
    const bool macValid = m_macAddress.isEmpty() || NetworkManager::macAddressIsValid(m_macAddress);
    const bool bssidValid = m_bssid.isEmpty() || NetworkManager::macAddressIsValid(m_bssid);

    return ssidValid && macValid && bssidValid;
}

QString WifiSetting::generateRandomClonedMac() const
{
    QByteArray mac;
    auto generator = QRandomGenerator::global();
    mac.resize(6);
    for (int i = 0; i < 6; i++) {
        const int random = generator->bounded(255);
        mac[i] = random;
    }

    // Disable the multicast bit and enable the locally administered bit.
    mac[0] = mac[0] & ~0x1;
    mac[0] = mac[0] | 0x2;

    return NetworkManager::macAddressAsString(mac);
}

void WifiSetting::loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings)
{
    NetworkManager::WirelessSetting::Ptr wifiSetting = settings->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();
    if (!wifiSetting) {
        return;
    }

    m_ssid = QString::fromUtf8(wifiSetting->ssid());
    m_mode = wifiSetting->mode();
    m_bssid = NetworkManager::macAddressAsString(wifiSetting->bssid());
    m_band = wifiSetting->band();
    m_channel = (wifiSetting->band() != NetworkManager::WirelessSetting::Automatic) ? static_cast<int>(wifiSetting->channel()) : 0;
    m_macAddress = NetworkManager::macAddressAsString(wifiSetting->macAddress());
    m_clonedMacAddress = wifiSetting->clonedMacAddress().isEmpty() ? QString() : NetworkManager::macAddressAsString(wifiSetting->clonedMacAddress());
    m_mtu = static_cast<int>(wifiSetting->mtu());
    m_hidden = wifiSetting->hidden();

    Q_EMIT ssidChanged();
    Q_EMIT modeChanged();
    Q_EMIT bssidChanged();
    Q_EMIT bandChanged();
    Q_EMIT channelChanged();
    Q_EMIT macAddressChanged();
    Q_EMIT clonedMacAddressChanged();
    Q_EMIT mtuChanged();
    Q_EMIT hiddenChanged();
    Q_EMIT availableSsidsChanged();
    Q_EMIT availableBssidsChanged();
    Q_EMIT channelsChanged();
    Q_EMIT validChanged();
}

QVariantMap WifiSetting::setting() const
{
    NetworkManager::WirelessSetting wifiSetting;

    wifiSetting.setSsid(m_ssid.toUtf8());
    wifiSetting.setMode(m_mode);
    wifiSetting.setBssid(NetworkManager::macAddressFromString(m_bssid));

    if (m_band != NetworkManager::WirelessSetting::Automatic) {
        wifiSetting.setBand(m_band);

        if (m_mode != NetworkManager::WirelessSetting::Infrastructure) {
            wifiSetting.setChannel(m_channel);
        }
    }

    wifiSetting.setMacAddress(NetworkManager::macAddressFromString(m_macAddress));

    if (!m_clonedMacAddress.isEmpty() && m_clonedMacAddress != QLatin1String(":::::")) {
        wifiSetting.setClonedMacAddress(NetworkManager::macAddressFromString(m_clonedMacAddress));
    }

    if (m_mtu > 0) {
        wifiSetting.setMtu(m_mtu);
    }

    wifiSetting.setHidden(m_hidden);

    return wifiSetting.toMap();
}
