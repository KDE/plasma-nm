/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "plasmanm_cellular_export.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQmlIntegration>

#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/Modem3Gpp>
#include <ModemManagerQt/ModemDevice>

#include <QCoroDBusPendingReply>

class CellularModem;

class PLASMANM_CELLULAR_EXPORT CellularSim : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool enabled READ enabled NOTIFY enabledChanged)
    Q_PROPERTY(bool pinEnabled READ pinEnabled NOTIFY pinEnabledChanged) // if there is a PIN set on the SIM
    Q_PROPERTY(int unlockRetriesLeft READ unlockRetriesLeft NOTIFY unlockRetriesLeftChanged)
    Q_PROPERTY(bool locked READ locked NOTIFY lockedChanged) // if the SIM is currently locked (requires entering PIN)
    Q_PROPERTY(QString lockedReason READ lockedReason NOTIFY lockedReasonChanged)
    Q_PROPERTY(QString imsi READ imsi NOTIFY imsiChanged)
    Q_PROPERTY(QString eid READ eid NOTIFY eidChanged)
    Q_PROPERTY(QString operatorIdentifier READ operatorIdentifier NOTIFY operatorIdentifierChanged)
    Q_PROPERTY(QString operatorName READ operatorName NOTIFY operatorNameChanged)
    Q_PROPERTY(QString simIdentifier READ simIdentifier NOTIFY simIdentifierChanged)
    Q_PROPERTY(QStringList emergencyNumbers READ emergencyNumbers NOTIFY emergencyNumbersChanged)
    Q_PROPERTY(QString uni READ uni NOTIFY uniChanged)
    Q_PROPERTY(QString displayId READ displayId NOTIFY displayIdChanged)
    Q_PROPERTY(CellularModem *modem READ modem NOTIFY modemChanged)

public:
    CellularSim(QObject *parent = nullptr,
                CellularModem *modem = nullptr,
                ModemManager::Sim::Ptr mmSim = ModemManager::Sim::Ptr{nullptr},
                ModemManager::Modem::Ptr mmModem = ModemManager::Modem::Ptr{nullptr},
                ModemManager::Modem3gpp::Ptr mmModem3gpp = ModemManager::Modem3gpp::Ptr{nullptr});

    bool enabled();
    bool pinEnabled();
    int unlockRetriesLeft();
    bool locked();
    QString lockedReason();
    QString imsi();
    QString eid();
    QString operatorIdentifier();
    QString operatorName();
    QString simIdentifier();
    QStringList emergencyNumbers();
    QString uni();
    QString displayId();
    CellularModem *modem();

    Q_INVOKABLE QCoro::Task<void> togglePinEnabled(const QString &pin);
    Q_INVOKABLE QCoro::Task<void> changePin(const QString &oldPin, const QString &newPin);
    Q_INVOKABLE QCoro::Task<void> sendPin(const QString &pin);
    Q_INVOKABLE QCoro::Task<void> sendPuk(const QString &pin, const QString &puk);

Q_SIGNALS:
    void enabledChanged();
    void pinEnabledChanged();
    void unlockRetriesLeftChanged();
    void lockedChanged();
    void lockedReasonChanged();
    void imsiChanged();
    void eidChanged();
    void operatorIdentifierChanged();
    void operatorNameChanged();
    void simIdentifierChanged();
    void emergencyNumbersChanged();
    void uniChanged();
    void displayIdChanged();
    void modemChanged();
    void errorOccurred(const QString &message);

private:
    CellularModem *m_modem;
    ModemManager::Sim::Ptr m_mmSim;
    ModemManager::Modem::Ptr m_mmModem;
    ModemManager::Modem3gpp::Ptr m_mmModem3gpp; // may be nullptr if no sim is inserted
};
