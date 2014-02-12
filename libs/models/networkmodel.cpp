/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "networkmodel.h"
#include "networkmodelitem.h"
#include "uiutils.h"

#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
#include <ModemManagerQt/manager.h>
#include <ModemManagerQt/modem.h>
#endif
#endif
#include <NetworkManagerQt/Settings>

NetworkModel::NetworkModel(QObject* parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[ConnectionDetailsRole] = "ConnectionDetails";
    roles[ConnectionIconRole] = "ConnectionIcon";
    roles[ConnectionPathRole] = "ConnectionPath";
    roles[ConnectionStateRole] = "ConnectionState";
    roles[DevicePathRole] = "DevicePath";
    roles[DeviceStateRole] = "DeviceState";
    roles[DownloadRole] = "Download";
    roles[ItemTypeRole] = "ItemType";
    roles[LastUsedRole] = "LastUsed";
    roles[LastUsedDateOnlyRole] = "LastUsedDateOnly";
    roles[NameRole] = "Name";
    roles[SectionRole] = "Section";
    roles[SignalRole] = "Signal";
    roles[SlaveRole] = "Slave";
    roles[SsidRole] = "Ssid";
    roles[SpecificPathRole] = "SpecificPath";
    roles[SpeedRole] = "Speed";
    roles[SecurityTypeRole] = "SecurityType";
    roles[SecurityTypeStringRole] = "SecurityTypeString";
    roles[TimeStampRole] = "TimeStamp";
    roles[TypeRole] = "Type";
    roles[UniRole] = "Uni";
    roles[UploadRole] = "Upload";
    roles[UuidRole] = "Uuid";
    roles[VpnState] = "VpnState";
    setRoleNames(roles);

    initialize();
}

NetworkModel::~NetworkModel()
{
}

QVariant NetworkModel::data(const QModelIndex& index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_list.count()) {
        NetworkModelItem * item = m_list.itemAt(row);

        switch (role) {
            case ConnectionDetailsRole:
                return item->details();
            case ConnectionIconRole:
                return item->icon();
            case ConnectionPathRole:
                return item->connectionPath();
            case ConnectionStateRole:
                return item->connectionState();
            case DevicePathRole:
                return item->devicePath();
            case DeviceStateRole:
                return item->deviceState();
            case DownloadRole:
                return item->download();
            case ItemTypeRole:
                return item->itemType();
            case LastUsedRole:
                return UiUtils::formatLastUsedDateRelative(item->timestamp());
            case LastUsedDateOnlyRole:
                return UiUtils::formatDateRelative(item->timestamp());
            case NameRole:
                if (m_list.returnItems(NetworkItemsList::Name, item->name()).count() > 1) {
                    return item->originalName();
                } else {
                    return item->name();
                }
            case SectionRole:
                return item->sectionType();
            case SignalRole:
                return item->signal();
            case SlaveRole:
                return item->slave();
            case SsidRole:
                return item->ssid();
            case SpecificPathRole:
                return item->specificPath();
            case SpeedRole:
                return item->speed();
            case SecurityTypeRole:
                return item->securityType();
            case SecurityTypeStringRole:
                return UiUtils::labelFromWirelessSecurity(item->securityType());
            case TimeStampRole:
                return item->timestamp();
            case TypeRole:
                return item->type();
            case UniRole:
                return item->uni();
            case UploadRole:
                return item->upload();
            case UuidRole:
                return item->uuid();
            case VpnState:
                return item->vpnState();
            default:
                break;
        }
    }

    return QVariant();
}

int NetworkModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

void NetworkModel::initialize()
{
    // Initialize existing connections
    foreach (const NetworkManager::Connection::Ptr& connection, NetworkManager::listConnections()) {
        addConnection(connection);
    }

    // Initialize existing devices
    foreach (const NetworkManager::Device::Ptr& dev, NetworkManager::networkInterfaces()) {
        addDevice(dev);
    }

    // Initialize existing active connections
    foreach (const NetworkManager::ActiveConnection::Ptr& active, NetworkManager::activeConnections()) {
        addActiveConnection(active);
    }

    initializeSignals();
}

void NetworkModel::initializeSignals()
{
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionAdded(QString)),
            SLOT(activeConnectionAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(activeConnectionRemoved(QString)),
            SLOT(activeConnectionRemoved(QString)));
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionAdded(QString)),
            SLOT(connectionAdded(QString)));
    connect(NetworkManager::settingsNotifier(), SIGNAL(connectionRemoved(QString)),
            SLOT(connectionRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceAdded(QString)),
            SLOT(deviceAdded(QString)));
    connect(NetworkManager::notifier(), SIGNAL(deviceRemoved(QString)),
            SLOT(deviceRemoved(QString)));
    connect(NetworkManager::notifier(), SIGNAL(statusChanged(NetworkManager::Status)),
            SLOT(statusChanged(NetworkManager::Status)));
    connect(NetworkManager::notifier(), SIGNAL(serviceAppeared()),
            SLOT(initialize()));
}

