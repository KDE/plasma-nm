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

    required property WifiSecuritySetting setting

    visible: setting.showLeap

    QQC2.TextField {
        Layout.fillWidth: true
        text: root.setting.leapUsername
        onTextEdited: root.setting.leapUsername = text

        Kirigami.FormData.label: i18n("Username:")
    }

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.leapPassword
        passwordOption: root.setting.leapPasswordOption

        onPasswordChanged: root.setting.leapPassword = password

        onPasswordOptionChanged: root.setting.leapPasswordOption = passwordOption
    }
}
