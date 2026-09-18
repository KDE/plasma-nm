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

    padding: 0

    required property var setting
    readonly property int authPassword: 0
    readonly property bool needsPeerEap: dialog.setting.authType !== dialog.authPassword

    parent: QQC2.Overlay.overlay

    header: null

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 23

    readonly property list<string> revertableProperties: ["allowPap", "allowChap", "allowMschap", "allowMschapv2", "allowEap", "useMppe", "mppeCrypto", "statefulEncryption", "allowBsdCompression", "allowDeflateCompression", "allowTcpHeaderCompression", "useProtocolFieldCompression", "useAddressControlCompression", "sendPppEchoPackets", "useMrru", "mrru", "mru", "mtu"]

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

    Kirigami.FormLayout {
        QQC2.Label {
            Kirigami.FormData.isSection: true

            visible: !dialog.needsPeerEap

            text: i18n("Authentication")
            horizontalAlignment: Text.AlignHCenter
        }

        QQC2.Label {
            Layout.fillWidth: true

            visible: !dialog.needsPeerEap

            text: i18n("Allow following authentication methods:")
            wrapMode: Text.WordWrap
        }

        QQC2.CheckBox {
            visible: !dialog.needsPeerEap

            text: i18n("PAP")

            checked: dialog.setting.allowPap
            onToggled: dialog.setting.allowPap = checked
        }

        QQC2.CheckBox {
            visible: !dialog.needsPeerEap

            text: i18n("CHAP")

            checked: dialog.setting.allowChap
            onToggled: dialog.setting.allowChap = checked
        }

        QQC2.CheckBox {
            visible: !dialog.needsPeerEap

            text: i18n("MSCHAP")

            checked: dialog.setting.allowMschap
            onToggled: dialog.setting.allowMschap = checked
        }

        QQC2.CheckBox {
            visible: !dialog.needsPeerEap

            text: i18n("MSCHAPv2")

            checked: dialog.setting.allowMschapv2
            onToggled: dialog.setting.allowMschapv2 = checked
        }

        QQC2.CheckBox {
            visible: !dialog.needsPeerEap

            text: i18n("EAP")

            checked: dialog.setting.allowEap
            onToggled: dialog.setting.allowEap = checked
        }

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("Encryption")
            horizontalAlignment: Text.AlignHCenter
        }

        QQC2.CheckBox {
            text: i18n("Use MPPE Encryption")

            checked: dialog.setting.useMppe
            onToggled: dialog.setting.useMppe = checked
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Crypto:")
            Layout.fillWidth: true

            enabled: dialog.setting.useMppe

            QQC2.ToolTip.text: i18n("Require the use of MPPE, with 40/128-bit encryption or all.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("like in use Any configuration", "Any"), i18n("128 bit"), i18n("40 bit")]

            currentIndex: dialog.setting.mppeCrypto
            onActivated: dialog.setting.mppeCrypto = currentIndex
        }

        QQC2.CheckBox {
            enabled: dialog.setting.useMppe

            text: i18n("Use stateful encryption")

            QQC2.ToolTip.text: i18n("Allow MPPE to use stateful mode. Stateless mode is still attempted first.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.statefulEncryption
            onToggled: dialog.setting.statefulEncryption = checked
        }

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("Compression")
            horizontalAlignment: Text.AlignHCenter
        }

        QQC2.CheckBox {
            text: i18n("Allow BSD compression")

            QQC2.ToolTip.text: i18n("Allow/disable BSD-Compress compression.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.allowBsdCompression
            onToggled: dialog.setting.allowBsdCompression = checked
        }

        QQC2.CheckBox {
            text: i18n("Allow Deflate compression")

            QQC2.ToolTip.text: i18n("Allow/disable Deflate compression.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.allowDeflateCompression
            onToggled: dialog.setting.allowDeflateCompression = checked
        }

        QQC2.CheckBox {
            text: i18n("Allow TCP header compression")

            checked: dialog.setting.allowTcpHeaderCompression
            onToggled: dialog.setting.allowTcpHeaderCompression = checked
        }

        QQC2.CheckBox {
            text: i18n("Use protocol field compression negotiation")

            checked: dialog.setting.useProtocolFieldCompression
            onToggled: dialog.setting.useProtocolFieldCompression = checked
        }

        QQC2.CheckBox {
            text: i18n("Use Address/Control compression")

            QQC2.ToolTip.text: i18n("Use Address/Control compression in both directions (send and receive).")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.useAddressControlCompression
            onToggled: dialog.setting.useAddressControlCompression = checked
        }

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("Echo")
            horizontalAlignment: Text.AlignHCenter
        }

        QQC2.CheckBox {
            text: i18n("Send PPP echo packets")

            QQC2.ToolTip.text: i18n("Send LCP echo-requests to find out whether peer is alive.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.sendPppEchoPackets
            onToggled: dialog.setting.sendPppEchoPackets = checked
        }

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("Other Settings")
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.largeSpacing

            QQC2.CheckBox {
                Layout.fillWidth: true

                text: i18n("Multilink PPP MRRU:")

                QQC2.ToolTip.text: i18n("Try to negotiate a PPP Multilink on a single connection.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                checked: dialog.setting.useMrru
                onToggled: dialog.setting.useMrru = checked
            }

            QQC2.SpinBox {
                enabled: dialog.setting.useMrru

                from: 1500
                to: 4500

                value: dialog.setting.mrru
                onValueModified: dialog.setting.mrru = value
            }
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("MRU:")

            from: 0
            to: 1500

            value: dialog.setting.mru
            onValueModified: dialog.setting.mru = value
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("MTU:")

            from: 0
            to: 1500

            value: dialog.setting.mtu
            onValueModified: dialog.setting.mtu = value
        }
    }
}
