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

    readonly property int secretPassword: 0
    readonly property int secretPrivateKeyPassword: 1
    readonly property int secretPin: 2

    spacing: Kirigami.Units.largeSpacing

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.passwordRequired

        headline: i18n("strongSwan VPN")

        passwordLabel: {
            switch (root.setting.secretKind) {
            case root.secretPrivateKeyPassword:
                return i18nc("@label:textbox password label for private key password", "Private Key Password:");
            case root.secretPin:
                return i18nc("@label:textbox password label for smartcard pin", "PIN:");
            default:
                return i18nc("@label:textbox password label for EAP password", "Password:");
            }
        }

        password: root.setting.password
        onPasswordEdited: password => root.setting.password = password
    }

    Item {
        Layout.fillHeight: true
    }
}
