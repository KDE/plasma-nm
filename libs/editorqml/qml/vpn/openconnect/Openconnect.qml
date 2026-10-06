/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

ColumnLayout {
    id: root

    required property var setting

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("General")
            horizontalAlignment: Text.AlignHCenter
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Address of the VPN gateway, optionally including the port.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("VPN Protocol:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Which VPN the gateway speaks. All of these are handled by openconnect.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18n("Cisco AnyConnect"), i18n("Juniper Network Connect"), i18n("PAN Global Protect"), i18n("Pulse Connect Secure"), i18n("F5 BIG-IP SSL VPN"), i18n("Fortinet SSL VPN"), i18n("Array SSL VPN")]

            currentIndex: root.setting.protocol
            onActivated: root.setting.protocol = currentIndex
        }

        RowLayout {
            Kirigami.FormData.label: i18n("CA Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.caCert
                onTextEdited: root.setting.caCert = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: caCertDialog.open()
            }
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Proxy:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Proxy to reach the gateway through, as a URL.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.proxy
            onTextEdited: root.setting.proxy = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("User Agent:")
            Layout.fillWidth: true

            text: root.setting.userAgent
            onTextEdited: root.setting.userAgent = text
        }

        RowLayout {
            Kirigami.FormData.label: i18n("CSD Wrapper Script:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            enabled: root.setting.allowTrojan

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.csdWrapper
                onTextEdited: root.setting.csdWrapper = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: csdWrapperDialog.open()
            }
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Allow Cisco Secure Desktop trojan")

            QQC2.ToolTip.text: i18n("Run the host-check program the gateway asks for. The wrapper script above replaces it.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: root.setting.allowTrojan
            onToggled: root.setting.allowTrojan = checked
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Reported OS:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Operating system to report to the gateway. Leave at the default to report the real one.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18nc("@item:inlistbox report the real operating system", "Default"), i18n("GNU/Linux"), i18n("GNU/Linux 64-bit"), i18n("Windows"), i18n("Mac OS X"), i18n("Android"), i18n("Apple iOS")]

            currentIndex: root.setting.reportedOs
            onActivated: root.setting.reportedOs = currentIndex
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Reported Version:")
            Layout.fillWidth: true

            text: root.setting.reportedVersion
            onTextEdited: root.setting.reportedVersion = text
        }

        QQC2.Label {
            Kirigami.FormData.isSection: true

            text: i18n("Certificate Authentication")
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Machine Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.machineCert
                onTextEdited: root.setting.machineCert = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: machineCertDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Machine Private Key:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.machineKey
                onTextEdited: root.setting.machineKey = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: machineKeyDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("User Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.userCert
                onTextEdited: root.setting.userCert = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: userCertDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("User Private Key:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.userKey
                onTextEdited: root.setting.userKey = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: userKeyDialog.open()
            }
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use FSID for key passphrase")

            checked: root.setting.useFsid
            onToggled: root.setting.useFsid = checked
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Prevent user from manually accepting invalid certificates")

            checked: root.setting.preventInvalidCert
            onToggled: root.setting.preventInvalidCert = checked
        }
    }

    RowLayout {
        Layout.fillWidth: true

        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18nc("@action:button", "Token Authentication…")

            onClicked: tokenDialog.open()
        }
    }

    Item {
        Layout.fillHeight: true
    }

    OpenconnectToken {
        id: tokenDialog

        setting: root.setting
    }

    FileDialog {
        id: caCertDialog

        onAccepted: root.setting.caCert = selectedFile
    }

    FileDialog {
        id: csdWrapperDialog

        onAccepted: root.setting.csdWrapper = selectedFile
    }

    FileDialog {
        id: machineCertDialog

        onAccepted: root.setting.machineCert = selectedFile
    }

    FileDialog {
        id: machineKeyDialog

        onAccepted: root.setting.machineKey = selectedFile
    }

    FileDialog {
        id: userCertDialog

        onAccepted: root.setting.userCert = selectedFile
    }

    FileDialog {
        id: userKeyDialog

        onAccepted: root.setting.userKey = selectedFile
    }
}
