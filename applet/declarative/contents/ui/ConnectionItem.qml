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
import org.kde.plasmanm 0.1 as PlasmaNM

PlasmaComponents.ListItem {
    id: connectionItem;

    property bool isWireless: (itemType == 14) ? true : false;
    property bool expanded: false;

    signal itemExpanded(string connectionPath, bool itemExpanded);

    enabled: true
    height: 40;

    onClicked: {
        if (itemUuid) {
            itemExpanded(itemConnectionPath, !expanded);
        } else {
            itemExpanded(itemName, !expanded);
        }
    }

    Item {
        id: connectionItemBasic;

        height: 30;
        anchors { left: parent.left; right: parent.right; top: parent.top }

        QIconItem {
            id: connectionTypeIcon;

            height: 30; width: 25;
            anchors { left: parent.left; verticalCenter: parent.verticalCenter; leftMargin: 5 }
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

            anchors { left: connectionTypeIcon.right; right: parent.right; verticalCenter: parent.verticalCenter; leftMargin: 5; rightMargin: 30 }
            text: itemName;
            elide: Text.ElideRight;
            font.weight: itemConnected ? Font.DemiBold : Font.Normal;
            font.italic: itemConnecting ? true : false;
        }

        MouseEventListener {
            id: leftActionArea;

            anchors { right: parent.right; verticalCenter: connectionTypeIcon.verticalCenter }
            width: theme.iconSizes.dialog*0.8;
            height: width;
            hoverEnabled: true;

            onClicked: {
                // TODO expand and show details
                if (configureButton.active) {
                    console.log("clicked");
                }
            }

            PlasmaComponents.BusyIndicator {
                id: connectingIndicator;

                width: 25;
                anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 5 }
                running: itemConnecting;
                visible: running;
            }

            PlasmaCore.IconItem {
                id: configureButton;

                width: 25;
                anchors { right: parent.right; verticalCenter: parent.verticalCenter; rightMargin: 5 }
                source: "configure";
                visible: expanded && !connectingIndicator.running;
                active: leftActionArea.containsMouse;
            }
        }
    }

    Item {
        id: connectionItemSettings;
        property Item connectionSettings;
    }

    Component {
        id: connectionComponent;

        Item {
            anchors { left: parent.left; right: parent.right; top: parent.top; topMargin: 40 }

            PlasmaComponents.TextField {
                id: passwordInput;

                anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }
                width: 200;
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                visible: predictableWirelessPassword();
                height: visible ? 25 : 0;
                placeholderText: i18n("Password...");
            }

            PlasmaComponents.CheckBox {
                id: showPasswordCheckbox;

                anchors { left: passwordInput.left; right: parent.right; top: passwordInput.bottom }
                visible: predictableWirelessPassword();
                height: visible ? 25 : 0;
                checked: false;
                text: i18n("Show password");
            }

            PlasmaComponents.CheckBox {
                id: automaticallyConnectCheckbox;

                anchors { left: passwordInput.left; right: parent.right; top: showPasswordCheckbox.bottom }
                visible: predictableWirelessPassword();
                height: visible ? 25 : 0;
                checked: true;
                text: i18n("Automatically connect");
            }

            PlasmaComponents.Button {
                id: connectDisconnectButton;

                anchors { horizontalCenter: parent.horizontalCenter; top: automaticallyConnectCheckbox.bottom }
                text: (itemConnected || itemConnecting)? i18n("Disconnect") : i18n("Connect");

                onClicked: {
                    if (itemUuid) {
                        itemExpanded(itemConnectionPath, false);
                    } else {
                        itemExpanded(itemName, false);
                    }

                    if (!itemConnected && !itemConnecting) {
                        if (itemUuid) {
                            handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
                        } else {
                            handler.addAndActivateConnection(itemDevicePath, itemSpecificPath, passwordInput.text, automaticallyConnectCheckbox.checked);
                        }
                    } else {
                        handler.deactivateConnection(itemConnectionPath);
                    }
                }
            }

            PlasmaNM.TrafficMonitor {
                id: trafficMonitor;

                visible: (itemDevicePath && itemConnected && itemType != 1)
                height: visible ? 100 : 0;
                device: itemDevicePath;
                anchors { top: connectDisconnectButton.bottom; left: parent.left; right: parent.right; topMargin: 5 }
            }
        }
    }

    states: [
        State {
            name: "Collapsed";
            when: !expanded && !sectionHidden();
                StateChangeScript { script: if (connectionItemSettings.connectionSettings) {connectionItemSettings.connectionSettings.destroy()} }
        },

        State {
            name: "CollapsedHidden";
            when: sectionHidden();
            StateChangeScript { script: if (connectionItemSettings.connectionSettings) {connectionItemSettings.connectionSettings.destroy()} }
            PropertyChanges { target: connectionItem; height: 0; }
            PropertyChanges { target: connectionItem; visible: false; }
        },

        State {
            name: "ConnectionExpanded";
            when: expanded && !sectionHidden();
            StateChangeScript { script: connectionItemSettings.connectionSettings = connectionComponent.createObject(connectionItem); }
            PropertyChanges { target: connectionItem; height: heightForConnectionSettings() }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 300; properties: "height, contentY" }
    }

    function sectionHidden() {
        return ((!connectionView.activeExpanded && itemSection == i18n("Active connections")) ||
                (!connectionView.previousExpanded && itemSection == i18n("Previous connections")) ||
                (!connectionView.unknownExpanded && itemSection == i18n("Unknown connections")))
    }

    function predictableWirelessPassword() {
        // Item is unknown && itemType == Wireless && itemSecurityType != DynamicWep && itemSecurityType != LEAP && itemSecurityType != WpaEap
        return !itemUuid && itemType == 14 && itemSecure && itemSecurityType != 2 && itemSecurityType != 5 && itemSecurityType != 7;
    }

    function heightForConnectionSettings() {
        // Item is connected, have a physical device and it's not VPN → display traffic monitor
        if (itemDevicePath && itemConnected && itemType != 11) {
            return 220;
        // Item is wireless connection with predictable password → display password input
        } else if (predictableWirelessPassword()) {
            return 145;
        // Otherwise display only connect/disconnect button
        } else {
            return 70;
        }
    }
}
