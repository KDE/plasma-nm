/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.FormLayout {
    id: root

    required property Security8021xSetting setting

    visible: setting.showPeap

    QQC2.TextField {
        Kirigami.FormData.label: i18n("Anonymous identity:")
        Layout.fillWidth: true

        text: root.setting.peapAnonIdentity
        onTextEdited: root.setting.peapAnonIdentity = text
    }

    QQC2.TextField {
        Kirigami.FormData.label: i18n("Domain:")
        Layout.fillWidth: true

        text: root.setting.peapDomain

        onTextEdited: root.setting.peapDomain = text
    }

    RowLayout {
        Kirigami.FormData.label: i18n("CA certificate:")
        Layout.fillWidth: true

        QQC2.TextField {
            Layout.fillWidth: true

            text: root.setting.peapCaCert
            placeholderText: i18n("*.der *.pem *.crt *.cer")

            onTextEdited: root.setting.peapCaCert = text
        }

        QQC2.Button {
            icon.name: "document-open"

            onClicked: peapCaCertDialog.open()
        }
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("PEAP version:")
        Layout.fillWidth: true

        model: [i18n("Automatic"), i18n("Version 0"), i18n("Version 1")]

        currentIndex: root.setting.peapVersion

        onActivated: root.setting.peapVersion = currentIndex
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Inner authentication:")
        Layout.fillWidth: true

        model: ["MSCHAPv2", "MD5", "GTC"]

        currentIndex: root.setting.peapInnerAuth

        onActivated: root.setting.peapInnerAuth = currentIndex
    }

    QQC2.TextField {
        Kirigami.FormData.label: i18n("Username:")
        Layout.fillWidth: true

        text: root.setting.peapUsername

        onTextEdited: root.setting.peapUsername = text
    }

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.peapPassword
        passwordOption: root.setting.peapPasswordOption

        onPasswordChanged: root.setting.peapPassword = password
        onPasswordOptionChanged: root.setting.peapPasswordOption = passwordOption
    }

    FileDialog {
        id: peapCaCertDialog

        nameFilters: [i18n("Certificates (*.der *.pem *.crt *.cer)")]

        onAccepted: root.setting.peapCaCert = selectedFile
    }
}
