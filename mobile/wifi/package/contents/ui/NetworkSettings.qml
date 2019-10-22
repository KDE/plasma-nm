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
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.3 as Kirigami
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kcm 1.1

SimpleKCM {
    property var path
    property var settings: ({})
    property var securityMap: ({})
    property var activeMap: ({})
    property alias password: wepWpaPasswordField.text

    property var ipmap: ({})
    property alias address: manualIPaddress.text
    property alias gateway: manualIPgateway.text
    property alias prefix: manualIPprefix.text
    property alias dns: manualIPdns.text
    property var ipRegex:  /^(([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))\.){3}([01]?[0-9]?[0-9]|2([0-4][0-9]|5[0-5]))$/
    property bool manualIp

    property bool enabledSave: (ipMethodComb.currentIndex == 0 || (
                                ipMethodComb.currentIndex == 1 && manualIPaddress.acceptableInput
                                && manualIPgateway.acceptableInput && manualIPprefix.acceptableInput
                                && manualIPdns.acceptableInput ))

    actions.main: Kirigami.Action {
        icon.name: "dialog-ok"
        text: i18n("Save")
        onTriggered: {
            save()
            kcm.pop()
        }
    }

    header: Kirigami.Heading {
        id: detailsName
        text: i18n("Connection Name")
        level: 2
        leftPadding: Kirigami.Units.smallSpacing
    }

    Kirigami.FormLayout {

        Item {
            Kirigami.FormData.label: i18n("Security")
            Kirigami.FormData.isSection: true
        }

        Controls.ComboBox {
            id: securityCombobox
            Kirigami.FormData.label: i18n("Security type:")
            model: ListModel {
                id: securityTypesModel
                // FIXME just placeholder element to set "text" property as default
                ListElement {
                    text: "placeholder"
                }
                Component.onCompleted: {
                    clear()
                    append({ "text": i18n("None"), "type": PlasmaNM.Enums.NoneSecurity })
                    append({ "text": i18n("WEP Key"), "type": PlasmaNM.Enums.StaticWep })
                    append({ "text": i18n("Dynamic WEP"), "type": PlasmaNM.Enums.DynamicWep })
                    append({ "text": i18n("WPA/WPA2 Personal"), "type": PlasmaNM.Enums.Wpa2Psk })
                    append({ "text": i18n("WPA/WPA2 Enterprise"), "type": PlasmaNM.Enums.Wpa2Eap })
                    securityCombobox.currentIndex = 0
                }
            }
            onCurrentIndexChanged: {
                state = securityTypesModel.get(currentIndex).type;
            }
        }

        PasswordField {
            id: wepWpaPasswordField
            Kirigami.FormData.label: i18n("Password:")
            width: parent.width
            securityType: securityTypesModel.get(securityCombobox.currentIndex).type
        }

        Controls.ComboBox {
            id: authComboBox
            Kirigami.FormData.label: i18n("Authentication:")
            visible: securityCombobox.currentIndex == 2 || securityCombobox.currentIndex == 4
            model: [i18n("TLS"), i18n("LEAP"), i18n("FAST"), i18n("Tunneled TLS"), i18n("Protected EAP")] // more - SIM, AKA, PWD ?
        }
        Controls.Label {
            visible: authComboBox.visible
            text: "----Not yet implemented----"
            color: "red"
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("IP settings")
            Kirigami.FormData.isSection: true
        }

        Controls.ComboBox {
            id: ipMethodComb
            width: parent.width
            model: [i18n("Automatic"), i18n("Manual")]
            onCurrentIndexChanged: {
                manualIp = currentIndex == 1

                if (manualIp) {
                    ipmap = {
                    "method": "manual",
                    "address": address,
                    "prefix": prefix,
                    "gateway": gateway,
                    "dns": dns
                    }
                } else {
                    ipmap = {"method": "auto"}
                }
            }
        }

        Controls.TextField {
            id: manualIPaddress
            visible: manualIp
            Kirigami.FormData.label: i18n("IP Address:")
            placeholderText: "193.168.1.128"
            text: address
            validator: RegExpValidator {
                regExp: ipRegex
            }
        }

        Controls.TextField {
            id: manualIPgateway
            visible: manualIp
            Kirigami.FormData.label: i18n("Gateway:")
            placeholderText: "192.168.1.1"
            text: gateway
            validator: RegExpValidator {
                regExp: ipRegex
            }
        }

        Controls.TextField {
            id: manualIPprefix
            visible: manualIp
            Kirigami.FormData.label: i18n("Network prefix length:")
            placeholderText: "32"
            text: prefix
            validator: IntValidator { bottom: 1; top: 32; }
        }

        Controls.TextField {
            id: manualIPdns
            visible: manualIp
            Kirigami.FormData.label: i18n("DNS:")
            placeholderText: "8.8.8.8"
            text: dns
            validator: RegExpValidator {
                regExp: ipRegex
            }
        }

    }

    function setStateFromMap() {

        if (!ipmap)
            return;
        if (ipmap["method"] === "auto")
            ipMethodComb.currentIndex = 0
        if (ipmap["method"] === "manual"){
            address = ipmap["address"];
            gateway = ipmap["gateway"];
            prefix = ipmap["prefix"];
            dns  = ipmap["dns"];
            ipMethodComb.currentIndex = 1
        }

        var x = securityMap["key-mgmt"]
        switch (x) {
            case "none":
                securityCombobox.currentIndex = 0
                break;
             case "ieee8021x":
                 securityCombobox.currentIndex = 1
                 break;
             case "wpa-psk":
                 securityCombobox.currentIndex = 3
                 break;
             case "wpa-eap":
                 securityCombobox.currentIndex = 4
                 break;
            default:
                securityCombobox.currentIndex = -1
                break;
        }
        wepWpaPasswordField.placeholderText = i18n("(Unchanged)")
        securityCombobox.enabled = false
    }

    states: [
        State {
            name: PlasmaNM.Enums.NoneSecurity
            PropertyChanges {
                target: securitySectionView; securityMap: {"type" : PlasmaNM.Enums.NoneSecurity }
            }
        },
        State {
            name: PlasmaNM.Enums.StaticWep
            PropertyChanges {
                target: securitySectionView; securityMap: { "type" : PlasmaNM.Enums.StaticWep,
                                                            "password" : password
                }
            }
        },
        State {
            name: PlasmaNM.Enums.Wpa2Psk
            PropertyChanges {
                target: securitySectionView; securityMap: { "type" : PlasmaNM.Enums.Wpa2Psk,
                                                            "password" : password
                }
            }
        }
    ]

    Component.onCompleted: {
        settings = kcm.getConnectionSettings(path,"connection");
        detailsName.text = settings["id"]
        securityMap = kcm.getConnectionSettings(path, "802-11-wireless-security");
        ipmap = kcm.getConnectionSettings(path,"ipv4");
        setStateFromMap();
    }

    function save() {
        settings = ipmap;
        if (password !== "") { //otherwise password is unchanged
            settings["802-11-wireless-security"] = detailsSecuritySection.securityMap;
        }
        kcm.updateConnectionFromQML(path,settings);
    }
}
