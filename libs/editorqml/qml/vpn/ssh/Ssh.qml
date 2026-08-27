/*
    SPDX-FileCopyrightText: 2015 Jan Grulich <jgrulich@redhat.com>
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

    spacing: Kirigami.Units.largeSpacing

    readonly property var ipv4Pattern: /^[0-9.]{0,15}$/
    readonly property var ipv6Pattern: /^[0-9A-Fa-f:.]{0,45}$/

    Kirigami.FormLayout {
        Layout.fillWidth: true

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("General")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Network Settings")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Remote IP Address:")
            Layout.fillWidth: true

            validator: RegularExpressionValidator {
                regularExpression: root.ipv4Pattern
            }

            text: root.setting.remoteIp
            onTextEdited: root.setting.remoteIp = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Local IP Address:")
            Layout.fillWidth: true

            validator: RegularExpressionValidator {
                regularExpression: root.ipv4Pattern
            }

            text: root.setting.localIp
            onTextEdited: root.setting.localIp = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Netmask:")
            Layout.fillWidth: true

            validator: RegularExpressionValidator {
                regularExpression: root.ipv4Pattern
            }

            text: root.setting.netmask
            onTextEdited: root.setting.netmask = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("IPv6 Network Settings")
        }

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use IPv6")

            checked: root.setting.useIpv6
            onToggled: root.setting.useIpv6 = checked
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Remote IP Address:")
            Layout.fillWidth: true

            enabled: root.setting.useIpv6

            validator: RegularExpressionValidator {
                regularExpression: root.ipv6Pattern
            }

            text: root.setting.remoteIpv6
            onTextEdited: root.setting.remoteIpv6 = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Local IP Address:")
            Layout.fillWidth: true

            enabled: root.setting.useIpv6

            validator: RegularExpressionValidator {
                regularExpression: root.ipv6Pattern
            }

            text: root.setting.localIpv6
            onTextEdited: root.setting.localIpv6 = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Prefix:")
            Layout.fillWidth: true

            enabled: root.setting.useIpv6

            text: root.setting.netmaskIpv6
            onTextEdited: root.setting.netmaskIpv6 = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Authentication")
        }

        QQC2.ComboBox {
            Kirigami.FormData.label: i18n("Type:")
            Layout.fillWidth: true

            model: [i18n("SSH Agent"), i18n("Password"), i18n("Key authentication")]

            currentIndex: root.setting.authType
            onActivated: root.setting.authType = currentIndex
        }

        RowLayout {
            Kirigami.FormData.label: i18n("SSH Key File:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            visible: root.setting.authType === 2 // Key

            QQC2.TextField {
                id: keyFileField

                Layout.fillWidth: true

                text: root.setting.keyFile
                onTextEdited: root.setting.keyFile = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: keyFileDialog.open()
            }
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.authType === 1 // Password

        showPasswordOptions: true

        password: root.setting.sshPassword
        passwordOption: root.setting.sshPasswordOption

        onPasswordEdited: password => root.setting.sshPassword = password
        onPasswordOptionEdited: option => root.setting.sshPasswordOption = option
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

    SshAdvanced {
        id: advancedDialog

        setting: root.setting
    }

    FileDialog {
        id: keyFileDialog

        onAccepted: root.setting.keyFile = selectedFile
    }
}
