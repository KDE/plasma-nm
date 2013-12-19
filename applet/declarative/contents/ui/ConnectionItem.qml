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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.networkmanagement 0.1 as PlasmaNM

ListItem {
    id: connectionItem;

    property bool expanded: false;

    enabled: true
    height: if (expanded) connectionItemBase.height + connectionItemDetails.height + padding.margins.top + padding.margins.bottom;
            else connectionItemBase.height + padding.margins.top + padding.margins.bottom;

    Item {
        id: connectionItemBase;

        height: Math.max(connectionIcon.height, connectionNameLabel.height + connectionStatusLabel.height);

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        PlasmaCore.SvgItem {
            id: connectionIcon;

            width: sizes.iconSize;
            height: width;
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            svg: svgNetworkIcons;
            elementId: itemConnectionIcon;

            PlasmaCore.Svg {
                id: svgNetworkIcons;

                multipleImages: true;
                imagePath: "icons/plasma-networkmanagement";
            }
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            height: paintedHeight;
            anchors {
                left: connectionIcon.right;
                leftMargin: padding.margins.left;
                right: !connectionItem.containsMouse ? connectingIndicator.left : configButton.left;
                top: parent.top;
                topMargin: 0;
            }
            text: itemName;
            elide: Text.ElideRight;
            font.weight: itemConnectionState == PlasmaNM.Enums.Activated || itemUuid ? Font.DemiBold : Font.Normal;
            font.italic: itemConnectionState == PlasmaNM.Enums.Activating ? true : false;
        }

        PlasmaComponents.Label {
            id: connectionStatusLabel;

            height: paintedHeight;
            anchors {
                left: connectionIcon.right;
                leftMargin: padding.margins.left;
                right: !connectionItem.containsMouse ? connectingIndicator.left : configButton.left;
                top: connectionNameLabel.bottom;
            }

            font.pointSize: theme.smallestFont.pointSize;
            color: "#99"+(theme.textColor.toString().substr(1))
            text:   if (itemConnectionState == PlasmaNM.Enums.Activated &&
                        (itemType == PlasmaNM.Enums.Wireless || itemType == PlasmaNM.Enums.Wired))
                        i18n("Connected") + ", " + itemSpeed;
                    else if (itemConnectionState == PlasmaNM.Enums.Activated)
                        i18n("Connected");
                    else if (itemConnectionState == PlasmaNM.Enums.Activating)
                        i18n("Connecting");
                    else if (itemUuid && itemSecurityType != PlasmaNM.Enums.None)
                        itemLastUsed + ", " + itemSecurityString;
                    else if (itemUuid)
                       itemLastUsed;
                    else if (itemSecurityType != PlasmaNM.Enums.None)
                        i18n("Never used") + ", " + itemSecurityString;
                    else
                        i18n("Never used");
            elide: Text.ElideRight;
        }

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator;

            width: sizes.iconSize;
            height: width;
            anchors {
                right: parent.right;
                rightMargin: padding.margins.right;
                verticalCenter: parent.verticalCenter;
            }
            running: itemConnectionState == PlasmaNM.Enums.Activating;
            visible: running && !connectionItem.containsMouse;
        }

        PlasmaComponents.Button {
            id: stateChangeButton;

            implicitWidth: minimumWidth + padding.margins.left + padding.margins.right;
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
            }
            text: if (itemConnectionState == PlasmaNM.Enums.Deactivated)
                    i18n("Connect");
                else
                    i18n("Disconnect");
            visible: connectionItem.containsMouse;

            onClicked: {
                expanded = false;
                if (itemConnectionState == PlasmaNM.Enums.Deactivated)
                    handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
                else
                    handler.deactivateConnection(itemConnectionPath, itemDevicePath);
            }
        }

        PlasmaCore.SvgItem {
            id: configButton;

            height: sizes.iconSize;
            width: height;
            anchors {
                verticalCenter: parent.verticalCenter;
                right: stateChangeButton.left;
                rightMargin: padding.margins.right;
            }
            svg: svgNotificationIcons;
            elementId: configButtonMouse.containsMouse ? "notification-inactive" : "notification-active";
            visible: connectionItem.containsMouse;

            PlasmaCore.Svg {
                id: svgNotificationIcons;

                multipleImages: true;
                imagePath: "icons/notification";
            }

            MouseArea {
                id: configButtonMouse;
                anchors.fill: parent;
                hoverEnabled: true;

                onClicked: {
                    expanded = !expanded;
                }
            }
        }
    }

    Loader {
        id: connectionItemDetails;

        anchors {
            left: parent.left;
//             leftMargin: sizes.iconSize + padding.margins.left;
            right: parent.right;
            top: connectionItemBase.bottom;
            topMargin: padding.margins.top;
        }
    }

    Component {
        id: connectionItemDetailsComponent;

        Item {
            height: childrenRect.height + padding.margins.top;

            // TODO only background, not necessary to use ListItem
            PlasmaComponents.ListItem {
                id: connectionDetailsBackground;

                PlasmaComponents.TabBar {
                    id: connectionItemDetailsTabBar;

                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top: parent.top;
                    }

                    PlasmaComponents.TabButton {
                        id: speedTabButton;
                        text: i18n("Speed");
                        visible: itemDevicePath && itemConnectionState == PlasmaNM.Enums.Activated && itemType != PlasmaNM.Enums.Vpn
                    }

                    PlasmaComponents.TabButton {
                        id: detailsTabButton;
                        text: i18n("Details");
                    }

                    Component.onCompleted: {
                        if (!speedTabButton.visible) {
                            currentTab = detailsTabButton;
                        }
                    }
                }

                Item {
                    id: connectionItemDetailsContent;

                    height: if (connectionItemDetailsTabBar.currentTab == speedTabButton) trafficMonitorTab.height;
                            else detailsTextTab.height;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top: connectionItemDetailsTabBar.bottom;
                    }

                    TrafficMonitor {
                        id: trafficMonitorTab;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            top: parent.top;
                        }
                        device: itemDevicePath;
                        visible: connectionItemDetailsTabBar.currentTab == speedTabButton &&
                                itemDevicePath && itemConnectionState == PlasmaNM.Enums.Activated && itemType != PlasmaNM.Enums.Vpn
                    }

                    TextEdit {
                        id: detailsTextTab;

                        height: implicitHeight;
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            top: parent.top;
                        }
                        color: theme.textColor;
                        readOnly: true;
                        selectByMouse: true;
                        wrapMode: TextEdit.WordWrap;
                        textFormat: Text.RichText;
                        text: itemDetails;
                        visible: connectionItemDetailsTabBar.currentTab == detailsTabButton
                    }
                }
            }

            PlasmaComponents.Button {
                id: editConnectionButton;

                height: !visible ? 0 : implicitHeight;
                implicitWidth: minimumWidth + padding.margins.left + padding.margins.right;
                anchors {
//                     horizontalCenter: parent.horizontalCenter;
                    left: parent.left;
                    top: connectionDetailsBackground.bottom;
                    topMargin: padding.margins.top;
                }
                text: i18n("Open configuration");
                visible: itemUuid;

                onClicked: {
                    handler.editConnection(itemUuid);
                }
            }
        }
    }

    states: [
        State {
            name: "collapsed";
            when: !expanded;
            StateChangeScript { script: if (connectionItemDetails.status == Loader.Ready) {connectionItemDetails.sourceComponent = undefined} }
        },

        State {
            name: "expanded";
            when: expanded;
            StateChangeScript { script: createContent(); }
            PropertyChanges { target: configButton; visible: true; }
            PropertyChanges { target: stateChangeButton; visible: true; }
            PropertyChanges { target: connectionItem; enabled: false; }
        }
    ]

    function createContent() {
        connectionItemDetails.sourceComponent = connectionItemDetailsComponent;
    }
}
