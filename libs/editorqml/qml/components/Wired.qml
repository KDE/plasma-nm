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
            text: i18n("Wired")
        }
        QQC2.TabButton {
            text: i18n("802.1x Security")
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

        Item {
            PlasmaNMQ.ConnectionStatusForm {
                anchors.fill: parent
                connectionStatus: kcm.connectionStatus
            }
        }

        Item {
            PlasmaNMQ.GeneralSettings {
                anchors.fill: parent
                setting: kcm.generalSettings
            }
        }

        Item {
            PlasmaNMQ.WiredConnectionSettings {
                anchors.fill: parent
                setting: kcm.wiredSetting
            }
        }

        Item {
            // TODO: Wired security settings
        }

        Item {
            PlasmaNMQ.IPv4Settings {
                anchors.fill: parent
                setting: kcm.ipv4Settings
            }
        }

        Item {
            PlasmaNMQ.IPv6Settings {
                anchors.fill: parent
                setting: kcm.ipv6Settings
            }
        }
    }
}