void NetworkModel::initializeSignals(const NetworkManager::ActiveConnection::Ptr& activeConnection)
{
    if (activeConnection->vpn()) {
        NetworkManager::VpnConnection::Ptr vpnConnection = activeConnection.objectCast<NetworkManager::VpnConnection>();
        if (vpnConnection) {
            connect(vpnConnection.data(), SIGNAL(stateChanged(NetworkManager::VpnConnection::State,NetworkManager::VpnConnection::StateChangeReason)),
                    SLOT(activeVpnConnectionStateChanged(NetworkManager::VpnConnection::State,NetworkManager::VpnConnection::StateChangeReason)));
        }
    } else {
        connect(activeConnection.data(), SIGNAL(stateChanged(NetworkManager::ActiveConnection::State)),
                SLOT(activeConnectionStateChanged(NetworkManager::ActiveConnection::State)));
    }
}

void NetworkModel::initializeSignals(const NetworkManager::Connection::Ptr& connection)
{
    connect(connection.data(), SIGNAL(updated()), SLOT(connectionUpdated()));
}

void NetworkModel::initializeSignals(const NetworkManager::Device::Ptr& device)
{
    connect(device.data(), SIGNAL(availableConnectionAppeared(QString)),
            SLOT(availableConnectionAppeared(QString)));
    connect(device.data(), SIGNAL(availableConnectionDisappeared(QString)),
            SLOT(availableConnectionDisappeared(QString)));
    connect(device.data(), SIGNAL(stateChanged(NetworkManager::Device::State,NetworkManager::Device::State,NetworkManager::Device::StateChangeReason)),
            SLOT(deviceStateChanged(NetworkManager::Device::State, NetworkManager::Device::State, NetworkManager::Device::StateChangeReason)));

    if (device->type() == NetworkManager::Device::Ethernet) {
        NetworkManager::WiredDevice::Ptr wiredDev = device.objectCast<NetworkManager::WiredDevice>();
        connect(wiredDev.data(), SIGNAL(bitRateChanged(int)),
                SLOT(bitrateChanged(int)));
    } else if (device->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
        connect(wifiDev.data(), SIGNAL(bitRateChanged(int)),
                SLOT(bitrateChanged(int)));
        connect(wifiDev.data(), SIGNAL(networkAppeared(QString)),
                SLOT(wirelessNetworkAppeared(QString)));
        connect(wifiDev.data(), SIGNAL(networkDisappeared(QString)),
                SLOT(wirelessNetworkDisappeared(QString)));
    }
#if WITH_MODEMMANAGER_SUPPORT
    else if (device->type() == NetworkManager::Device::Modem) {
        NetworkManager::ModemDevice::Ptr modemDev = device.objectCast<NetworkManager::ModemDevice>();
#ifdef MODEMMANAGERQT_ONE
        ModemManager::Modem::Ptr modemNetwork = modemDev->getModemNetworkIface();
        if (modemDev->isValid()) {
            connect(modemNetwork.data(), SIGNAL(signalQualityChanged(uint)),
                    SLOT(gsmNetworkSignalQualityChanged(uint)));
            connect(modemNetwork.data(), SIGNAL(accessTechnologyChanged(ModemManager::Modem::AccessTechnologies)),
                    SLOT(gsmNetworkAccessTechnologyChanged(ModemManager::Modem::AccessTechnologies)));
            connect(modemNetwork.data(), SIGNAL(currentModesChanged()),
                    SLOT(gsmNetworkCurrentModesChanged()));
        }
#else
        ModemManager::ModemGsmNetworkInterface::Ptr modemNetwork = modemDev->getModemNetworkIface().objectCast<ModemManager::ModemGsmNetworkInterface>();
        if (modemNetwork) {
            connect(modemNetwork.data(), SIGNAL(signalQualityChanged(uint)),
                    SLOT(gsmNetworkSignalQualityChanged(uint)));
            connect(modemNetwork.data(), SIGNAL(accessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)),
                    SLOT(gsmNetworkAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology)));
            connect(modemNetwork.data(), SIGNAL(allowedModeChanged(ModemManager::ModemInterface::AllowedMode)),
                    SLOT(gsmNetworkAllowedModeChanged(ModemManager::ModemInterface::AllowedMode)));
        }
#endif
    }
#endif
}

