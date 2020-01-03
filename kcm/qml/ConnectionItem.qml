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
import QtQuick.Controls 1.4 as QQC
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

ListItem {
    id: connectionItem

    checked: mouseArea.containsMouse || ConnectionPath === connectionView.currentConnectionPath
    height: connectionItemBase.height

    signal aboutToChangeConnection(bool exportable, string name, string path)
    signal aboutToExportConnection(string path)
    signal aboutToRemoveConnection(string name, string path)

    Item {
        id: connectionItemBase

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            topMargin: -Math.round(units.gridUnit / 3)
        }
        height: Math.max(units.iconSizes.medium, connectionNameLabel.height + connectionStatusLabel.height) + Math.round(units.gridUnit / 2)

        KQuickControlsAddons.QIconItem {
            id: connectionIcon

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            height: units.iconSizes.medium; width: height
            icon: KcmConnectionIcon
        }

        Text {
            id: connectionNameLabel

            anchors {
                bottom: connectionIcon.verticalCenter
                left: connectionIcon.right
                leftMargin: Math.round(units.gridUnit / 2)
                right: connectingIndicator.visible ? connectingIndicator.left : parent.right
            }
            color: textColor
            height: paintedHeight
            elide: Text.ElideRight
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
            text: Name
            textFormat: Text.PlainText
        }

        Text {
            id: connectionStatusLabel

            anchors {
                left: connectionIcon.right
                leftMargin: Math.round(units.gridUnit / 2)
                right: connectingIndicator.visible ? connectingIndicator.left : parent.right
                top: connectionNameLabel.bottom
            }
            color: textColor
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

    QQC.Menu {
        id: connectionItemMenu

        QQC.MenuItem {
            text: ConnectionState == PlasmaNM.Enums.Deactivated ? i18n("Connect") : i18n("Disconnect")
            visible: ItemType == 1
            onTriggered: {
                if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                    handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
                } else {
                    handler.deactivateConnection(ConnectionPath, DevicePath);
                }
            }
        }

        QQC.MenuItem {
            iconName: "list-remove"
            text: i18n("Delete");

            onTriggered: {
                aboutToRemoveConnection(Name, ConnectionPath)
            }
        }

        QQC.MenuItem {
            iconName: "document-export"
            visible: KcmVpnConnectionExportable
            text: i18n("Export");

            onTriggered: aboutToExportConnection(ConnectionPath)
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons;
        hoverEnabled: true

        onClicked: {
            if (mouse.button === Qt.LeftButton) {
                aboutToChangeConnection(KcmVpnConnectionExportable, Name, ConnectionPath)
            } else if (mouse.button == Qt.RightButton) {
                connectionItemMenu.popup()
            }
        }
    }

    /* This generates the status description under each connection
       in the list at the left side of the applet. */
    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else if (ConnectionState == PlasmaNM.Enums.Activating) {
            return i18n("Connecting")
        } else {
            return LastUsed
        }
    }
}
