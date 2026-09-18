/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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

    readonly property int authPrivateKey: 0
    readonly property int authSshAgent: 1
    readonly property int authSmartcard: 2
    readonly property int authEap: 3
    readonly property int authEapTtls: 4

    readonly property bool usesEap: root.setting.authMethod === root.authEap || root.setting.authMethod === root.authEapTtls
    readonly property bool usesCertificate: root.setting.authMethod === root.authPrivateKey || root.setting.authMethod === root.authSshAgent

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Gateway")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.gatewayCertificate
                onTextEdited: root.setting.gatewayCertificate = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: gatewayCertificateDialog.open()
            }
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Identity:")
            Layout.fillWidth: true

            text: root.setting.remoteIdentity
            onTextEdited: root.setting.remoteIdentity = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Authentication")
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Method:")
            Layout.fillWidth: true

            model: [i18n("Certificate/private key"), i18n("Certificate/ssh-agent"), i18n("Smartcard"), i18n("EAP"), i18n("EAP-TTLS")]

            currentIndex: root.setting.authMethod
            onActivated: root.setting.authMethod = currentIndex
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesCertificate

            QQC2.TextField {
                Layout.fillWidth: true

                text: root.setting.userCertificate
                onTextEdited: root.setting.userCertificate = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: userCertificateDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Private key:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.setting.authMethod === root.authPrivateKey

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

        QQC2.Label {
            Layout.fillWidth: true

            visible: root.setting.authMethod === root.authSmartcard

            text: i18n("The PIN is asked for when the connection is activated.")
            wrapMode: Text.WordWrap
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Username:")
            Layout.fillWidth: true

            visible: root.usesEap

            text: root.setting.username
            onTextEdited: root.setting.username = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.usesEap

        passwordLabel: i18n("User Password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.userPassword
        passwordOption: root.setting.userPasswordOption

        onPasswordEdited: password => root.setting.userPassword = password
        onPasswordOptionEdited: option => root.setting.userPasswordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Options")
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Request an inner IP address")

            checked: root.setting.requestInnerIp
            onToggled: root.setting.requestInnerIp = checked
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Enforce UDP encapsulation")

            checked: root.setting.enforceUdpEncapsulation
            onToggled: root.setting.enforceUdpEncapsulation = checked
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use IP compression")

            checked: root.setting.useIpCompression
            onToggled: root.setting.useIpCompression = checked
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Enable Custom Cipher Proposals")

            checked: root.setting.useCustomProposals
            onToggled: root.setting.useCustomProposals = checked
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("IKE:")
            Layout.fillWidth: true

            enabled: root.setting.useCustomProposals

            QQC2.ToolTip.text: i18n("A list of proposals for IKE separated by \";\"")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.ike
            onTextEdited: root.setting.ike = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("ESP:")
            Layout.fillWidth: true

            enabled: root.setting.useCustomProposals

            QQC2.ToolTip.text: i18n("A list of proposals for ESP separated by \";\"")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.esp
            onTextEdited: root.setting.esp = text
        }
    }

    Item {
        Layout.fillHeight: true
    }

    FileDialog {
        id: gatewayCertificateDialog

        onAccepted: root.setting.gatewayCertificate = selectedFile
    }

    FileDialog {
        id: userCertificateDialog

        onAccepted: root.setting.userCertificate = selectedFile
    }

    FileDialog {
        id: userKeyDialog

        onAccepted: root.setting.userKey = selectedFile
    }
}
