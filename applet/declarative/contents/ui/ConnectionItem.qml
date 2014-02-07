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
                                               (itemSecurityType == PlasmaNM.Enums.StaticWep || itemSecurityType == PlasmaNM.Enums.WpaPsk ||
                                                itemSecurityType == PlasmaNM.Enums.Wpa2Psk);
    property bool visibleDetails: false;
    property bool visiblePasswordDialog: false;

    enabled: true
    height: if (visibleDetails || visiblePasswordDialog)
                connectionItemBase.height + expandableComponentLoader.height + padding.margins.top + padding.margins.bottom;
            else
                connectionItemBase.height + padding.margins.top + padding.margins.bottom;

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

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            svg: svgNetworkIcons;
            elementId: itemConnectionIcon;
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            height: paintedHeight;
            anchors {
                left: connectionIcon.right;
                leftMargin: padding.margins.left;
                right: !connectionItem.containsMouse ? connectingIndicator.left : buttonRow.left;
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
                right: !connectionItem.containsMouse ? connectingIndicator.left : buttonRow.left;
                top: connectionNameLabel.bottom;
            }

            font.pointSize: theme.smallestFont.pointSize;
            color: "#99"+(theme.textColor.toString().substr(1))
            text: itemText();
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

        Row {
            id: buttonRow;

            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
            }
            spacing: 5;

            PlasmaCore.IconItem {
                id: openDetailsButton;

                height: sizes.iconSize;
                width: height;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                source: openDetailsButtonMouse.containsMouse ? "notification-inactive" : "notification-active";
                visible: connectionItem.containsMouse;

                MouseArea {
                    id: openDetailsButtonMouse;
                    anchors.fill: parent;
                    hoverEnabled: true;

                    onClicked: {
                        visiblePasswordDialog = false;
                        visibleDetails = !visibleDetails;
                    }
                }
            }

            PlasmaCore.IconItem {
                id: configureButton;

                height: sizes.iconSize;
                width: itemUuid ? height: 0;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                source: "configure";
                visible: connectionItem.containsMouse;
                active: configureButtonMouse.containsMouse;

                MouseArea {
                    id: configureButtonMouse;
                    anchors.fill: parent;
                    hoverEnabled: true;

                    onClicked: {
                        handler.editConnection(itemUuid);
                    }
                }
            }

            PlasmaComponents.Button {
                id: stateChangeButton;

                implicitWidth: minimumWidth + padding.margins.left + padding.margins.right;
                text:if (itemConnectionState == PlasmaNM.Enums.Deactivated)
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
                                handler.addAndActivateConnection(itemDevicePath, itemSpecificPath, expandableComponentLoader.item.password);
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

            PlasmaCore.SvgItem {
                id: detailsSeparator;

                height: lineSvg.elementSize("horizontal-line").height;
                width: parent.width;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top;
                }
                elementId: "horizontal-line";

                svg: PlasmaCore.Svg {
                    id: lineSvg;
                    imagePath: "widgets/line";
                }
            }

            PlasmaComponents.TabBar {
                id: detailsTabBar;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: detailsSeparator.bottom;
                    topMargin: padding.margins.top;
                }
                visible: itemDevicePath && itemConnectionState == PlasmaNM.Enums.Activated && itemType != PlasmaNM.Enums.Vpn;

                PlasmaComponents.TabButton {
                    id: speedTabButton;
                    text: i18n("Speed");
                    visible: itemDevicePath && itemConnectionState == PlasmaNM.Enums.Activated && itemType != PlasmaNM.Enums.Vpn;
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
                    top: detailsTabBar.visible ? detailsTabBar.bottom : detailsSeparator.bottom;
                    topMargin: padding.margins.top;
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
        }
    }

    Component {
        id: passwordDialogComponent;

        Item {
            height: childrenRect.height + padding.margins.top;

            property alias password: passwordInput.text;
            property alias passwordFocus: passwordInput

            PlasmaCore.SvgItem {
                id: passwordSeparator;

                height: lineSvg.elementSize("horizontal-line").height;
                width: parent.width;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top;
                }
                elementId: "horizontal-line";

                svg: PlasmaCore.Svg {
                    id: lineSvg;
                    imagePath: "widgets/line";
                }
            }

            PlasmaComponents.TextField {
                id: passwordInput;

                width: 200;
                height: implicitHeight;
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    top: passwordSeparator.bottom;
                    topMargin: padding.margins.top;
                }
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                placeholderText: i18n("Password...");
                onAccepted: {
                    stateChangeButton.clicked();
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
            PropertyChanges { target: openDetailsButton; visible: true; }
            PropertyChanges { target: stateChangeButton; visible: true; }
            PropertyChanges { target: connectionItem; enabled: false; }
        }
    ]

    function createContent() {
        if (visibleDetails) {
            expandableComponentLoader.sourceComponent = detailsComponent;
        } else if (visiblePasswordDialog) {
            expandableComponentLoader.sourceComponent = passwordDialogComponent;
            expandableComponentLoader.item.passwordFocus.forceActiveFocus();
        }
    }

    function itemText() {
        if (itemConnectionState == PlasmaNM.Enums.Activating) {
            return i18n("Connecting");
        } else if (itemConnectionState == PlasmaNM.Enums.Deactivating) {
            return i18n("Disconnecting");
        } else if (itemConnectionState == PlasmaNM.Enums.Deactivated) {
            var result = itemLastUsed;
            if (itemSecurityType > PlasmaNM.Enums.None)
                result += ", " + itemSecurityString;

//             if (itemType == PlasmaNM.Enums.Wireless)
//                 result += ", " + i18n("Strength: %1%", itemSignal);

            return result;
        } else if (itemConnectionState == PlasmaNM.Enums.Activated) {
            if (itemType == PlasmaNM.Enums.Wired) {
                return i18n("Connected") + ", " + itemSpeed;
            } else if (itemType == PlasmaNM.Enums.Wireless) {
                return i18n("Connected") + ", " + itemSpeed/* + ", " + i18n("Strength: %1%", itemSignal);*/
            } else if (itemType == PlasmaNM.Enums.Gsm || itemType == PlasmaNM.Enums.Cdma) {
                // TODO
            } else {
                return i18n("Connected");
            }
        }
    }
}
