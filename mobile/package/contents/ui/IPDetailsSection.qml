/*
 *   Copyright 2017 Martin Kacej <m.kacej@atlas.sk>
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

import QtQuick 2.2
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents

ColumnLayout{
    property var ipmap: []
    property var address: ""
    property var gateway: ""

    spacing: units.gridUnit

    ColumnLayout{
        id: manuaIPSettings
        visible: true
        Layout.fillWidth: true
        PlasmaComponents.Label {
            text: i18n("IP Address")
        }

        Controls.TextField {
            placeholderText: i18n("193.168.1.128")
            text: address ? address : ""
        }

        PlasmaComponents.Label {
            text: i18n("Gateway")
        }

        Controls.TextField {
            placeholderText: i18n("192.168.1.1")
        }

        PlasmaComponents.Label {
            text: i18n("Network prefix length")
        }

        Controls.TextField {
            placeholderText: i18n("24")
        }

        PlasmaComponents.Label{
            text: i18n("DNS")
        }

        Controls.TextField {
            placeholderText: i18n("8.8.8.8")
        }
    }
    onVisibleChanged: {
        if (visible == true){
            address = ipmap["address-data"]
        }
    }
}
