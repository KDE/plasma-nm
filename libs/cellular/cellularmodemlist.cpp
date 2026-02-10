/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "cellularmodemlist.h"
#include "cellularmodem.h"
#include "cellularsim.h"

#include "plasma_nm_cellular.h"

CellularModemList::CellularModemList(QObject *parent)
    : QObject{parent}
{
    updateModemList();

    connect(ModemManager::notifier(), &ModemManager::Notifier::modemAdded, this, &CellularModemList::updateModemList);
    connect(ModemManager::notifier(), &ModemManager::Notifier::modemRemoved, this, &CellularModemList::updateModemList);
}

QList<CellularModem *> CellularModemList::modems() const
{
    return m_modemList;
}

CellularModem *CellularModemList::primaryModem() const
{
    if (m_modemList.count() > 0) {
        return m_modemList[0];
    }
    return nullptr;
}

bool CellularModemList::modemAvailable() const
{
    return !m_modemList.empty();
}

QList<CellularSim *> CellularModemList::sims() const
{
    QList<CellularSim *> allSims;
    for (auto modem : m_modemList) {
        allSims.append(modem->sims());
    }
    return allSims;
}

void CellularModemList::updateModemList()
{
    // Clean up old modems
    qDeleteAll(m_modemList);
    m_modemList.clear();

    for (ModemManager::ModemDevice::Ptr device : ModemManager::modemDevices()) {
        ModemManager::Modem::Ptr modem = device->modemInterface();

        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Found modem:" << device->uni();

        auto cellModem = new CellularModem(this, device, modem);
        connect(cellModem, &CellularModem::simsChanged, this, &CellularModemList::simsChanged);
        m_modemList.push_back(cellModem);
    }

    if (m_modemList.empty()) {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "No modems found.";
    }

    Q_EMIT modemsChanged();
    Q_EMIT primaryModemChanged();
    Q_EMIT modemAvailableChanged();
    Q_EMIT simsChanged();
}
