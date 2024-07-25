/*
 * SPDX-FileCopyrightText: 2024 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "mecardhandler.h"

#include "plasma_nm_libs.h"

#include <Prison/MeCard>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessSetting>

#include <QCoroDBus>

#include "uiutils.h"

using namespace Qt::Literals::StringLiterals;

MeCardHandler::MeCardHandler(QObject *parent)
    : QObject(parent)
{
}

MeCardHandler::~MeCardHandler() = default;

QString MeCardHandler::text() const
{
    return m_text;
}

void MeCardHandler::setText(const QString &text)
{
    if (m_text == text) {
        return;
    }

    m_text = text;
    Q_EMIT textChanged(text);

    std::optional<Prison::MeCard> meCard = Prison::MeCard::parse(text);

    QVariantMap meCardMap;

    if (meCard && meCard->headerView() == "WIFI"_L1) {
        meCardMap = meCard->toVariantMap();
    }

    setMeCard(meCardMap);
}

QVariantMap MeCardHandler::meCard() const
{
    return m_meCard;
}

void MeCardHandler::setMeCard(const QVariantMap &meCard)
{
    if (m_meCard == meCard) {
        return;
    }

    const QString oldSsid = ssid();

    m_meCard = meCard;
    Q_EMIT meCardChanged();

    if (oldSsid != ssid()) {
        Q_EMIT ssidChanged();
    }
}

QString MeCardHandler::ssid() const
{
    return m_meCard.value("S"_L1).toString();
}

void MeCardHandler::reset()
{
    setText({});
    setMeCard({});
}

void MeCardHandler::activateConnection()
{
    activateConnectionInternal();
}

QCoro::Task<void> MeCardHandler::activateConnectionInternal()
{
    if (m_meCard.isEmpty()) {
        co_return;
    }

    // Find an existing configuration to update if hotspot isn't visible or in reach.
    NetworkManager::Connection::Ptr existingConnection;

    auto settings = NetworkManager::ConnectionSettings::Ptr(new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Wireless));
    settings->fromMeCard(m_meCard);

    const auto connections = NetworkManager::listConnections();
    for (const auto &connection : connections) {
        if (connection->name().isEmpty() || connection->uuid().isEmpty()) {
            continue;
        }

        NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
        if (settings->connectionType() != NetworkManager::ConnectionSettings::Wireless) {
            continue;
        }

        auto wirelessSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
        if (wirelessSetting->ssid() != ssid()) {
            continue;
        }

        existingConnection = connection;
        break;
    }

    // TODO Pick the wifi device that has the corresponding access point.
    NetworkManager::WirelessDevice::Ptr wifiDevice;

    const auto networkInterfaces = NetworkManager::networkInterfaces();
    for (const auto &device : networkInterfaces) {
        if (device->type() == NetworkManager::Device::Wifi) {
            wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
            break;
        }
    }

    if (!wifiDevice) {
        qCWarning(PLASMA_NM_LIBS_LOG) << "Failed to find wifi device for MeCard";
        co_return;
    }

    if (existingConnection) {
        qCDebug(PLASMA_NM_LIBS_LOG) << "Updating existing connection" << existingConnection->uuid() << existingConnection->path() << "from MeCard";

        // Override default null UUID with existing one.
        settings->setUuid(existingConnection->uuid());

        co_await existingConnection->update(settings->toMap());

        NetworkManager::activateConnection(existingConnection->path(), wifiDevice->uni(), QString());
        co_return;
    }

    qCDebug(PLASMA_NM_LIBS_LOG) << "Adding new connection from MeCard";

    // Add new connection and activate it.
    settings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    settings->setAutoconnect(true);
    UiUtils::setConnectionDefaultPermissions(settings);

    NetworkManager::addAndActivateConnection(settings->toMap(), wifiDevice->uni(), QString());
    // TODO report error.
}
