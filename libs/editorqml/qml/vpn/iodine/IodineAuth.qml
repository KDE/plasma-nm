/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

ColumnLayout {
    id: root

    required property var setting

    spacing: Kirigami.Units.largeSpacing

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        headline: i18n("Iodine VPN")
        passwordLabel: i18n("Password:")

        password: root.setting.password
        onPasswordEdited: password => root.setting.password = password
    }

    Item {
        Layout.fillHeight: true
    }
}
