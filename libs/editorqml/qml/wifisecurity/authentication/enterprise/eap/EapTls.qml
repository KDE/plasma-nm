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

    visible: setting.showTls

    QQC2.TextField {
        text: root.setting.tlsIdentity
        onTextEdited: root.setting.tlsIdentity = text

        Kirigami.FormData.label: i18n("Identity:")
    }

    QQC2.TextField {
        text: root.setting.tlsDomain
        onTextEdited: root.setting.tlsDomain = text

        Kirigami.FormData.label: i18n("Domain:")
    }

    RowLayout {
        QQC2.TextField {
            id: tlsUserCertField
            Layout.fillWidth: true

            text: root.setting.tlsUserCert
            placeholderText: i18n("*.der *.pem *.crt *.cer")

            onTextEdited: root.setting.tlsUserCert = text
        }

        QQC2.Button {
            icon.name: "document-open"
            onClicked: tlsUserCertDialog.open()
        }

        Kirigami.FormData.label: i18n("User certificate:")
    }

    RowLayout {
        QQC2.TextField {
            id: tlsCaCertField
            Layout.fillWidth: true

            text: root.setting.tlsCaCert
            placeholderText: i18n("*.der *.pem *.crt *.cer")

            onTextEdited: root.setting.tlsCaCert = text
        }

        QQC2.Button {
            icon.name: "document-open"
            onClicked: tlsCaCertDialog.open()
        }

        Kirigami.FormData.label: i18n("CA certificate:")
    }

    QQC2.TextField {
        text: root.setting.tlsSubjectMatch
        onTextEdited: root.setting.tlsSubjectMatch = text

        Kirigami.FormData.label: i18n("Subject match:")
    }

    RowLayout {
        QQC2.TextField {
            id: altSubjectField
            Layout.fillWidth: true

            text: root.setting.tlsAltSubjectMatches
            onTextEdited: root.setting.tlsAltSubjectMatches = text
        }

        QQC2.Button {
            text: "…"
            onClicked: {
                alternativeSubjectDialog.addresses = altSubjectField.text.length > 0 ? altSubjectField.text.split(",").filter(s => s.length > 0) : [];
                alternativeSubjectDialog.open();
            }
        }

        Kirigami.FormData.label: i18n("Alternative subject matches:")
    }

    AlternativeSubjectMatch {
        id: alternativeSubjectDialog

        onAccepted: {
            root.setting.tlsAltSubjectMatches = addresses.join(",");
            altSubjectField.text = root.setting.tlsAltSubjectMatches;
        }
    }

    RowLayout {
        Kirigami.FormData.label: i18n("Connect to these servers:")

        QQC2.TextField {
            id: serversField
            Layout.fillWidth: true
            text: root.setting.tlsConnectToServers
            onTextEdited: root.setting.tlsConnectToServers = text
        }

        QQC2.Button {
            text: i18n("…")
            onClicked: {
                connectToServersDialog.addresses = serversField.text.length > 0 ? serversField.text.split(",").filter(s => s.length > 0) : [];
                connectToServersDialog.open();
            }
        }
    }

    DnsList {
        id: connectToServersDialog
        onAccepted: {
            root.setting.tlsConnectToServers = addresses.join(",");
        }
    }

    RowLayout {
        QQC2.TextField {
            id: tlsPrivateKeyField
            Layout.fillWidth: true

            text: root.setting.tlsPrivateKey
            placeholderText: i18n("*.der *.pem *.p12 *.key")

            onTextEdited: root.setting.tlsPrivateKey = text
        }

        QQC2.Button {
            icon.name: "document-open"
            onClicked: tlsPrivateKeyDialog.open()
        }

        Kirigami.FormData.label: i18n("Private key:")
    }

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.tlsPrivateKeyPassword
        passwordOption: root.setting.tlsPrivateKeyPasswordOption

        onPasswordChanged: root.setting.tlsPrivateKeyPassword = password
        onPasswordOptionChanged: root.setting.tlsPrivateKeyPasswordOption = passwordOption
    }

    FileDialog {
        id: tlsUserCertDialog
        nameFilters: [i18n("Certificates (*.der *.pem *.crt *.cer)")]
        onAccepted: root.setting.tlsUserCert = selectedFile
    }

    FileDialog {
        id: tlsCaCertDialog
        nameFilters: [i18n("Certificates (*.der *.pem *.crt *.cer)")]
        onAccepted: root.setting.tlsCaCert = selectedFile
    }

    FileDialog {
        id: tlsPrivateKeyDialog
        nameFilters: [i18n("Keys (*.der *.pem *.p12 *.key)")]
        onAccepted: root.setting.tlsPrivateKey = selectedFile
    }
}
