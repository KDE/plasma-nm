/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.FormLayout {
    id: root
    required property Security8021xSetting setting

    visible: setting.showPwd

    QQC2.TextField {
        visible: root.setting.securityType === Security8021xSetting.Ethernet

        text: root.setting.pwdSubjectMatch
        onTextEdited: root.setting.pwdSubjectMatch = text

        Kirigami.FormData.label: i18n("Subject match:")
    }

    RowLayout {
        visible: root.setting.securityType === Security8021xSetting.Ethernet

        QQC2.TextField {
            id: pwdAltSubjectField
            Layout.fillWidth: true

            text: root.setting.pwdAltSubjectMatches
            onTextEdited: root.setting.pwdAltSubjectMatches = text
        }

        QQC2.Button {
            text: "…"
            onClicked: {
                pwdAlternativeSubjectDialog.addresses = pwdAltSubjectField.text.length > 0 ? pwdAltSubjectField.text.split(",").filter(s => s.length > 0) : [];
                pwdAlternativeSubjectDialog.open();
            }
        }

        Kirigami.FormData.label: i18n("Alternative subject matches:")
    }

    AlternativeSubjectMatch {
        id: pwdAlternativeSubjectDialog

        onAccepted: {
            root.setting.pwdAltSubjectMatches = addresses.join(",");
            pwdAltSubjectField.text = root.setting.pwdAltSubjectMatches;
        }
    }

    RowLayout {
        visible: root.setting.securityType === Security8021xSetting.Ethernet

        Kirigami.FormData.label: i18n("Connect to these servers:")

        QQC2.TextField {
            id: pwdServersField
            Layout.fillWidth: true

            text: root.setting.pwdConnectToServers
            onTextEdited: root.setting.pwdConnectToServers = text
        }

        QQC2.Button {
            text: i18n("…")
            onClicked: {
                pwdConnectToServersDialog.addresses = pwdServersField.text.length > 0 ? pwdServersField.text.split(",").filter(s => s.length > 0) : [];
                pwdConnectToServersDialog.open();
            }
        }
    }

    DnsList {
        id: pwdConnectToServersDialog

        addressPattern: serverNamePattern

        onAccepted: {
            root.setting.pwdConnectToServers = addresses.join(",");
        }
    }

    QQC2.TextField {
        text: root.setting.pwdUsername
        onTextEdited: root.setting.pwdUsername = text

        Kirigami.FormData.label: i18n("Username:")
    }

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.pwdPassword
        passwordOption: root.setting.pwdPasswordOption
        onPasswordEdited: root.setting.pwdPassword = password
        onPasswordOptionEdited: root.setting.pwdPasswordOption = option
    }
}