void NetworkModel::initializeSignals(const NetworkManager::WirelessNetwork::Ptr& network)
{
    connect(network.data(), SIGNAL(signalStrengthChanged(int)),
            SLOT(wirelessNetworkSignalChanged(int)));
    connect(network.data(), SIGNAL(referenceAccessPointChanged(QString)),
            SLOT(wirelessNetworkReferenceApChanged(QString)));
}

void NetworkModel::addActiveConnection(const NetworkManager::ActiveConnection::Ptr& activeConnection)
{
    qDebug() << "Add active connection - " << activeConnection->connection()->name();

    initializeSignals(activeConnection);

    NetworkManager::Connection::Ptr connection = activeConnection->connection();
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(activeConnection->devices().first());

    // Check whether we have a base connection
    if (!m_list.contains(NetworkItemsList::Uuid, connection->uuid())) {
        qDebug() << "Base connection doesn't exist, let's add it";
        // Active connection appeared before a base connection, so we have to add its base connection first
        addConnection(connection);
    }

    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::NetworkItemsList::Uuid, connection->uuid())) {
        if ((device && device->uni() == item->devicePath()) || item->type() == NetworkManager::ConnectionSettings::Vpn) {
            item->setActiveConnectionPath(activeConnection->path());
            item->setConnectionState(activeConnection->state());
            if (activeConnection->vpn()) {
                NetworkManager::VpnConnection::Ptr vpnConnection = activeConnection.objectCast<NetworkManager::VpnConnection>();
                NetworkManager::VpnConnection::State state = vpnConnection->state();
                if (state == NetworkManager::VpnConnection::Prepare ||
                    state == NetworkManager::VpnConnection::NeedAuth ||
                    state == NetworkManager::VpnConnection::Connecting ||
                    state == NetworkManager::VpnConnection::GettingIpConfig) {
                    item->setConnectionState(NetworkManager::ActiveConnection::Activating);
                } else if (state == NetworkManager::VpnConnection::Activated) {
                    item->setConnectionState(NetworkManager::ActiveConnection::Activated);
                } else {
                    item->setConnectionState(NetworkManager::ActiveConnection::Deactivated);
                }
                item->setVpnState(state);
            }
            updateItem(item);
        }
    }
}

void NetworkModel::addAvailableConnection(const QString& connection, const NetworkManager::Device::Ptr& device)
{
    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Connection, connection)) {
        // TODO: check whether the item is already associated with some device, in that case the connection
        //       is available for more devices
        if (device->ipInterfaceName().isEmpty()) {
            item->setDeviceName(device->interfaceName());
        } else {
            item->setDeviceName(device->ipInterfaceName());
        }
        item->setDevicePath(device->uni());
        item->setDeviceState(device->state());
        if (device->type() == NetworkManager::Device::Ethernet) {
            NetworkManager::WiredDevice::Ptr wiredDev = device.objectCast<NetworkManager::WiredDevice>();
            if (wiredDev) {
                item->setSpeed(wiredDev->bitRate());
            }
        } else if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice::Ptr wirelessDev = device.objectCast<NetworkManager::WirelessDevice>();
            if (wirelessDev) {
                item->setSpeed(wirelessDev->bitRate());
            }
        }
#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
        if (device->type() == NetworkManager::Device::Modem) {
            ModemManager::ModemDevice::Ptr modemDevice = ModemManager::findModemDevice(device->udi());
            if (modemDevice) {
                ModemManager::Modem::Ptr modemInterface = modemDevice->interface(ModemManager::ModemDevice::ModemInterface).objectCast<ModemManager::Modem>();
                if (modemInterface) {
                    item->setSignal(modemInterface->signalQuality().signal);
                }
            }
        }
#endif
#endif
        if (item->type() == NetworkManager::ConnectionSettings::Wireless && item->mode() == NetworkManager::WirelessSetting::Infrastructure) {
            bool apFound = false;
            // Check whether is there an available access point for this connection and remove it, because it should be part of the connection
            foreach (NetworkModelItem * secondItem, m_list.returnItems(NetworkItemsList::Ssid, item->ssid())) {
                if (secondItem->itemType() == NetworkModelItem::AvailableAccessPoint &&
                    secondItem->devicePath() == item->devicePath()) {
                    item->setSignal(secondItem->signal());
                    item->setSpecificPath(secondItem->specificPath());
                    apFound = true;
                    const int row = m_list.indexOf(secondItem);
                    if (row >= 0) {
                        beginRemoveRows(QModelIndex(), row, row);
                        m_list.removeItem(secondItem);
                        secondItem->deleteLater();
                        endRemoveRows();
                    }
                    break;
                }
            }

            if (!apFound) {
                NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
                NetworkManager::WirelessNetwork::Ptr wifiNetwork = wifiDevice->findNetwork(item->ssid());
            }
        }
        updateItem(item);
    }
}

