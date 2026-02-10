/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "cellularmodem.h"
#include "cellularconnectionprofile.h"
#include "cellularmodemdetails.h"
#include "cellularsim.h"

#include "plasma_nm_cellular.h"

#include <mobileproviders.h>

#include <KLocalizedString>
#include <KUser>
#include <NetworkManagerQt/Settings>
#include <QDBusReply>

using namespace Qt::Literals::StringLiterals;

CellularModem::CellularModem(QObject *parent)
    : QObject{parent}
{
}

CellularModem::CellularModem(QObject *parent, ModemManager::ModemDevice::Ptr mmModem, ModemManager::Modem::Ptr mmInterface)
    : QObject{parent}
    , m_mmModem{mmModem}
    , m_nmModem{nullptr}
    , m_mmInterface{mmInterface}
{
    // Create the connection profile model
    m_profiles = new CellularConnectionProfile(this);

    // Rebuild SIM list when SIMs are added/removed
    connect(m_mmModem.data(), &ModemManager::ModemDevice::simAdded, this, &CellularModem::refreshSims);
    connect(m_mmModem.data(), &ModemManager::ModemDevice::simRemoved, this, &CellularModem::refreshSims);

    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionAdded, this, &CellularModem::mobileDataEnabledChanged);
    connect(NetworkManager::settingsNotifier(), &NetworkManager::SettingsNotifier::connectionRemoved, this, &CellularModem::mobileDataEnabledChanged);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::activeConnectionAdded, this, &CellularModem::mobileDataEnabledChanged);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::activeConnectionRemoved, this, &CellularModem::mobileDataEnabledChanged);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceAdded, this, &CellularModem::findNetworkManagerDevice);
    connect(NetworkManager::notifier(), &NetworkManager::Notifier::deviceRemoved, this, &CellularModem::findNetworkManagerDevice);

    // this is guaranteed to be a GSM modem
    m_mm3gppDevice = m_mmModem->interface(ModemManager::ModemDevice::GsmInterface).objectCast<ModemManager::Modem3gpp>();

    // if no sim is inserted, m_mm3gppDevice is nullptr
    if (m_mm3gppDevice) {
        m_mm3gppDevice->setTimeout(60000); // scanning networks likely takes longer than the default timeout

        // Signal strength and operator name from 3GPP
        connect(m_mm3gppDevice.data(), &ModemManager::Modem3gpp::operatorNameChanged, this, &CellularModem::operatorNameChanged);
    }

    // Signal strength from modem interface
    connect(m_mmInterface.data(), &ModemManager::Modem::signalQualityChanged, this, &CellularModem::signalStrengthChanged);

    // SIM lock status
    connect(m_mmInterface.data(), &ModemManager::Modem::unlockRequiredChanged, this, &CellularModem::simLockedChanged);

    // Build initial SIM list (must be after m_mm3gppDevice is set)
    refreshSims();

    // find networkmanager modem, if it exists
    findNetworkManagerDevice();

    // we need to initialize it after m_mm3gppDevice has been set
    m_details = new CellularModemDetails(this, this);

    // Forward details error signals
    connect(m_details, &CellularModemDetails::errorOccurred, this, &CellularModem::addError);
}

void CellularModem::findNetworkManagerDevice()
{
    m_nmModem = nullptr;

    for (NetworkManager::Device::Ptr nmDevice : NetworkManager::networkInterfaces()) {
        if (nmDevice->udi() == m_mmModem->uni()) {
            m_nmModem = nmDevice.objectCast<NetworkManager::ModemDevice>();
        }
    }

    if (m_nmModem) {
        connect(m_nmModem.data(), &NetworkManager::Device::autoconnectChanged, this, &CellularModem::mobileDataEnabledChanged);
        connect(m_nmModem.data(), &NetworkManager::Device::stateChanged, this, &CellularModem::mobileDataEnabledChanged);
        connect(m_nmModem.data(), &NetworkManager::Device::availableConnectionAppeared, this, &CellularModem::mobileDataEnabledChanged);
        connect(m_nmModem.data(), &NetworkManager::Device::availableConnectionDisappeared, this, &CellularModem::mobileDataEnabledChanged);

        connect(m_nmModem.data(), &NetworkManager::ModemDevice::availableConnectionChanged, this, [this]() {
            refreshProfiles();
        });
        connect(m_nmModem.data(), &NetworkManager::ModemDevice::activeConnectionChanged, this, [this]() {
            refreshProfiles();
            Q_EMIT activeConnectionUniChanged();
        });

        m_profiles->setNmModem(m_nmModem);
        refreshProfiles();
    }

    Q_EMIT nmModemChanged();
    Q_EMIT mobileDataEnabledChanged();
    Q_EMIT mobileDataSupportedChanged();
}

