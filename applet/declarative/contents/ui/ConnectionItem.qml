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

    property bool predictableWirelessPassword: !itemUuid && itemType == PlasmaNM.Enums.Wireless &&
                                                itemSecurityType != PlasmaNM.Enums.None && itemSecurityType != PlasmaNM.Enums.DynamicWep && itemSecurityType != PlasmaNM.Enums.LEAP &&
                                                                                           itemSecurityType != PlasmaNM.Enums.WpaEap && itemSecurityType != PlasmaNM.Enums.Wpa2Eap;
    property bool visibleDetails: false;
    property bool visiblePasswordDialog: false;

    enabled: true
    height: if (visibleDetails || visiblePasswordDialog) connectionItemBase.height + expandableComponentLoader.height + padding.margins.top + padding.margins.bottom;
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
                visibleDetails = false;
                if (itemUuid || !predictableWirelessPassword || visiblePasswordDialog) {
                    if (itemConnectionState == PlasmaNM.Enums.Deactivated) {
                        if (!predictableWirelessPassword && !itemUuid) {
                            handler.addAndActivateConnection(itemDevicePath, itemSpecificPath);
                        } else if (visiblePasswordDialog) {
                            handler.addAndActivateConnection(itemDevicePath, itemSpecificPath, expandableComponentLoader.item.password, expandableComponentLoader.item.autoconnect);
                            visiblePasswordDialog = false;
                        } else {
                            handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
                        }
                    } else {
                        handler.deactivateConnection(itemConnectionPath, itemDevicePath);
                    }
                } else if (predictableWirelessPassword) {
                    visiblePasswordDialog = true;
                }
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
                    visiblePasswordDialog = false;
                    visibleDetails = !visibleDetails;
                }
            }
        }

        PlasmaComponents.ToolButton {
            height: sizes.iconSize;
            width: height;
            anchors {
                verticalCenter: parent.verticalCenter;
                right: configButton.left;
                rightMargin: padding.margins.right;
            }
            iconSource: "window-close";
            visible: visiblePasswordDialog;

            onClicked: {
                visiblePasswordDialog = !visiblePasswordDialog;
            }
        }
    }

    Loader {
        id: expandableComponentLoader;

        anchors {
            left: parent.left;
            right: parent.right;
            top: connectionItemBase.bottom;
            topMargin: padding.margins.top;
        }
    }

    Component {
        id: detailsComponent;

        Item {
            height: childrenRect.height + padding.margins.top;

            PlasmaComponents.TabBar {
                id: detailsTabBar;

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
                id: detailsContent;

                height: if (detailsTabBar.currentTab == speedTabButton) trafficMonitorTab.height;
                        else detailsTextTab.height;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: detailsTabBar.bottom;
                }

                TrafficMonitor {
                    id: trafficMonitorTab;
                    anchors {
                        left: parent.left;
                        right: parent.right;
                        top: parent.top;
                    }
                    device: itemDevicePath;
                    visible: detailsTabBar.currentTab == speedTabButton &&
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
                    visible: detailsTabBar.currentTab == detailsTabButton
                }
            }

            PlasmaComponents.Button {
                id: editConnectionButton;

                height: !visible ? 0 : implicitHeight;
                implicitWidth: minimumWidth + padding.margins.left + padding.margins.right;
                anchors {
                    left: parent.left;
                    top: detailsContent.bottom;
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

    Component {
        id: passwordDialogComponent;

        Item {
            height: childrenRect.height + padding.margins.top;

            property alias password: passwordInput.text;
            property alias autoconnect: automaticallyConnectCheckbox.checked;

            PlasmaComponents.TextField {
                id: passwordInput;

                width: 200;
                height: implicitHeight;
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    top: parent.top;
                }
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                placeholderText: i18n("Password...");
                onAccepted: {
                    connectButton.clicked();
                }
            }

            PlasmaComponents.CheckBox {
                id: showPasswordCheckbox;

                anchors {
                    left: passwordInput.left;
                    right: parent.right;
                    top: passwordInput.bottom;
                }
                checked: false;
                text: i18n("Show password");
            }

            PlasmaComponents.CheckBox {
                id: automaticallyConnectCheckbox;

                anchors {
                    left: passwordInput.left;
                    right: parent.right;
                    top: showPasswordCheckbox.bottom;
                }
                checked: true;
                text: i18n("Connect automatically");
            }
        }
    }

    states: [
        State {
            name: "collapsed";
            when: !(visibleDetails || visiblePasswordDialog);
            StateChangeScript { script: if (expandableComponentLoader.status == Loader.Ready) {expandableComponentLoader.sourceComponent = undefined} }
        },

        State {
            name: "expanded";
            when: visibleDetails || visiblePasswordDialog;
            StateChangeScript { script: createContent(); }
            PropertyChanges { target: configButton; visible: true; }
            PropertyChanges { target: stateChangeButton; visible: true; }
            PropertyChanges { target: connectionItem; enabled: false; }
        }
    ]

    function createContent() {
        if (visibleDetails) {
            expandableComponentLoader.sourceComponent = detailsComponent;
        } else if (visiblePasswordDialog) {
            expandableComponentLoader.sourceComponent = passwordDialogComponent;
        }
    }
}
