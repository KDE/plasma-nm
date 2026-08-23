/*
    SPDX-FileCopyrightText: 2013 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "wiredsettings.h"

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/WiredDevice>

#include <KLocalizedString>

#include <QRandomGenerator>

WiredSetting::WiredSetting(QObject *parent)
    : QObject(parent)
{
}

QString WiredSetting::macAddress() const
{
    return m_macAddress;
}

void WiredSetting::setMacAddress(const QString &macAddress)
{
    if (m_macAddress == macAddress) {
        return;
    }
    m_macAddress = macAddress;
    Q_EMIT macAddressChanged();
    Q_EMIT validChanged();
}

QString WiredSetting::clonedMacAddress() const
{
    return m_clonedMacAddress;
}

void WiredSetting::setClonedMacAddress(const QString &clonedMacAddress)
{
    if (m_clonedMacAddress == clonedMacAddress) {
        return;
    }
    m_clonedMacAddress = clonedMacAddress;
    Q_EMIT clonedMacAddressChanged();
    Q_EMIT validChanged();
}

int WiredSetting::mtu() const
{
    return m_mtu;
}

void WiredSetting::setMtu(int mtu)
{
    if (m_mtu == mtu) {
        return;
    }
    m_mtu = mtu;
    Q_EMIT mtuChanged();
    Q_EMIT validChanged();
}

WiredSetting::LinkNegotiation WiredSetting::linkNegotiation() const
{
    return m_linkNegotiation;
}

void WiredSetting::setLinkNegotiation(LinkNegotiation negotiation)
{
    if (m_linkNegotiation == negotiation) {
        return;
    }
    m_linkNegotiation = negotiation;
    Q_EMIT linkNegotiationChanged();
    Q_EMIT validChanged();
}

int WiredSetting::speed() const
{
    return m_speed;
}

void WiredSetting::setSpeed(int speed)
{
    if (m_speed == speed) {
        return;
    }
    m_speed = speed;
    Q_EMIT speedChanged();
    Q_EMIT validChanged();
}

WiredSetting::DuplexType WiredSetting::duplex() const
{
    return m_duplex;
}

void WiredSetting::setDuplex(DuplexType duplex)
{
    if (m_duplex == duplex) {
        return;
    }
    m_duplex = duplex;
    Q_EMIT duplexChanged();
    Q_EMIT validChanged();
}

QStringList WiredSetting::macAddresses() const
{
    QStringList result;
    for (const NetworkManager::Device::Ptr &device : NetworkManager::networkInterfaces()) {
        if (device->type() != NetworkManager::Device::Ethernet) {
            continue;
        }
        NetworkManager::WiredDevice::Ptr wiredDevice = device.objectCast<NetworkManager::WiredDevice>();
        const QString mac = wiredDevice->permanentHardwareAddress();
        if (!mac.isEmpty()) {
            result << mac;
        }
    }
    return result;
}

QVariantList WiredSetting::speeds() const
{
    static const QList<QPair<int, QString>> speeds = {
        {10, i18n("10 Mb/s")},
        {100, i18n("100 Mb/s")},
        {1000, i18n("1 Gb/s")},
        {2500, i18n("2.5 Gb/s")},
        {5000, i18n("5 Gb/s")},
        {10000, i18n("10 Gb/s")},
        {40000, i18n("40 Gb/s")},
        {100000, i18n("100 Gb/s")},
    };

    QVariantList result;
    for (const auto &speed : speeds) {
        result.append(QVariantMap{
            {QStringLiteral("text"), speed.second},
            {QStringLiteral("value"), speed.first},
        });
    }

    return result;
}

bool WiredSetting::manualLinkEnabled() const
{
    return m_linkNegotiation == Manual;
}

bool WiredSetting::isValid() const
{
    const bool macValid = m_macAddress.isEmpty() || NetworkManager::macAddressIsValid(m_macAddress);
    const bool clonedMacValid =
        m_clonedMacAddress.isEmpty() || m_clonedMacAddress == QLatin1String(":::::") || NetworkManager::macAddressIsValid(m_clonedMacAddress);

    return macValid && clonedMacValid;
}

QString WiredSetting::generateRandomClonedMac() const
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

void WiredSetting::loadConfig(const NetworkManager::ConnectionSettings::Ptr &settings)
{
    NetworkManager::WiredSetting::Ptr wiredSetting = settings->setting(NetworkManager::Setting::Wired).staticCast<NetworkManager::WiredSetting>();
    if (!wiredSetting) {
        return;
    }

    m_macAddress = NetworkManager::macAddressAsString(wiredSetting->macAddress());
    m_clonedMacAddress = wiredSetting->clonedMacAddress().isEmpty() ? QString() : NetworkManager::macAddressAsString(wiredSetting->clonedMacAddress());
    m_mtu = static_cast<int>(wiredSetting->mtu());

    if (wiredSetting->autoNegotiate()) {
        m_linkNegotiation = Automatic;
    } else if (wiredSetting->speed() && wiredSetting->duplexType() != NetworkManager::WiredSetting::UnknownDuplexType) {
        m_linkNegotiation = Manual;
    } else {
        m_linkNegotiation = Ignore;
    }

    if (wiredSetting->speed()) {
        m_speed = static_cast<int>(wiredSetting->speed());
    }

    m_duplex = wiredSetting->duplexType() == NetworkManager::WiredSetting::Half ? Half : Full;

    Q_EMIT macAddressChanged();
    Q_EMIT clonedMacAddressChanged();
    Q_EMIT mtuChanged();
    Q_EMIT linkNegotiationChanged();
    Q_EMIT speedChanged();
    Q_EMIT duplexChanged();
    Q_EMIT macAddressesChanged();
    Q_EMIT validChanged();
}

QVariantMap WiredSetting::setting() const
{
    NetworkManager::WiredSetting wiredSetting;

    wiredSetting.setMacAddress(NetworkManager::macAddressFromString(m_macAddress));

    if (!m_clonedMacAddress.isEmpty() && m_clonedMacAddress != QLatin1String(":::::")) {
        wiredSetting.setClonedMacAddress(NetworkManager::macAddressFromString(m_clonedMacAddress));
    }

    if (m_mtu > 0) {
        wiredSetting.setMtu(m_mtu);
    }

    if (m_linkNegotiation == Manual) {
        wiredSetting.setSpeed(m_speed);
        wiredSetting.setDuplexType(m_duplex == Half ? NetworkManager::WiredSetting::Half : NetworkManager::WiredSetting::Full);
    } else {
        wiredSetting.setSpeed(0);
        wiredSetting.setDuplexType(NetworkManager::WiredSetting::UnknownDuplexType);
    }

    wiredSetting.setAutoNegotiate(m_linkNegotiation == Automatic);

    return wiredSetting.toMap();
}
