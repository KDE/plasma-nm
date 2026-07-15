/*
      SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

      SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
  */

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

ColumnLayout {
    anchors.fill: parent

    QQC2.TabBar {
        id: tabBar
        Layout.fillWidth: true

        QQC2.TabButton {
            text: i18n("Status")
        }
        QQC2.TabButton {
            text: i18n("General")
        }
        QQC2.TabButton {
            text: i18n("Wi-Fi")
        }
        QQC2.TabButton {
            text: i18n("Wi-Fi Security")
        }
        QQC2.TabButton {
            text: i18n("IPv4")
        }
        QQC2.TabButton {
            text: i18n("IPv6")
        }
    }

    StackLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: tabBar.currentIndex

        Item {/* TODO: connectionstatus page */}

        Item { /* TODO: general settings page */ }

        Item { /* TODO: wifi settings page */ }

        Item {
            PlasmaNMQ.WifiSecurityForm {
                anchors.fill: parent
                wifiSetting: kcm.wifiSecurity
                setting8021x: kcm.wifiSecurity.setting8021x
            }
        }

        Item { /* TODO: ipv4 settings page */ }

        Item { /* TODO: ipv6 settings page */ }
    }
}
