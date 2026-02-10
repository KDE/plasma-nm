/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "plasmanm_cellular_export.h"

#include <QList>
#include <QObject>
#include <QtQmlIntegration>

#include <ModemManagerQt/Manager>
#include <ModemManagerQt/ModemDevice>

class CellularModem;
class CellularSim;

class PLASMANM_CELLULAR_EXPORT CellularModemList : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QList<CellularModem *> modems READ modems NOTIFY modemsChanged)
    Q_PROPERTY(CellularModem *primaryModem READ primaryModem NOTIFY primaryModemChanged)
    Q_PROPERTY(bool modemAvailable READ modemAvailable NOTIFY modemAvailableChanged)
    Q_PROPERTY(QList<CellularSim *> sims READ sims NOTIFY simsChanged)

public:
    explicit CellularModemList(QObject *parent = nullptr);

    QList<CellularModem *> modems() const;
    CellularModem *primaryModem() const;
    bool modemAvailable() const;
    QList<CellularSim *> sims() const;

Q_SIGNALS:
    void modemsChanged();
    void primaryModemChanged();
    void modemAvailableChanged();
    void simsChanged();

private:
    void updateModemList();

    QList<CellularModem *> m_modemList;
};
