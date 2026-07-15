/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement as PlasmaNM

QQC2.Dialog {
    id: root

    property string uniqueName
    property string devicePath
    property string specificPath
    property int securityType
    property var handler

    footer: QQC2.DialogButtonBox {
        QQC2.Button {
            text: i18nc("@action:button Connect to network", "Connect")
            icon.name: "network-connect"
            enabled: passwordField.acceptableInput
            QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
        }

        QQC2.Button {
            text: i18nc("@action:button Cancel connecting to network", "Cancel")
            icon.name: "dialog-cancel"
            QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.RejectRole
        }
    }

    title: i18nc("@title:window", "Enter Password")

    contentItem: ColumnLayout {
        QQC2.Label {
            Layout.fillWidth: true
            wrapMode: Text.Wrap
            text: i18nc("@label %1 is the name of the network the user is connecting to", "Network: %1", root.uniqueName)
            textFormat: Text.PlainText
        }

        Kirigami.PasswordField {
            id: passwordField

            Layout.fillWidth: true

            validator: RegularExpressionValidator {
                regularExpression: root.securityType === PlasmaNM.Enums.StaticWep ? /^(?:.{5}|[0-9a-fA-F]{10}|.{13}|[0-9a-fA-F]{26})$/ : /^(?:.{8,64})$/
            }

            onAccepted: root.accept()
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            visible: !passwordField.acceptableInput
            text: root.securityType === PlasmaNM.Enums.StaticWep ? i18nc("@label key is a passcode", "Password must be a valid WEP key") : i18nc("@label password invalid length message", "Password must be between 8 and 64 characters")
        }
    }

    onAccepted: {
        handler.addAndActivateConnection(devicePath, specificPath, passwordField.text);
        passwordField.clear();
    }

    onRejected: passwordField.clear()
}
