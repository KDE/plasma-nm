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

        visible: root.setting.usesChallenge

        headline: i18n("OpenVPN VPN")
        passwordLabel: root.setting.challengeLabel

        password: root.setting.challengePassword
        onPasswordEdited: password => root.setting.challengePassword = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: !root.setting.usesChallenge && root.setting.passwordRequired

        headline: i18n("OpenVPN VPN")
        passwordLabel: i18n("Password:")

        password: root.setting.password
        onPasswordEdited: password => root.setting.password = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: !root.setting.usesChallenge && root.setting.privateKeyPasswordRequired

        passwordLabel: i18n("Key Password:")

        password: root.setting.privateKeyPassword
        onPasswordEdited: password => root.setting.privateKeyPassword = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: !root.setting.usesChallenge && root.setting.proxyPasswordRequired

        passwordLabel: i18n("Proxy Password:")

        password: root.setting.proxyPassword
        onPasswordEdited: password => root.setting.proxyPassword = password
    }

    Item {
        Layout.fillHeight: true
    }
}
