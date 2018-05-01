import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.3
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage {
    property var connectPath
    property var devicePath
    property alias name: hotSpotName.text

    header: RowLayout {
        width: parent.width
        Controls.Label {
            text: "Wi-fi hotspot"
            Layout.fillHeight: true
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
    }

    Column {
        Row {
            id: hotSpotStatus
            property alias text: hotSpotStatusLabel.text
            anchors.left: parent.left
            Controls.Label {
                id: hotSpotStatusLabel
            }
            Item {
                id: hotSpotStatusIcon
                //TODO
            }
        }
        Controls.Button {
            id: hotSpotConfigButton
            checkable: true
            checked: false
            text: i18n("Configure")
        }

        Column{
            id: hotSpotSettings
            visible: hotSpotConfigButton.checked
            Column {
                Controls.Label {
                    text: i18n("SSID")
                    font.weight: Font.Bold
                }

                Controls.TextField {
                    id: hotSpotName
                    placeholderText: i18n("My Hotspot")
                }
            }
            Row {
                Controls.CheckBox {
                    id: hotSpotConfigHidden
                    checked: false
                }

                Controls.Label {
                    text: i18n("Hide this network")
                }
            }
            Row {
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

    actions {
        left:
            Kirigami.Action {
            iconName: "dialog-cancel"
            text: i18n("Cancel")
            onTriggered: {
                applicationWindow().pageStack.pop()
            }
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
            hotSpotSwitch.checked = false
            return
        }
        loadSettings()
        handler.activateConnection(connectPath,devicePath,"")
        hotSpotStatus.text = i18n('Access point running: ') + name

    }

    function disableTethering(){
        if (connectPath !== "") {
            handler.deactivateConnection(connectPath,devicePath)
            hotSpotStatus.text = i18n("HotSpot is inactive")
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
