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
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2 as Layouts
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Layouts.ColumnLayout {
    id:securitySectionView
    property var securityMap: ({})
    property var enabledSave: !wepWpaPasswordField.visible || (wepWpaPasswordField.visible && wepWpaPasswordField.acceptableInput)
    property alias password: wepWpaPasswordField.text
    width: parent.width

    Column {
        id: securitySectionHeader

        width: parent.width
        anchors {
            top:parent.top
            topMargin: units.Gridunit
            bottomMargin: units.Gridunit
            left: parent.left
        }
        Controls.Label {
            text: i18n("Security")
            font.weight: Font.Bold
        }
        Controls.ComboBox {
            id: securityCombobox
            //anchors.bottomMargin: units.Gridunit
            anchors.bottomMargin: 50
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
                securitySectionView.state = securityTypesModel.get(currentIndex).type;
            }
        }

        Controls.Label {
            anchors.bottomMargin: units.Gridunit
            text: securityCombobox.currentText
        }
    }

    Layouts.ColumnLayout {
        id: wepWpaSecurity
        anchors.top: securitySectionHeader.bottom
        width: parent.width
        Column {
            width: parent.width
            visible: securityCombobox.currentIndex == 1 || securityCombobox.currentIndex == 3
            PasswordField {
                id: wepWpaPasswordField
                width: parent.width
                securityType: securityTypesModel.get(securityCombobox.currentIndex).type

            }
        }
    }

    Layouts.ColumnLayout {
        id: eap
        anchors.top: securitySectionHeader.bottom
        Column {
            visible: securityCombobox.currentIndex == 2 || securityCombobox.currentIndex == 4
            Controls.Label {
                text: i18n("Authentication")
            }
            Controls.ComboBox {
                id: authComboBox
                model: [i18n("TLS"), i18n("LEAP"), i18n("FAST"), i18n("Tunneled TLS"), i18n("Protected EAP")] // more - SIM, AKA, PWD ?
            }
            Controls.Label{
                text: "----Not yet implemented----"
                color: "red"
            }
        }
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
}
