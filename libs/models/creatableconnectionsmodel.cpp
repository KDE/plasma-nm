/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

#include "creatableconnectionsmodel.h"

#include <KLocalizedString>
#include <KService>
#include <KServiceTypeTrader>

CreatableConnectionItem::CreatableConnectionItem(const QString &typeName, const QString &typeSection,
                                                 const QString &description, const QString &icon,
                                                 NetworkManager::ConnectionSettings::ConnectionType type,
                                                 const QString &vpnType, const QString &specificType,
                                                 bool shared,
                                                 QObject *parent)
    : QObject(parent)
    , m_shared(shared)
    , m_connectionType(type)
    , m_description(description)
    , m_icon(icon)
    , m_specificType(specificType)
    , m_typeName(typeName)
    , m_typeSection(typeSection)
    , m_vpnType(vpnType)
{
}

CreatableConnectionItem::CreatableConnectionItem(QObject *parent)
    : QObject(parent)
{
}

CreatableConnectionItem::~CreatableConnectionItem()
{
}
NetworkManager::ConnectionSettings::ConnectionType CreatableConnectionItem::connectionType() const
{
    return m_connectionType;
}

void CreatableConnectionItem::setConnectionType(NetworkManager::ConnectionSettings::ConnectionType type)
{
    m_connectionType = type;
}

QString CreatableConnectionItem::description() const
{
    return m_description;
}

void CreatableConnectionItem::setDescription(const QString &description)
{
    m_description = description;
}

QString CreatableConnectionItem::icon() const
{
    return m_icon;
}

void CreatableConnectionItem::setIcon(const QString &icon)
{
    m_icon = icon;
}

bool CreatableConnectionItem::shared() const
{
    return m_shared;
}

void CreatableConnectionItem::setShared(bool shared)
{
    m_shared = shared;
}

QString CreatableConnectionItem::specificType() const
{
    return m_specificType;
}

void CreatableConnectionItem::setSpecificType(const QString &specificType)
{
    m_specificType = specificType;
}

QString CreatableConnectionItem::typeName() const
{
    return m_typeName;
}

void CreatableConnectionItem::setTypeName(const QString &typeName)
{
    m_typeName = typeName;
}

QString CreatableConnectionItem::typeSection() const
{
    return m_typeSection;
}

void CreatableConnectionItem::setTypeSection(const QString &typeSection)
{
    m_typeSection = typeSection;
}

QString CreatableConnectionItem::vpnType() const
{
    return m_vpnType;
}

void CreatableConnectionItem::setVpnType(const QString &vpnType)
{
    m_vpnType = vpnType;
}

CreatableConnectionsModel::CreatableConnectionsModel(QObject *parent)
    : QAbstractListModel(parent)
{ 
    CreatableConnectionItem *connectionItem;
    connectionItem = new CreatableConnectionItem(i18n("DSL"), i18n("Hardware based connections"),
                                                 i18n("Some DSL description"), QStringLiteral("network-modem"),
                                                 NetworkManager::ConnectionSettings::Pppoe);
    m_list << connectionItem;
    
    connectionItem = new CreatableConnectionItem(i18n("Infiniband"), i18n("Hardware based connections"),
                                                 i18n("Some infiniband description"), QStringLiteral("network-wired"),
                                                 NetworkManager::ConnectionSettings::Infiniband);
    m_list << connectionItem;
    
#if WITH_MODEMMANAGER_SUPPORT
    connectionItem = new CreatableConnectionItem(i18n("Mobile Broadband"), i18n("Hardware based connections"),
                                                 i18n("Some mobile broadband description"), QStringLiteral("smartphone"),
                                                 NetworkManager::ConnectionSettings::Gsm);
    m_list << connectionItem;
#endif

    connectionItem = new CreatableConnectionItem(i18n("Wired Ethernet"), i18n("Hardware based connections"),
                                                 i18n("Some wired ethernet description"), QStringLiteral("network-wired"),
                                                 NetworkManager::ConnectionSettings::Wired);
    m_list << connectionItem;
    
    connectionItem = new CreatableConnectionItem(i18n("Wired Ethernet (shared)"), i18n("Hardware based connections"),
                                                 i18n("Some wired ethernet description"), QStringLiteral("network-wired"),
                                                 NetworkManager::ConnectionSettings::Wired,
                                                 QString(), QString(), true); // VpnType and SpecificType are empty
    m_list << connectionItem;

    connectionItem = new CreatableConnectionItem(i18n("Wi-Fi"), i18n("Hardware based connections"),
                                                 i18n("Some wi-fi description"), QStringLiteral("network-wireless"),
                                                 NetworkManager::ConnectionSettings::Wireless);
    m_list << connectionItem;
    
    connectionItem = new CreatableConnectionItem(i18n("Wi-Fi (shared)"), i18n("Hardware based connections"),
                                                 i18n("Some wi-fi description"), QStringLiteral("network-wireless"),
                                                 NetworkManager::ConnectionSettings::Wireless,
                                                 QString(), QString(), true); // VpnType and SpecificType are empty
    m_list << connectionItem;
    
    KService::List services = KServiceTypeTrader::self()->query("PlasmaNetworkManagement/VpnUiPlugin");

    std::sort(services.begin(), services.end(), [] (const KService::Ptr &left, const KService::Ptr &right)
    {
        return QString::localeAwareCompare(left->name(), right->name()) <= 0;
    });

    Q_FOREACH (const KService::Ptr & service, services) {
        const QString vpnType = service->property("X-NetworkManager-Services", QVariant::String).toString();
        const QString vpnSubType = service->property("X-NetworkManager-Services-Subtype", QVariant::String).toString();
        const QString vpnDescription = service->property("Comment", QVariant::String).toString();
        
        connectionItem = new CreatableConnectionItem(service->name(), i18n("VPN connections"),
                                                     vpnDescription, QStringLiteral("network-vpn"),
                                                     NetworkManager::ConnectionSettings::Vpn,
                                                     vpnType, vpnSubType, false);
        m_list << connectionItem;
    }
}

CreatableConnectionsModel::~CreatableConnectionsModel()
{
}

QVariant CreatableConnectionsModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    if (row >= 0 && row < m_list.count()) {
        CreatableConnectionItem * item = m_list.at(row);

        switch (role) {
            case ConnectionDescription:
                return item->description();
            case ConnectionIcon:
                return item->icon();
            case ConnectionSpeficType:
                return item->specificType();
            case ConnectionShared:
                return item->shared();
            case ConnectionType:
                return item->connectionType();
            case ConnectionTypeName:
                return item->typeName();
            case ConnectionTypeSection:
                return item->typeSection();
            case ConnectionVpnType:
                return item->vpnType();
            default:
                break;
        }
    }

    return QVariant();
}

QHash<int, QByteArray> CreatableConnectionsModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractListModel::roleNames();
    roles[ConnectionDescription] = "ConnectionDescription";
    roles[ConnectionIcon] = "ConnectionIcon";
    roles[ConnectionSpeficType] = "ConnectionSpecificType";
    roles[ConnectionShared] = "ConnectionShared";
    roles[ConnectionType] = "ConnectionType";
    roles[ConnectionTypeName] = "ConnectionTypeName";
    roles[ConnectionTypeSection] = "ConnectionTypeSection";
    roles[ConnectionVpnType] = "ConnectionVpnType";

    return roles;
}

int CreatableConnectionsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.count();
}

