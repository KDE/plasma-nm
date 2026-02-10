// SPDX-FileCopyrightText: 2021 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

import org.kde.plasma.networkmanagement.cellular as Cellular

import org.kde.kirigamiaddons.formcard 1 as FormCard

import cellularnetworkkcm 1.0

Kirigami.ScrollablePage {
    id: root
    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit
    leftPadding: 0
    rightPadding: 0

    property Cellular.CellularModem modem
    property bool editMode: false

    title: i18n("APNs")
    actions: [
        Kirigami.Action {
            text: i18n("Edit")
            icon.name: 'entry-edit'
            checkable: true
            onCheckedChanged: root.editMode = checked
        }
    ]

    ColumnLayout {
        spacing: 0

        MessagesList {
            id: messagesList
            visible: count != 0
            Layout.fillWidth: true
            Layout.margins: Kirigami.Units.largeSpacing
            model: kcm.messages
        }

        Kirigami.InlineMessage {
            id: cannotFindWarning
            Layout.margins: visible ? Kirigami.Units.largeSpacing : 0
            Layout.topMargin: visible && !messagesList.visible ? Kirigami.Units.largeSpacing : 0
            Layout.fillWidth: true

            visible: false
            type: Kirigami.MessageType.Warning
            showCloseButton: true
            text: i18n("Unable to autodetect connection settings for your carrier. Please find your carrier's APN settings by either contacting support or searching online.")

            Connections {
                target: modem
                function onCouldNotAutodetectSettings() {
                    cannotFindWarning.visible = true;
                }
            }
        }

        FormCard.FormHeader {
            title: i18n("APN List")
        }

        FormCard.FormCard {
            Repeater {
                id: profilesRepeater
                model: modem.profiles

                delegate: FormCard.FormRadioDelegate {
                    required property int index
                    required property string connectionName
                    required property string connectionAPN
                    required property string connectionUser
                    required property string connectionPassword
                    required property string connectionNetworkType
                    required property string connectionUni
                    required property bool roamingAllowed

                    text: connectionName
                    description: connectionAPN

                    checked: modem.activeConnectionUni == connectionUni
                    onClicked: {
                        modem.activateProfile(connectionUni);

                        // reapply binding
                        checked = Qt.binding(() => { return modem.activeConnectionUni == connectionUni });
                    }

                    trailing: RowLayout {
                        Controls.ToolButton {
                            visible: root.editMode
                            icon.name: "entry-edit"
                            text: i18n("Edit")
                            display: Controls.ToolButton.IconOnly
                            onClicked: {
                                kcm.push("EditProfilePage.qml", {
                                    "modem": modem,
                                    "isNewProfile": false,
                                    "profileConnectionUni": connectionUni,
                                    "profileName": connectionName,
                                    "profileApn": connectionAPN,
                                    "profileUser": connectionUser,
                                    "profilePassword": connectionPassword,
                                    "profileNetworkType": connectionNetworkType,
                                    "profileRoamingAllowed": roamingAllowed
                                });
                            }
                        }

                        Controls.ToolButton {
                            visible: root.editMode
                            icon.name: "delete"
                            text: i18n("Delete")
                            display: Controls.ToolButton.IconOnly
                            onClicked: modem.removeProfile(connectionUni)
                        }
                    }
                }
            }

            FormCard.FormButtonDelegate {
                text: i18n("Add APN")
                icon.name: 'list-add'
                onClicked: {
                    kcm.push("EditProfilePage.qml", { "modem": modem, "isNewProfile": true });
                }
            }

            FormCard.FormButtonDelegate {
                text: i18n("Automatically detect APN")
                icon.name: 'list-add'
                onClicked: {
                    modem.addDetectedProfileSettings();
                }
            }
        }
    }
}