CellularModemDetails *CellularModem::modemDetails() const
{
    return m_details;
}

CellularConnectionProfile *CellularModem::profiles() const
{
    return m_profiles;
}

QString CellularModem::displayId() const
{
    QStringList uniSplit = uni().split(u"/"_s);
    return uniSplit.count() == 0 ? u"(empty)"_s : QString(uniSplit[uniSplit.size() - 1]);
}

QString CellularModem::uni() const
{
    return m_mmInterface->uni();
}

QString CellularModem::activeConnectionUni() const
{
    if (m_nmModem && m_nmModem->activeConnection() && m_nmModem->activeConnection()->connection()) {
        return m_nmModem->activeConnection()->connection()->uuid();
    }
    return QString();
}

QCoro::Task<void> CellularModem::reset()
{
    qCDebug(PLASMA_NM_CELLULAR_LOG) << "Resetting the modem...";

    QDBusReply<void> reply = co_await m_mmInterface->reset();

    if (!reply.isValid()) {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Error resetting the modem:" << reply.error().message();
        addError(i18n("Error resetting the modem: %1", reply.error().message()));
    }
}

bool CellularModem::mobileDataSupported() const
{
    return m_nmModem && hasSim();
}

bool CellularModem::needsAPNAdded() const
{
    return m_nmModem && mobileDataSupported() && m_nmModem->availableConnections().count() == 0;
}

bool CellularModem::mobileDataEnabled() const
{
    // if wwan is globally disabled
    if (!NetworkManager::isWwanEnabled()) {
        return false;
    }

    // no modem -> no mobile data -> report disabled
    if (!m_nmModem) {
        return false;
    }

    // mobile data already activated -> report enabled
    if (m_nmModem->state() == NetworkManager::Device::Activated) {
        return true;
    }

    // autoconnect disabled on the entire modem -> report disabled
    if (!m_nmModem->autoconnect()) {
        return false;
    }

    // at least one connection set to autoconnect -> report enabled
    for (NetworkManager::Connection::Ptr con : m_nmModem->availableConnections()) {
        if (con->settings()->autoconnect()) {
            return true;
        }
    }

    // modem, but no connection set to autoconnect -> report disabled
    return false;
}

void CellularModem::setMobileDataEnabled(bool enabled)
{
    // ensure that wwan is on
    if (enabled && !NetworkManager::isWwanEnabled()) {
        NetworkManager::setWwanEnabled(true);
    }

    if (!m_nmModem) {
        return;
    }

    if (enabled) {
        // enable mobile data...
        m_nmModem->setAutoconnect(true);

        // activate the connection that was last used
        QDateTime latestTimestamp;
        NetworkManager::Connection::Ptr latestCon;
        for (NetworkManager::Connection::Ptr con : m_nmModem->availableConnections()) {
            QDateTime timestamp = con->settings()->timestamp();
            // if con was not used yet, skip it, otherwise:
            // if we have no latestTimestamp yet, con is the latest
            // otherwise, compare the timestamps
            // in case of a tie, use the first connection that was found
            if (!timestamp.isNull() && (latestTimestamp.isNull() || timestamp > latestTimestamp)) {
                latestTimestamp = timestamp;
                latestCon = con;
            }
        }

        // if we found the last used connection, set it to autoconnect and connect it immediately
        if (!latestCon.isNull()) {
            latestCon->settings()->setAutoconnect(true);
            latestCon->update(latestCon->settings()->toMap());
            NetworkManager::activateConnection(latestCon->path(), m_nmModem->uni(), QString());
        }

    } else {
        // disable mobile data...

        // we do not call NetworkManager::setWwanEnabled(false), because it turns off cellular
        m_nmModem->setAutoconnect(false);
        // we need to also set all connections to not autoconnect (#182)
        for (NetworkManager::Connection::Ptr con : m_nmModem->availableConnections()) {
            con->settings()->setAutoconnect(false);
            con->update(con->settings()->toMap());
        }
        // disconnect network
        m_nmModem->disconnectInterface();
    }
}

bool CellularModem::hasSim() const
{
    if (!m_mmModem) {
        return false;
    }
    return m_mmModem && m_mmModem->sim() && m_mmModem->sim()->uni() != u"/"_s;
}

bool CellularModem::simLocked() const
{
    return m_mmInterface && m_mmInterface->unlockRequired() == MM_MODEM_LOCK_SIM_PIN;
}

bool CellularModem::simEmpty() const
{
    return !hasSim();
}

int CellularModem::signalStrength() const
{
    return m_mmInterface ? m_mmInterface->signalQuality().signal : 0;
}

QString CellularModem::operatorName() const
{
    return m_mm3gppDevice ? m_mm3gppDevice->operatorName() : QString{};
}

