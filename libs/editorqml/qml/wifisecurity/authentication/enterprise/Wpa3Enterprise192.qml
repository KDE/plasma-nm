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

    onVisibleChanged: {
        if (visible) {
            root.setting.securityType = Security8021xSetting.WirelessWpaEapSuiteB192;
            root.setting.eapMethod = Security8021xSetting.EapMethodTls;
        }
    }
    Component.onCompleted: {
        console.log("eapMethod =", root.setting.eapMethod);
        root.setting.securityType = Security8021xSetting.WirelessWpaEapSuiteB192;
        root.setting.eapMethod = Security8021xSetting.EapMethodTls;
    }

    QQC2.ComboBox {
        Layout.fillWidth: true
        Kirigami.FormData.label: i18n("Authentication:")
        model: ["TLS"]
        currentIndex: 0
    }
    EapTls {
        Layout.fillWidth: true
        setting: root.setting
    }
}
