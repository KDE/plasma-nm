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

#include "modem.h"

#include <utility>

#include <KLocalizedString>
#include <KUser>

Modem::Modem(QObject *parent, ModemManager::ModemDevice::Ptr mmDevice, NetworkManager::ModemDevice::Ptr nmDevice , ModemManager::Modem::Ptr mmInterface, MobileProviders *providers) 
    : QObject{ parent },
      m_mmDevice{ mmDevice },
      m_nmDevice{ nmDevice },
      m_mmInterface{ mmInterface },
      m_providers{ providers }
{
    connect(m_mmDevice.data(), &ModemManager::ModemDevice::simAdded, this, [this]() -> void { Q_EMIT simsChanged(); Q_EMIT hasSimChanged(); });
    connect(m_mmDevice.data(), &ModemManager::ModemDevice::simRemoved, this, [this]() -> void { Q_EMIT simsChanged(); Q_EMIT hasSimChanged(); });
    
    // this is guaranteed to be a GSM modem
    m_mm3gppDevice = m_mmDevice->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();
    
    // add profiles
    refreshProfiles();
    connect(m_nmDevice.data(), &NetworkManager::ModemDevice::availableConnectionChanged, this, [this]() -> void { refreshProfiles(); });
    connect(m_nmDevice.data(), &NetworkManager::ModemDevice::activeConnectionChanged, this, [this]() -> void { refreshProfiles(); Q_EMIT activeConnectionUniChanged(); });
    
    connect(m_nmDevice.data(), &NetworkManager::ModemDevice::stateChanged, this, [this](NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason) -> void {
        qDebug() << "Modem" << m_nmDevice->uni() << "changed state:" << nmDeviceStateStr(oldstate) << "->" << nmDeviceStateStr(newstate) << "due to:" << reason;
        Q_EMIT mobileDataActiveChanged();
    });
    
    // we need to initialize it after m_mm3gppDevice has been set
    m_details = new ModemDetails(this, this);
}

Modem &Modem::operator=(Modem &&other)
{
    swap(other);
    return *this;
}

void Modem::swap(Modem &other)
{
    std::swap(m_details, other.m_details);
    std::swap(m_nmDevice, other.m_nmDevice);
    std::swap(m_mmDevice, other.m_mmDevice);
    std::swap(m_mmInterface, other.m_mmInterface);
    std::swap(m_mm3gppDevice, other.m_mm3gppDevice);
    std::swap(m_profileList, other.m_profileList);
    std::swap(m_providers, other.m_providers);
}

ModemDetails *Modem::modemDetails()
{
    return m_details;
}

QString Modem::displayId()
{
    // in the form /org/freedesktop/ModemManager1/Modem/0
    QStringList uniSplit = uni().split("/");
    return uniSplit.count() == 0 ? "(empty)" : QString(uniSplit[uniSplit.size() - 1]);
}

QString Modem::uni() 
{
    return m_mmInterface->uni();
}

QString Modem::activeConnectionUni()
{
    if (m_nmDevice->activeConnection() && m_nmDevice->activeConnection()->connection()) {
        return m_nmDevice->activeConnection()->connection()->uuid();
    }
    return QString();
}

void Modem::reset() 
{
    qDebug() << "Resetting the modem...";
    QDBusPendingReply<void> reply = m_mmInterface->reset();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Error resetting the modem:" << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error resetting the modem: " + reply.error().message());
    }
}

void Modem::setEnabled(bool enabled)
{
    if (enabled != m_mmInterface->isEnabled()) {
        qDebug() << (enabled ? "Enabling" : "Disabling") << "the modem...";
        QDBusPendingReply<void> reply = m_mmInterface->setEnabled(enabled);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << "Error setting the enabled state of the modem: " << reply.error().message();
        }
    }
}

bool Modem::isRoaming()
{
    if (m_nmDevice->activeConnection() && m_nmDevice->activeConnection()->connection()) {
        auto connection = m_nmDevice->activeConnection()->connection();
        NetworkManager::GsmSetting::Ptr gsmSetting = connection->settings()->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
        if (gsmSetting) {
            return !gsmSetting->homeOnly();
        }
    }
    return false;
}

