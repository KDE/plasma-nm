/*
 *   Copyright 2021 Devin Lin <espidev@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "sim.h"

#include <KLocalizedString>

Sim::Sim(QObject *parent, Modem *modem, ModemManager::Sim::Ptr mmSim, ModemManager::Modem::Ptr mmModem)
    : QObject{ parent },
      m_modem{ modem },
      m_mmSim{ mmSim },
      m_mmModem{ mmModem }
{
    connect(m_mmSim.data(), &ModemManager::Sim::imsiChanged, this, [this]() -> void { Q_EMIT imsiChanged(); });
    connect(m_mmSim.data(), &ModemManager::Sim::operatorIdentifierChanged, this, [this]() -> void { Q_EMIT operatorIdentifierChanged(); });
    connect(m_mmSim.data(), &ModemManager::Sim::operatorNameChanged, this, [this]() -> void { Q_EMIT operatorNameChanged(); });
    connect(m_mmSim.data(), &ModemManager::Sim::simIdentifierChanged, this, [this]() -> void { Q_EMIT simIdentifierChanged(); });

    connect(m_mmModem.data(), &ModemManager::Modem::unlockRequiredChanged, this, [this]() -> void {
        Q_EMIT lockedChanged();
        Q_EMIT lockedReasonChanged();
    });
}

bool Sim::enabled()
{
    return uni() != "/";
}

bool Sim::pinEnabled()
{
}

bool Sim::locked()
{
    return m_mmModem->unlockRequired() != MM_MODEM_LOCK_NONE && m_mmModem->unlockRequired() != MM_MODEM_LOCK_SIM_PIN2;
}

QString Sim::lockedReason()
{
    switch (m_mmModem->unlockRequired()) {
        case MM_MODEM_LOCK_UNKNOWN:
            return i18n("Lock reason unknown.");
        case MM_MODEM_LOCK_NONE:
            return i18n("Modem is unlocked.");
        case MM_MODEM_LOCK_SIM_PIN:
            return i18n("SIM requires the PIN code.");
        case MM_MODEM_LOCK_SIM_PIN2:
            return i18n("SIM requires the PIN2 code.");
        case MM_MODEM_LOCK_SIM_PUK:
            return i18n("SIM requires the PUK code.");
        case MM_MODEM_LOCK_SIM_PUK2:
            return i18n("SIM requires the PUK2 code.");
        case MM_MODEM_LOCK_PH_SP_PIN:
            return i18n("Modem requires the service provider PIN code.");
        case MM_MODEM_LOCK_PH_SP_PUK:
            return i18n("Modem requires the service provider PUK code.");
        case MM_MODEM_LOCK_PH_NET_PIN:
            return i18n("Modem requires the network PIN code.");
        case MM_MODEM_LOCK_PH_NET_PUK:
            return i18n("Modem requires the network PUK code.");
        case MM_MODEM_LOCK_PH_SIM_PIN:
            return i18n("Modem requires the PIN code.");
        case MM_MODEM_LOCK_PH_CORP_PIN:
            return i18n("Modem requires the corporate PIN code.");
        case MM_MODEM_LOCK_PH_CORP_PUK:
            return i18n("Modem requires the corporate PUK code.");
        case MM_MODEM_LOCK_PH_FSIM_PIN:
            return i18n("Modem requires the PH-FSIM PIN code.");
        case MM_MODEM_LOCK_PH_FSIM_PUK:
            return i18n("Modem requires the PH-FSIM PUK code.");
        case MM_MODEM_LOCK_PH_NETSUB_PIN:
            return i18n("Modem requires the network subset PIN code.");
        case MM_MODEM_LOCK_PH_NETSUB_PUK:
            return i18n("Modem requires the network subset PUK code.");
    }
    return QString{};
}

QString Sim::imsi()
{
    return m_mmSim->imsi();
}

QString Sim::eid()
{
    return ""; // TODO add in mm-qt
}

QString Sim::operatorIdentifier()
{
    return m_mmSim->operatorIdentifier();
}

QString Sim::operatorName()
{
    return m_mmSim->operatorName();
}

QString Sim::simIdentifier()
{
    return m_mmSim->simIdentifier();
}

QStringList Sim::emergencyNumbers()
{
    return {}; // TODO add in mm-qt
}

QString Sim::uni()
{
    return m_mmSim->uni();
}

QString Sim::displayId()
{
    // in the form /org/freedesktop/ModemManager1/Sim/0
    QStringList uniSplit = uni().split("/");
    return (uniSplit.count() == 0 || uni() == "/") ? "(empty)" : QString(uniSplit[uniSplit.size() - 1]);
}

Modem *Sim::modem()
{
    return m_modem;
}

void Sim::togglePinEnabled(const QString &pin)
{
    bool isLocked = locked();
    QDBusPendingReply reply = m_mmSim->enablePin(pin, !isLocked);
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Error toggling SIM lock to" << isLocked << ":" << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error toggling SIM lock: " + reply.error().message());
    }
}

void Sim::changePin(const QString &oldPin, const QString &newPin)
{
    if (locked()) {
        QDBusPendingReply reply = m_mmSim->changePin(oldPin, newPin);
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "Error changing the PIN:" << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error changing the PIN: " + reply.error().message());
        }
    } else {
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Warning, "The SIM card is not locked.");
    }
}

void Sim::sendPin(const QString &pin)
{
    if (m_mmModem->unlockRequired() != MM_MODEM_LOCK_NONE) {
        QDBusPendingReply reply = m_mmSim->sendPin(pin);
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "Error sending the PIN:" << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error sending the PIN: " + reply.error().message());
        }
    }
}

void Sim::sendPuk(const QString &pin, const QString &puk)
{
    if (m_mmModem->unlockRequired() != MM_MODEM_LOCK_NONE) {
        QDBusPendingReply reply = m_mmSim->sendPuk(pin, puk);
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "Error sending the PUK:" << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error sending the PUK: " + reply.error().message());
        }
    }
}