QStringList CellularModem::errors() const
{
    return m_errors;
}

void CellularModem::clearErrors()
{
    m_errors.clear();
    Q_EMIT errorsChanged();
}

void CellularModem::addError(const QString &message)
{
    m_errors.push_back(message);
    Q_EMIT errorsChanged();
    Q_EMIT errorOccurred(message);
}

void CellularModem::refreshSims()
{
    qDeleteAll(m_sims);
    m_sims.clear();

    if (!m_mmModem) {
        Q_EMIT simsChanged();
        Q_EMIT hasSimChanged();
        Q_EMIT simEmptyChanged();
        return;
    }

    // TODO: When ModemManagerQt exposes sims(), iterate over all SIM slots:
    //
    // const auto simSlots = m_mmModem->sims();
    // for (const auto &sim : simSlots) {
    //     if (sim) {
    //         m_sims.append(new CellularSim{this, this, sim, m_mmInterface, m_mm3gppDevice});
    //     }
    // }

    if (m_mmModem->sim()) {
        m_sims.append(new CellularSim{this, this, m_mmModem->sim(), m_mmInterface, m_mm3gppDevice});
    }

    // Connect error signals from all SIMs
    for (auto sim : m_sims) {
        connect(sim, &CellularSim::errorOccurred, this, &CellularModem::addError);
    }

    Q_EMIT simsChanged();
    Q_EMIT hasSimChanged();
    Q_EMIT simEmptyChanged();
}

void CellularModem::refreshProfiles()
{
    m_profiles->refreshProfiles();
    Q_EMIT profilesChanged();
}

QCoro::Task<void> CellularModem::activateProfile(const QString &connectionUni)
{
    if (!m_nmModem) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Cannot activate profile since there is no NetworkManager modem";
        co_return;
    }

    qCDebug(PLASMA_NM_CELLULAR_LOG) << "Activating profile on modem" << m_nmModem->uni() << "for connection" << connectionUni;

    NetworkManager::Connection::Ptr con;

    // activate connection manually, disable autoconnect for all other connections
    for (auto connection : m_nmModem->availableConnections()) {
        if (connection->uuid() == connectionUni) {
            connection->settings()->setAutoconnect(true);
            con = connection;
        } else {
            connection->settings()->setAutoconnect(false);
        }
    }

    if (!con) {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Connection" << connectionUni << "not found.";
        co_return;
    }

    // despite the documentation saying otherwise, activateConnection seems to need the DBus path, not uuid of the connection
    QDBusReply<QDBusObjectPath> reply = co_await NetworkManager::activateConnection(con->path(), m_nmModem->uni(), QString());
    if (!reply.isValid()) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Error activating connection:" << reply.error().message();
        addError(i18n("Error activating connection: %1", reply.error().message()));
        co_return;
    }

    refreshProfiles();
    Q_EMIT activeConnectionUniChanged();
}

QCoro::Task<void> CellularModem::addProfile(QString name, QString apn, QString username, QString password, QString networkType)
{
    if (!m_nmModem) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Cannot add profile since there is no NetworkManager modem";
        co_return;
    }

    NetworkManager::ConnectionSettings::Ptr settings{new NetworkManager::ConnectionSettings(NetworkManager::ConnectionSettings::Gsm)};
    settings->setId(name);
    settings->setUuid(NetworkManager::ConnectionSettings::createNewUuid());
    settings->setAutoconnect(true);
    settings->addToPermissions(KUser().loginName(), QString());

    NetworkManager::GsmSetting::Ptr gsmSetting = settings->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
    gsmSetting->setApn(apn);
    gsmSetting->setUsername(username);
    gsmSetting->setPassword(password);
    gsmSetting->setPasswordFlags(password.isEmpty() ? NetworkManager::Setting::NotRequired : NetworkManager::Setting::AgentOwned);
    gsmSetting->setNetworkType(CellularConnectionProfile::networkTypeFlag(networkType));
    gsmSetting->setHomeOnly(true); // New profiles default to roaming disabled

    gsmSetting->setInitialized(true);

    NetworkManager::Ipv6Setting::Ptr ipv6Setting = settings->setting(NetworkManager::Setting::Ipv6).dynamicCast<NetworkManager::Ipv6Setting>();
    ipv6Setting->setMethod(NetworkManager::Ipv6Setting::ConfigMethod::Automatic);
    ipv6Setting->setInitialized(true);

    QDBusReply<QDBusObjectPath> reply = co_await NetworkManager::addAndActivateConnection(settings->toMap(), m_nmModem->uni(), QString());
    if (!reply.isValid()) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Error adding connection:" << reply.error().message();
        addError(i18n("Error adding connection: %1", reply.error().message()));
    } else {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Successfully added a new connection" << name << "with APN" << apn;
    }
}

