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

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Gateway:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("IP/hostname of IPsec gateway.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("User name:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("User name for the connection.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.user
            onTextEdited: root.setting.user = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        passwordLabel: i18n("User password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.userPassword
        passwordOption: root.setting.userPasswordOption

        onPasswordEdited: password => root.setting.userPassword = password
        onPasswordOptionEdited: option => root.setting.userPasswordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Group name:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Group name")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.group
            onTextEdited: root.setting.group = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        passwordLabel: i18n("Group password:")
        showPasswordOptions: true
        showNotRequired: true

        password: root.setting.groupPassword
        passwordOption: root.setting.groupPasswordOption

        onPasswordEdited: password => root.setting.groupPassword = password
        onPasswordOptionEdited: option => root.setting.groupPasswordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Use hybrid authentication")

            QQC2.ToolTip.text: i18n("Enable hybrid authentication, i.e. use certificate in addition to password.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            checked: root.setting.useHybridAuth
            onToggled: root.setting.useHybridAuth = checked
        }

        RowLayout {
            Kirigami.FormData.label: i18n("CA file:")
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            enabled: root.setting.useHybridAuth

            QQC2.TextField {
                Layout.fillWidth: true

                QQC2.ToolTip.text: i18n("CA certificate in PEM format.")
                QQC2.ToolTip.visible: hovered
                QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

                text: root.setting.caFile
                onTextEdited: root.setting.caFile = text
            }

            QQC2.Button {
                icon.name: "document-open"
                onClicked: caFileDialog.open()
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

    VpncAdvanced {
        id: advancedDialog

        setting: root.setting
    }

    FileDialog {
        id: caFileDialog

        onAccepted: root.setting.caFile = selectedFile
    }
}
