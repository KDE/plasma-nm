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

    required property WifiSecuritySetting wifiSetting
    required property Security8021xSetting setting8021x

    QQC2.ComboBox {
        id: securityCombo

        Layout.fillWidth: true

        model: [
            i18n("None"), 
            i18n("WEP 40/128-bit key (Hex)"), 
            i18n("WEP 128-bit Passphrase"), i18n("LEAP"), 
            i18n("Dynamic WEP (802.1x)"), 
            i18n("WPA/WPA2 Personal"), 
            i18n("WPA/WPA2 Enterprise"), 
            i18n("WPA3 Personal"), 
            i18n("WPA3 Enterprise 192-bit"), 
            i18n("Enhanced Open (OWE)")
        ]
        currentIndex: root.wifiSetting ? root.wifiSetting.securityType : 0
        onActivated: if (root.wifiSetting)
            root.wifiSetting.securityType = currentIndex

        Kirigami.FormData.label: i18n("Security:")
    }

    Owe {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.OWE
        setting: root.wifiSetting
    }

    WepKey {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.WepHex || root.wifiSetting.securityType === WifiSecuritySetting.WepPassphrase
        setting: root.wifiSetting
    }

    Leap {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.Leap
        setting: root.wifiSetting
    }

    WpaPersonal {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.WpaPsk || root.wifiSetting.securityType === WifiSecuritySetting.SAE
        setting: root.wifiSetting
    }
    DynamicWep {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.DynamicWep
        setting: root.setting8021x
    }

    WpaEnterprise {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.WpaEap
        setting: root.setting8021x
    }

    Wpa3Enterprise192 {
        Layout.fillWidth: true
        visible: root.wifiSetting.securityType === WifiSecuritySetting.Wpa3SuiteB192
        setting: root.setting8021x
    }
}
