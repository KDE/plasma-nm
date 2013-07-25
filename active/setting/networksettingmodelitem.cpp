/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

#include "networksettingmodelitem.h"
#include "uiutils.h"

#include <KLocale>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>


NetworkSettingModelItem::NetworkSettingModelItem(NetworkSettingModelItem::NetworkType type, const QString &path, QObject *parent):
    QObject(parent),
    m_type(type),
    m_path(path)
{
}

NetworkSettingModelItem::~NetworkSettingModelItem()
{
}

NetworkSettingModelItem::NetworkType NetworkSettingModelItem::type() const
{
    return m_type;
}

void NetworkSettingModelItem::setType(NetworkSettingModelItem::NetworkType type)
{
    m_type = type;
}

QString NetworkSettingModelItem::path() const
{
    return m_path;
}

void NetworkSettingModelItem::setPath(const QString &path)
{
    m_path = path;
}

QString NetworkSettingModelItem::name() const
{
    switch (m_type) {
        case NetworkSettingModelItem::Ethernet:
            return i18n("Ethernet");
            break;
        case NetworkSettingModelItem::Modem:
            return i18n("Modem");
            break;
        case NetworkSettingModelItem::Vpn:
            return i18n("VPN");
            break;
        case NetworkSettingModelItem::Wifi:
            return i18n("Wireless");
            break;
        default:
            break;
    }

    return i18n("Uknown");
}

QString NetworkSettingModelItem::icon() const
{
    if (m_type == NetworkSettingModelItem::Ethernet) {
        return "network-wired-activated";
    } else if (m_type == NetworkSettingModelItem::Modem) {
        return "phone";
    } else if (m_type == NetworkSettingModelItem::Vpn) {
        return "secure-card";
    } else if (m_type == NetworkSettingModelItem::Wifi) {
        return "network-wireless-connected-100";
    }

    return "network-wired";
}

bool NetworkSettingModelItem::operator==(const NetworkSettingModelItem *item) const
{
    if (item->type() == m_type &&
        item->path() == m_path) {
        return true;
    }

    return false;
}
