/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.FormLayout {
    id: root

    required property Security8021xSetting setting

    visible: setting.showFast

    QQC2.TextField {
        Layout.fillWidth: true

        text: root.setting.fastAnonIdentity
        onTextEdited: root.setting.fastAnonIdentity = text

        Kirigami.FormData.label: i18nc("@label:textbox", "Anonymous identity:")
    }

    RowLayout {
        Layout.fillWidth: true

        QQC2.CheckBox {
            text: i18n("Automatic PAC provisioning")

            checked: root.setting.fastAllowPacProvisioning
            onToggled: root.setting.fastAllowPacProvisioning = checked
        }

        QQC2.ComboBox {
            Layout.fillWidth: true

            enabled: root.setting.fastAllowPacProvisioning

            model: [i18nc("@item:inlist authentication mode", "Anonymous"), i18nc("@item:inlist authentication mode", "Authenticated"), i18nc("@item:inlist authentication mode", "Both")]

            currentIndex: root.setting.fastPacMethod
            onActivated: root.setting.fastPacMethod = currentIndex
        }

        Kirigami.FormData.label: ""
    }

    RowLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Layout.fillWidth: true

            text: root.setting.fastPacFile
            placeholderText: "*.pac"

            onTextEdited: root.setting.fastPacFile = text
        }

        QQC2.Button {
            icon.name: "document-open"
            onClicked: pacFileDialog.open()
        }

        Kirigami.FormData.label: i18nc("@label PAC stands for Proxy Auto-Configuration and should not be translated", "PAC file:")
    }

    QQC2.ComboBox {
        Layout.fillWidth: true

        model: [
            {
                text: "GTC",
                value: Security8021xSetting.AuthMethodGtc
            },
            {
                text: "MSCHAPv2",
                value: Security8021xSetting.AuthMethodMschapv2
            }
        ]

        textRole: "text"

        currentIndex: model.findIndex(item => item.value === root.setting.fastInnerAuth)

        onActivated: root.setting.fastInnerAuth = model[currentIndex].value

        Kirigami.FormData.label: i18n("Inner authentication:")
    }
    QQC2.TextField {
        Layout.fillWidth: true

        text: root.setting.fastUsername
        onTextEdited: root.setting.fastUsername = text

        Kirigami.FormData.label: i18n("Username:")
    }

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.fastPassword
        passwordOption: root.setting.fastPasswordOption

        onPasswordChanged: root.setting.fastPassword = password
        onPasswordOptionChanged: root.setting.fastPasswordOption = passwordOption
    }

    FileDialog {
        id: pacFileDialog

        nameFilters: [i18nc("@item:filefilter", "PAC files (*.pac)")]

        onAccepted: root.setting.fastPacFile = selectedFile
    }
}
