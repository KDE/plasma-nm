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

Item {
    id: main
    objectName: "wifiMain"
    width: units.gridUnit * 30
    height: width * 1.5

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
    }

    Item {
        id: formLayout
        anchors {
            fill: parent
            margins: units.gridUnit
            leftMargin: units.gridUnit / 2
        }

        RowLayout {
            id: layoutrow
            width: parent.width

            PlasmaComponents.Label {
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
        }

        Rectangle {
            id: separator
            anchors.top: layoutrow.bottom
            anchors.topMargin: 10
            width: parent.width
            height: units.gridUnit / 8
            border.color: "grey"
        }

        Controls.Label {
            id: label
            anchors {
                left: parent.left
                top: separator.bottom
                bottomMargin: 10
            }
            text: i18n("Available wifi networks")
            font.bold: true
            Layout.fillWidth: true
        }

        PlasmaExtras.ScrollArea {
            id: wifiarea
            anchors {
                top: label.bottom
                bottomMargin: units.gridUnit*2
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            ListView {
                id: view
                anchors.fill: parent
                clip: true
                width: parent.width
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                model: mobileProxyModel
                delegate: RowItemDelegate {
                    onClicked: {
                        connect()
                    }
                }
            }
        }

        PlasmaComponents.Button {
            id: customConnectionButton
            anchors.top: wifiarea.bottom
            anchors.topMargin:  units.gridUnit
            text: i18n("Add custom connection")
            onClicked: connectionEditorDialog.open()
        }
    }

    PlasmaComponents.Dialog {
        id: connectionEditorDialog
        title: i18n("Connection Editor")
        buttons: RowLayout {
            width: parent.width
            PlasmaComponents.Button {
                text: 'Save'
                onClicked:{
                    connectionEditorDialogContent.save()
                    connectionEditorDialog.close()
                }
            }
            PlasmaComponents.Button {
                anchors.right: parent.right
                text: 'Cancel'
                onClicked: connectionEditorDialog.close()
            }
        }
        content: ConnectionEditorDialog {
            id: connectionEditorDialogContent
            width: units.gridUnit * 22
            height: units.gridUnit * 25
            Component.onDestruction: {
                console.info("Destroyed editor content")
            }
        }
        Component.onDestruction: {
            console.info("Destroyed editor")
            connectionEditorDialogContent.destroy()
        }
    }

    PlasmaComponents.CommonDialog {
        id: detailsDialog
        titleText: i18n("Network Details")
        buttonTexts: [i18n("Close")]
        onButtonClicked: {
            networkDetailsViewContent.clearDetails()
            close()
        }
        content: NetworkDetailsView {
            id: networkDetailsViewContent
            width: units.gridUnit * 22
            height: units.gridUnit * 25
            Component.onDestruction: {
                console.info("Destroyed details content")
            }
        }
        Component.onDestruction: {
            console.info("Destroyed details")
            networkDetailsViewContent.destroy()
        }
    }
    Component.onDestruction: {
        console.error("Destroyed main")
        connectionEditorDialog.destroy()
        detailsDialog.destroy()
        //connectionEditorDialog.connectionEditorDialogContent.destroy()
    }

}
