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

import QtQuick 1.1

ListModel {
    id: availableDetailsModel;

    function insert(key, name) {
        append({"name": name, "key":key})
    }

    function init() {
//         insert("interface:type", i18n("Type"));
//         insert("interface:status", i18n("Connection State"));
        insert("interface:driver", i18n("Driver"));
        insert("interface:bitrate", i18n("Connection Speed"));
        insert("interface:name", i18n("System Name"));
        insert("interface:hardwareaddress", i18n("MAC Address"));

        // IPv4
        insert("ipv4:address", i18n("IPv4 Address"));
        insert("ipv4:gateway", i18n("IPv4 Gateway"));

        // IPv6
        insert("ipv6:address", i18n("IPv6 Address"));
        insert("ipv6:gateway", i18n("IPv6 Gateway"));

        // Wimax
//         insert("wimax:bsid", i18n("Wimax Bsid"));
//         insert("wimax:nsp", i18n("Wimax Nsp"));
//         insert("wimax:signal", i18n("Wimax Signal"));
//         insert("wimax:type", i18n("Wimax NSP Type"));

        // Wireless
        insert("wireless:ssid", i18n("Access Point (SSID)"));
        insert("wireless:signal", i18n("Signal Strength"));
        insert("wireless:accesspoint", i18n("Access Point (MAC)"));
        insert("wireless:band", i18n("Wireless Band"));
        insert("wireless:channel", i18n("Wireless Channel"));
        insert("wireless:security", i18n("Wireless Security"));
        insert("wireless:mode", i18n("Wireless Mode"));

        // Mobile Broadband
        insert("mobile:operator", i18n("Mobile Operator"));
        insert("mobile:quality", i18n("Mobile Signal Quality"));
        insert("mobile:technology", i18n("Mobile Access Technology"));
        insert("mobile:band", i18n("Mobile Frequency Band"));
        insert("mobile:mode", i18n("Mobile Allowed Mode"));
        insert("mobile:unlock", i18n("Mobile Unlock Required"));
        insert("mobile:imei", i18n("Mobile Device IMEI"));
        insert("mobile:imsi", i18n("Mobile Device IMSI"));

        // Others
        insert("bluetooth:name", i18n("Bluetooth Name"));
        insert("vpn:plugin", i18n("VPN Plugin"));
        insert("vpn:banner", i18n("VPN Banner"));
    }
}