void NetworkModel::addConnection(const NetworkManager::Connection::Ptr& connection)
{
    //TODO remove debug
    qDebug() << "Adding connection - " << connection->name();

    // Can't add a connection without name or uuid
    if (connection->name().isEmpty() || connection->uuid().isEmpty()) {
        return;
    }

    initializeSignals(connection);

    NetworkManager::ConnectionSettings::Ptr settings = connection->settings();
    NetworkManager::WirelessSetting::Ptr wirelessSetting;
    if (settings->connectionType() == NetworkManager::ConnectionSettings::Wireless) {
        wirelessSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
        // Check whether is there already an access point in the model for this connection, so we can only extend it
        if (m_list.contains(NetworkItemsList::Ssid, wirelessSetting->ssid())) {
            bool apFound = false;
            foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Ssid, wirelessSetting->ssid())) {
                // Applicable only to APs without connection
                if (item->itemType() == NetworkModelItem::AvailableAccessPoint) {
                    // FIXME find a proper way how to compare configurations
                    NetworkManager::Utils::WirelessSecurityType securityType = NetworkManager::Utils::securityTypeFromConnectionSetting(settings);
                    NetworkManager::Utils::WirelessSecurityType alternativeSecurityType = alternativeWirelessSecurity(securityType);
                    // TODO check more properties??? - bssid???
                    if ((item->securityType() == securityType || item->securityType() == alternativeSecurityType) && item->mode() == wirelessSetting->mode()) {
                        if (!wirelessSetting->macAddress().isEmpty()) {
                            NetworkManager::WirelessDevice::Ptr device = NetworkManager::findNetworkInterface(item->devicePath()).objectCast<NetworkManager::WirelessDevice>();
                            if (device && NetworkManager::Utils::macAddressAsString(wirelessSetting->macAddress()) == device->hardwareAddress()) {
                                apFound = true;
                            }
                        } else {
                            apFound = true;
                        }

                        if (apFound) {
                            item->setConnectionPath(connection->path());
                            item->setName(settings->id());
                            item->setTimestamp(settings->timestamp());
                            item->setType(settings->connectionType());
                            item->setUuid(settings->uuid());
                            updateItem(item);
                        }
                    }
                }
            }

            if (apFound) {
                return;
            }
        }
    }

    // Check whether the connection is already in the model to avoid duplicates, but this shouldn't happen
    if (!m_list.contains(NetworkItemsList::Connection, connection->path())) {
        NetworkModelItem * item = new NetworkModelItem();
        item->setConnectionPath(connection->path());
        item->setName(settings->id());
        item->setTimestamp(settings->timestamp());
        item->setType(settings->connectionType());
        item->setUuid(settings->uuid());
        item->setSlave(settings->isSlave());


        if (item->type() == NetworkManager::ConnectionSettings::Wireless) {
            item->setMode(wirelessSetting->mode());
            item->setSecurityType(NetworkManager::Utils::securityTypeFromConnectionSetting(settings));
            item->setSsid(wirelessSetting->ssid());
        }
        item->updateDetails();

        connect(item, SIGNAL(itemUpdated()), SLOT(onItemUpdated()));

        const int index = m_list.count();
        beginInsertRows(QModelIndex(), index, index);
        m_list.insertItem(item);
        endInsertRows();
    }
}

void NetworkModel::addDevice(const NetworkManager::Device::Ptr& device)
{
    //TODO remove debug
    qDebug() << "Adding device - " << device->udi();

    initializeSignals(device);

    if (device->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wifiDev = device.objectCast<NetworkManager::WirelessDevice>();
        foreach (const NetworkManager::WirelessNetwork::Ptr& wifiNetwork, wifiDev->networks()) {
            addWirelessNetwork(wifiNetwork, wifiDev);
        }
    }

    foreach (const NetworkManager::Connection::Ptr & connection, device->availableConnections()) {
        addAvailableConnection(connection->path(), device);
    }
}