void Modem::setIsRoaming(bool roaming)
{
    if (m_nmDevice->activeConnection() && m_nmDevice->activeConnection()->connection()) {
        auto connection = m_nmDevice->activeConnection()->connection();
        
        NetworkManager::GsmSetting::Ptr gsmSetting = connection->settings()->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
        if (gsmSetting) {
            gsmSetting->setHomeOnly(!roaming); // set roaming setting
            
            QDBusPendingReply reply = connection->update(connection->settings()->toMap());
            reply.waitForFinished();
            if (reply.isError()) {
                qWarning() << "Error updating connection settings for" << connection->uuid() << ":" << reply.error().message() << ".";
                CellularNetworkSettings::instance()->addMessage(InlineMessage::Error,
                                                                "Error updating connection settings for " + connection->uuid() + ": " + reply.error().message()
                                                                    + ".");
            } else {
                qDebug() << "Successfully updated connection settings" << connection->uuid() << ".";
            }
        }
        
        // the connection uni has changed, refresh the profiles list
        refreshProfiles(); 
        Q_EMIT activeConnectionUniChanged();
    }
}

bool Modem::mobileDataActive()
{
    if (m_nmDevice) {
        if (m_nmDevice->state() == NetworkManager::Device::State::UnknownState ||
            m_nmDevice->state() == NetworkManager::Device::State::Unmanaged ||
            m_nmDevice->state() == NetworkManager::Device::State::Unavailable ||
            m_nmDevice->state() == NetworkManager::Device::State::Disconnected ||
            m_nmDevice->state() == NetworkManager::Device::State::Deactivating ||
            m_nmDevice->state() == NetworkManager::Device::State::Failed) {
                return false;
        } else {
            return m_nmDevice->activeConnection() != nullptr;
        }
    }
    return false;
}

void Modem::setMobileDataActive(bool active)
{
    if (active && !mobileDataActive()) { // turn on mobile data
        m_nmDevice->setAutoconnect(true);
        
        // activate connection that already has autoconnect set to true
        for (auto connection : m_nmDevice->availableConnections()) {
            if (connection->settings()->autoconnect()) {
                activateProfile(connection->uuid());
                break;
            }
        }
        Q_EMIT mobileDataActiveChanged();
    } else if (!active && mobileDataActive()) { // turn off mobile data
        m_nmDevice->setAutoconnect(false);
        
        QDBusPendingReply reply = m_nmDevice->disconnectInterface(); 
        reply.waitForFinished();
        
        if (reply.isError()) {
            qWarning() << "Error disconnecting modem interface:" << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error disconnecting modem interface: " + reply.error().message());
        }
        Q_EMIT mobileDataActiveChanged();
    }
}

bool Modem::hasSim()
{
    return m_mmDevice->sim() != nullptr;
}

QList<ProfileSettings *> &Modem::profileList()
{
    return m_profileList;
}

void Modem::refreshProfiles()
{
    m_profileList.clear();
    for (auto connection : m_nmDevice->availableConnections()) {
        for (auto setting : connection->settings()->settings()) {
            if (setting.dynamicCast<NetworkManager::GsmSetting>()) {
                m_profileList.append(new ProfileSettings(this, setting.dynamicCast<NetworkManager::GsmSetting>(), connection));
            }
        }
    }
    Q_EMIT profileListChanged();
}

void Modem::activateProfile(const QString &connectionUni)
{
    qDebug() << "Activating profile on modem" << m_nmDevice->uni() << "for connection" << connectionUni << "."; 
 
    // cache roaming setting
    bool roaming = isRoaming();
    
    NetworkManager::Connection::Ptr con;
    
    // disable autoconnect for all other connections
    for (auto connection : m_nmDevice->availableConnections()) {
        if (connection->uuid() == connectionUni) {
            connection->settings()->setAutoconnect(true);
            con = connection;
        } else {
            connection->settings()->setAutoconnect(false);
        }
    }
    
    if (!con) {
        qDebug() << "Connection" << connectionUni << "not found.";
        return;
    }
    
    // activate connection manually
    // despite the documentation saying otherwise, activateConnection seems to need the DBus path, not uuid of the connection
    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::activateConnection(con->path(), m_nmDevice->uni(), "");
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Error activating connection:" << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error activating connection: " + reply.error().message());
        return;
    }
    
    // set roaming settings separately (since it changes the uni)
    setIsRoaming(roaming);
}

void Modem::addProfile(QString name, QString apn, QString username, QString password, QString networkType)
{
    NetworkManager::ConnectionSettings::Ptr settings{ new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Gsm) };
    settings->setId(name);
    settings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    settings->setAutoconnect(true);
    settings->addToPermissions(KUser().loginName(), QString());
    
    NetworkManager::GsmSetting::Ptr gsmSetting = settings->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
    gsmSetting->setApn(apn);
    gsmSetting->setUsername(username);
    gsmSetting->setPassword(password);
    gsmSetting->setPasswordFlags(password == "" ? NetworkManager::Setting::NotRequired : NetworkManager::Setting::AgentOwned);
    gsmSetting->setNetworkType(ProfileSettings::networkTypeFlag(networkType));
    gsmSetting->setHomeOnly(!isRoaming());

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addAndActivateConnection(settings->toMap(), m_nmDevice->uni(), "");
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << "Error adding connection:" << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error adding connection: " + reply.error().message());
    } else {
        qDebug() << "Successfully added a new connection" << name << "with APN" << apn << ".";

        // HACK: TODO GSM settings don't seem to get set when adding the connection, mm-qt bug?
        updateProfile(reply.value().path(), name, apn, username, password, networkType);
    }
}

