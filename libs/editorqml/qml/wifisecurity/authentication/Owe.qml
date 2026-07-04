/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.plasma.networkmanagement.editorqml

Item {
    id: root

    required property WifiSecuritySetting setting

    visible: root.setting.securityType === WifiSecuritySetting.None || root.setting.securityType === WifiSecuritySetting.OWE

    implicitWidth: oweMessage.implicitWidth
    implicitHeight: oweMessage.visible ? oweMessage.implicitHeight : 0

    Kirigami.InlineMessage {
        id: oweMessage

        anchors.left: parent.left
        anchors.right: parent.right

        visible: root.setting.securityType === WifiSecuritySetting.OWE

        type: Kirigami.MessageType.Information

        text: i18n("Enhanced Open (OWE) provides encryption without authentication. No password required.")
    }
}
