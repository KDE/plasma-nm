/*
 *   Copyright 2017-2018 Martin Kacej <m.kacej@atlas.sk>
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
import org.kde.kirigami 2.2 as Kirigami

Kirigami.ScrollablePage {
    anchors.leftMargin: Kirigami.Units.largeSpacing * 2

    header: RowLayout {
        id: layoutrow
        width: parent.width

        Controls.Label {
            anchors.left: parent.left
            text: i18n("Wi-fi")
            Layout.fillWidth: true
            font.bold: true
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

    ListView {
        id: view

        anchors.fill: parent
        clip: true
        width: parent.width
        currentIndex: -1
        boundsBehavior: Flickable.StopAtBounds
        header: Column {
                width: parent.width
            Controls.Label {
                text: (mobileProxyModel.showSavedMode) ? i18n("Saved networks") : i18n("Available networks")
            }
            Rectangle { width: parent.width; height: 2; color: Kirigami.Theme.disabledTextColor}
        }
        model: mobileProxyModel
        delegate: ConnectionItemDelegate {}
    }

    actions.contextualActions: [

        Kirigami.Action {
            iconName: "edit"
            text: i18n("Add custom connection")
            onTriggered: {
                applicationWindow().pageStack.push(connectionEditorDialogComponent)
                contextDrawer.close()
            }
        },

        Kirigami.Action {
            iconName: "edit"
            text: i18n("Create Hotspot")
            onTriggered: {
                applicationWindow().pageStack.push(tetheringComponent)
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
/*
    footer: Controls.Button {
        width: parent.width
        text: "ContextualActions"
        iconName: "edit"
        onClicked: bottomDrawer.open()
    }

    Kirigami.OverlayDrawer {
            id: bottomDrawer
            edge: Qt.BottomEdge
            contentItem: Item {
                implicitHeight: childrenRect.height + Kirigami.Units.gridUnit
                ColumnLayout{
                    anchors.centerIn: parent
                    Controls.Button {
                        text: "Add custom connection"
                        onClicked: applicationWindow().pageStack.push(connectionEditorDialogComponent)
                    }
                    Controls.Button {
                        text: "Create Hotspot"
                        onClicked: showPassiveNotification("Open tethering")
                    }
                    Item {
                        Layout.minimumHeight: Units.gridUnit * 4
                    }
                }
            }
        }
*/
    Kirigami.OverlayDrawer {
        id: deleteConnectionDialog
        property var name
        property var dbusPath
        edge: Qt.BottomEdge
        contentItem: Item {
            implicitHeight: childrenRect.height + Kirigami.Units.gridUnit
            ColumnLayout {
                anchors.centerIn: parent
                Controls.Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: i18n("Delete connection ") + deleteConnectionDialog.name + " from device ?"
                }
                Controls.Button {
                    text: i18n("Delete")
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        handler.removeConnection(deleteConnectionDialog.dbusPath)
                        deleteConnectionDialog.close()
                    }
                }
                Controls.Button {
                    text: i18n("Cancel")
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: deleteConnectionDialog.close()
                }
                Item {
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 4
                }
            }
            onVisibleChanged: {
                if (!visible) {
                    deleteConnectionDialog.name = ""
                    deleteConnectionDialog.dbusPath = ""
                }
            }
        }
    }
}
