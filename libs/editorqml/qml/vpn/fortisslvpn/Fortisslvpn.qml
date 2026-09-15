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

    spacing: Kirigami.Units.largeSpacing

    Kirigami.FormLayout {
        Layout.fillWidth: true

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("General")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("SSLVPN server IP or name.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Authentication")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("User name:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Set the name used for authenticating the local system to the peer to <name>.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.user
            onTextEdited: root.setting.user = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        passwordLabel: i18n("Password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.password
        passwordOption: root.setting.passwordOption

        onPasswordEdited: password => root.setting.password = password
        onPasswordOptionEdited: option => root.setting.passwordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

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
            Kirigami.FormData.label: i18n("User Key:")
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

    FortisslvpnAdvanced {
        id: advancedDialog

        setting: root.setting
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
        id: userKeyDialog

        onAccepted: root.setting.userKey = selectedFile
    }
}
