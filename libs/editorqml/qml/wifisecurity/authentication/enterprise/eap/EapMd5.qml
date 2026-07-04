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

    visible: setting.showMd5

    QQC2.TextField {
        text: root.setting.md5Username
        onTextEdited: root.setting.md5Username = text

        Kirigami.FormData.label: i18n("Username:")
    }

    PasswordField {
        Layout.fillWidth: true
        showPasswordOptions: true
        password: root.setting.md5Password
        passwordOption: root.setting.md5PasswordOption
        onPasswordChanged: root.setting.md5Password = password
        onPasswordOptionChanged: root.setting.md5PasswordOption = passwordOption
    }
}