QCoro::Task<void> CellularModem::removeProfile(const QString &connectionUni)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connectionUni);
    if (!con) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Could not find connection" << connectionUni << "to remove!";
        co_return;
    }

    QDBusReply<void> reply = co_await con->remove();
    if (!reply.isValid()) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Error removing connection:" << reply.error().message();
        addError(i18n("Error removing connection: %1", reply.error().message()));
    }
}

QCoro::Task<void> CellularModem::updateProfile(QString connectionUni, QString name, QString apn, QString username, QString password, QString networkType)
{
    NetworkManager::Connection::Ptr con = NetworkManager::findConnectionByUuid(connectionUni);
    if (!con) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Could not find connection" << connectionUni << "to update!";
        co_return;
    }

    NetworkManager::ConnectionSettings::Ptr conSettings = con->settings();
    if (!conSettings) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Could not find connection settings for" << connectionUni << "to update!";
        co_return;
    }

    conSettings->setId(name);

    NetworkManager::GsmSetting::Ptr gsmSetting = conSettings->setting(NetworkManager::Setting::Gsm).dynamicCast<NetworkManager::GsmSetting>();
    gsmSetting->setApn(apn);
    gsmSetting->setUsername(username);
    gsmSetting->setPassword(password);
    gsmSetting->setPasswordFlags(password.isEmpty() ? NetworkManager::Setting::NotRequired : NetworkManager::Setting::AgentOwned);
    gsmSetting->setNetworkType(CellularConnectionProfile::networkTypeFlag(networkType));
    // Note: roaming is not set here, it's managed per-profile via CellularConnectionProfile::setRoamingAllowed()

    gsmSetting->setInitialized(true);

    NetworkManager::Ipv6Setting::Ptr ipv6Setting = conSettings->setting(NetworkManager::Setting::Ipv6).dynamicCast<NetworkManager::Ipv6Setting>();
    ipv6Setting->setMethod(NetworkManager::Ipv6Setting::ConfigMethod::Automatic);
    ipv6Setting->setInitialized(true);

    QDBusReply<void> reply = co_await con->update(conSettings->toMap());
    if (!reply.isValid()) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Error updating connection settings for" << connectionUni << ":" << reply.error().message();
        addError(i18n("Error updating connection settings for %1: %2.", connectionUni, reply.error().message()));
    } else {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Successfully updated connection settings" << connectionUni;
    }
}

void CellularModem::addDetectedProfileSettings()
{
    if (!m_mmModem) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "ModemManager device missing, cannot detect profile settings";
        return;
    }

    if (!hasSim() || !m_mmModem->sim()) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "No SIM found, cannot detect profile settings";
        return;
    }

    if (!m_mm3gppDevice) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "3gpp object not found, cannot detect profile settings";
        return;
    }

    bool found = false;
    static MobileProviders mobileProviders{};

    QString operatorCode = m_mmModem->sim()->operatorIdentifier();
    qCWarning(PLASMA_NM_CELLULAR_LOG) << "Detecting profile settings. Using MCCMNC:" << operatorCode;

    // lookup apns with mccmnc codes
    for (QString &provider : mobileProviders.getProvidersFromMCCMNC(operatorCode)) {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "Provider:" << provider;

        for (auto apn : mobileProviders.getApns(provider)) {
            QVariantMap apnInfo = mobileProviders.getApnInfo(apn);
            qCWarning(PLASMA_NM_CELLULAR_LOG) << "Found gsm profile settings. Type:" << apnInfo[u"usageType"_s];

            // only add mobile data apns (not mms)
            // TODO in the future for MMS settings, add else if here for == "mms"
            if (apnInfo[u"usageType"_s].toString() == u"internet"_s) {
                found = true;

                QString name = provider;
                if (!apnInfo[u"name"_s].isNull()) {
                    name += u" - "_s + apnInfo[u"name"_s].toString();
                }

                addProfile(name, apn, apnInfo[u"username"_s].toString(), apnInfo[u"password"_s].toString(), u"4G/3G/2G"_s);
            }
        }
    }

    if (!found) {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "No profiles were found.";
        Q_EMIT couldNotAutodetectSettings();
    }
}

QList<CellularSim *> CellularModem::sims()
{
    return m_sims;
}

ModemManager::ModemDevice::Ptr CellularModem::mmModemDevice()
{
    return m_mmModem;
}

NetworkManager::ModemDevice::Ptr CellularModem::nmModemDevice()
{
    return m_nmModem;
}

ModemManager::Modem::Ptr CellularModem::mmModemInterface()
{
    return m_mmInterface;
}

ModemManager::Modem3gpp::Ptr CellularModem::mm3gppDevice()
{
    return m_mm3gppDevice;
}
