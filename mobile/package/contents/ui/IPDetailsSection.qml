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

import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents

ColumnLayout {
    id: ipmain

    property var ipmap: ({})
    property alias address: manualIPaddress.text
    property alias gateway: manualIPgateway.text
    property alias prefix: manualIPprefix.text
    property alias dns: manualIPdns.text
    property bool enabledSave: false

    spacing: units.gridUnit

    ColumnLayout {

        PlasmaComponents.Label {
            anchors.left: parent.left
            text: i18n("IP settings")
            font.weight: Font.Bold
        }

        Controls.ComboBox {
            id: ipMethodComb
            model: ["Automatic","Manual"]
            onCurrentIndexChanged: {
                if (ipMethodComb.currentIndex == 0){
                    ipmain.state = "Automatic"
                }
                if(ipMethodComb.currentIndex == 1){
                    ipmain.state = "Manual"
                }
            }
        }
    }

    ColumnLayout{
        id: manualIPSettings
        Layout.fillWidth: true

        PlasmaComponents.Label {
            text: i18n("IP Address")
        }

        Controls.TextField {
            id: manualIPaddress
            placeholderText: i18n("193.168.1.128")
            text: address
        }

        PlasmaComponents.Label {
            text: i18n("Gateway")
        }

        Controls.TextField {
            id: manualIPgateway
            placeholderText: i18n("192.168.1.1")
            text: gateway
        }

        PlasmaComponents.Label {
            text: i18n("Network prefix length")
        }

        Controls.TextField {
            id: manualIPprefix
            placeholderText: i18n("24")
            text: prefix
        }

        PlasmaComponents.Label {
            text: i18n("DNS")
        }

        Controls.TextField {
            id: manualIPdns
            placeholderText: i18n("8.8.8.8")
            text: dns
        }
    }

    states: [
        State {
            name: "Automatic"
            PropertyChanges{ target: manualIPSettings; visible : false }
            PropertyChanges{ target: ipmain; ipmap : {"method":"auto"}}
        },

        State {
            name:"Manual"
            PropertyChanges { target: manualIPSettings; visible : true }
            PropertyChanges {
                target: ipmain;
                ipmap : {
                    "method" : "manual",
                    "address-data" : [{"address":address, "prefix":prefix}],
                    "gateway" : gateway,
                    "dns" : dns
                }
            }
        }
    ]
}
