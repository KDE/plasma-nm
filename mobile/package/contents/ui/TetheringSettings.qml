import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.3
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage {
    property var connectPath
    property var devicePath
    property alias name: hotSpotName.text

    header:
        ColumnLayout {
        width: parent.width
        anchors.leftMargin: Kirigami.Units.largeSpacing * 2
        Kirigami.Separator {}
        RowLayout{
            Kirigami.Separator {}
            Controls.Label {
                anchors.leftMargin: Kirigami.Units.largeSpacing * 2
                text: "Wi-fi hotspot"
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
        Column{
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
                text: i18n("Save Hospot configuration")
                enabled: name && (!hotSpotConfigSecurity.checked || (hotSpotConfigSecurity.checked && hotSpotConfigPassword.acceptableInput))
                onPressed: {
                    saveSettings()
                    hotSpotConfigButton.checked = false
                }
            }
        }
    }

    footer:
        Controls.Button {
            height: Kirigami.Units.gridUnit * 2
            width: height
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

    Component.onCompleted: {
        hotSpotStatus.text = i18n("HotSpot is inactive")
        checkTethering()
    }

    function checkTethering() {
        devicePath = utils.getAccessPointDevice();
        if (devicePath === "") {
            hotSpotStatus.text = i18n('Not possible to start Acces point.')
            hotSpotStatusIcon.source = "dialog-close"
            hotSpotSwitch.enabled = false
            return
        }
        connectPath = utils.getAccessPointConnection()
        var map = utils.getActiveConnectionInfo(connectPath);
        if (map["address"]) { // means AP connection is active
            hotSpotSwitch.checked = true
            hotSpotStatus.text = i18n('Access point running: ') + name
        }
    }

    function initTethering() {
        connectPath = utils.getAccessPointConnection()
        if (connectPath === "") {
            hotSpotStatus.text = i18n('No suitable configuration found.')
            hotSpotStatusIcon.source = "error"
            hotSpotSwitch.checked = false
            return
        }
        loadSettings()
        handler.activateConnection(connectPath,devicePath,"")
        hotSpotStatus.text = i18n('Access point running: ') + name
        hotSpotStatusIcon.source = "network-wireless-symbolic"

    }

    function disableTethering(){
        if (connectPath !== "") {
            handler.deactivateConnection(connectPath,devicePath)
            hotSpotStatus.text = i18n("HotSpot is inactive")
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
            utils.addConnectionFromQML(map)
        } else {
            utils.updateConnectionFromQML(connectPath,map)
        }
    }

    function loadSettings() {
        var map = utils.getConnectionSettings(connectPath,"connection");
        name = map["id"]
        map = utils.getConnectionSettings(connectPath,"802-11-wireless");
        if (map["hidden"])
            hotSpotConfigHidden.checked = map["hidden"]
        map = utils.getConnectionSettings(connectPath,"802-11-wireless-security")
        if (map["key-mgmt"]) {
            hotSpotConfigSecurity.checked = true
        }
        hotSpotStatus.text = i18n('Access point avialable: ') + name
    }
}
