/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.plasma.networkmanagement.editorqml as PlasmaNMQ

Item {
    id: root

    property int connectionType: kcm.connectionType

    property alias active: pageLoader.active

    readonly property alias page: pageLoader.item

    function showStatusTab(): void {
        if (pageLoader.item?.showStatusTab) {
            pageLoader.item.showStatusTab();
        }
    }

    Component {
        id: wireless

        PlasmaNMQ.Wireless {}
    }

    Component {
        id: wired

        PlasmaNMQ.Wired {}
    }

    Component {
        id: vpn

        PlasmaNMQ.Vpn {}
    }

    Component {
        id: wireguard

        PlasmaNMQ.WireGuard {}
    }

    Component {
        id: disconnected

        Item {
            QQC2.Label {
                anchors.centerIn: parent

                text: i18n("Disconnected")
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    Loader {
        id: pageLoader

        anchors.fill: parent

        sourceComponent: {
            switch (root.connectionType) {
            case PlasmaNM.Enums.Wireless:
                return wireless;
            case PlasmaNM.Enums.Wired:
                return wired;
            case PlasmaNM.Enums.Vpn:
                return vpn;
            case PlasmaNM.Enums.WireGuard:
                return wireguard;
            default:
                return disconnected;
            }
        }
    }
}
