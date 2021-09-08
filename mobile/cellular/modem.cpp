/*
 * SPDX-FileCopyrightText: 2021 Devin Lin <espidev@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

    // if no sim is inserted, m_mm3gppDevice is nullptr
    if (m_mm3gppDevice) {
        m_mm3gppDevice->setTimeout(60000); // scanning networks likely takes longer than the default timeout
    }

    // add profiles
    refreshProfiles();
    connect(m_nmDevice.data(), &NetworkManager::ModemDevice::availableConnectionChanged, this, [this]() -> void { refreshProfiles(); });
    connect(m_nmDevice.data(), &NetworkManager::ModemDevice::activeConnectionChanged, this, [this]() -> void { refreshProfiles(); Q_EMIT activeConnectionUniChanged(); });

    connect(m_nmDevice.data(),
            &NetworkManager::ModemDevice::stateChanged,
            this,
            [this](NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason) -> void {
                qDebug() << QStringLiteral("Modem") << m_nmDevice->uni() << QStringLiteral("changed state:") << nmDeviceStateStr(oldstate)
                         << QStringLiteral("->") << nmDeviceStateStr(newstate) << QStringLiteral("due to:") << reason;
                Q_EMIT mobileDataActiveChanged();
            });

    // we need to initialize it after m_mm3gppDevice has been set
    m_details = new ModemDetails(this, this);
}

ModemDetails *Modem::modemDetails()
{
    return m_details;
}

QString Modem::displayId()
{
    // in the form /org/freedesktop/ModemManager1/Modem/0
    QStringList uniSplit = uni().split("/");
    return uniSplit.count() == 0 ? QStringLiteral("(empty)") : QString(uniSplit[uniSplit.size() - 1]);
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
    qDebug() << QStringLiteral("Resetting the modem...");
    QDBusPendingReply<void> reply = m_mmInterface->reset();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << QStringLiteral("Error resetting the modem:") << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, i18n("Error resetting the modem: %1", reply.error().message()));
    }
}

void Modem::setEnabled(bool enabled)
{
    if (enabled != m_mmInterface->isEnabled()) {
        qDebug() << (enabled ? QStringLiteral("Enabling") : QStringLiteral("Disabling")) << QStringLiteral("the modem...");
        QDBusPendingReply<void> reply = m_mmInterface->setEnabled(enabled);
        reply.waitForFinished();
        if (reply.isError()) {
            qDebug() << QStringLiteral("Error setting the enabled state of the modem: ") << reply.error().message();
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
                qWarning() << QStringLiteral("Error updating connection settings for") << connection->uuid() << QStringLiteral(":") << reply.error().message()
                           << QStringLiteral(".");
                CellularNetworkSettings::instance()->addMessage(
                    InlineMessage::Error,
                    i18n("Error updating connection settings for %1: %2.", connection->uuid(), reply.error().message()));
            } else {
                qDebug() << QStringLiteral("Successfully updated connection settings") << connection->uuid() << QStringLiteral(".");
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
            qWarning() << QStringLiteral("Error disconnecting modem interface:") << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, i18n("Error disconnecting modem interface: %1", reply.error().message()));
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
    qDebug() << QStringLiteral("Activating profile on modem") << m_nmDevice->uni() << QStringLiteral("for connection") << connectionUni << ".";

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
        qDebug() << QStringLiteral("Connection") << connectionUni << QStringLiteral("not found.");
        return;
    }
    
    // activate connection manually
    // despite the documentation saying otherwise, activateConnection seems to need the DBus path, not uuid of the connection
    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::activateConnection(con->path(), m_nmDevice->uni(), "");
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << QStringLiteral("Error activating connection:") << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, i18n("Error activating connection: %1", reply.error().message()));
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
    gsmSetting->setPasswordFlags(password.isEmpty() ? NetworkManager::Setting::NotRequired : NetworkManager::Setting::AgentOwned);
    gsmSetting->setNetworkType(ProfileSettings::networkTypeFlag(networkType));
    gsmSetting->setHomeOnly(!isRoaming());

    gsmSetting->setInitialized(true);

    QDBusPendingReply<QDBusObjectPath> reply = NetworkManager::addAndActivateConnection(settings->toMap(), m_nmDevice->uni(), "");
    reply.waitForFinished();
    if (reply.isError()) {
        qWarning() << QStringLiteral("Error adding connection:") << reply.error().message();
        CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, i18n("Error adding connection: %1", reply.error().message()));
    } else {
        qDebug() << QStringLiteral("Successfully added a new connection") << name << QStringLiteral("with APN") << apn << ".";
    }
}

void Modem::removeProfile(const QString &connectionUni)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connectionUni);
    if (con) {
        QDBusPendingReply reply = con->remove();
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << QStringLiteral("Error removing connection") << reply.error().message();
            CellularNetworkSettings::instance()->addMessage(InlineMessage::Error, i18n("Error removing connection: %1", reply.error().message()));
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

            gsmSetting->setInitialized(true);

            QDBusPendingReply reply = con->update(conSettings->toMap());
            reply.waitForFinished();
            if (reply.isError()) {
                qWarning() << QStringLiteral("Error updating connection settings for") << connectionUni << QStringLiteral(":") << reply.error().message()
                           << QStringLiteral(".");
                CellularNetworkSettings::instance()->addMessage(InlineMessage::Error,
                                                                i18n("Error updating connection settings for %1: %2.", connectionUni, reply.error().message()));
            } else {
                qDebug() << QStringLiteral("Successfully updated connection settings") << connectionUni << QStringLiteral(".");
            }
            
        } else {
            qWarning() << QStringLiteral("Could not find connection settings for") << connectionUni << QStringLiteral("to update!");
        }
    } else {
        qWarning() << QStringLiteral("Could not find connection") << connectionUni << QStringLiteral("to update!");
    }
}

void Modem::addDetectedProfileSettings()
{
    bool found = false;
    
    if (m_mmDevice && hasSim()) {
        if (m_mm3gppDevice) {
            QString operatorCode = m_mm3gppDevice->operatorCode();
            qWarning() << QStringLiteral("Detecting profile settings. Using MCCMNC:") << operatorCode;

            // lookup apns with mccmnc codes
            for (QString &provider : m_providers->getProvidersFromMCCMNC(operatorCode)) {
                qWarning() << QStringLiteral("Provider:") << provider;

                for (auto apn : m_providers->getApns(provider)) {
                    QVariantMap apnInfo = m_providers->getApnInfo(apn);
                    qWarning() << QStringLiteral("Found gsm profile settings. Type:") << apnInfo[QStringLiteral("usageType")];

                    // only add mobile data apns (not mms)
                    if (apnInfo[QStringLiteral("usageType")].toString() == QStringLiteral("internet")) {
                        found = true;

                        QString name = provider;
                        if (!apnInfo[QStringLiteral("name")].isNull()) {
                            name += " - " + apnInfo[QStringLiteral("name")].toString();
                        }

                        addProfile(name,
                                   apn,
                                   apnInfo[QStringLiteral("username")].toString(),
                                   apnInfo[QStringLiteral("password")].toString(),
                                   QStringLiteral("4G/3G/2G"));
                    }

                    // TODO in the future for MMS settings, add else if here for == "mms"
                }
            }
        }
    }
    if (!found) {
        qDebug() << QStringLiteral("No profiles were found. :(");
        Q_EMIT couldNotAutodetectSettings();
    }
}

QList<Sim *> Modem::sims()
{
    if (m_mmDevice->sim()) {
        return {new Sim{this, this, m_mmDevice->sim(), m_mmInterface, m_mm3gppDevice}};
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
    if (state == NetworkManager::Device::State::UnknownState)
        return i18n("Unknown");
    else if (state == NetworkManager::Device::State::Unmanaged)
        return i18n("Unmanaged");
    else if (state == NetworkManager::Device::State::Unavailable)
        return i18n("Unavailable");
    else if (state == NetworkManager::Device::State::Disconnected)
        return i18n("Disconnected");
    else if (state == NetworkManager::Device::State::Preparing)
        return i18n("Preparing");
    else if (state == NetworkManager::Device::State::ConfiguringHardware)
        return i18n("ConfiguringHardware");
    else if (state == NetworkManager::Device::State::NeedAuth)
        return i18n("NeedAuth");
    else if (state == NetworkManager::Device::State::ConfiguringIp)
        return i18n("ConfiguringIp");
    else if (state == NetworkManager::Device::State::CheckingIp)
        return i18n("CheckingIp");
    else if (state == NetworkManager::Device::State::WaitingForSecondaries)
        return i18n("WaitingForSecondaries");
    else if (state == NetworkManager::Device::State::Activated)
        return i18n("Activated");
    else if (state == NetworkManager::Device::State::Deactivating)
        return i18n("Deactivating");
    else if (state == NetworkManager::Device::State::Failed)
        return i18n("Failed");
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
        return QStringLiteral("Any");
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::GprsEdgeOnly) {
        return QStringLiteral("Only 2G");
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Only3G) {
        return QStringLiteral("Only 3G");
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Only4GLte) {
        return QStringLiteral("Only 4G");
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer2G) {
        return QStringLiteral("2G");
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer3G) {
        return QStringLiteral("3G/2G");
    } else if (networkType == NetworkManager::GsmSetting::NetworkType::Prefer4GLte) {
        return QStringLiteral("4G/3G/2G");
    }
    return QStringLiteral("Any");
}

NetworkManager::GsmSetting::NetworkType ProfileSettings::networkTypeFlag(const QString &networkType)
{
    if (networkType == QStringLiteral("Any")) {
        return NetworkManager::GsmSetting::NetworkType::Any;
    } else if (networkType == QStringLiteral("Only 2G")) {
        return NetworkManager::GsmSetting::NetworkType::GprsEdgeOnly;
    } else if (networkType == QStringLiteral("Only 3G")) {
        return NetworkManager::GsmSetting::NetworkType::Only3G;
    } else if (networkType == QStringLiteral("Only 4G")) {
        return NetworkManager::GsmSetting::NetworkType::Only4GLte;
    } else if (networkType == QStringLiteral("2G")) {
        return NetworkManager::GsmSetting::NetworkType::Prefer2G;
    } else if (networkType == QStringLiteral("3G/2G")) {
        return NetworkManager::GsmSetting::NetworkType::Prefer3G;
    } else if (networkType == QStringLiteral("4G/3G/2G")) {
        return NetworkManager::GsmSetting::NetworkType::Prefer4GLte;
    }
    return NetworkManager::GsmSetting::NetworkType::Any;
}