void NetworkModel::addWirelessNetwork(const NetworkManager::WirelessNetwork::Ptr& network, const NetworkManager::WirelessDevice::Ptr& device)
{
    //TODO remove debug
    qDebug() << "Adding wireless network - " << network->ssid();

    initializeSignals(network);

    bool connectionFound = false;
    NetworkManager::WirelessSetting::NetworkMode mode = NetworkManager::WirelessSetting::Infrastructure;
    NetworkManager::Utils::WirelessSecurityType securityType = NetworkManager::Utils::Unknown;
    NetworkManager::AccessPoint::Ptr ap = network->referenceAccessPoint();
    if (ap && ap->capabilities() & NetworkManager::AccessPoint::Privacy) {
        securityType = NetworkManager::Utils::findBestWirelessSecurity(device->wirelessCapabilities(), true, (device->mode() == NetworkManager::WirelessDevice::Adhoc),
                                                                       ap->capabilities(), ap->wpaFlags(), ap->rsnFlags());
        if (network->referenceAccessPoint()->mode() == NetworkManager::AccessPoint::Infra) {
            mode = NetworkManager::WirelessSetting::Infrastructure;
        } else if (network->referenceAccessPoint()->mode() == NetworkManager::AccessPoint::Adhoc) {
            mode = NetworkManager::WirelessSetting::Adhoc;
        } else if (network->referenceAccessPoint()->mode() == NetworkManager::AccessPoint::ApMode) {
            mode = NetworkManager::WirelessSetting::Ap;
        }
    }
    // Check whether there is an available connection for this network. It's possible that some
    // connection has same SSID, but it might not be available due to specified properties like mode, security etc.
    // FIXME find a proper way how to compare configurations
    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Ssid, network->ssid())) {
        // TODO check more properties??? - bssid???
        NetworkManager::Utils::WirelessSecurityType alternativeSecurityType = alternativeWirelessSecurity(item->securityType());
        if ((item->securityType() == securityType || alternativeSecurityType == securityType) &&
            item->mode() == mode && item->itemType() == NetworkModelItem::UnavailableConnection) {
            NetworkManager::WirelessSetting::Ptr setting;
            NetworkManager::Connection::Ptr connection = NetworkManager::findConnection(item->connectionPath());
            if (connection) {
                setting = connection->settings()->setting(NetworkManager::Setting::Wireless).staticCast<NetworkManager::WirelessSetting>();
            }
            if (setting && (setting->macAddress().isEmpty() || NetworkManager::Utils::macAddressAsString(setting->macAddress()) == device->hardwareAddress())) {
                connectionFound = true;
                if (device->ipInterfaceName().isEmpty()) {
                    item->setDeviceName(device->interfaceName());
                } else {
                    item->setDeviceName(device->ipInterfaceName());
                }
                item->setDevicePath(device->uni());
                item->setMode(mode);
                item->setSignal(network->signalStrength());
                item->setSpecificPath(network->referenceAccessPoint()->uni());
                updateItem(item);
            }
        }
    }

    if (!connectionFound) {
        NetworkModelItem * item = new NetworkModelItem();
        if (device->ipInterfaceName().isEmpty()) {
            item->setDeviceName(device->interfaceName());
        } else {
            item->setDeviceName(device->ipInterfaceName());
        }
        item->setDevicePath(device->uni());
        item->setMode(mode);
        item->setName(network->ssid());
        item->setSignal(network->signalStrength());
        item->setSpecificPath(network->referenceAccessPoint()->uni());
        item->setSsid(network->ssid());
        item->setType(NetworkManager::ConnectionSettings::Wireless);
        item->setSecurityType(securityType);

        item->updateDetails();

        connect(item, SIGNAL(itemUpdated()), SLOT(onItemUpdated()));

        const int index = m_list.count();
        beginInsertRows(QModelIndex(), index, index);
        m_list.insertItem(item);
        endInsertRows();
    }
}

void NetworkModel::onItemUpdated()
{
    NetworkModelItem * item = static_cast<NetworkModelItem*>(sender());
    if (item) {
        updateItem(item);
    }
}

void NetworkModel::updateItem(NetworkModelItem * item)
{
    const int row = m_list.indexOf(item);

    if (row >= 0) {
        item->updateDetails();
        QModelIndex index = createIndex(row, 0);
        emit dataChanged(index, index);
    }
}

void NetworkModel::updateItems()
{
    foreach (NetworkModelItem * item, m_list.items()) {
        updateItem(item);
    }
}

void NetworkModel::activeConnectionAdded(const QString& activeConnection)
{
    //TODO remove debug
    qDebug() << "Adding active connection " << activeConnection;

    NetworkManager::ActiveConnection::Ptr activeCon = NetworkManager::findActiveConnection(activeConnection);

    if (activeCon) {
        addActiveConnection(activeCon);
    }
}

void NetworkModel::activeConnectionRemoved(const QString& activeConnection)
{
    //TODO remove debug
    qDebug() << "Removing active connection " << activeConnection;

    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::ActiveConnection, activeConnection)) {
        item->setActiveConnectionPath(QString());
        item->setConnectionState(NetworkManager::ActiveConnection::Deactivated);
        item->setVpnState(NetworkManager::VpnConnection::Disconnected);
        updateItem(item);
    }
}

