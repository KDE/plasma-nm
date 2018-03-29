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
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ApplicationItem {
    id: main
    objectName: "wifiMain"

    pageStack.defaultColumnWidth: Kirigami.Units.gridUnit * 25
    pageStack.initialPage: formLayout
    anchors.fill: parent

    PlasmaNM.Handler {
        id: handler
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections

        onWirelessEnabledChanged: {
            wifiSwitchButton.checked = wifiSwitchButton.enabled && enabled
        }
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    PlasmaNM.NetworkModel {
        id: connectionModel
    }

    PlasmaNM.MobileProxyModel {
        id: mobileProxyModel
        sourceModel: connectionModel
    }

    Kirigami.ScrollablePage  {
        id: formLayout
        anchors.fill: parent

        header: RowLayout {
            id: layoutrow
            width: parent.width

            Controls.Label {
                anchors.left: parent.left
                text: i18n("Wifi")
                Layout.fillWidth: true
            }

            Controls.Switch {
                id: wifiSwitchButton
                checked: enabled && enabledConnections.wirelessEnabled
                enabled: enabledConnections.wirelessHwEnabled
                onClicked: {
                    handler.enableWireless(checked);
                }
            }

            Kirigami.Separator {
                id: separator
                anchors.top: layoutrow.bottom
                width: parent.width
            }
        }

        ListView {
            id: view
            anchors.fill: parent
            clip: true
            width: parent.width
            currentIndex: -1
            boundsBehavior: Flickable.StopAtBounds
            model: mobileProxyModel
            delegate: RowItemDelegate {}
        }

        actions.contextualActions: [

            Kirigami.Action {
                iconName: "edit"
                text:"Add custom connection"
                onTriggered: {
                    applicationWindow().pageStack.push(connectionEditorDialogcomponent)
                }
            },

            Kirigami.Action {
                iconName: "edit"
                text: "Create Hotspot"
                onTriggered: {
                    showPassiveNotification("Open tethering")
                }
            }
        ]
    }

    Component {
        id: connectionEditorDialogcomponent
        ConnectionEditorDialog {
        }
    }

    Component {
        id: networkDetailsViewComponent
        NetworkDetailsView {
            id: networkDetailsViewComponentView
        }
    }
}