void Modem::removeProfile(const QString &connectionUni)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connectionUni);
    if (con) {
        QDBusPendingReply reply = con->remove();
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << "Error removing connection" << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, "Error removing connection " + reply.error().message());
        }
    }
}

void Modem::updateProfile(QString connectionUni, QString name, QString apn, QString username, QString password, QString networkType)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connectionUni);
    if (con) {
        NetworkManager::ConnectionSettings::Ptr conSettings = con->settings();
        if (conSettings) {
            conSettings->setId(name);
            
            NetworkManager::GsmSetting::Ptr gsmSetting = conSettings->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
            gsmSetting->setApn(apn);
            gsmSetting->setUsername(username);
            gsmSetting->setPassword(password);
            gsmSetting->setPasswordFlags(password == "" ? NetworkManager::Setting::NotRequired : NetworkManager::Setting::AgentOwned);
            gsmSetting->setNetworkType(ProfileSettings::networkTypeFlag(networkType));
            gsmSetting->setHomeOnly(!isRoaming());
            
            QDBusPendingReply reply = con->update(conSettings->toMap());
            reply.waitForFinished();
            if (reply.isError()) {
                qWarning() << "Error updating connection settings for" << connectionUni << ":" << reply.error().message() << ".";
                CellularNetworkSettings::instance()->addMessage(InlineMessage::Error,
                                                                "Error updating connection settings for" + connectionUni + ":" + reply.error().message() + ".");
            } else {
                qDebug() << "Successfully updated connection settings" << connectionUni << ".";
            }
            
        } else {
            qWarning() << "Could not find connection settings for" << connectionUni << "to update!";
        }
    } else {
        qWarning() << "Could not find connection" << connectionUni << "to update!";
    }
}

void Modem::addDetectedProfileSettings()
{
    // load into cache
    m_providers->fillProvidersList();

    bool found = false;
    
    if (m_mmDevice && hasSim()) {
        QString operatorName = m_details->operatorName(); // operator currently connected to
        if (operatorName == "") { // if not connected to a network, use the operator provided on the SIM
            operatorName = m_mmDevice->sim()->operatorName();
        }

        qWarning() << "Detecting profile settings. Operator:" << operatorName;

        if (m_mm3gppDevice) {
            QString operatorCode = m_mm3gppDevice->operatorCode();
            qWarning() << "Using MCCMNC:" << operatorCode << "Provider:" << m_providers->getProvider(operatorCode);

            // lookup apns with mccmnc codes
            QStringList apns = m_providers->getApns(m_providers->getProvider(operatorCode));
            for (auto apn : apns) {
                QVariantMap apnInfo = m_providers->getApnInfo(apn);
                qWarning() << "Found gsm profile settings. Type:" << apnInfo["usageType"];
                
                // only add mobile data apns (not mms)
                if (apnInfo["usageType"].toString() == "internet") {
                    found = true;

                    QString name = operatorName;
                    if (!apnInfo["name"].isNull()) {
                        name += " - " + apnInfo["name"].toString();
                    }
                    
                    addProfile(name, apn, apnInfo["username"].toString(), apnInfo["password"].toString(), "4G/3G/2G");
                }
                
                // TODO in the future for MMS settings, add else if here for == "mms"
            }
        }
    }
    if (!found) {
        qDebug() << "No profiles were found. :(";
        Q_EMIT couldNotAutodetectSettings();
    }
}

QList<Sim *> Modem::sims()
{
    if (m_mmDevice->sim()) {
        return { new Sim{ this, this, m_mmDevice->sim(), m_mmInterface } };
    }
    return {};
}

ModemManager::ModemDevice::Ptr Modem::mmModemDevice()
{
    return m_mmDevice;
}

NetworkManager::ModemDevice::Ptr Modem::nmModemDevice()
{
    return m_nmDevice;
}

ModemManager::Modem::Ptr Modem::mmModemInterface()
{
    return m_mmInterface;
}

