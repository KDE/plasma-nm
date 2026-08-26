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

    required property Security8021xSetting setting8021x

    property alias use8021x: use8021xCheck.checked

    Component.onCompleted: {
        root.setting8021x.securityType = Security8021xSetting.Ethernet;
    }

    QQC2.CheckBox {
        id: use8021xCheck

        Layout.fillWidth: true

        text: i18n("Use 802.1x security for this connection")
    }

    QQC2.ComboBox {
        id: authCombo

        Kirigami.FormData.label: i18n("Authentication:")
        Layout.fillWidth: true

        enabled: use8021xCheck.checked

        model: [i18n("TLS"), i18n("PWD"), i18n("FAST"), i18n("Tunneled TLS (TTLS)"), i18n("Protected EAP (PEAP)")]

        readonly property var methods: [Security8021xSetting.EapMethodTls, Security8021xSetting.EapMethodPwd, Security8021xSetting.EapMethodFast, Security8021xSetting.EapMethodTtls, Security8021xSetting.EapMethodPeap]

        currentIndex: {
            const index = authCombo.methods.indexOf(root.setting8021x.eapMethod);
            return index < 0 ? 0 : index;
        }

        onActivated: root.setting8021x.eapMethod = authCombo.methods[currentIndex]
    }

    EapTls {
        Layout.fillWidth: true
        enabled: use8021xCheck.checked
        setting: root.setting8021x
    }

    EapPwd {
        Layout.fillWidth: true
        enabled: use8021xCheck.checked
        setting: root.setting8021x
    }

    EapFast {
        Layout.fillWidth: true
        enabled: use8021xCheck.checked
        setting: root.setting8021x
    }

    EapTtls {
        Layout.fillWidth: true
        enabled: use8021xCheck.checked
        setting: root.setting8021x
    }

    EapPeap {
        Layout.fillWidth: true
        enabled: use8021xCheck.checked
        setting: root.setting8021x
    }
}
