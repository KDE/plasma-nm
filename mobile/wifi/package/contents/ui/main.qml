/*
 *   Copyright 2017 Martin Kacej <m.kacej@atlas.sk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as Controls
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.10 as Kirigami
import org.kde.kcm 1.2

ScrollViewKCM {
    id: main

    PlasmaNM.Handler {
        id: handler
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections

        onWirelessEnabledChanged: {
            wifiSwitchButton.checked = wifiSwitchButton.enabled && enabled
        }
    }

    PlasmaNM.NetworkModel {
        id: connectionModel
    }

    PlasmaNM.MobileProxyModel {
        id: mobileProxyModel
        sourceModel: connectionModel
        showSavedMode: false
    }

    Component.onCompleted: handler.requestScan()

    Timer {
        id: scanTimer
        interval: 10200
        repeat: true
        running: parent.visible

        onTriggered: handler.requestScan()
    }

    header: Kirigami.InlineMessage {
        id: inlineError
        showCloseButton: true

        type: Kirigami.MessageType.Warning
        Connections {
            target: handler
            onConnectionActivationFailed: {
                inlineError.text = message;
                inlineError.visible = true;
            }
        }
    }

    view: ListView {
        id: view

        clip: true
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds

        section.property: "Section"
        section.delegate: Kirigami.ListSectionHeader {
            text: section
        }

        model: mobileProxyModel
        delegate: ConnectionItemDelegate {}
    }

    actions.main: Kirigami.Action {
        iconName: enabledConnections.wirelessEnabled ? "network-wireless-disconnected" : "network-wireless-connected"
        text: enabledConnections.wirelessEnabled ? i18n("Disable Wi-Fi") : i18n("Enable Wi-Fi")
        onTriggered: handler.enableWireless(!enabledConnections.wirelessEnabled);
    }

    actions.contextualActions: [

        Kirigami.Action {
            iconName: "edit"
            text: i18n("Add custom connection")
            onTriggered: {
                kcm.push("NetworkSettings.qml")
                contextDrawer.close()
            }
        },

        Kirigami.Action {
            iconName: "edit"
            text: i18n("Create Hotspot")
            onTriggered: {
                kcm.push("TetheringSetting.qml")
                contextDrawer.close()
            }
        },
        Kirigami.Action {
            iconName: "edit"
            text: i18n("Saved Connections")
            checkable: true
            checked: false
            onTriggered: {
                mobileProxyModel.showSavedMode = !mobileProxyModel.showSavedMode
            }
        }
    ]
}
