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
#include "modemdetails.h"

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

class ProfileSettings;
class Sim;
class AvailableNetwork;
class ModemDetails;

// only supports GSM/UMTS/LTE
class Modem : public QObject {
    Q_OBJECT
    Q_PROPERTY(ModemDetails* details READ modemDetails NOTIFY modemDetailsChanged)
    Q_PROPERTY(QString uni READ uni NOTIFY uniChanged)
    Q_PROPERTY(QString displayId READ displayId NOTIFY displayIdChanged)
    
    Q_PROPERTY(bool isRoaming READ isRoaming WRITE setIsRoaming NOTIFY isRoamingChanged)
    Q_PROPERTY(bool hasSim READ hasSim NOTIFY hasSimChanged)
    Q_PROPERTY(QList<ProfileSettings *> profiles READ profileList NOTIFY profileListChanged)
    Q_PROPERTY(QString activeConnectionUni READ activeConnectionUni NOTIFY activeConnectionUniChanged)
    Q_PROPERTY(bool mobileDataActive READ mobileDataActive WRITE setMobileDataActive NOTIFY mobileDataActiveChanged)
    
public:
    Modem(QObject *parent = nullptr) : QObject{ parent } {}
    Modem(QObject *parent, ModemManager::ModemDevice::Ptr mmDevice, NetworkManager::ModemDevice::Ptr nmDevice, ModemManager::Modem::Ptr m_mmInterface, MobileProviders *providers);
    Modem &operator=(Modem &&other);
    void swap(Modem &other);
    
    ModemDetails *modemDetails();
    QString displayId(); // splits uni and obtains the number suffix
    QString uni();
    QString activeConnectionUni();
    
    Q_INVOKABLE void reset();
    Q_INVOKABLE void setEnabled(bool enabled);
    
    bool isRoaming();
    void setIsRoaming(bool roaming);
    bool mobileDataActive();
    void setMobileDataActive(bool active);
    bool hasSim();
    
    // connection profiles
    QList<ProfileSettings *> &profileList();
    void refreshProfiles();
    Q_INVOKABLE void activateProfile(const QString &connectionUni);
    Q_INVOKABLE void addProfile(QString name, QString apn, QString username, QString password, QString networkType);
    Q_INVOKABLE void removeProfile(const QString &connectionUni);
    Q_INVOKABLE void updateProfile(QString connectionUni, QString name, QString apn, QString username, QString password, QString networkType);
    Q_INVOKABLE void addDetectedProfileSettings(); // detect modem connection settings (ex. apn) and add a new connection
    
    QList<Sim *> sims();
    
    ModemManager::ModemDevice::Ptr mmModemDevice();
    NetworkManager::ModemDevice::Ptr nmModemDevice();
    ModemManager::Modem::Ptr mmModemInterface();
    
Q_SIGNALS:
    void modemDetailsChanged();
    void uniChanged();
    void displayIdChanged();
    void activeConnectionUniChanged();
    
    void isRoamingChanged();
    void simsChanged();
    void hasSimChanged();
    void profileListChanged();
    void mobileDataActiveChanged();
    
    void couldNotAutodetectSettings();
    
private:
    QString nmDeviceStateStr(NetworkManager::Device::State state);
    
    ModemDetails *m_details;
    
    ModemManager::ModemDevice::Ptr m_mmDevice;
    NetworkManager::ModemDevice::Ptr m_nmDevice;
    ModemManager::Modem::Ptr m_mmInterface = nullptr;
    ModemManager::Modem3gpp::Ptr m_mm3gppDevice = nullptr;
    
    QList<ProfileSettings *> m_profileList;
    
    MobileProviders *m_providers;
    
    friend class ModemDetails;
};

class ProfileSettings : public QObject 
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString apn READ apn WRITE setApn NOTIFY apnChanged)
    Q_PROPERTY(QString user READ user WRITE setUser NOTIFY userChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString networkType READ networkType WRITE setNetworkType NOTIFY networkTypeChanged)
    Q_PROPERTY(bool allowRoaming READ allowRoaming WRITE setAllowRoaming NOTIFY allowRoamingChanged)
    Q_PROPERTY(QString connectionUni READ connectionUni NOTIFY connectionUniChanged)
    
public:
    ProfileSettings(QObject* parent = nullptr) : QObject{ parent } {}
    ProfileSettings(QObject* parent, QString name, QString apn, QString user, QString password, NetworkManager::GsmSetting::NetworkType networkType, bool allowRoaming, QString connectionUni);
    ProfileSettings(QObject* parent, NetworkManager::Setting::Ptr settings, NetworkManager::Connection::Ptr connection);
    
    QString name();
    QString apn();
    void setApn(QString apn);
    QString user();
    void setUser(QString user);
    QString password();
    void setPassword(QString password);
    QString networkType();
    void setNetworkType(QString ipType);
    bool allowRoaming();
    void setAllowRoaming(bool allowRoaming);
    QString connectionUni();
    
    // utilities
    static QString networkTypeStr(NetworkManager::GsmSetting::NetworkType networkType);
    static NetworkManager::GsmSetting::NetworkType networkTypeFlag(const QString &networkType);
    
Q_SIGNALS:
    void nameChanged();
    void apnChanged();
    void userChanged();
    void passwordChanged();
    void networkTypeChanged();
    void allowRoamingChanged();
    void connectionUniChanged();

private:
    QString m_name, m_apn, m_user, m_password, m_networkType, m_connectionUni;
    bool m_gsm, m_allowRoaming;
};
