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

    Component.onCompleted: {
        root.setting.securityType = Security8021xSetting.WirelessWpaEap;
    }

    QQC2.ComboBox {
        id: authCombo

        Layout.fillWidth: true

        model: [i18n("TLS"), i18n("LEAP"), i18n("PWD"), i18n("FAST"), i18n("Tunneled TLS (TTLS)"), i18n("Protected EAP (PEAP)")]

        currentIndex: {
            switch (root.setting.eapMethod) {
            case Security8021xSetting.EapMethodTls:
                return 0;
            case Security8021xSetting.EapMethodLeap:
                return 1;
            case Security8021xSetting.EapMethodPwd:
                return 2;
            case Security8021xSetting.EapMethodFast:
                return 3;
            case Security8021xSetting.EapMethodTtls:
                return 4;
            case Security8021xSetting.EapMethodPeap:
                return 5;
            default:
                return 5;
            }
        }

        onActivated: {
            const methods = [Security8021xSetting.EapMethodTls, Security8021xSetting.EapMethodLeap, Security8021xSetting.EapMethodPwd, Security8021xSetting.EapMethodFast, Security8021xSetting.EapMethodTtls, Security8021xSetting.EapMethodPeap];

            root.setting.eapMethod = methods[currentIndex];
        }

        Kirigami.FormData.label: i18n("Authentication:")
    }

    EapTls {
        Layout.fillWidth: true
        setting: root.setting
    }

    EapLeap {
        Layout.fillWidth: true
        setting: root.setting
    }

    EapPwd {
        Layout.fillWidth: true
        setting: root.setting
    }

    EapFast {
        Layout.fillWidth: true
        setting: root.setting
    }

    EapTtls {
        Layout.fillWidth: true
        setting: root.setting
    }

    EapPeap {
        Layout.fillWidth: true
        setting: root.setting
    }
}
