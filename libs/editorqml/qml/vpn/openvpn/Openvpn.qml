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

    readonly property int typeCertificates: 0
    readonly property int typeStaticKey: 1
    readonly property int typePassword: 2
    readonly property int typeCertsPassword: 3

    readonly property bool usesCertificates: root.setting.connectionType === root.typeCertificates || root.setting.connectionType === root.typeCertsPassword
    readonly property bool usesPassword: root.setting.connectionType === root.typePassword || root.setting.connectionType === root.typeCertsPassword
    readonly property bool usesStaticKey: root.setting.connectionType === root.typeStaticKey
    readonly property bool usesCaCert: root.usesCertificates || root.setting.connectionType === root.typePassword

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Remote gateways, with optional port and protocol (e.g. ovpn.corp.com:1194:udp).")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Connection type:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Select an authentication mode.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18n("Certificates (TLS)"), i18n("Static Key"), i18n("Password"), i18n("Password with Certificates (TLS)")]

            currentIndex: root.setting.connectionType
            onActivated: root.setting.connectionType = currentIndex
        }

        RowLayout {
            Kirigami.FormData.label: i18n("CA Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesCaCert

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Certificate authority (CA) file in .pem format.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.caCert
                onTextEdited: root.setting.caCert = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: caCertDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("User Certificate:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesCertificates

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Local peer's signed certificate in .pem format (signed by CA of CA Certificate).")
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

                QQC2.ToolTip.text: i18n("Local peer's private key in .pem format.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.privateKey
                onTextEdited: root.setting.privateKey = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: privateKeyDialog.open()
            }
        }

        RowLayout {
            Kirigami.FormData.label: i18n("Static Key:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.usesStaticKey

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("Pre-shared file for Static Key encryption mode (non-TLS).")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.staticKey
                onTextEdited: root.setting.staticKey = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: staticKeyDialog.open()
            }
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Key Direction:")
            Layout.fillWidth: true

            visible: root.usesStaticKey

            QQC2.ToolTip.text: i18n("If key direction is used, it must be the opposite of that used on the VPN peer. For example, if the peer uses '1', this connection must use '0'. If you are unsure what value to use, contact your system administrator.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            model: [i18n("None"), i18n("0"), i18n("1")]

            currentIndex: root.setting.keyDirection
            onActivated: root.setting.keyDirection = currentIndex
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Local IP Address:")
            Layout.fillWidth: true

            visible: root.usesStaticKey

            QQC2.ToolTip.text: i18n("IP address of the local VPN endpoint.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.localIp
            onTextEdited: root.setting.localIp = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Remote IP Address:")
            Layout.fillWidth: true

            visible: root.usesStaticKey

            QQC2.ToolTip.text: i18n("IP address of the remote VPN endpoint.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.remoteIp
            onTextEdited: root.setting.remoteIp = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Username:")
            Layout.fillWidth: true

            visible: root.usesPassword

            QQC2.ToolTip.text: i18n("Username passed to OpenVPN when prompted for it.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.username
            onTextEdited: root.setting.username = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.usesPassword

        passwordLabel: i18n("Password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.password
        passwordOption: root.setting.passwordOption

        onPasswordEdited: password => root.setting.password = password
        onPasswordOptionEdited: option => root.setting.passwordOption = option
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.usesCertificates

        passwordLabel: i18n("Private Key Password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.privateKeyPassword
        passwordOption: root.setting.privateKeyPasswordOption

        onPasswordEdited: password => root.setting.privateKeyPassword = password
        onPasswordOptionEdited: option => root.setting.privateKeyPasswordOption = option
    }

    RowLayout {
        Layout.fillWidth: true

        Item {
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18nc("@action:button", "Advanced…")

            onClicked: advancedDialog.open()
        }
    }

    Item {
        Layout.fillHeight: true
    }

    OpenvpnAdvanced {
        id: advancedDialog

        setting: root.setting.advanced
    }

    FileDialog {
        id: caCertDialog

        onAccepted: root.setting.caCert = selectedFile
    }

    FileDialog {
        id: userCertDialog

        onAccepted: root.setting.userCert = selectedFile
    }

    FileDialog {
        id: privateKeyDialog

        onAccepted: root.setting.privateKey = selectedFile
    }

    FileDialog {
        id: staticKeyDialog

        onAccepted: root.setting.staticKey = selectedFile
    }
}
