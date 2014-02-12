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

    property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless &&
                                               (SecurityType == PlasmaNM.Enums.StaticWep || SecurityType == PlasmaNM.Enums.WpaPsk ||
                                                SecurityType == PlasmaNM.Enums.Wpa2Psk);

    property bool visibleDetails: false;
    property bool visiblePasswordDialog: false;

    enabled: true
    height: if (visibleDetails || visiblePasswordDialog)
                connectionItemBase.height + expandableComponentLoader.height + padding.margins.top + padding.margins.bottom;
            else
                connectionItemBase.height + padding.margins.top + padding.margins.bottom;

    Item {
        id: connectionItemBase;

        height: Math.max(connectionSvgIcon.height, connectionNameLabel.height + connectionStatusLabel.height);

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        PlasmaCore.SvgItem {
            id: connectionSvgIcon;

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            svg: svgNetworkIcons;
            elementId: ConnectionIcon;
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            height: paintedHeight;
            anchors {
                left: connectionSvgIcon.right;
                leftMargin: padding.margins.left;
                right: !connectionItem.containsMouse ? connectingIndicator.left : buttonRow.left;
                top: parent.top;
                topMargin: 0;
            }
            text: Name;
            elide: Text.ElideRight;
            font.weight: ConnectionState == PlasmaNM.Enums.Activated || Uuid ? Font.DemiBold : Font.Normal;
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false;
        }

        PlasmaComponents.Label {
            id: connectionStatusLabel;

            height: paintedHeight;
            anchors {
                left: connectionSvgIcon.right;
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
            running: ConnectionState == PlasmaNM.Enums.Activating;
            visible: running && !connectionItem.containsMouse;
        }

        Row {
            id: buttonRow;

            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.right;
            }
            spacing: 8;

            PlasmaCore.SvgItem {
                id: openDetailsButton;

                height: sizes.iconSize;
                width: height;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                visible: connectionItem.containsMouse;
                svg: svgNetworkIcons;
                elementId: openDetailsButtonMouse.containsMouse ? "showinfo-hover" : "showinfo";

                MouseArea {
                    id: openDetailsButtonMouse;
                    anchors {
                        bottom: parent.bottom;
                        left: parent.left;
                        right: parent.right;
                        top: parent.top;
                        bottomMargin: -4;
                        leftMargin: -4;
                        rightMargin: -4;
                        topMargin: -4;
                    }
                    hoverEnabled: true;

                    onClicked: {
                        visiblePasswordDialog = false;
                        visibleDetails = !visibleDetails;
                    }
                }
            }

            PlasmaCore.SvgItem {
                id: configureButton;

                height: sizes.iconSize;
                width: Uuid ? height: 0;
                anchors {
                    verticalCenter: parent.verticalCenter;
                }
                svg: svgNetworkIcons;
                elementId: configureButtonMouse.containsMouse ? "edit-hover" : "edit";
                visible: connectionItem.containsMouse;

                MouseArea {
                    id: configureButtonMouse;
                    anchors {
                        bottom: parent.bottom;
                        left: parent.left;
                        right: parent.right;
                        top: parent.top;
                        bottomMargin: -4;
                        leftMargin: -4;
                        rightMargin: -4;
                        topMargin: -4;
                    }
                    hoverEnabled: true;

                    onClicked: {
                        handler.editConnection(Uuid);
                    }
                }
            }

            PlasmaComponents.Button {
                id: stateChangeButton;

                implicitWidth: minimumWidth + padding.margins.left + padding.margins.right;
                text:if (ConnectionState == PlasmaNM.Enums.Deactivated)
                        i18n("Connect");
                    else
                        i18n("Disconnect");
                visible: connectionItem.containsMouse;

                onClicked: {
                    visibleDetails = false;
                    if (Uuid || !predictableWirelessPassword || visiblePasswordDialog) {
                        if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                            if (!predictableWirelessPassword && !Uuid) {
                                handler.addAndActivateConnection(DevicePath, SpecificPath);
                            } else if (visiblePasswordDialog) {
                                handler.addAndActivateConnection(DevicePath, SpecificPath, expandableComponentLoader.item.password);
                                visiblePasswordDialog = false;
                            } else {
                                handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
                            }
                        } else {
                            handler.deactivateConnection(ConnectionPath, DevicePath);
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
                visible: DevicePath && ConnectionState == PlasmaNM.Enums.Activated && Type != PlasmaNM.Enums.Vpn;

                PlasmaComponents.TabButton {
                    id: speedTabButton;
                    text: i18n("Speed");
                    visible: DevicePath && ConnectionState == PlasmaNM.Enums.Activated && Type != PlasmaNM.Enums.Vpn;
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
                    device: DevicePath;
                    visible: detailsTabBar.currentTab == speedTabButton &&
                             DevicePath && ConnectionState == PlasmaNM.Enums.Activated && Type != PlasmaNM.Enums.Vpn
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
                    text: ConnectionDetails;
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
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            return i18n("Connecting");
        } else if (ConnectionState == PlasmaNM.Enums.Deactivating) {
            return i18n("Disconnecting");
        } else if (ConnectionState == PlasmaNM.Enums.Deactivated) {
            var result = LastUsed;
            if (SecurityType > PlasmaNM.Enums.None)
                result += ", " + SecurityTypeString;

//             if (itemType == PlasmaNM.Enums.Wireless)
//                 result += ", " + i18n("Strength: %1%", itemSignal);

            return result;
        } else if (ConnectionState == PlasmaNM.Enums.Activated) {
            if (Type == PlasmaNM.Enums.Wired) {
                return i18n("Connected") + ", ⬇ " + Download + ", ⬆ " + Upload;
            } else if (Type == PlasmaNM.Enums.Wireless) {
                return i18n("Connected") + ", ⬇ " + Download + ", ⬆ " + Upload;/* + ", " + i18n("Strength: %1%", itemSignal);*/
            } else if (Type == PlasmaNM.Enums.Gsm || Type == PlasmaNM.Enums.Cdma) {
                // TODO
            } else {
                return i18n("Connected");
            }
        }
    }
}
