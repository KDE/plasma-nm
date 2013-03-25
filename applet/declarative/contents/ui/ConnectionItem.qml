/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.qtextracomponents 0.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: connectionItem;

    property bool isWireless: (itemType == 14) ? true : false;
    property bool expanded: false;

    signal activateConnectionItem(string connectionPath, string devicePath, string specificObjectPath);
    signal addAndActivateConnectionItem(string connectionPath, string devicePath, string specificObjectPath);
    signal deactivateConnectionItem(string connectionPath);
    signal removeConnectionItem(string connectionName, string connectionPath);
    signal itemExpanded();

    function hideDetails() {
        state = '';
        expanded = false;
    }

    height: 30;
    anchors { left: parent.left; right: parent.right }

    QIconItem {
        id: connectionTypeIcon;

        height: 30; width: 30;
        anchors { left: parent.left; top: parent.top; leftMargin: 5 }
        icon: QIcon(itemConnectionIcon);

        QIconItem {
            id: connectionSecurityIcon;
            width: 15; height: 15;
            anchors { bottom: parent.bottom; right: parent.right }
            icon: QIcon("object-locked");
            visible: itemSecure;
        }
    }

    PlasmaComponents.Label {
        id: connectionNameLabel;

        height: itemSignal ? 15 : 30;
        anchors { left: connectionTypeIcon.right; right: connectButton.left; top: parent.top; leftMargin: 5 }
        text: itemName;
        elide: Text.ElideRight;
        font.weight: itemConnected ? Font.DemiBold : Font.Normal;
    }

    PlasmaComponents.ToolButton {
        id: connectButton;

        width: 35; height: 35;
        anchors { right: parent.right; top: parent.top; rightMargin: 5 }
        iconSource: itemConnected ? "user-online" : "user-offline";

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator;
            anchors.fill: parent;
            running: itemConnecting;
            visible: running;
        }

        onClicked: {
            if (!itemConnected && !itemConnecting) {
                if (itemUuid) {
                    activateConnectionItem(itemConnectionPath, itemDevicePath, itemSpecificPath);
                } else {
                    addAndActivateConnectionItem(itemConnectionPath, itemDevicePath, itemSpecificPath);
                }
            } else {
                deactivateConnectionItem(itemConnectionPath);
            }

            if (expanded) {
                connectionItem.state = '';
                expanded = false;
            }
        }
    }

    PlasmaComponents.ProgressBar {
        id: connectionSignalMeter;

        height: itemSignal ? 15 : 0;
        anchors {
            top: connectionNameLabel.bottom;
            left: connectionTypeIcon.right;
            right: connectButton.left;
            leftMargin: 5;
            rightMargin: 10;
        }
        visible: itemSignal ? true : false;
        minimumValue: 0; maximumValue:100;
        value: itemSignal;
    }

    MouseArea {
        id: mouseAreaShowInfo;

        anchors {
            top: connectionItem.top;
            bottom: connectionTypeIcon.bottom;
            left: connectionItem.left;
            right: connectButton.left;
        }

        onClicked: {
            if (!expanded) {
                connectionItem.state = 'Details';
                itemExpanded();
            } else {
                connectionItem.state = '';
            }
            expanded = !expanded;
        }
    }

    DetailsWidget {
        id: detailWidget;

        anchors {
            left: parent.left;
            right: parent.right;
            top: connectionTypeIcon.bottom;
            bottom: parent.bottom;
            topMargin: 10;
            leftMargin: 10;
            rightMargin: 10
            bottomMargin: 5;
        }
        text: itemDetailInformations;
//         connectionName: itemName;
        visible: false;
        editable: itemUuid == "" ? false : true;

        onHideDetails: {
            connectionItem.hideDetails();
        }

        onRemoveConnection: {
            connectionItem.removeConnectionItem(itemName, itemConnectionPath);
        }
    }

    states: [
        State {
            name: "Details";
            PropertyChanges { target: connectionItem; height: connectionItem.ListView.view.height }
            PropertyChanges { target: detailWidget; visible: true}
            PropertyChanges { target: connectionItem.ListView.view; interactive: false }
            PropertyChanges { target: connectionItem.ListView.view; explicit: true; contentY: connectionItem.y }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 200; properties: "height, contentY, visible" }
    }
}
