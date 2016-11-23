/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    id: connectionItem

    checked: mouseArea.containsMouse || ConnectionPath === connectionView.currentConnection
    height: connectionItemBase.height

    signal aboutToRemove(string name, string path)

    Item {
        id: connectionItemBase

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            // Reset top margin from PlasmaComponents.ListItem
            topMargin: -Math.round(units.gridUnit / 3)
        }
        height: Math.max(units.iconSizes.medium, connectionNameLabel.height + connectionStatusLabel.height) + Math.round(units.gridUnit / 2)

        PlasmaCore.IconItem {
            id: connectionIcon

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            height: units.iconSizes.medium; width: height
            source: KcmConnectionIcon
        }

        PlasmaComponents.Label {
            id: connectionNameLabel

            anchors {
                bottom: connectionIcon.verticalCenter
                left: connectionIcon.right
                leftMargin: Math.round(units.gridUnit / 2)
                right: connectingIndicator.visible ? connectingIndicator.left : parent.right
            }
            height: paintedHeight
            elide: Text.ElideRight
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
            text: Name
            textFormat: Text.PlainText
        }

        PlasmaComponents.Label {
            id: connectionStatusLabel

            anchors {
                left: connectionIcon.right
                leftMargin: Math.round(units.gridUnit / 2)
                right: connectingIndicator.visible ? connectingIndicator.left : parent.right
                top: connectionNameLabel.bottom
            }
            height: paintedHeight
            elide: Text.ElideRight
            font.pointSize: theme.smallestFont.pointSize
            opacity: 0.6
            text: itemText()
        }

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator

            anchors {
                right: parent.right
                rightMargin: Math.round(units.gridUnit / 2)
                verticalCenter: connectionIcon.verticalCenter
            }
            height: units.iconSizes.medium; width: height
            running: ConnectionState == PlasmaNM.Enums.Activating
            visible: running
        }
    }

    PlasmaComponents.Menu {
        id: connectionItemMenu
        visualParent: mouseArea

        PlasmaComponents.MenuItem {
            text: ConnectionState == PlasmaNM.Enums.Deactivated ? i18n("Connect") : i18n("Disconnect")
            visible: ItemType == 1
            onClicked: {
                if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                    handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
                } else {
                    handler.deactivateConnection(ConnectionPath, DevicePath);
                }
            }
        }

        PlasmaComponents.MenuItem {
            icon: "list-remove"
            text: i18n("Delete");

            onClicked: {
                aboutToRemove(Name, ConnectionPath)
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons;
        hoverEnabled: true

        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                connectionView.currentConnection = ConnectionPath
            } else if (mouse.button == Qt.RightButton) {
                connectionItemMenu.open(mouse.x, mouse.y)
            }
        }
    }

    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else {
            return LastUsed
        }
    }
}