void NetworkModel::activeConnectionStateChanged(NetworkManager::ActiveConnection::State state)
{
    NetworkManager::ActiveConnection * activePtr = qobject_cast<NetworkManager::ActiveConnection*>(sender());
    if (activePtr) {
        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::ActiveConnection, activePtr->path())) {
            item->setConnectionState(state);
            updateItem(item);
        }
    }
}

void NetworkModel::activeVpnConnectionStateChanged(NetworkManager::VpnConnection::State state, NetworkManager::VpnConnection::StateChangeReason reason)
{
    Q_UNUSED(reason)
    NetworkManager::ActiveConnection *activePtr = qobject_cast<NetworkManager::ActiveConnection*>(sender());

    if (activePtr) {
        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::ActiveConnection, activePtr->path())) {
            if (state == NetworkManager::VpnConnection::Prepare ||
                state == NetworkManager::VpnConnection::NeedAuth ||
                state == NetworkManager::VpnConnection::Connecting ||
                state == NetworkManager::VpnConnection::GettingIpConfig) {
                item->setConnectionState(NetworkManager::ActiveConnection::Activating);
            } else if (state == NetworkManager::VpnConnection::Activated) {
                item->setConnectionState(NetworkManager::ActiveConnection::Activated);
            } else {
                item->setConnectionState(NetworkManager::ActiveConnection::Deactivated);
            }
            item->setVpnState(state);
            updateItem(item);
        }
    }
}

void NetworkModel::availableConnectionAppeared(const QString& connection)
{
    //TODO remove debug
    qDebug() << "Adding available connection " << connection;

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    addAvailableConnection(connection, device);
}

void NetworkModel::availableConnectionDisappeared(const QString& connection)
{
    //TODO remove debug
    qDebug() << "Removing available connection " << connection;

    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Connection, connection)) {
        QString devicePath = item->devicePath();
        QString specificPath = item->specificPath();
        item->setDeviceName(QString());
        item->setDevicePath(QString());
        item->setDeviceState(NetworkManager::Device::UnknownState);
        item->setSignal(0);
        item->setSpecificPath(QString());
        item->setSpeed(0);
        // Check whether the connection is still available as an access point, this happens
        // when we change its properties, like ssid, bssid, security etc.
        if (item->type() == NetworkManager::ConnectionSettings::Wireless && !specificPath.isEmpty()) {
            NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(devicePath);
            if (device && device->type() == NetworkManager::Device::Wifi) {
                NetworkManager::WirelessDevice::Ptr wifiDevice = device.objectCast<NetworkManager::WirelessDevice>();
                if (wifiDevice) {
                    NetworkManager::AccessPoint::Ptr ap = wifiDevice->findAccessPoint(specificPath);
                    if (ap) {
                        NetworkManager::WirelessNetwork::Ptr network = wifiDevice->findNetwork(ap->ssid());
                        if (network) {
                            addWirelessNetwork(network, wifiDevice);
                        }
                    }
                }
            }
        }
        updateItem(item);
    }
}

void NetworkModel::bitrateChanged(int bitrate)
{
    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Device, device->uni())) {
        if (item->type() == NetworkManager::ConnectionSettings::Wired || item->type() == NetworkManager::ConnectionSettings::Wireless) {
            item->setSpeed(bitrate);
            updateItem(item);
        }
    }
}

void NetworkModel::connectionAdded(const QString& connection)
{
    // TODO remove debug
    qDebug() << "Connectin added " << connection;

    NetworkManager::Connection::Ptr newConnection = NetworkManager::findConnection(connection);
    if (newConnection) {
        addConnection(newConnection);
    }
}

void NetworkModel::connectionRemoved(const QString& connection)
{
    // TODO remove debug
    qDebug() << "Removing connection " << connection;

    bool remove = false;
    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Connection, connection)) {
        // When the item type is wireless, we can remove only the connection and leave it as an available access point
        if (item->type() == NetworkManager::ConnectionSettings::Wireless && !item->devicePath().isEmpty()) {
            // Remove it entirely when there is another connection with the same configuration and for the same device
            foreach (NetworkModelItem * secondItem, m_list.items()) {
                if (item->connectionPath() != secondItem->connectionPath() &&
                    item->devicePath() == secondItem->devicePath() &&
                    item->mode() == secondItem->mode() &&
                    item->securityType() == secondItem->securityType() &&
                    item->ssid() == secondItem->ssid()) {
                    remove = true;
                }
            }

            if (!remove) {
                item->setConnectionPath(QString());
                item->setName(item->ssid());
                item->setSlave(false);
                item->setTimestamp(QDateTime());
                item->setUuid(QString());
                updateItem(item);
            }
        } else {
            remove = true;
        }

        if (remove) {
            const int row = m_list.indexOf(item);
            if (row >= 0) {
                beginRemoveRows(QModelIndex(), row, row);
                m_list.removeItem(item);
                item->deleteLater();
                endRemoveRows();
            }
        }
        remove = false;
    }
}

