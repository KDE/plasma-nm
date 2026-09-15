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

    parent: QQC2.Overlay.overlay

    header: null

    standardButtons: Kirigami.Dialog.Ok | Kirigami.Dialog.Cancel

    preferredWidth: Kirigami.Units.gridUnit * 34
    preferredHeight: Kirigami.Units.gridUnit * 24

    Kirigami.FormLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing

        wideMode: true

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Identification")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Domain:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("(NT-)Domain name for authentication.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: dialog.setting.domain
            onTextEdited: dialog.setting.domain = text
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Vendor:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Vendor of your IPsec gateway.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("VPNC vendor name", "Cisco"), i18nc("VPNC vendor name", "Netscreen"), i18nc("VPNC vendor name", "Fortigate")]

            currentIndex: dialog.setting.vendor
            onActivated: dialog.setting.vendor = currentIndex
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Transport and Security")
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Encryption method:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Encryption method.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("VPNC encryption method", "Secure (default)"), i18nc("VPNC encryption method", "Weak (DES encryption, use with caution)"), i18nc("VPNC encryption method", "None (completely insecure)")]

            currentIndex: dialog.setting.encryption
            onActivated: dialog.setting.encryption = currentIndex
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("NAT traversal:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("NAT traversal method to use.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("NAT traversal method", "NAT-T when available (default)"), i18nc("NAT traversal method", "NAT-T always"), i18nc("NAT traversal method", "Cisco UDP"), i18nc("NAT traversal method", "Disabled")]

            currentIndex: dialog.setting.natTraversal
            onActivated: dialog.setting.natTraversal = currentIndex
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("IKE DH Group:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Name of the IKE DH group.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("IKE DH group", "DH Group 1"), i18nc("IKE DH group", "DH Group 2 (default)"), i18nc("IKE DH group", "DH Group 5")]

            currentIndex: dialog.setting.dhGroup
            onActivated: dialog.setting.dhGroup = currentIndex
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Perfect Forward Secrecy:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Diffie-Hellman group to use for PFS.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("Perfect Forward Secrecy", "Server (default)"), i18nc("Perfect Forward Secrecy", "None"), i18nc("Perfect Forward Secrecy", "DH Group 1"), i18nc("Perfect Forward Secrecy", "DH Group 2"), i18nc("Perfect Forward Secrecy", "DH Group 5")]

            currentIndex: dialog.setting.perfectForwardSecrecy
            onActivated: dialog.setting.perfectForwardSecrecy = currentIndex
        }

        QQC2.SpinBox {
            Kirigami.FormData.label: i18n("Local Port:")

            QQC2.ToolTip.text: i18n("Local port to use (0-65535). 0 (default value) means random port. 500 is vpnc's default.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            from: 0
            to: 65535

            // 0 shows as "Random", like the spin box's specialValueText did.
            textFromValue: (value, locale) => value === 0 ? i18n("Random") : Number(value).toLocaleString(locale, 'f', 0)
            valueFromText: (text, locale) => text === i18n("Random") ? 0 : Number.fromLocaleString(locale, text)

            value: dialog.setting.localPort
            onValueModified: dialog.setting.localPort = value
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Disable dead peer detection")

            QQC2.ToolTip.text: i18n("Disable sending DPD packets (set timeout to 0).")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: dialog.setting.disableDeadPeerDetection
            onToggled: dialog.setting.disableDeadPeerDetection = checked
        }
    }
}
