/*
    SPDX-FileCopyrightText: 2021-2026 Devin Lin <devin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "cellularmodemdetails.h"
#include "cellularmodem.h"
#include "plasma_nm_cellular.h"

#include <KLocalizedString>
#include <QDBusPendingCallWatcher>

using namespace Qt::Literals::StringLiterals;

CellularModemDetails::CellularModemDetails(QObject *parent, CellularModem *modem)
    : QObject{parent}
    , m_modem{modem}
    , m_scanNetworkWatcher{nullptr}
    , m_isScanningNetworks{false}
    , m_cachedScannedNetworks{}
{
    auto mmInterfacePointer = m_modem->mmModemInterface().data();
    connect(mmInterfacePointer, &ModemManager::Modem::accessTechnologiesChanged, this, [this]() {
        Q_EMIT accessTechnologiesChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::deviceChanged, this, [this]() {
        Q_EMIT deviceChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::deviceIdentifierChanged, this, [this]() {
        Q_EMIT deviceIdentifierChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::driversChanged, this, [this]() {
        Q_EMIT driversChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::equipmentIdentifierChanged, this, [this]() {
        Q_EMIT equipmentIdentifierChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::manufacturerChanged, this, [this]() {
        Q_EMIT manufacturerChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::modelChanged, this, [this]() {
        Q_EMIT modelChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::ownNumbersChanged, this, [this]() {
        Q_EMIT ownNumbersChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::pluginChanged, this, [this]() {
        Q_EMIT pluginChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::powerStateChanged, this, [this]() {
        Q_EMIT powerStateChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::revisionChanged, this, [this]() {
        Q_EMIT revisionChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::signalQualityChanged, this, [this]() {
        Q_EMIT signalQualityChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::simPathChanged, this, [this]() {
        Q_EMIT simPathChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::stateChanged, this, [this]() {
        Q_EMIT stateChanged();
    });
    connect(mmInterfacePointer, &ModemManager::Modem::stateFailedReasonChanged, this, [this]() {
        Q_EMIT stateFailedReasonChanged();
    });

    auto mm3gpp = m_modem->mm3gppDevice();
    if (mm3gpp) {
        connect(mm3gpp.data(), &ModemManager::Modem3gpp::operatorCodeChanged, this, [this]() {
            Q_EMIT operatorCodeChanged();
        });
        connect(mm3gpp.data(), &ModemManager::Modem3gpp::operatorNameChanged, this, [this]() {
            Q_EMIT operatorNameChanged();
        });
        connect(mm3gpp.data(), &ModemManager::Modem3gpp::registrationStateChanged, this, [this]() {
            Q_EMIT registrationStateChanged();
        });
    } else {
        qCWarning(PLASMA_NM_CELLULAR_LOG) << "3gpp device not found!";
    }

    // m_modem->nmModemDevice() may be nullptr, listen for updates
    connect(m_modem, &CellularModem::nmModemChanged, this, &CellularModemDetails::updateNMSignals);
    updateNMSignals();
}

void CellularModemDetails::updateNMSignals()
{
    auto nmModem = m_modem->nmModemDevice();
    if (nmModem) {
        connect(nmModem.data(), &NetworkManager::ModemDevice::firmwareVersionChanged, this, [this]() {
            Q_EMIT firmwareVersionChanged();
        });
        connect(nmModem.data(), &NetworkManager::ModemDevice::interfaceNameChanged, this, [this]() {
            Q_EMIT interfaceNameChanged();
        });
        connect(nmModem.data(), &NetworkManager::ModemDevice::meteredChanged, this, [this]() {
            Q_EMIT meteredChanged();
        });
    }
}

QStringList CellularModemDetails::accessTechnologies()
{
    QStringList list;
    auto flags = m_modem->mmModemInterface()->accessTechnologies();
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN) {
        list.push_back(i18n("Unknown"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_POTS) {
        list.push_back(i18n("POTS"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_GSM) {
        list.push_back(i18n("GSM"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT) {
        list.push_back(i18n("GSM Compact"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_GPRS) {
        list.push_back(i18n("GPRS"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_EDGE) {
        list.push_back(i18n("EDGE"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_UMTS) {
        list.push_back(i18n("UMTS"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_HSDPA) {
        list.push_back(i18n("HSDPA"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_HSUPA) {
        list.push_back(i18n("HSUPA"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_HSPA) {
        list.push_back(i18n("HSPA"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS) {
        list.push_back(i18n("HSPA+"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_1XRTT) {
        list.push_back(i18n("CDMA2000 1xRTT"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_EVDO0) {
        list.push_back(i18n("CDMA2000 EVDO-0"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_EVDOA) {
        list.push_back(i18n("CDMA2000 EVDO-A"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_EVDOB) {
        list.push_back(i18n("CDMA2000 EVDO-B"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_LTE) {
        list.push_back(i18n("LTE"));
    }
    if (flags & MM_MODEM_ACCESS_TECHNOLOGY_5GNR) {
        list.push_back(i18n("5GNR"));
    }
    return list;
}

QString CellularModemDetails::device()
{
    return m_modem->mmModemInterface()->device();
}

QString CellularModemDetails::deviceIdentifier()
{
    return m_modem->mmModemInterface()->deviceIdentifier();
}

QStringList CellularModemDetails::drivers()
{
    return m_modem->mmModemInterface()->drivers();
}

QString CellularModemDetails::equipmentIdentifier()
{
    return m_modem->mmModemInterface()->equipmentIdentifier();
}

bool CellularModemDetails::isEnabled()
{
    return m_modem->mmModemInterface()->isEnabled();
}

QString CellularModemDetails::manufacturer()
{
    return m_modem->mmModemInterface()->manufacturer();
}

QString CellularModemDetails::model()
{
    return m_modem->mmModemInterface()->model();
}

QStringList CellularModemDetails::ownNumbers()
{
    return m_modem->mmModemInterface()->ownNumbers();
}

QString CellularModemDetails::plugin()
{
    return m_modem->mmModemInterface()->plugin();
}

QString CellularModemDetails::powerState()
{
    switch (m_modem->mmModemInterface()->powerState()) {
    case MM_MODEM_POWER_STATE_UNKNOWN:
        return i18n("Unknown");
    case MM_MODEM_POWER_STATE_OFF:
        return i18n("Off");
    case MM_MODEM_POWER_STATE_LOW:
        return i18n("Low-power mode");
    case MM_MODEM_POWER_STATE_ON:
        return i18n("Full power mode");
    }
    return {};
}

QString CellularModemDetails::revision()
{
    return m_modem->mmModemInterface()->revision();
}

uint CellularModemDetails::signalQuality()
{
    return m_modem->mmModemInterface()->signalQuality().signal;
}

QString CellularModemDetails::simPath()
{
    return m_modem->mmModemInterface()->simPath();
}

QString CellularModemDetails::state()
{
    switch (m_modem->mmModemInterface()->state()) {
    case MM_MODEM_STATE_FAILED:
        return i18n("Failed");
    case MM_MODEM_STATE_UNKNOWN:
        return i18n("Unknown");
    case MM_MODEM_STATE_INITIALIZING:
        return i18n("Initializing");
    case MM_MODEM_STATE_LOCKED:
        return i18n("Locked");
    case MM_MODEM_STATE_DISABLED:
        return i18n("Disabled");
    case MM_MODEM_STATE_DISABLING:
        return i18n("Disabling");
    case MM_MODEM_STATE_ENABLING:
        return i18n("Enabling");
    case MM_MODEM_STATE_ENABLED:
        return i18n("Enabled");
    case MM_MODEM_STATE_SEARCHING:
        return i18n("Searching for network provider");
    case MM_MODEM_STATE_REGISTERED:
        return i18n("Registered with network provider");
    case MM_MODEM_STATE_DISCONNECTING:
        return i18n("Disconnecting");
    case MM_MODEM_STATE_CONNECTING:
        return i18n("Connecting");
    case MM_MODEM_STATE_CONNECTED:
        return i18n("Connected");
    }
    return {};
}

QString CellularModemDetails::stateFailedReason()
{
    switch (m_modem->mmModemInterface()->stateFailedReason()) {
    case MM_MODEM_STATE_FAILED_REASON_NONE:
        return i18n("No error.");
    case MM_MODEM_STATE_FAILED_REASON_UNKNOWN:
        return i18n("Unknown error.");
    case MM_MODEM_STATE_FAILED_REASON_SIM_MISSING:
        return i18n("SIM is required but missing.");
    case MM_MODEM_STATE_FAILED_REASON_SIM_ERROR:
        return i18n("SIM is available but unusable.");
    case MM_MODEM_STATE_FAILED_REASON_UNKNOWN_CAPABILITIES:
        return i18n("Unknown modem capabilities.");
    case MM_MODEM_STATE_FAILED_REASON_ESIM_WITHOUT_PROFILES:
        return i18n("eSIM is not initialized.");
    }
    return {};
}

QString CellularModemDetails::operatorCode()
{
    auto mm3gpp = m_modem->mm3gppDevice();
    return mm3gpp ? mm3gpp->operatorCode() : QString{};
}

QString CellularModemDetails::operatorName()
{
    auto mm3gpp = m_modem->mm3gppDevice();
    return mm3gpp ? mm3gpp->operatorName() : QString{};
}

QString CellularModemDetails::registrationState()
{
    auto mm3gpp = m_modem->mm3gppDevice();
    if (!mm3gpp) {
        return QString{};
    }

    switch (mm3gpp->registrationState()) {
    case MM_MODEM_3GPP_REGISTRATION_STATE_IDLE:
        return i18n("Not registered, not searching for new operator to register.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_HOME:
        return i18n("Registered on home network.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_SEARCHING:
        return i18n("Not registered, searching for new operator to register with.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_DENIED:
        return i18n("Registration denied.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_UNKNOWN:
        return i18n("Unknown registration status.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING:
        return i18n("Registered on a roaming network.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_HOME_SMS_ONLY:
        return i18n("Registered for \"SMS only\", on home network.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_SMS_ONLY:
        return i18n("Registered for \"SMS only\", roaming network.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_EMERGENCY_ONLY:
        return i18n("Emergency services only.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_HOME_CSFB_NOT_PREFERRED:
        return i18n("Registered for \"CSFB not preferred\", home network.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_ROAMING_CSFB_NOT_PREFERRED:
        return i18n("Registered for \"CSFB not preferred\", roaming network.");
    case MM_MODEM_3GPP_REGISTRATION_STATE_ATTACHED_RLOS:
        return i18n("Attached for access to Restricted Local Operator Services.");
    }
    return {};
}

Q_DECLARE_METATYPE(MMModem3gppNetworkAvailability)
Q_DECLARE_METATYPE(MMModemAccessTechnology)

QList<CellularAvailableNetwork *> CellularModemDetails::networks()
{
    return m_cachedScannedNetworks;
}

void CellularModemDetails::scanNetworks()
{
    for (auto p : m_cachedScannedNetworks) {
        p->deleteLater();
    }
    m_cachedScannedNetworks.clear();

    auto mm3gpp = m_modem->mm3gppDevice();
    if (mm3gpp) {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Scanning for available networks...";

        QDBusPendingReply<ModemManager::QVariantMapList> reply = mm3gpp->scan();

        m_isScanningNetworks = true;
        Q_EMIT isScanningNetworksChanged();
        m_scanNetworkWatcher = new QDBusPendingCallWatcher(reply, this);
        connect(m_scanNetworkWatcher, &QDBusPendingCallWatcher::finished, this, &CellularModemDetails::scanNetworksFinished);
    }

    Q_EMIT networksChanged();
}

void CellularModemDetails::scanNetworksFinished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<ModemManager::QVariantMapList> reply = *call;
    if (reply.isError()) {
        qCDebug(PLASMA_NM_CELLULAR_LOG) << "Scanning failed:" << reply.error().message();
        Q_EMIT errorOccurred(i18n("Scanning networks failed: %1", reply.error().message()));
    } else {
        auto mm3gpp = m_modem->mm3gppDevice();
        ModemManager::QVariantMapList list = reply.value();

        for (auto &var : list) {
            auto status = var[u"status"_s].value<MMModem3gppNetworkAvailability>();

            if (status == MM_MODEM_3GPP_NETWORK_AVAILABILITY_CURRENT || status == MM_MODEM_3GPP_NETWORK_AVAILABILITY_AVAILABLE) {
                auto network = new CellularAvailableNetwork{this,
                                                            mm3gpp,
                                                            status == MM_MODEM_3GPP_NETWORK_AVAILABILITY_CURRENT,
                                                            var[u"operator-long"_s].toString(),
                                                            var[u"operator-short"_s].toString(),
                                                            var[u"operator-code"_s].toString(),
                                                            var[u"access-technology"_s].value<MMModemAccessTechnology>()};
                m_cachedScannedNetworks.push_back(network);
            }
        }
    }
    m_isScanningNetworks = false;
    Q_EMIT networksChanged();
    Q_EMIT isScanningNetworksChanged();

    call->deleteLater();
}

bool CellularModemDetails::isScanningNetworks()
{
    return m_isScanningNetworks;
}

QString CellularModemDetails::firmwareVersion()
{
    auto nmModem = m_modem->nmModemDevice();
    if (!nmModem) {
        return QString{};
    }
    return nmModem->firmwareVersion();
}

QString CellularModemDetails::interfaceName()
{
    auto nmModem = m_modem->nmModemDevice();
    if (!nmModem) {
        return QString{};
    }
    return nmModem->interfaceName();
}

QString CellularModemDetails::metered()
{
    auto nmModem = m_modem->nmModemDevice();
    if (!nmModem) {
        return QString{};
    }

    switch (nmModem->metered()) {
    case NetworkManager::Device::MeteredStatus::UnknownStatus:
        return i18n("Unknown");
    case NetworkManager::Device::MeteredStatus::Yes:
        return i18n("Yes");
    case NetworkManager::Device::MeteredStatus::No:
        return i18n("No");
    case NetworkManager::Device::MeteredStatus::GuessYes:
        return i18n("GuessYes");
    case NetworkManager::Device::MeteredStatus::GuessNo:
        return i18n("GuessNo");
    }
    return QString{};
}

// CellularAvailableNetwork implementation

CellularAvailableNetwork::CellularAvailableNetwork(QObject *parent,
                                                   ModemManager::Modem3gpp::Ptr mm3gppDevice,
                                                   bool isCurrentlyUsed,
                                                   QString operatorLong,
                                                   QString operatorShort,
                                                   QString operatorCode,
                                                   MMModemAccessTechnology accessTechnology)
    : QObject{parent}
    , m_isCurrentlyUsed{isCurrentlyUsed}
    , m_operatorLong{operatorLong}
    , m_operatorShort{operatorShort}
    , m_operatorCode{operatorCode}
    , m_accessTechnology{}
    , m_mm3gppDevice{mm3gppDevice}
{
    switch (accessTechnology) {
    case MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN:
        m_accessTechnology = i18n("Unknown");
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_POTS:
        m_accessTechnology = i18n("POTS");
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_GSM:
    case MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT:
    case MM_MODEM_ACCESS_TECHNOLOGY_GPRS:
    case MM_MODEM_ACCESS_TECHNOLOGY_EDGE:
        m_accessTechnology = i18n("2G");
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_UMTS:
    case MM_MODEM_ACCESS_TECHNOLOGY_HSDPA:
    case MM_MODEM_ACCESS_TECHNOLOGY_HSUPA:
    case MM_MODEM_ACCESS_TECHNOLOGY_HSPA:
    case MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS:
    case MM_MODEM_ACCESS_TECHNOLOGY_1XRTT:
    case MM_MODEM_ACCESS_TECHNOLOGY_EVDO0:
    case MM_MODEM_ACCESS_TECHNOLOGY_EVDOA:
    case MM_MODEM_ACCESS_TECHNOLOGY_EVDOB:
        m_accessTechnology = i18n("3G");
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_LTE:
    case MM_MODEM_ACCESS_TECHNOLOGY_LTE_CAT_M:
    case MM_MODEM_ACCESS_TECHNOLOGY_LTE_NB_IOT:
        m_accessTechnology = i18n("4G");
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_5GNR:
        m_accessTechnology = i18n("5G");
        break;
    case MM_MODEM_ACCESS_TECHNOLOGY_ANY:
        m_accessTechnology = i18n("Any");
        break;
    }
}

bool CellularAvailableNetwork::isCurrentlyUsed()
{
    return m_isCurrentlyUsed;
}

QString CellularAvailableNetwork::operatorLong()
{
    return m_operatorLong;
}

QString CellularAvailableNetwork::operatorShort()
{
    return m_operatorShort;
}

QString CellularAvailableNetwork::operatorCode()
{
    return m_operatorCode;
}

QString CellularAvailableNetwork::accessTechnology()
{
    return m_accessTechnology;
}

void CellularAvailableNetwork::registerToNetwork()
{
    if (!m_isCurrentlyUsed && m_mm3gppDevice) {
        m_mm3gppDevice->registerToNetwork(m_operatorCode);
    }
}
