/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick
import QtQuick.Controls as QQC2
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

            QQC2.ToolTip.text: i18n("PPTP server IP or name.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Login:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Set the name used for authenticating the local system to the peer to <name>.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.login
            onTextEdited: root.setting.login = text
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

        QQC2.TextField {
            Kirigami.FormData.label: i18n("NT Domain:")
            Layout.fillWidth: true

            text: root.setting.ntDomain
            onTextEdited: root.setting.ntDomain = text
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

    PptpAdvanced {
        id: advancedDialog

        setting: root.setting
    }
}
