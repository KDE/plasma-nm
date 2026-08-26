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

    visible: setting.showTtls

    QQC2.TextField {
        visible: root.setting.securityType === Security8021xSetting.Ethernet

        text: root.setting.ttlsSubjectMatch
        onTextEdited: root.setting.ttlsSubjectMatch = text

        Kirigami.FormData.label: i18n("Subject match:")
    }

    RowLayout {
        visible: root.setting.securityType === Security8021xSetting.Ethernet

        QQC2.TextField {
            id: ttlsAltSubjectField
            Layout.fillWidth: true

            text: root.setting.ttlsAltSubjectMatches
            onTextEdited: root.setting.ttlsAltSubjectMatches = text
        }

        QQC2.Button {
            text: "…"
            onClicked: {
                ttlsAlternativeSubjectDialog.addresses = ttlsAltSubjectField.text.length > 0 ? ttlsAltSubjectField.text.split(",").filter(s => s.length > 0) : [];
                ttlsAlternativeSubjectDialog.open();
            }
        }

        Kirigami.FormData.label: i18n("Alternative subject matches:")
    }

    AlternativeSubjectMatch {
        id: ttlsAlternativeSubjectDialog

        onAccepted: {
            root.setting.ttlsAltSubjectMatches = addresses.join(",");
            ttlsAltSubjectField.text = root.setting.ttlsAltSubjectMatches;
        }
    }

    RowLayout {
        visible: root.setting.securityType === Security8021xSetting.Ethernet

        Kirigami.FormData.label: i18n("Connect to these servers:")

        QQC2.TextField {
            id: ttlsServersField
            Layout.fillWidth: true

            text: root.setting.ttlsConnectToServers
            onTextEdited: root.setting.ttlsConnectToServers = text
        }

        QQC2.Button {
            text: i18n("…")
            onClicked: {
                ttlsConnectToServersDialog.addresses = ttlsServersField.text.length > 0 ? ttlsServersField.text.split(",").filter(s => s.length > 0) : [];
                ttlsConnectToServersDialog.open();
            }
        }
    }

    DnsList {
        id: ttlsConnectToServersDialog

        addressPattern: serverNamePattern

        onAccepted: {
            root.setting.ttlsConnectToServers = addresses.join(",");
        }
    }

    QQC2.TextField {
        Kirigami.FormData.label: i18n("Anonymous identity:")
        Layout.fillWidth: true

        text: root.setting.ttlsAnonIdentity

        onTextEdited: root.setting.ttlsAnonIdentity = text
    }

    QQC2.TextField {
        Kirigami.FormData.label: i18n("Domain:")
        Layout.fillWidth: true

        text: root.setting.ttlsDomain

        onTextEdited: root.setting.ttlsDomain = text
    }

    RowLayout {
        Kirigami.FormData.label: i18n("CA certificate:")
        Layout.fillWidth: true

        QQC2.TextField {
            Layout.fillWidth: true

            text: root.setting.ttlsCaCert
            placeholderText: i18n("*.der *.pem *.crt *.cer")

            onTextEdited: root.setting.ttlsCaCert = text
        }

        QQC2.Button {
            icon.name: "document-open"
            onClicked: ttlsCaCertDialog.open()
        }
    }

    QQC2.ComboBox {
        Kirigami.FormData.label: i18n("Inner authentication:")
        Layout.fillWidth: true

        model: ["PAP", "MSCHAP", "MSCHAPv2", "CHAP"]

        currentIndex: root.setting.ttlsInnerAuth

        onActivated: root.setting.ttlsInnerAuth = currentIndex
    }

    QQC2.TextField {
        Kirigami.FormData.label: i18n("Username:")
        Layout.fillWidth: true

        text: root.setting.ttlsUsername

        onTextEdited: root.setting.ttlsUsername = text
    }

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.ttlsPassword
        passwordOption: root.setting.ttlsPasswordOption

        onPasswordEdited: root.setting.ttlsPassword = password
        onPasswordOptionEdited: root.setting.ttlsPasswordOption = option
    }

    FileDialog {
        id: ttlsCaCertDialog

        nameFilters: [i18n("Certificates (*.der *.pem *.crt *.cer)")]

        onAccepted: root.setting.ttlsCaCert = selectedFile
    }
}
