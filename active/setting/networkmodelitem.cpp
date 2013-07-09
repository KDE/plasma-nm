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

#include "networkmodelitem.h"
#include "uiutils.h"

#include <KLocale>

#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>


NetworkModelItem::NetworkModelItem(NetworkModelItem::NetworkType type, const QString &path, QObject *parent):
    QObject(parent),
    m_type(type),
    m_path(path)
{
}

NetworkModelItem::~NetworkModelItem()
{
}

NetworkModelItem::NetworkType NetworkModelItem::type() const
{
    return m_type;
}

void NetworkModelItem::setType(NetworkModelItem::NetworkType type)
{
    m_type = type;
}

QString NetworkModelItem::path() const
{
    return m_path;
}

void NetworkModelItem::setPath(const QString &path)
{
    m_path = path;
}

QString NetworkModelItem::name() const
{
    switch (m_type) {
        case NetworkModelItem::General:
            return i18n("General");
            break;
        case NetworkModelItem::Ethernet:
            return i18n("Ethernet");
            break;
        case NetworkModelItem::Modem:
            return i18n("Modem");
            break;
        case NetworkModelItem::Vpn:
            return i18n("VPN");
            break;
        case NetworkModelItem::Wifi:
            return i18n("Wireless");
            break;
        default:
            break;
    }

    return i18n("Uknown");
}

QString NetworkModelItem::icon() const
{
    if (m_type == NetworkModelItem::General) {
        return "network-defaultroute";
    } else if (m_type == NetworkModelItem::Ethernet) {
        return "network-wired-activated";
    } else if (m_type == NetworkModelItem::Modem) {
        return "phone";
    } else if (m_type == NetworkModelItem::Vpn) {
        return "secure-card";
    } else if (m_type == NetworkModelItem::Wifi) {
        return "network-wireless-connected-100";
    }

    return "network-wired";
}

bool NetworkModelItem::operator==(const NetworkModelItem *item) const
{
    if (item->type() == m_type &&
        item->path() == m_path) {
        return true;
    }

    return false;
}
