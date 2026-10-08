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

    required property var setting

    readonly property var peers: dialog.setting.peers

    readonly property string publicKeyLabel: i18n("Public key:")
    readonly property string allowedIpsLabel: i18n("Allowed IPs:")
    readonly property string endpointAddressLabel: i18n("Endpoint address:")
    readonly property string endpointPortLabel: i18n("Endpoint port:")
    readonly property string presharedKeyLabel: i18n("Preshared key:")
    readonly property string keepaliveLabel: i18n("Persistent keepalive:")

    readonly property list<string> keyStorageOptions: [i18n("Store secret for this user only (encrypted)"), i18n("Store secret for all users (not encrypted)"), i18n("This secret is not required")]

    function peerLabel(index: int): string {
        return i18nc("@title:tab numbered WireGuard peer", "Peer %1", index + 1);
    }

    parent: QQC2.Overlay.overlay

    header: null

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 26

    Component.onCompleted: {
        const okButton = dialog.standardButton(Kirigami.Dialog.Ok);
        if (okButton) {
            okButton.enabled = Qt.binding(() => dialog.peers.valid);
        }
    }

    onOpened: peerTabs.currentIndex = dialog.peers.count > 0 ? 0 : -1

    onRejected: dialog.peers.revertEdit()

    ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        QQC2.TabBar {
            id: peerTabs

            Layout.fillWidth: true

            Repeater {
                model: dialog.peers

                QQC2.TabButton {
                    required property int index

                    text: dialog.peerLabel(index)
                }
            }
        }

        QQC2.Label {
            Layout.fillWidth: true

            visible: dialog.peers.count === 0

            text: i18n("This interface has no peers, so it cannot connect to anything yet.")
            wrapMode: Text.WordWrap
        }

        StackLayout {
            Layout.fillWidth: true

            currentIndex: peerTabs.currentIndex

            Repeater {
                model: dialog.peers

                Kirigami.FormLayout {
                    id: peerForm

                    required property var model

                    QQC2.TextField {
                        Kirigami.FormData.label: dialog.publicKeyLabel
                        Layout.fillWidth: true

                        validator: dialog.setting.keyValidator

                        text: peerForm.model.publicKey
                        onTextEdited: peerForm.model.publicKey = text
                    }

                    QQC2.TextField {
                        Kirigami.FormData.label: dialog.allowedIpsLabel
                        Layout.fillWidth: true

                        text: peerForm.model.allowedIps
                        onTextEdited: peerForm.model.allowedIps = text
                    }

                    QQC2.TextField {
                        Kirigami.FormData.label: dialog.endpointAddressLabel
                        Layout.fillWidth: true

                        text: peerForm.model.endpointAddress
                        onTextEdited: peerForm.model.endpointAddress = text
                    }

                    QQC2.TextField {
                        Kirigami.FormData.label: dialog.endpointPortLabel
                        Layout.fillWidth: true

                        inputMethodHints: Qt.ImhDigitsOnly
                        validator: IntValidator {
                            bottom: 1
                            top: 65535
                        }

                        text: peerForm.model.endpointPort
                        onTextEdited: peerForm.model.endpointPort = text
                    }

                    Kirigami.PasswordField {
                        Kirigami.FormData.label: dialog.presharedKeyLabel
                        Layout.fillWidth: true

                        validator: dialog.setting.keyValidator

                        text: peerForm.model.presharedKey
                        onTextEdited: peerForm.model.presharedKey = text
                    }

                    QQC2.ComboBox {
                        Layout.fillWidth: true

                        model: dialog.keyStorageOptions

                        currentIndex: peerForm.model.presharedKeyOption
                        onActivated: peerForm.model.presharedKeyOption = currentIndex
                    }

                    QQC2.TextField {
                        Kirigami.FormData.label: dialog.keepaliveLabel
                        Layout.fillWidth: true

                        inputMethodHints: Qt.ImhDigitsOnly
                        validator: IntValidator {
                            bottom: 0
                            top: 65535
                        }

                        text: peerForm.model.persistentKeepalive > 0 ? peerForm.model.persistentKeepalive : ""
                        onTextEdited: peerForm.model.persistentKeepalive = text.length > 0 ? Number(text) : 0
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.smallSpacing

            spacing: Kirigami.Units.smallSpacing

            QQC2.Button {
                text: i18nc("@action:button", "Add new Peer")
                icon.name: "list-add"

                onClicked: {
                    dialog.peers.addPeer();
                    peerTabs.currentIndex = dialog.peers.count - 1;
                }
            }

            QQC2.Button {
                text: i18nc("@action:button", "Remove this Peer")
                icon.name: "list-remove"

                enabled: dialog.peers.count > 0

                onClicked: {
                    const removed = peerTabs.currentIndex;
                    dialog.peers.removePeer(removed);
                    peerTabs.currentIndex = Math.min(removed, dialog.peers.count - 1);
                }
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }
}
