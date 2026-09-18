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

        visible: root.setting.userPasswordRequired

        headline: i18n("L2TP VPN")
        passwordLabel: i18n("User Password:")

        password: root.setting.userPassword
        onPasswordEdited: password => root.setting.userPassword = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.userCertPasswordRequired

        headline: i18n("L2TP VPN")
        passwordLabel: i18n("User Certificate Password:")

        password: root.setting.userCertPassword
        onPasswordEdited: password => root.setting.userCertPassword = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.machineCertPasswordRequired

        passwordLabel: i18n("Machine Certificate Password:")

        password: root.setting.machineCertPassword
        onPasswordEdited: password => root.setting.machineCertPassword = password
    }

    PlasmaNMQ.PasswordField {
        Layout.fillWidth: true

        visible: root.setting.presharedKeyRequired

        passwordLabel: i18n("Pre-shared Key:")

        password: root.setting.presharedKey
        onPasswordEdited: password => root.setting.presharedKey = password
    }

    Item {
        Layout.fillHeight: true
    }
}
