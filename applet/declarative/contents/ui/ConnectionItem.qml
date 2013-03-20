/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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

    property bool isWireless: (type == 14) ? true : false;
    property bool expanded: false;

    signal activateConnectionItem(string connection, string device, string specificObject);
    signal addAndActivateConnectionItem(string connection, string device, string specificObject);
    signal deactivateConnectionItem(string connection);

    height: 30;
    anchors { left: parent.left; right: parent.right }

    QIconItem {
        id: connectionType;

        height: 30; width: 30;
        anchors { left: parent.left; top: parent.top; leftMargin: 5 }
        icon: QIcon(connectionIcon);

        QIconItem {
            id: connectionSecurity;
            width: 15; height: 15;
            anchors { bottom: parent.bottom; right: parent.right }
            icon: QIcon("object-locked");
            visible: secure;
        }
    }

    PlasmaComponents.Label {
        id: connectionName;

        height: signal ? 15 : 30;
        anchors { left: connectionType.right; top: parent.top; leftMargin: 5 }
        text: name;
        font.weight: connected ? Font.DemiBold : Font.Normal;
    }

    QIconItem {
        id: connectionConnected;

        width: 30; height: 30;
        anchors { right: parent.right; top: parent.top; rightMargin: 5 }
        icon: connected ? QIcon("network-connect") : QIcon("network-disconnect");

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator;
            anchors.fill: parent;
            running: connecting;
            visible: running;
        }

        MouseArea {
            id: clickArea;

            anchors.fill: parent;
            onClicked: {
                if (!connected) {
                    if (uuid) {
                        activateConnectionItem(connectionPath, devicePath, specificPath);
                    } else {
                        addAndActivateConnectionItem(connectionPath, devicePath, specificPath);
                    }
                } else {
                    deactivateConnectionItem(connectionPath);
                }

                if (expanded) {
                    connectionItem.state = '';
                    expanded = false;
                }
            }
        }
    }

    PlasmaComponents.ProgressBar {
        id: connectionSignal;

        height: signal ? 15 : 0;
        anchors {
            top: connectionName.bottom;
            left: connectionType.right;
            right: connectionConnected.left;
            leftMargin: 5;
            rightMargin: 10;
        }
        visible: signal ? true : false;
        minimumValue: 0; maximumValue:100;
        value: signal;
    }

    MouseArea {
        id: showInfo;

        anchors {
            top: connectionItem.top;
            bottom: connectionType.bottom;
            left: connectionItem.left;
            right: connectionConnected.left;
        }

        onClicked: {
            if (!expanded) {
                connectionItem.state = 'Details';
            } else {
                connectionItem.state = '';
            }
            expanded = !expanded;
        }
    }

    DetailsWidget {
        id: details;

        anchors {
            left: parent.left;
            right: parent.right;
            top: connectionType.bottom;
            bottom: parent.bottom;
            topMargin: 10;
            leftMargin: 10;
            rightMargin: 10
            bottomMargin: 5;
        }
        text: detailInformations;
        visible: false;
        editable: uuid == "" ? false : true;

        onHideDetails: {
            connectionItem.state = '';
            expanded = false;
        }
    }

    states: [
        State {
            name: "Details";
            PropertyChanges { target: connectionItem; height: connectionItem.ListView.view.height }
            PropertyChanges { target: details; visible: true}
            PropertyChanges { target: connectionItem.ListView.view; interactive: false }
            PropertyChanges { target: connectionItem.ListView.view; explicit: true; contentY: connectionItem.y }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 200; properties: "height, contentY, visible" }
    }
}
