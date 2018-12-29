/*
 *   Copyright 2017-2018 Martin Kacej <m.kacej@atlas.sk>
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as Controls
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage {
    property var connectPath
    property var devicePath
    property alias name: hotSpotName.text

    header: ColumnLayout {
        width: parent.width
        anchors.leftMargin: Kirigami.Units.largeSpacing * 2

        Kirigami.Separator {}

        RowLayout{
            Kirigami.Separator {}
            Controls.Label {
                anchors.leftMargin: Kirigami.Units.largeSpacing * 2
                text: i18n("Wi-fi hotspot")
                Layout.fillWidth:  true
                font.weight: Font.Bold
            }
            Controls.Switch {
                id: hotSpotSwitch
                onCheckedChanged: {
                    if (checked) {
                        initTethering()
                    } else {
                        disableTethering()
                    }
                }
            }
            Kirigami.Separator {}
            Rectangle{
                height: 1
                Layout.fillWidth: true
                color: "black"
            }
            }
    }
//        RowLayout {
//        width: parent.width
//        Controls.Label {
//            text: "Wi-fi hotspot"
//            Layout.fillWidth:  true
//        }
//        Controls.Switch {
//            id: hotSpotSwitch
//            onCheckedChanged: {
//                if (checked) {
//                    initTethering()
//                } else {
//                    disableTethering()
//                }
//            }
//        }
//    }

    ColumnLayout {
        spacing: Kirigami.Units.gridUnit
        Kirigami.Separator{}

        RowLayout {
            id: hotSpotStatus
            spacing: Kirigami.Units.gridUnit
            property alias text: hotSpotStatusLabel.text
            Controls.Label {
                id: hotSpotStatusLabel
            }
            Kirigami.Icon {
                id: hotSpotStatusIcon
                width: Kirigami.Units.iconSizes.smallMedium
                height: width
                source: "network-wireless-disconnected"
            }
        }
        Controls.Button {
            id: hotSpotConfigButton
            checkable: true
            checked: false
            text: i18n("Configure")
            onPressed: {
                loadSettings()
            }
        }
        Column {
            id: hotSpotSettings
            width: parent.width / 2
            visible: hotSpotConfigButton.checked
            Column {
                width: parent.width
                Controls.Label {
                    text: i18n("SSID")
                    font.weight: Font.Bold
                }

                Controls.TextField {
                    id: hotSpotName
                    width: parent.width / 2
                    placeholderText: i18n("My Hotspot")
                }
            }
            RowLayout {
                Controls.CheckBox {
                    id: hotSpotConfigHidden
                    checked: false
                }

                Controls.Label {
                    text: i18n("Hide this network")
                }
            }
            RowLayout {
                Controls.CheckBox {
                    id: hotSpotConfigSecurity
                    checked: false
                }

                Controls.Label {
                    text: i18n("Protect hotspot with WPA2/PSK password")
                }
            }
            PasswordField {
                id: hotSpotConfigPassword
                width: parent.width / 2
                visible: hotSpotConfigSecurity.checked
                securityType: PlasmaNM.Enums.Wpa2Psk
            }
            Controls.Button {
                text: i18n("Save Hotspot configuration")
                enabled: name && (!hotSpotConfigSecurity.checked || (hotSpotConfigSecurity.checked && hotSpotConfigPassword.acceptableInput))
                onPressed: {
                    saveSettings()
                    hotSpotConfigButton.checked = false
                }
            }
        }
    }

    footer: Item {
        height: Kirigami.Units.gridUnit * 4
        Controls.Button {
            anchors.centerIn: parent
            Kirigami.Icon {
                anchors.centerIn: parent
                width: Kirigami.Units.iconSizes.medium
                height: width
                source: "dialog-close"
            }
            onPressed: {
                applicationWindow().pageStack.pop()
            }

        }
    }

    Component.onCompleted: {
        hotSpotStatus.text = i18n("Hotspot is inactive")
        checkTethering()
    }

    function checkTethering() {
        devicePath = kcm.getAccessPointDevice();
        if (devicePath === "") {
            hotSpotStatus.text = i18n("Not possible to start Access point.")
            hotSpotStatusIcon.source = "dialog-close"
            hotSpotSwitch.enabled = false
            return
        }
        connectPath = kcm.getAccessPointConnection()
        var map = kcm.getActiveConnectionInfo(connectPath);
        if (map["address"]) { // means AP connection is active
            hotSpotSwitch.checked = true
            hotSpotStatus.text = i18n("Access point running: %1", name)
        }
    }

    function initTethering() {
        connectPath = kcm.getAccessPointConnection()
        if (connectPath === "") {
            hotSpotStatus.text = i18n("No suitable configuration found.")
            hotSpotStatusIcon.source = "error"
            hotSpotSwitch.checked = false
            return
        }
        loadSettings()
        handler.activateConnection(connectPath,devicePath,"")
        hotSpotStatus.text = i18n("Access point running: %1", name)
        hotSpotStatusIcon.source = "network-wireless-symbolic"
    }

    function disableTethering(){
        if (connectPath !== "") {
            handler.deactivateConnection(connectPath,devicePath)
            hotSpotStatus.text = i18n("Hotspot is inactive")
            hotSpotStatusIcon.source = "network-wireless-disconnected"
        }
    }

    function saveSettings() {
        var map = {}
        map["id"] = name
        map["mode"] = "ap"
        map["method"] = "shared"
        map["hidden"] = hotSpotConfigHidden.checked
        if (hotSpotConfigSecurity.checked) {
            var securityMap = {}
            securityMap["type"] = PlasmaNM.Enums.Wpa2Psk
            securityMap["password"] = hotSpotConfigPassword.text
            map["802-11-wireless-security"] = securityMap
        }
        if (connectPath === "") {
            kcm.addConnectionFromQML(map)
        } else {
            kcm.updateConnectionFromQML(connectPath,map)
        }
    }

    function loadSettings() {
        var map = kcm.getConnectionSettings(connectPath,"connection");
        name = map["id"]
        map = kcm.getConnectionSettings(connectPath,"802-11-wireless");
        if (map["hidden"])
            hotSpotConfigHidden.checked = map["hidden"]
        map = kcm.getConnectionSettings(connectPath,"802-11-wireless-security")
        if (map["key-mgmt"]) {
            hotSpotConfigSecurity.checked = true
        }
        hotSpotStatus.text = i18n("Access point available: %1", name)
    }
}
