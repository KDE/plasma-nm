// SPDX-FileCopyrightText: 2017 Martin Kacej <m.kacej@atlas.sk>
// SPDX-FileCopyrightText: 2023 Devin Lin <devin@kde.org>
// SPDX-FileCopyrightText: 2025 Sebastian Kügler <sebas@kde.org>
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

        FormCard.FormHeader {
            visible: savedCard.visible
            title: i18nc("@title:group", "Wired Network Connections")
        }

        FormCard.FormCard {
            id: savedCard

            // number of visible entries
            property int count: 0
            property int invisibleCount: 0
            function updateCount() {
                count = 0;
                invisibleCount = 0;
                for (let i = 0; i < connectedRepeater.count; i++) {
                    let item = connectedRepeater.itemAt(i);
                    if (item) {
                        if (item.shouldDisplay) {
                            count++;
                        } else {
                            invisibleCount++;
                        }
                    }
                }
            }

            Repeater {
                id: connectedRepeater

                model: mobileProxyModel
                delegate: PlasmaNM.ConnectionItemDelegate {
                    editMode: root.editMode
                    // connected or saved
                    property bool shouldDisplay: (Uuid != "") || ConnectionState === PlasmaNM.Enums.Activated
                    onShouldDisplayChanged: savedCard.updateCount()
                    // separate property for visible since visible is false when the whole card is not visible
                    visible: shouldDisplay

                    Component.onDestruction: {
                        // needed to show no connection label when unplugging network adapter
                        savedCard.updateCount();
                    }
                }

                Component.onCompleted: {
                    savedCard.updateCount();
                }
            }

            Connections {
                target: mobileProxyModel
                function onCountChanged() {
                    savedCard.updateCount();
                }
            }

            Controls.Label {
                id: noConnectionsLabel
                visible: savedCard.count == 0
                padding: Kirigami.Units.gridUnit
                text: i18n("No wired connection available.")
                textFormat: Text.PlainText
            }
        }
    }
}
