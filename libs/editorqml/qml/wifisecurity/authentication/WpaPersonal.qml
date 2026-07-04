/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Kirigami.FormLayout {
    id: root
    required property WifiSecuritySetting setting

    visible: setting.showPsk

    PasswordField {
        Layout.fillWidth: true
        showPasswordOptions: true
        password: root.setting.psk
        passwordOption: root.setting.pskOption
        onPasswordChanged: root.setting.psk = password
        onPasswordOptionChanged: root.setting.pskOption = passwordOption
    }
}
