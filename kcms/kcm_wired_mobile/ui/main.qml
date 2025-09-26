// SPDX-FileCopyrightText: 2017 Martin Kacej <m.kacej@atlas.sk>
// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.kirigami as Kirigami
import org.kde.kcmutils
import org.kde.kirigamiaddons.formcard 1 as FormCard

SimpleKCM {
    id: root

    property bool editMode: true

    topPadding: Kirigami.Units.gridUnit
    bottomPadding: Kirigami.Units.gridUnit
    leftPadding: 0
    rightPadding: 0

    PlasmaNM.Handler {
        id: handler
    }

    PlasmaNM.NetworkModel {
        id: connectionModel
    }

    PlasmaNM.MobileProxyModel {
        id: mobileProxyModel
        sourceModel: connectionModel
        showSavedMode: false
        wired: true
    }

    ColumnLayout {

        Kirigami.InlineMessage {
            id: inlineError
            showCloseButton: true
            Layout.fillWidth: true

            type: Kirigami.MessageType.Warning
            Connections {
                target: handler
                function onConnectionActivationFailed(connectionPath, message) {
                    inlineError.text = message;
                    inlineError.visible = true;
                }
            }
        }

        // FormCard.FormCard {
        //     FormCard.FormSwitchDelegate {
        //         id: wifiSwitch
        //         text: i18n("Wired")
        //         checked: enabledConnections.wirelessEnabled
        //         onCheckedChanged: {
        //             handler.enableWireless(checked);
        //             checked = Qt.binding(() => enabledConnections.wirelessEnabled);
        //         }
        //     }
        // }

        FormCard.FormHeader {
            visible: savedCard.visible
            title: i18nc("@title:group", "Wired Network Interfaces")
        }
        /*
        FormCard.FormCard {
            id: interfacesCard
            //visible: enabledConnections.wirelessEnabled && count > 0

            // number of visible entries
            // property int count: 0
            // function updateCount() {
            //     count = 0;
            //     for (let i = 0; i < connectedRepeater.count; i++) {
            //         let item = connectedRepeater.itemAt(i);
            //         if (item && item.shouldDisplay) {
            //             count++;
            //         }
            //     }
            // }

            Repeater {
                id: interfacesRepeater
                //model: kcm.interfaces
                delegate: ConnectionItemDelegate {
                    //activeConnection: ActiveConnectionPlasmaNM
                    //editMode: root.editMode

                    // connected or saved
                    //property bool shouldDisplay: (Uuid != "") || ConnectionState === PlasmaNM.Enums.Activated
                    //onShouldDisplayChanged: savedCard.updateCount()

                    // separate property for visible since visible is false when the whole card is not visible
                    //visible: shouldDisplay
                    Component.onCompleted: {
                        console.log("activeConnection: " + modelData);
                    }
                }
            }
        }
        */
        FormCard.FormCard {
            id: savedCard

            // number of visible entries
            property int count: 0
            function updateCount() {
                count = 0;
                for (let i = 0; i < connectedRepeater.count; i++) {
                    let item = connectedRepeater.itemAt(i);
                    if (item && item.shouldDisplay) {
                        count++;
                    }
                }
            }

            Repeater {
                id: connectedRepeater
                //visible: savedCard.count > 0

                model: mobileProxyModel
                delegate: ConnectionItemDelegate {
                    editMode: root.editMode

                    // connected or saved
                    property bool shouldDisplay: (Uuid != "") || ConnectionState === PlasmaNM.Enums.Activated
                    onShouldDisplayChanged: savedCard.updateCount()

                    // separate property for visible since visible is false when the whole card is not visible
                    visible: shouldDisplay
                }
            }

            // Controls.Label {
            //     visible: savedCard.count == 0
            //     text: i18n("No wired connection available.")
            // }
        }
    }
}
