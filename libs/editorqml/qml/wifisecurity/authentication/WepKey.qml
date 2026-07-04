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

    visible: setting.showWep

    PasswordField {
        Layout.fillWidth: true

        showPasswordOptions: true

        password: root.setting.wepKey
        passwordOption: root.setting.wepKeyOption

        onPasswordChanged: root.setting.wepKey = password
        onPasswordOptionChanged: root.setting.wepKeyOption = passwordOption
    }

    QQC2.ComboBox {
        Layout.fillWidth: true

        model: ["1 (default)", "2", "3", "4"]
        currentIndex: root.setting.wepKeyIndex
        onActivated: root.setting.wepKeyIndex = currentIndex

        Kirigami.FormData.label: i18n("Key index:")
    }

    QQC2.ComboBox {
        Layout.fillWidth: true

        model: [i18n("Open System"), i18n("Shared Key")]
        currentIndex: root.setting.wepAuth
        onActivated: root.setting.wepAuth = currentIndex

        Kirigami.FormData.label: i18n("Authentication:")
    }
}
