/*
 *
 *   Copyright 2017 Martin Kacej <>
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
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras

PlasmaExtras.ScrollArea{
    property var details
    property var signal_strength: 0
    property var signal_speed: 0
    property var ip_address: 0
    property var security: "Unknown"

    Column {

        Column{
            id: staticInfo
            anchors.bottomMargin: units.gridUnit

            PlasmaComponents.Label {
                text: i18n("<b>Strength: </b>" + signal_strength)
            }
            PlasmaComponents.Label{
                text: i18n("<b>Link Speed:</b> " + signal_speed)
            }
            PlasmaComponents.Label{
                text: i18n("<b>Security: </b>" +security)
            }
            PlasmaComponents.Label{
                text: i18n("<b>IP Address: </b>" + ip_address)
            }
        }

        RowLayout {
            PlasmaComponents.Label {
                anchors.left: parent.left
                text: i18n("Advanced options")
            }
            PlasmaComponents.Switch {
                id: advancedOptionsSwitch
                checked: false
            }
        }

        IPDetailsSection{
            id: detailsIP
            visible: advancedOptionsSwitch.checked
        }

        ProxyDetailsSection{
            visible: advancedOptionsSwitch.checked
        }
    }

    function fillDetails() {
        var d = {}
        for (var i = 0; i < (details.length / 2); i++){
            console.info(details[i])
            d[details[(i * 2)]] = details[(i * 2) + 1]
        }
        if(d['Access point (SSID)']) detailsDialog.titleText = d['Access point (SSID)']
        signal_strength = d['Signal strength']
        if (d['IPv4 Address']) ip_address = detailsIP.adress = d['IPv4 Address']
        if (d['Security type']) security = d['Security type']
        if (d['Connection speed']) signal_speed = d['Connection speed']
    }
    function clearDetails(){
        signal_speed = signal_strength = ip_address = 0
        security = "Unknown"
        detailsIP.adress = detailsIP.gateway = ''
    }
}
