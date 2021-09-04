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

#pragma once

#include "mobileproviders.h"
#include "sim.h"
#include "modem.h"

#include <QList>
#include <QString>

#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/GsmSetting>
#include <NetworkManagerQt/CdmaSetting>
#include <NetworkManagerQt/ModemDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>

#include <ModemManagerQt/Manager>
#include <ModemManagerQt/GenericTypes>
#include <ModemManagerQt/ModemDevice>

#include <modem3gpp.h>

class Modem;
class AvailableNetwork;

class ModemDetails : public QObject {
    Q_OBJECT
    // modemmanager device
    Q_PROPERTY(QStringList accessTechnologies READ accessTechnologies NOTIFY accessTechnologiesChanged) // currently used tech
    Q_PROPERTY(QString device READ device NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceIdentifier READ deviceIdentifier NOTIFY deviceIdentifierChanged)
    Q_PROPERTY(QStringList drivers READ drivers NOTIFY driversChanged)
    Q_PROPERTY(QString equipmentIdentifier READ equipmentIdentifier NOTIFY equipmentIdentifierChanged)
    // TODO add bands
    Q_PROPERTY(bool isEnabled READ isEnabled NOTIFY isEnabledChanged)
    Q_PROPERTY(QString manufacturer READ manufacturer NOTIFY manufacturerChanged)
    Q_PROPERTY(uint maxActiveBearers READ maxActiveBearers NOTIFY maxActiveBearersChanged)
    Q_PROPERTY(uint maxBearers READ maxBearers NOTIFY maxBearersChanged)
    Q_PROPERTY(QString model READ model NOTIFY modelChanged)
    Q_PROPERTY(QStringList ownNumbers READ ownNumbers NOTIFY ownNumbersChanged)
    Q_PROPERTY(QString plugin READ plugin NOTIFY pluginChanged)
    Q_PROPERTY(QStringList ports READ ports NOTIFY portsChanged)
    Q_PROPERTY(QString powerState READ powerState NOTIFY powerStateChanged)
    Q_PROPERTY(QString primaryPort READ primaryPort NOTIFY primaryPortChanged)
    Q_PROPERTY(QString revision READ revision NOTIFY revisionChanged)
    Q_PROPERTY(uint signalQuality READ signalQuality NOTIFY signalQualityChanged)
    Q_PROPERTY(QString simPath READ simPath NOTIFY simPathChanged)
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString stateFailedReason READ stateFailedReason NOTIFY stateFailedReasonChanged)
    Q_PROPERTY(QStringList supportedCapabilities READ supportedCapabilities NOTIFY supportedCapabilitiesChanged)
    
    // modemmanager 3gpp device
    Q_PROPERTY(QString operatorCode READ operatorCode NOTIFY operatorCodeChanged)
    Q_PROPERTY(QString operatorName READ operatorName NOTIFY operatorNameChanged)
    Q_PROPERTY(QString registrationState READ registrationState NOTIFY registrationStateChanged)
    Q_PROPERTY(QList<AvailableNetwork *> networks READ networks NOTIFY networksChanged)
    Q_PROPERTY(bool isScanningNetworks READ isScanningNetworks NOTIFY isScanningNetworksChanged)
    
    // networkmanager device
    Q_PROPERTY(QString firmwareVersion READ firmwareVersion NOTIFY firmwareVersionChanged)
    Q_PROPERTY(QString interfaceName READ interfaceName NOTIFY interfaceNameChanged)
    Q_PROPERTY(QString metered READ metered NOTIFY meteredChanged)
    Q_PROPERTY(QString rxBytes READ rxBytes NOTIFY rxBytesChanged)
    Q_PROPERTY(QString txBytes READ rxBytes NOTIFY txBytesChanged)
    
public:
    ModemDetails(QObject *parent = nullptr, Modem *modem = nullptr);
    ModemDetails &operator=(ModemDetails &&other);
    void swap(ModemDetails &other);
    
    QStringList accessTechnologies();
    QString device();
    QString deviceIdentifier();
    QStringList drivers();
    QString equipmentIdentifier();
    bool isEnabled();
    QString manufacturer();
    uint maxActiveBearers();
    uint maxBearers();
    QString model();
    QStringList ownNumbers();
    QString plugin();
    QStringList ports();
    QString powerState();
    QString primaryPort();
    QString revision();
    uint signalQuality();
    QString simPath();
    QString state();
    QString stateFailedReason();
    QStringList supportedCapabilities();
    
    QString operatorCode();
    QString operatorName();
    QString registrationState();
    
    Q_INVOKABLE void scanNetworks();
    QList<AvailableNetwork *> networks();
    bool isScanningNetworks();
    void scanNetworksFinished(QDBusPendingCallWatcher *call);
    
    QString firmwareVersion();
    QString interfaceName();
    QString metered();
    QString rxBytes();
    QString txBytes();
    
Q_SIGNALS:
    void accessTechnologiesChanged();
    void deviceChanged();
    void deviceIdentifierChanged();
    void driversChanged();
    void equipmentIdentifierChanged();
    void isEnabledChanged();
    void manufacturerChanged();
    void maxActiveBearersChanged();
    void maxBearersChanged();
    void modelChanged();
    void ownNumbersChanged();
    void pluginChanged();
    void portsChanged();
    void powerStateChanged();
    void primaryPortChanged();
    void revisionChanged();
    void signalQualityChanged();
    void simPathChanged();
    void stateChanged();
    void stateFailedReasonChanged();
    void supportedCapabilitiesChanged();
    
    void operatorCodeChanged();
    void operatorNameChanged();
    void registrationStateChanged();
    void networksChanged();
    void isScanningNetworksChanged();
    
    void firmwareVersionChanged();
    void interfaceNameChanged();
    void meteredChanged();
    void rxBytesChanged();
    void txBytesChanged();
    
private:
    Modem *m_modem;
    
    QDBusPendingCallWatcher *m_scanNetworkWatcher;
    bool m_isScanningNetworks;
    QList<AvailableNetwork *> m_cachedScannedNetworks;
};

class AvailableNetwork : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isCurrentlyUsed READ isCurrentlyUsed NOTIFY isCurrentlyUsedChanged)
    Q_PROPERTY(QString operatorLong READ operatorLong NOTIFY operatorLongChanged)
    Q_PROPERTY(QString operatorShort READ operatorShort NOTIFY operatorShortChanged)
    Q_PROPERTY(QString operatorCode READ operatorCode NOTIFY operatorCodeChanged)
    Q_PROPERTY(QString accessTechnology READ accessTechnology NOTIFY accessTechnologyChanged)
    
public:
    AvailableNetwork(QObject *parent = nullptr,
                     ModemManager::Modem3gpp::Ptr mm3gppDevice = nullptr,
                     bool isCurrentlyUsed = false,
                     QString operatorLong = "",
                     QString operatorShort = "",
                     QString operatorCode = "",
                     MMModemAccessTechnology accessTechnology = MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN);

    bool isCurrentlyUsed();
    QString operatorLong();
    QString operatorShort();
    QString operatorCode();
    QString accessTechnology();

    Q_INVOKABLE void registerToNetwork();

Q_SIGNALS:
    void isCurrentlyUsedChanged();
    void operatorLongChanged();
    void operatorShortChanged();
    void operatorCodeChanged();
    void accessTechnologyChanged();
    
private:
    bool m_isCurrentlyUsed;
    QString m_operatorLong, m_operatorShort, m_operatorCode, m_accessTechnology;

    ModemManager::Modem3gpp::Ptr m_mm3gppDevice; // this may be a nullptr if no sim is inserted
};