void NetworkModel::connectionUpdated()
{
    NetworkManager::Connection * connectionPtr = qobject_cast<NetworkManager::Connection*>(sender());
    if (connectionPtr) {
        NetworkManager::ConnectionSettings::Ptr settings = connectionPtr->settings();
        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Connection, connectionPtr->path())) {
            item->setConnectionPath(connectionPtr->path());
            item->setName(settings->id());
            item->setTimestamp(settings->timestamp());
            item->setType(settings->connectionType());
            item->setUuid(settings->uuid());

            if (item->type() == NetworkManager::ConnectionSettings::Wireless) {
                NetworkManager::WirelessSetting::Ptr wirelessSetting;
                wirelessSetting = settings->setting(NetworkManager::Setting::Wireless).dynamicCast<NetworkManager::WirelessSetting>();
                item->setMode(wirelessSetting->mode());
                item->setSecurityType(NetworkManager::Utils::securityTypeFromConnectionSetting(settings));
                item->setSsid(wirelessSetting->ssid());
            }
            updateItem(item);
        }
    }
}

void NetworkModel::deviceAdded(const QString& device)
{
    // TODO: remove debug
    qDebug() << "Device added " << device;

    NetworkManager::Device::Ptr dev = NetworkManager::findNetworkInterface(device);
    if (dev) {
        addDevice(dev);
    }
}

void NetworkModel::deviceRemoved(const QString& device)
{
    // TODO: remove debug
    qDebug() << "Device removed " << device;

    // Make all items unavailable
    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Device, device)) {
        availableConnectionDisappeared(item->connectionPath());
    }
}

void NetworkModel::deviceStateChanged(NetworkManager::Device::State state, NetworkManager::Device::State oldState, NetworkManager::Device::StateChangeReason reason)
{
    Q_UNUSED(oldState);
    Q_UNUSED(reason);

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());

    if (device) {
        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Device, device->uni())) {
            item->setDeviceState(state);
        }
    }
}

#if WITH_MODEMMANAGER_SUPPORT
#ifdef MODEMMANAGERQT_ONE
void NetworkModel::gsmNetworkAccessTechnologyChanged(ModemManager::Modem::AccessTechnologies technology)
#else
void NetworkModel::gsmNetworkAccessTechnologyChanged(ModemManager::ModemInterface::AccessTechnology technology)
#endif
{
    Q_UNUSED(technology);
#ifdef MODEMMANAGERQT_ONE
    ModemManager::Modem * gsmNetwork = qobject_cast<ModemManager::Modem*>(sender());
#else
    ModemManager::ModemGsmNetworkInterface * gsmNetwork = qobject_cast<ModemManager::ModemGsmNetworkInterface*>(sender());
#endif
    if (gsmNetwork) {
        foreach (const NetworkManager::Device::Ptr & dev, NetworkManager::networkInterfaces()) {
            if (dev->type() == NetworkManager::Device::Modem) {
                NetworkManager::ModemDevice::Ptr modem = dev.objectCast<NetworkManager::ModemDevice>();
                if (modem) {
                    if (modem->getModemNetworkIface()->device() == gsmNetwork->device()) {
                        // TODO store access technology internally?
                        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Device, modem->uni())) {
                            updateItem(item);
                        }
                    }
                }
            }
        }
    }
}

#ifdef MODEMMANAGERQT_ONE
void NetworkModel::gsmNetworkCurrentModesChanged()
{
#else
void NetworkModel::gsmNetworkAllowedModeChanged(ModemManager::ModemInterface::AllowedMode mode)
{
    Q_UNUSED(mode);
#endif
#ifdef MODEMMANAGERQT_ONE
    ModemManager::Modem * gsmNetwork = qobject_cast<ModemManager::Modem*>(sender());
#else
    ModemManager::ModemGsmNetworkInterface * gsmNetwork = qobject_cast<ModemManager::ModemGsmNetworkInterface*>(sender());
#endif
    if (gsmNetwork) {
        foreach (const NetworkManager::Device::Ptr & dev, NetworkManager::networkInterfaces()) {
            if (dev->type() == NetworkManager::Device::Modem) {
                NetworkManager::ModemDevice::Ptr modem = dev.objectCast<NetworkManager::ModemDevice>();
                if (modem) {
                    if (modem->getModemNetworkIface()->device() == gsmNetwork->device()) {
                        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Device, modem->uni())) {
                            updateItem(item);
                        }
                    }
                }
            }
        }
    }
}

