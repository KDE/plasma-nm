/*
    SPDX-FileCopyrightText: 2017 Martin Kacej <m.kacej@atlas.sk>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
            function onConnectionActivationFailed(connectionPath, message) {
                inlineError.text = message;
                inlineError.visible = true;
            }
        }
    }

    ConnectDialog {
        id: connectionDialog
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
            text: i18n("Saved Connections")
            checkable: true
            checked: false
            onTriggered: {
                mobileProxyModel.showSavedMode = !mobileProxyModel.showSavedMode
            }
        }
    ]
}
