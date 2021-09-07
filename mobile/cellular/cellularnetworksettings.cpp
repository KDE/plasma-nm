/*
 * SPDX-FileCopyrightText: 2018 Martin Kacej <m.kacej@atlas.sk>
 * SPDX-FileCopyrightText: 2020-2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "cellularnetworksettings.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KAboutData>
#include <KUser>

#include <QQmlEngine>

K_PLUGIN_CLASS_WITH_JSON(CellularNetworkSettings, "cellularnetworksettings.json")

CellularNetworkSettings *CellularNetworkSettings::staticInst = nullptr;

CellularNetworkSettings::CellularNetworkSettings(QObject* parent, const QVariantList& args) 
    : KQuickAddons::ConfigModule(parent, args),
      m_modemList{},
      m_simList{},
      m_providers{ nullptr }
{
    CellularNetworkSettings::staticInst = this;

    KAboutData* about = new KAboutData("kcm_cellular_network", i18n("Cellular Networks"), "0.1", QString(), KAboutLicense::GPL);
    about->addAuthor(i18n("Devin Lin"), QString(), QStringLiteral("espidev@gmail.com"));
    about->addAuthor(i18n("Martin Kacej"), QString(), QStringLiteral("m.kacej@atlas.sk"));
    setAboutData(about);
    
    qmlRegisterType<ProfileSettings>("cellularnetworkkcm", 1, 0, "ProfileSettings");
    qmlRegisterType<Modem>("cellularnetworkkcm", 1, 0, "Modem");
    qmlRegisterType<ModemDetails>("cellularnetworkkcm", 1, 0, "ModemDetails");
    qmlRegisterType<AvailableNetwork>("cellularnetworkkcm", 1, 0, "AvailableNetwork");
    qmlRegisterType<Sim>("cellularnetworkkcm", 1, 0, "Sim");
    qmlRegisterType<InlineMessage>("cellularnetworkkcm", 1, 0, "InlineMessage");

    // parse mobile providers list
    m_providers = new MobileProviders();
    
    // find modems
    ModemManager::scanDevices();

    qDebug() << QStringLiteral("Scanning for modems...");
    for (ModemManager::ModemDevice::Ptr device : ModemManager::modemDevices()) {
        ModemManager::Modem::Ptr modem = device->modemInterface();
        NetworkManager::ModemDevice::Ptr nmModem;
        
        for (NetworkManager::Device::Ptr nmDevice : NetworkManager::networkInterfaces()) {
            if (nmDevice->udi() == device->uni()) {
                nmModem = nmDevice.objectCast<NetworkManager::ModemDevice>();
            }
        }

        qDebug() << QStringLiteral("Found modem:") << device->uni();
        if (!nmModem) {
            qWarning() << QStringLiteral("NetworkManager ModemDevice could not be found for this modem! Ignoring...");
        } else if ((nmModem->currentCapabilities() & NetworkManager::ModemDevice::GsmUmts) || 
                   (nmModem->currentCapabilities() & NetworkManager::ModemDevice::Lte)) {
            
            m_modemList.push_back(new Modem(this, device, nmModem, modem, m_providers));
            
            // update sims list if modem's list changes
            connect(m_modemList[m_modemList.size() - 1], &Modem::simsChanged, this, [this]() -> void { fillSims(); });
        } else {
            qDebug() << QStringLiteral("Modem is not 3GPP (CDMA not supported), skipping...");
        }
    }
    
    if (m_modemList.empty()) {
        qDebug() << QStringLiteral("No modems found.");
    }
    
    Q_EMIT selectedModemChanged();
    
    fillSims();
}

CellularNetworkSettings *CellularNetworkSettings::instance()
{
    return CellularNetworkSettings::staticInst;
}

Modem *CellularNetworkSettings::selectedModem()
{
    // TODO
    if (m_modemList.count() > 0) {
        return m_modemList[0];
    }
    return nullptr;
}

QList<Modem *> CellularNetworkSettings::modems()
{
    return m_modemList;
}

QList<Sim *> CellularNetworkSettings::sims()
{
    return m_simList;
}

bool CellularNetworkSettings::modemFound()
{
    return !m_modemList.empty();
}

void CellularNetworkSettings::fillSims()
{
    for (auto p : m_simList) {
        delete p;
    }
    m_simList.clear();

    qDebug() << QStringLiteral("Scanning SIMs list...");
    for (auto modem : m_modemList) {
        auto sims = modem->sims();
        for (auto simp : sims) {
            qDebug() << QStringLiteral("Found SIM") << simp->uni() << simp->imsi();
            m_simList.push_back(simp);
        }
    }
    
    Q_EMIT simsChanged();
}

QList<InlineMessage *> CellularNetworkSettings::messages()
{
    return m_messages;
}

void CellularNetworkSettings::addMessage(InlineMessage::Type type, QString msg)
{
    m_messages.push_back(new InlineMessage{this, type, msg});
    Q_EMIT messagesChanged();
}

void CellularNetworkSettings::removeMessage(int index)
{
    if (index >= 0 && index < m_messages.size()) {
        m_messages.removeAt(index);
        Q_EMIT messagesChanged();
    }
}

InlineMessage::InlineMessage(QObject *parent, Type type, QString message)
    : QObject{parent}
    , m_type{type}
    , m_message{message}
{
}

InlineMessage::Type InlineMessage::type()
{
    return m_type;
}

QString InlineMessage::message()
{
    return m_message;
}

#include "cellularnetworksettings.moc"
