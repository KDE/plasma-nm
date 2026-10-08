/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root

    required property var setting

    readonly property list<string> keyStorageOptions: [
        i18n("Store secret for this user only (encrypted)"),
        i18n("Store secret for all users (not encrypted)"),
        i18n("This secret is not required")
    ]

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("Interface")
            horizontalAlignment: Text.AlignHCenter
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Name:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Name of the network interface this connection creates.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.interfaceName
            onTextEdited: root.setting.interfaceName = text
        }

        Kirigami.PasswordField {
            Kirigami.FormData.label: i18n("Private Key:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("The private key of this interface, as a base64 string.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            validator: root.setting.keyValidator

            text: root.setting.privateKey
            onTextEdited: root.setting.privateKey = text
        }

        QQC2.ComboBox {
            Layout.fillWidth: true

            model: root.keyStorageOptions

            currentIndex: root.setting.privateKeyOption
            onActivated: root.setting.privateKeyOption = currentIndex
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("Listen Port:")

            QQC2.ToolTip.text: i18n("Port to listen on. Automatic lets WireGuard choose one.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            from: 0
            to: 65535

            textFromValue: (value, locale) => value === 0 ? i18n("Automatic") : String(value)
            valueFromText: (text, locale) => text === i18n("Automatic") ? 0 : Number.fromLocaleString(locale, text.replace(/[^0-9]/g, ""))

            value: root.setting.listenPort
            onValueModified: root.setting.listenPort = value
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("fwmark:")

            QQC2.ToolTip.text: i18n("Firewall mark to apply to outgoing packets. Zero disables it.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            from: 0
            to: 2147483647

            value: root.setting.fwmark
            onValueModified: root.setting.fwmark = value
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("MTU:")

            from: 0
            to: 65535

            textFromValue: (value, locale) => value === 0 ? i18n("Automatic") : String(value)
            valueFromText: (text, locale) => text === i18n("Automatic") ? 0 : Number.fromLocaleString(locale, text.replace(/[^0-9]/g, ""))

            value: root.setting.mtu
            onValueModified: root.setting.mtu = value
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Add routes for the peers' allowed IPs")

            QQC2.ToolTip.text: i18n("Turn this off to route the tunnel yourself.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: root.setting.peerRoutes
            onToggled: root.setting.peerRoutes = checked
        }
    }

    RowLayout {
        Layout.fillWidth: true

        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18nc("@action:button", "Peers…")

            onClicked: {
                root.setting.peers.beginEdit();
                peersDialog.open();
            }
        }
    }

    Item {
        Layout.fillHeight: true
    }

    WireguardPeers {
        id: peersDialog

        setting: root.setting
    }
}