void NetworkModel::gsmNetworkSignalQualityChanged(uint signal)
{
#ifdef MODEMMANAGERQT_ONE
    ModemManager::Modem * gsmNetwork = qobject_cast<ModemManager::Modem*>(sender());
#else
    ModemManager::ModemGsmNetworkInterface * gsmNetwork = qobject_cast<ModemManager::ModemGsmNetworkInterface*>(sender());
#endif
    if (gsmNetwork) {
        foreach (const NetworkManager::Device::Ptr & dev, NetworkManager::networkInterfaces()) {
            if (dev->type() == NetworkManager::Device::Modem) {
                NetworkManager::ModemDevice::Ptr modem = dev.objectCast<NetworkManager::ModemDevice>();
                if (modem) {
                    if (modem->getModemNetworkIface()->device() == gsmNetwork->device()) {
                        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Device, modem->uni())) {
                            item->setSignal(signal);
                            updateItem(item);
                        }
                    }
                }
            }
        }
    }
}
#endif

void NetworkModel::statusChanged(NetworkManager::Status status)
{
    Q_UNUSED(status);

    // This has probably effect only for VPN connections
    foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Type, NetworkManager::ConnectionSettings::Vpn)) {
        updateItem(item);
    }
}

void NetworkModel::wirelessNetworkAppeared(const QString& ssid)
{
    // TODO: remove debug
    qDebug() << "Wireless network appeared " << ssid;

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    if (device && device->type() == NetworkManager::Device::Wifi) {
        NetworkManager::WirelessDevice::Ptr wirelessDevice = device.objectCast<NetworkManager::WirelessDevice>();
        NetworkManager::WirelessNetwork::Ptr network = wirelessDevice->findNetwork(ssid);
        addWirelessNetwork(network, wirelessDevice);
    }
}

void NetworkModel::wirelessNetworkDisappeared(const QString& ssid)
{
    // TODO: remove debug
    qDebug() << "Wireless network disappeared " << ssid;

    NetworkManager::Device::Ptr device = NetworkManager::findNetworkInterface(qobject_cast<NetworkManager::Device*>(sender())->uni());
    if (device) {
        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Ssid, ssid, device->uni())) {
            // Remove the entire item, because it's only AP
            if (item->itemType() == NetworkModelItem::AvailableAccessPoint) {
                const int row = m_list.indexOf(item);
                if (row >= 0) {
                    beginRemoveRows(QModelIndex(), row, row);
                    m_list.removeItem(item);
                    item->deleteLater();
                    endRemoveRows();
                }
            // Remove only AP and device from the item and leave it as an unavailable connection
            } else {
                item->setDeviceName(QString());
                item->setDevicePath(QString());
                item->setSignal(0);
                item->setSpecificPath(QString());
            }
        }
    }
}

void NetworkModel::wirelessNetworkReferenceApChanged(const QString& accessPoint)
{
    NetworkManager::WirelessNetwork * networkPtr = qobject_cast<NetworkManager::WirelessNetwork*>(sender());

    if (networkPtr) {
        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Ssid, networkPtr->ssid(), networkPtr->device())) {
            item->setSpecificPath(accessPoint);
            updateItem(item);
        }
    }
}

void NetworkModel::wirelessNetworkSignalChanged(int signal)
{
    NetworkManager::WirelessNetwork * networkPtr = qobject_cast<NetworkManager::WirelessNetwork*>(sender());
    if (networkPtr) {
        // TODO remove debug
        qDebug() << "Wireless network " << networkPtr->ssid() << " signal changed - " << signal;

        foreach (NetworkModelItem * item, m_list.returnItems(NetworkItemsList::Ssid, networkPtr->ssid(), networkPtr->device())) {
            item->setSignal(signal);
            updateItem(item);
        }
    }
}

NetworkManager::Utils::WirelessSecurityType NetworkModel::alternativeWirelessSecurity(const NetworkManager::Utils::WirelessSecurityType type)
{
    if (type == NetworkManager::Utils::WpaPsk) {
        return NetworkManager::Utils::Wpa2Psk;
    } else if (type == NetworkManager::Utils::WpaEap) {
        return NetworkManager::Utils::Wpa2Eap;
    } else if (type == NetworkManager::Utils::Wpa2Psk) {
        return NetworkManager::Utils::WpaPsk;
    } else if (type == NetworkManager::Utils::Wpa2Eap) {
        return NetworkManager::Utils::WpaEap;
    }
    return type;
}