QString Modem::nmDeviceStateStr(NetworkManager::Device::State state)
{
    if (state == NetworkManager::Device::State::UnknownState) return "Unknown";
    else if (state == NetworkManager::Device::State::Unmanaged) return "Unmanaged";
    else if (state == NetworkManager::Device::State::Unavailable) return "Unavailable";
    else if (state == NetworkManager::Device::State::Disconnected) return "Disconnected";
    else if (state == NetworkManager::Device::State::Preparing) return "Preparing";
    else if (state == NetworkManager::Device::State::ConfiguringHardware) return "ConfiguringHardware";
    else if (state == NetworkManager::Device::State::NeedAuth) return "NeedAuth";
    else if (state == NetworkManager::Device::State::ConfiguringIp) return "ConfiguringIp";
    else if (state == NetworkManager::Device::State::CheckingIp) return "CheckingIp";
    else if (state == NetworkManager::Device::State::WaitingForSecondaries) return "WaitingForSecondaries";
    else if (state == NetworkManager::Device::State::Activated) return "Activated";
    else if (state == NetworkManager::Device::State::Deactivating) return "Deactivating";
    else if (state == NetworkManager::Device::State::Failed) return "Failed";
    else return "";
}

ProfileSettings::ProfileSettings(QObject* parent, QString name, QString apn, QString user, QString password, NetworkManager::GsmSetting::NetworkType networkType, QString connectionUni)
    : QObject{ parent },
      m_name(name), 
      m_apn(apn), 
      m_user(user), 
      m_password(password), 
      m_networkType(networkTypeStr(networkType)), 
      m_connectionUni(connectionUni)
{
    setParent(parent);
}

ProfileSettings::ProfileSettings(QObject* parent, NetworkManager::Setting::Ptr setting, NetworkManager::Connection::Ptr connection)
    : QObject{ parent },
      m_connectionUni(connection->uuid())
{
    setParent(parent);
    
    NetworkManager::GsmSetting::Ptr gsmSetting = setting.staticCast<NetworkManager::GsmSetting>();
    
    m_name = connection->name();
    m_apn = gsmSetting->apn();
    m_user = gsmSetting->username();
    m_password = gsmSetting->password();
    m_networkType = networkTypeStr(gsmSetting->networkType());
}

QString ProfileSettings::name()
{
    return m_name;
}

QString ProfileSettings::apn()
{
    return m_apn;
}

void ProfileSettings::setApn(QString apn)
{
    if (apn != m_apn) {
        m_apn = apn;
        Q_EMIT apnChanged();
    }
}

QString ProfileSettings::user()
{
    return m_user;
}

void ProfileSettings::setUser(QString user)
{
    if (user != m_user) {
        m_user = user;
        Q_EMIT userChanged();
    }
}

QString ProfileSettings::password()
{
    return m_password;
}

void ProfileSettings::setPassword(QString password)
{
    if (password != m_password) {
        m_password = password; 
        Q_EMIT passwordChanged();
    }
}

QString ProfileSettings::networkType() {
    return m_networkType;
}

void ProfileSettings::setNetworkType(QString networkType) {
    if (networkType != m_networkType) {
        m_networkType = networkType;
        Q_EMIT networkTypeChanged();
    }
}

QString ProfileSettings::connectionUni()
{
    return m_connectionUni;
}

QString ProfileSettings::networkTypeStr(NetworkManager::GsmSetting::NetworkType networkType)
{
    if (networkType == NetworkManager::GsmSetting::NetworkType::Any) {
        return "Any";
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::GprsEdgeOnly) {
        return "Only 2G";
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Only3G) {
        return "Only 3G";
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Only4GLte) {
        return "Only 4G";
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer2G) {
        return "2G";
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer3G) {
        return "3G/2G";
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer4GLte) {
        return "4G/3G/2G";
    }
    return "Any";
}

NetworkManager::GsmSetting::NetworkType ProfileSettings::networkTypeFlag(const QString &networkType)
{
    if (networkType == "Any") {
        return NetworkManager::GsmSetting::NetworkType::Any;
    } else if (networkType == "Only 2G") {
        return NetworkManager::GsmSetting::NetworkType::GprsEdgeOnly;
    } else if (networkType == "Only 3G") {
        return NetworkManager::GsmSetting::NetworkType::Only3G;
    } else if (networkType == "Only 4G") {
        return NetworkManager::GsmSetting::NetworkType::Only4GLte;
    } else if (networkType == "2G") {
        return NetworkManager::GsmSetting::NetworkType::Prefer2G;
    } else if (networkType == "3G/2G") {
        return NetworkManager::GsmSetting::NetworkType::Prefer3G;
    } else if (networkType == "4G/3G/2G") {
        return NetworkManager::GsmSetting::NetworkType::Prefer4GLte;
    }
    return NetworkManager::GsmSetting::NetworkType::Any;
}
