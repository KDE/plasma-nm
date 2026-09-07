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

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        Item {
            Kirigami.FormData.isSection: true
            Kirigami.FormData.label: i18n("Optional")
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Username:")
            Layout.fillWidth: true

            text: root.setting.username
            onTextEdited: root.setting.username = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("NT Domain:")
            Layout.fillWidth: true

            text: root.setting.ntDomain
            onTextEdited: root.setting.ntDomain = text
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

        QQC2.CheckBox {
            Layout.fillWidth: true

            text: i18n("Ignore certificate warnings")

            checked: root.setting.ignoreCertificateWarnings
            onToggled: root.setting.ignoreCertificateWarnings = checked
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.sstpPassword
        passwordOption: root.setting.sstpPasswordOption

        onPasswordEdited: password => root.setting.sstpPassword = password
        onPasswordOptionEdited: option => root.setting.sstpPasswordOption = option
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

    SstpAdvanced {
        id: advancedDialog

        setting: root.setting
    }

    FileDialog {
        id: caCertDialog

        onAccepted: root.setting.caCert = selectedFile
    }
}
