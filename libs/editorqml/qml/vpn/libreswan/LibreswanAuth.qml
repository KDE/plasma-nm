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

        wideMode: true

        QQC2.Label {
            Kirigami.FormData.label: i18n("Group name:")

            text: root.setting.groupName
            textFormat: Text.PlainText
        }
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.userPasswordRequired

        headline: i18n("Libreswan VPN")
        passwordLabel: i18n("User password:")

        password: root.setting.userPassword
        onPasswordEdited: password => root.setting.userPassword = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.groupPasswordRequired

        passwordLabel: i18n("Group password:")

        password: root.setting.groupPassword
        onPasswordEdited: password => root.setting.groupPassword = password
    }

    Item {
        Layout.fillHeight: true
    }
}
