/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.Dialog {
    id: dialog

    required property var webAuth

    readonly property int stateInactive: 0
    readonly property int stateSelectAccount: 1
    readonly property int stateCollectPin: 2
    readonly property int stateFinishTokenCollection: 3
    readonly property int stateRequestFailed: 4

    parent: QQC2.Overlay.overlay

    title: i18nc("@title:window", "Security Key")

    preferredWidth: Kirigami.Units.gridUnit * 24

    closePolicy: QQC2.Popup.NoAutoClose

    standardButtons: Kirigami.Dialog.NoButton

    customFooterActions: [
        Kirigami.Action {
            text: dialog.webAuth.acceptText
            visible: dialog.webAuth.canAccept
            enabled: dialog.webAuth.state !== dialog.stateSelectAccount || accountGroup.selectedName !== ""

            onTriggered: {
                if (dialog.webAuth.state === dialog.stateSelectAccount) {
                    dialog.webAuth.acceptAccount(accountGroup.selectedName);
                } else {
                    dialog.webAuth.acceptPin(pinField.text);
                }
            }
        },
        Kirigami.Action {
            text: i18nc("@action:button", "Retry")
            visible: dialog.webAuth.canRetry

            onTriggered: dialog.webAuth.retry()
        },
        Kirigami.Action {
            text: dialog.webAuth.cancelText

            onTriggered: dialog.webAuth.cancel()
        }
    ]

    Connections {
        target: dialog.webAuth

        function onChanged() {
            pinField.text = "";
            confirmPinField.text = "";
            accountGroup.selectedName = "";
        }
    }

    ColumnLayout {
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            Layout.fillWidth: true

            level: 3
            text: dialog.webAuth.heading
            wrapMode: Text.WordWrap
        }

        QQC2.Label {
            Layout.fillWidth: true

            visible: text !== ""

            text: dialog.webAuth.description
            wrapMode: Text.WordWrap
        }

        // SelectAccount
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: dialog.webAuth.state === dialog.stateSelectAccount

            QQC2.ButtonGroup {
                id: accountGroup

                property string selectedName: ""
            }

            Repeater {
                model: dialog.webAuth.userNames

                QQC2.RadioButton {
                    required property string modelData

                    Layout.fillWidth: true

                    text: modelData
                    QQC2.ButtonGroup.group: accountGroup

                    onToggled: if (checked) {
                        accountGroup.selectedName = modelData;
                    }
                }
            }
        }

        // CollectPin
        Kirigami.FormLayout {
            Layout.fillWidth: true

            visible: dialog.webAuth.state === dialog.stateCollectPin

            Kirigami.PasswordField {
                id: pinField

                Kirigami.FormData.label: i18n("PIN:")
                Layout.fillWidth: true

                inputMethodHints: Qt.ImhDigitsOnly
            }

            Kirigami.PasswordField {
                id: confirmPinField

                Kirigami.FormData.label: i18n("Confirm PIN:")
                Layout.fillWidth: true

                visible: dialog.webAuth.confirmPinRequired
                inputMethodHints: Qt.ImhDigitsOnly
            }
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true

            visible: dialog.webAuth.pinError !== ""

            type: Kirigami.MessageType.Error
            text: dialog.webAuth.pinError
        }

        QQC2.BusyIndicator {
            Layout.alignment: Qt.AlignHCenter

            visible: dialog.webAuth.state === dialog.stateFinishTokenCollection
            running: visible
        }
    }
}
