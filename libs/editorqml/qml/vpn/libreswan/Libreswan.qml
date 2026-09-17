/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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

            QQC2.ToolTip.text: i18n("IP or hostname of the IPsec gateway.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.gateway
            onTextEdited: root.setting.gateway = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Group name:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("Group name, or local identifier, this connection belongs to.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.groupName
            onTextEdited: root.setting.groupName = text
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        passwordLabel: i18n("User password:")
        showPasswordOptions: true

        password: root.setting.userPassword
        passwordOption: root.setting.userPasswordOption

        onPasswordEdited: password => root.setting.userPassword = password
        onPasswordOptionEdited: option => root.setting.userPasswordOption = option
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        passwordLabel: i18n("Group password:")
        showPasswordOptions: true

        password: root.setting.groupPassword
        passwordOption: root.setting.groupPasswordOption

        onPasswordEdited: password => root.setting.groupPassword = password
        onPasswordOptionEdited: option => root.setting.groupPasswordOption = option
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        QQC2.TextField {
            Kirigami.FormData.label: i18n("User name:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("User name for XAUTH authentication.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.username
            onTextEdited: root.setting.username = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Phase1 algorithms:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("IKE algorithms to propose. Leave empty for the defaults.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.phase1Algorithms
            onTextEdited: root.setting.phase1Algorithms = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Phase2 algorithms:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("ESP algorithms to propose. Leave empty for the defaults.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.phase2Algorithms
            onTextEdited: root.setting.phase2Algorithms = text
        }

        QQC2.TextField {
            Kirigami.FormData.label: i18n("Domain:")
            Layout.fillWidth: true

            QQC2.ToolTip.text: i18n("(NT-)Domain name for authentication.")
            QQC2.ToolTip.visible: hovered
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay

            text: root.setting.domain
            onTextEdited: root.setting.domain = text
        }
    }

    Item {
        Layout.fillHeight: true
    }
}
