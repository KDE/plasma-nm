/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

#include "connectiontypeitem.h"

#include <QtCore/QDateTime>
#include <KIcon>
#include <KLocale>

ConnectionTypeItem::ConnectionTypeItem(QTreeWidget * parent, const QString &type):
    QTreeWidgetItem(parent)
{
    setData(0, Qt::UserRole, type);
    QString text;

    if (type == QLatin1String("adsl")) {
        text = i18n("Adsl");
        setIcon(0, KIcon("modem"));
    } else if (type == QLatin1String("bluetooth")) {
        text = i18n("Bluetooth");
        setIcon(0, KIcon("bluetooth"));
    } else if (type == QLatin1String("bond")) {
        text = i18n("Bond");
    } else if (type == QLatin1String("bridge")) {
        text = i18n("Bridge");
    } else if (type == QLatin1String("cdma")) {
        text = i18n("Mobile broadband");
        setIcon(0, KIcon("phone"));
    } else if (type == QLatin1String("gsm")) {
        text = i18n("Mobile broadband");
        setIcon(0, KIcon("phone"));
    } else if (type == QLatin1String("infiniband")) {
        text = i18n("InfiniBand");
    } else if (type == QLatin1String("802-11-olpc-mesh")) {
        text = i18n("Olpc mesh");
    } else if (type == QLatin1String("pppoe")) {
        text = i18n("Pppoe");
    } else if (type == QLatin1String("vlan")) {
        text = i18n("VLAN");
    } else if (type == QLatin1String("vpn")) {
        text = i18n("Vpn");
        setIcon(0, KIcon("secure-card"));
    } else if (type == QLatin1String("wimax")) {
        text = i18n("WiMAX");
    } else if (type == QLatin1String("802-3-ethernet")) {
        text = i18n("Wired");
        setIcon(0, KIcon("network-wired"));
    } else if (type == QLatin1String("802-11-wireless")) {
        text = i18n("Wi-Fi");
        setIcon(0, KIcon("network-wireless"));
    }

    setText(0, text);

    QFont fnt = font(0);
    fnt.setBold(true);

    setFont(0, fnt);

    setExpanded(true);
}
