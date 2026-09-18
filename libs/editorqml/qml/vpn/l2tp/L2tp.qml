/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

ColumnLayout {
    id: root

    required property var setting

    readonly property int authPassword: 0
    readonly property int authTls: 1

    readonly property bool usesCertificates: root.setting.authType === root.authTls

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("L2TP server IP or name.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Authentication type:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Select an authentication mode.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18n("Password"), i18n("Certificates (TLS)")]

            currentIndex: root.setting.authType
            onActivated: root.setting.authType = currentIndex
        }

        Item {
            Kirigami.FormData.isSection: true
        }
        QQC2.TextField {
            Kirigami.FormData.label: i18n("Username:")
            Layout.fillWidth: true

            visible: !root.usesCertificates

            QQC2.ToolTip.text: i18n("Set the name used for authenticating the local system to the peer to <name>.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.username
            onTextEdited: root.setting.username = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: !root.usesCertificates

        passwordLabel: i18n("Password:")
        showPasswordOptions: true

        password: root.setting.password
        passwordOption: root.setting.passwordOption

        onPasswordEdited: password => root.setting.password = password
        onPasswordOptionEdited: option => root.setting.passwordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Kirigami.FormData.label: i18n("NT Domain:")
            Layout.fillWidth: true

            visible: !root.usesCertificates

            QQC2.ToolTip.text: i18n("Append the domain name <domain> to the local host name for authentication purposes.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.domain
            onTextEdited: root.setting.domain = text
        }

        RowLayout {
            Kirigami.FormData.label: i18n("CA Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesCertificates

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Certificate authority (CA) file in .pem, .der, .crt, .cer or .p12 formats.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.userCa
                onTextEdited: root.setting.userCa = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: userCaDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("User Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesCertificates

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Certificate in .pem, .der, .crt, .cer or .p12 formats.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.userCert
                onTextEdited: root.setting.userCert = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: userCertDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Private Key:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesCertificates

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Private key in .pem, .der, .key, .pk8 or .p12 formats.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.userKey
                onTextEdited: root.setting.userKey = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: userKeyDialog.open()
            }
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.usesCertificates

        passwordLabel: i18n("Private Key Password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.userKeyPassword
        passwordOption: root.setting.userKeyPasswordOption

        onPasswordEdited: password => root.setting.userKeyPassword = password
        onPasswordOptionEdited: option => root.setting.userKeyPasswordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use L2TP ephemeral source port")

            checked: root.setting.ephemeralPort
            onToggled: root.setting.ephemeralPort = checked
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: Kirigami.Units.smallSpacing

        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18nc("@action:button", "IPsec Settings…")

            enabled: root.setting.ipsecDaemonAvailable

            QQC2.ToolTip.text: i18n("No IPsec daemon (libreswan or strongSwan) was found.")
            QQC2.ToolTip.visible: hovered && !enabled
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            onClicked: ipsecDialog.open()
        }

        QQC2.Button {
            text: i18nc("@action:button", "PPP Settings…")

            onClicked: pppDialog.open()
        }
    }

    Item {
        Layout.fillHeight: true
    }

    L2tpIpsec {
        id: ipsecDialog

        setting: root.setting
    }

    L2tpPpp {
        id: pppDialog

        setting: root.setting
    }

    FileDialog {
        id: userCaDialog

        onAccepted: root.setting.userCa = selectedFile
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
