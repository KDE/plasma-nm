/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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

        headline: i18n("PPTP VPN")
        passwordLabel: i18n("Password:")
        text: i18n("Password passed to PPTP when prompted for it.")

        password: root.setting.password
        onPasswordEdited: password => root.setting.password = password
    }

    Item {
        Layout.fillHeight: true
    }
}
