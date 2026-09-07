/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Kirigami.Dialog {
    id: dialog

    required property var setting

    parent: QQC2.Overlay.overlay

    header: null

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 34
    preferredHeight: Kirigami.Units.gridUnit * 32

    readonly property list<string> revertableProperties: ["allowPap", "allowChap", "allowMschap", "allowMschapv2", "allowEap", "useMppe", "mppeCrypto", "statefulEncryption", "allowBsdCompression", "allowDeflateCompression", "allowTcpHeaderCompression", "sendPppEchoPackets", "useCustomUnitNumber", "customUnitNumber", "proxyAddress", "proxyPort", "proxyUsername", "proxyPassword"]

    property var previousState: null

    onOpened: {
        const state = {};
        for (const name of dialog.revertableProperties) {
            state[name] = dialog.setting[name];
        }
        dialog.previousState = state;
    }

    onRejected: {
        if (!dialog.previousState) {
            return;
        }

        for (const name of dialog.revertableProperties) {
            dialog.setting[name] = dialog.previousState[name];
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.smallSpacing

        QQC2.TabBar {
            id: tabBar

            Layout.fillWidth: true

            QQC2.TabButton {
                text: i18n("Point-to-Point")
            }

            QQC2.TabButton {
                text: i18n("Proxy")
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: tabBar.currentIndex

            // Point-to-Point
            Kirigami.FormLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("Authentication")
                }

                QQC2.Label {
                    Layout.fillWidth: true

                    text: i18n("Allow following authentication methods:")
                    wrapMode: Text.WordWrap
                }

                QQC2.CheckBox {
                    text: i18n("PAP")

                    checked: dialog.setting.allowPap
                    onToggled: dialog.setting.allowPap = checked
                }

                QQC2.CheckBox {
                    text: i18n("CHAP")

                    checked: dialog.setting.allowChap
                    onToggled: dialog.setting.allowChap = checked
                }

                QQC2.CheckBox {
                    text: i18n("MSCHAP")

                    checked: dialog.setting.allowMschap
                    onToggled: dialog.setting.allowMschap = checked
                }

                QQC2.CheckBox {
                    text: i18n("MSCHAPv2")

                    checked: dialog.setting.allowMschapv2
                    onToggled: dialog.setting.allowMschapv2 = checked
                }

                QQC2.CheckBox {
                    text: i18n("EAP")

                    checked: dialog.setting.allowEap
                    onToggled: dialog.setting.allowEap = checked
                }

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("Encryption")
                }

                QQC2.CheckBox {
                    text: i18n("Use MPPE Encryption")

                    QQC2.ToolTip.text: i18n("Use Microsoft Point-to-Point Encryption")
                    QQC2.ToolTip.visible: hovered
                    QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                    checked: dialog.setting.useMppe
                    onToggled: dialog.setting.useMppe = checked
                }

                QQC2.ComboBox {
                    Kirigami.FormData.label: i18n("Crypto:")
                    Layout.fillWidth: true

                    enabled: dialog.setting.useMppe

                    model: [i18nc("like in use Any configuration", "Any"), i18n("128 bit"), i18n("40 bit")]

                    currentIndex: dialog.setting.mppeCrypto
                    onActivated: dialog.setting.mppeCrypto = currentIndex
                }

                QQC2.CheckBox {
                    text: i18n("Use stateful encryption")

                    enabled: dialog.setting.useMppe

                    checked: dialog.setting.statefulEncryption
                    onToggled: dialog.setting.statefulEncryption = checked
                }

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("Compression")
                }

                QQC2.CheckBox {
                    text: i18n("Allow BSD compression")

                    checked: dialog.setting.allowBsdCompression
                    onToggled: dialog.setting.allowBsdCompression = checked
                }

                QQC2.CheckBox {
                    text: i18n("Allow Deflate compression")

                    checked: dialog.setting.allowDeflateCompression
                    onToggled: dialog.setting.allowDeflateCompression = checked
                }

                QQC2.CheckBox {
                    text: i18n("Allow TCP header compression")

                    checked: dialog.setting.allowTcpHeaderCompression
                    onToggled: dialog.setting.allowTcpHeaderCompression = checked
                }

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("Echo")
                }

                QQC2.CheckBox {
                    text: i18n("Send PPP echo packets")

                    checked: dialog.setting.sendPppEchoPackets
                    onToggled: dialog.setting.sendPppEchoPackets = checked
                }

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("Misc")
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.largeSpacing

                    QQC2.CheckBox {
                        Layout.fillWidth: true

                        text: i18n("Use custom unit number:")

                        checked: dialog.setting.useCustomUnitNumber
                        onToggled: dialog.setting.useCustomUnitNumber = checked
                    }

                    QQC2.SpinBox {
                        enabled: dialog.setting.useCustomUnitNumber

                        from: 0
                        to: 65535

                        value: dialog.setting.customUnitNumber
                        onValueModified: dialog.setting.customUnitNumber = value
                    }
                }
            }

            // Proxy
            Kirigami.FormLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("General")
                }

                QQC2.TextField {
                    Kirigami.FormData.label: i18n("Address:")
                    Layout.fillWidth: true

                    text: dialog.setting.proxyAddress
                    onTextEdited: dialog.setting.proxyAddress = text
                }

                QQC2.SpinBox {
                    Kirigami.FormData.label: i18n("Port:")

                    from: 0
                    to: 65535

                    value: dialog.setting.proxyPort
                    onValueModified: dialog.setting.proxyPort = value
                }

                Item {
                    Kirigami.FormData.isSection: true
                    Kirigami.FormData.label: i18n("Credentials")
                }

                QQC2.TextField {
                    Kirigami.FormData.label: i18n("Username:")
                    Layout.fillWidth: true

                    text: dialog.setting.proxyUsername
                    onTextEdited: dialog.setting.proxyUsername = text
                }

                PlasmaNMQ.PasswordField {
                    Layout.fillWidth: true

                    password: dialog.setting.proxyPassword
                    onPasswordEdited: password => dialog.setting.proxyPassword = password
                }
            }
        }
    }
}
