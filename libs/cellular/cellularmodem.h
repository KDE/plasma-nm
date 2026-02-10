/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "plasmanm_cellular_export.h"

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQmlIntegration>

#include <NetworkManagerQt/CdmaSetting>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/Ipv6Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Settings>

#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/Manager>
#include <ModemManagerQt/Modem3Gpp>
#include <ModemManagerQt/ModemDevice>

#include <QCoroDBusPendingReply>

#include "cellularconnectionprofile.h"
#include "cellularmodemdetails.h"
#include "cellularsim.h"

class MobileProviders;

class PLASMANM_CELLULAR_EXPORT CellularModem : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(CellularModemDetails *details READ modemDetails NOTIFY modemDetailsChanged)
    Q_PROPERTY(CellularConnectionProfile *profiles READ profiles NOTIFY profilesChanged)
    Q_PROPERTY(QString uni READ uni NOTIFY uniChanged)
    Q_PROPERTY(QString displayId READ displayId NOTIFY displayIdChanged)

    Q_PROPERTY(bool hasSim READ hasSim NOTIFY hasSimChanged)
    Q_PROPERTY(bool simLocked READ simLocked NOTIFY simLockedChanged)
    Q_PROPERTY(bool simEmpty READ simEmpty NOTIFY simEmptyChanged)
    Q_PROPERTY(QString activeConnectionUni READ activeConnectionUni NOTIFY activeConnectionUniChanged)

    Q_PROPERTY(bool mobileDataEnabled READ mobileDataEnabled WRITE setMobileDataEnabled NOTIFY mobileDataEnabledChanged)
    Q_PROPERTY(bool mobileDataSupported READ mobileDataSupported NOTIFY mobileDataSupportedChanged)
    Q_PROPERTY(bool needsAPNAdded READ needsAPNAdded NOTIFY mobileDataEnabledChanged)

    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(QString operatorName READ operatorName NOTIFY operatorNameChanged)

    Q_PROPERTY(QStringList errors READ errors NOTIFY errorsChanged)

public:
    CellularModem(QObject *parent = nullptr);
    CellularModem(QObject *parent, ModemManager::ModemDevice::Ptr mmModem, ModemManager::Modem::Ptr mmInterface);

    CellularModemDetails *modemDetails() const;
    CellularConnectionProfile *profiles() const;
    QString displayId() const;
    QString uni() const;
    QString activeConnectionUni() const;

    Q_INVOKABLE QCoro::Task<void> reset();

    bool mobileDataEnabled() const;
    void setMobileDataEnabled(bool enabled);
    bool mobileDataSupported() const;
    bool needsAPNAdded() const;

    bool hasSim() const;
    bool simLocked() const;
    bool simEmpty() const;

    int signalStrength() const;
    QString operatorName() const;

    QStringList errors() const;
    Q_INVOKABLE void clearErrors();

    // Connection profile management
    Q_INVOKABLE QCoro::Task<void> activateProfile(const QString &connectionUni);
    Q_INVOKABLE QCoro::Task<void> addProfile(QString name, QString apn, QString username, QString password, QString networkType);
    Q_INVOKABLE QCoro::Task<void> removeProfile(const QString &connectionUni);
    Q_INVOKABLE QCoro::Task<void> updateProfile(QString connectionUni, QString name, QString apn, QString username, QString password, QString networkType);
    Q_INVOKABLE void addDetectedProfileSettings(); // detect modem connection settings (ex. apn) and add a new connection

    QList<CellularSim *> sims();

    ModemManager::ModemDevice::Ptr mmModemDevice();
    NetworkManager::ModemDevice::Ptr nmModemDevice();
    ModemManager::Modem::Ptr mmModemInterface();
    ModemManager::Modem3gpp::Ptr mm3gppDevice();

Q_SIGNALS:
    void modemDetailsChanged();
    void profilesChanged();
    void uniChanged();
    void displayIdChanged();
    void activeConnectionUniChanged();
    void nmModemChanged();
    void mobileDataEnabledChanged();
    void mobileDataSupportedChanged();
    void simsChanged();
    void hasSimChanged();
    void simLockedChanged();
    void simEmptyChanged();
    void signalStrengthChanged();
    void operatorNameChanged();
    void errorsChanged();
    void errorOccurred(const QString &message);
    void couldNotAutodetectSettings();

private:
    void findNetworkManagerDevice();
    void refreshSims();
    void refreshProfiles();
    void addError(const QString &message);

    CellularModemDetails *m_details = nullptr;
    CellularConnectionProfile *m_profiles = nullptr;

    ModemManager::ModemDevice::Ptr m_mmModem;
    NetworkManager::ModemDevice::Ptr m_nmModem; // may be nullptr if the NM modem hasn't been found yet
    ModemManager::Modem::Ptr m_mmInterface = nullptr;
    ModemManager::Modem3gpp::Ptr m_mm3gppDevice = nullptr; // may be nullptr if no sim is inserted

    QList<CellularSim *> m_sims;
    QStringList m_errors;
};
