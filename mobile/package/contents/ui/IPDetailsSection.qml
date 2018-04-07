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
    property bool enabledSave: (ipMethodComb.currentIndex == 0 || (
                                ipMethodComb.currentIndex == 1 && manualIPaddress.acceptableInput
                                && manualIPgateway.acceptableInput && manualIPprefix.acceptableInput
                                && manualIPdns.acceptableInput ))
    property var ipRegex:  /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/

    spacing: units.gridUnit

    ColumnLayout {

        Controls.Label {
            anchors.left: parent.left
            text: i18n("IP settings")
            font.weight: Font.Bold
        }

        Controls.ComboBox {
            id: ipMethodComb
            model: [i18n("Automatic"), i18n("Manual")]
            onCurrentIndexChanged: {
                if (ipMethodComb.currentIndex == 0) {
                    ipmain.state = "Automatic"
                }
                if (ipMethodComb.currentIndex == 1) {
                    ipmain.state = "Manual"
                }
            }
        }
    }

    ColumnLayout{
        id: manualIPSettings
        Layout.fillWidth: true

        Controls.Label {
            text: i18n("IP Address")
        }

        Controls.TextField {
            id: manualIPaddress
            placeholderText: "193.168.1.128"
            text: address
            validator: RegExpValidator {
                regExp: ipRegex
            }
        }

        Controls.Label {
            text: i18n("Gateway")
        }

        Controls.TextField {
            id: manualIPgateway
            placeholderText: "192.168.1.1"
            text: gateway
            validator: RegExpValidator {
                regExp: ipRegex
            }
        }

        Controls.Label {
            text: i18n("Network prefix length")
        }

        Controls.TextField {
            id: manualIPprefix
            placeholderText: "32"
            text: prefix
            validator: IntValidator { bottom: 1; top: 32; }
        }

        Controls.Label {
            text: i18n("DNS")
        }

        Controls.TextField {
            id: manualIPdns
            placeholderText: "8.8.8.8"
            text: dns
            validator: RegExpValidator {
                regExp: ipRegex
            }
        }
    }

    states: [
        State {
            name: "Automatic"
            PropertyChanges{ target: manualIPSettings; visible: false }
            PropertyChanges{ target: ipmain; ipmap: {"method": "auto"} }
        },

        State {
            name:"Manual"
            PropertyChanges { target: manualIPSettings; visible: true }
            PropertyChanges {
                target: ipmain;
                ipmap : {
                    "method": "manual",
                    "address": address,
                    "prefix": prefix,
                    "gateway": gateway,
                    "dns": dns
                }
            }
        }
    ]
}
