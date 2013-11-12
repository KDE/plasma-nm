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

PlasmaComponents.ListItem {
    id: connectionItem;

    property bool expanded: false;
    property bool detailsView: false;

    // Read only properties
    property bool sectionHidden: (!connectionView.activeExpanded && itemSection == i18n("Active connections")) ||
                                 (!connectionView.previousExpanded && itemSection == i18n("Previous connections")) ||
                                 (!connectionView.unknownExpanded && itemSection == i18n("Unknown connections"))

    property bool predictableWirelessPassword: !itemUuid && itemType == PlasmaNM.Enums.Wireless &&
                                                itemSecurityType != PlasmaNM.Enums.None && itemSecurityType != PlasmaNM.Enums.DynamicWep && itemSecurityType != PlasmaNM.Enums.LEAP &&
                                                                                           itemSecurityType != PlasmaNM.Enums.WpaEap && itemSecurityType != PlasmaNM.Enums.Wpa2Eap;

    property int defaultCheckboxHeight: theme.defaultFont.mSize.height * 1.6 + buttonPadding.margins.top;

    signal itemExpanded(string itemUni, bool itemExpanded);

    enabled: true
    height: sizes.itemSize + ((connectionItemSettings.status != Loader.Ready || !expanded) ? 0 : connectionItemSettings.item.childrenRect.height + padding.margins.top);

    onClicked: {
        itemExpanded(itemUni, !expanded);
    }

    Item {
        id: connectionItemBasic;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;
        }

        PlasmaCore.Svg {
            id: svgIcons;

            multipleImages: true;
            imagePath: "icons/plasma-networkmanagement";
        }

        Item {
            id: connectionTypeIcon;

            height: sizes.iconSize;
            width: height;
            anchors {
                left: parent.left;
                verticalCenter: parent.verticalCenter;
            }

            PlasmaCore.SvgItem {
                id: svgConnectionTypeIcon;

                anchors.fill: parent;
                svg: svgIcons;
                elementId: itemConnectionIcon;
                visible: itemType != PlasmaNM.Enums.Vpn && itemType != PlasmaNM.Enums.Adsl && itemType != PlasmaNM.Enums.Pppoe;

                QIconItem {
                    id: connectionSecurityIcon;

                    width: connectionTypeIcon.width/2;
                    height: width;
                    anchors {
                        bottom: parent.bottom;
                        right: parent.right;
                    }
                    icon: QIcon("object-locked");
                    visible: itemSecurityType != PlasmaNM.Enums.None;
                }
            }

            QIconItem {
                id: pngConnectionTypeIcon;

                anchors.fill: parent;
                icon: QIcon("secure-card");
                visible: !svgConnectionTypeIcon.visible;
            }
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            anchors {
                left: connectionTypeIcon.right;
                right: parent.right;
                verticalCenter: parent.verticalCenter;
                leftMargin: padding.margins.left;
                rightMargin: padding.margins.right;
            }
            text: itemName;
            elide: Text.ElideRight;
//             font.weight: itemConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal;
            font.italic: itemConnectionState == PlasmaNM.Enums.Activating ? true : false;
        }

        MouseEventListener {
            id: leftActionArea;

            width: sizes.iconSize * 2;
            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
            }
            hoverEnabled: expanded;

            onClicked: {
                if (!expanded) {
                    itemExpanded(itemUni, !expanded);
                } else {
                    if (configureButton.active) {
                        detailsView = !detailsView
                    }
                }
            }

            PlasmaComponents.BusyIndicator {
                id: connectingIndicator;

                width: sizes.iconSize;
                height: width;
                anchors {
                    right: parent.right;
                    verticalCenter: parent.verticalCenter;
                    rightMargin: padding.margins.right;
                }
                running: itemConnectionState == PlasmaNM.Enums.Activating;
                visible: running;
            }

            PlasmaCore.IconItem {
                id: configureButton;

                width: sizes.iconSize;
                height: width;
                anchors {
                    right: parent.right;
                    verticalCenter: parent.verticalCenter;
                    rightMargin: padding.margins.right;
                }
                source: "configure";
                visible: expanded && !connectingIndicator.running;
                active: leftActionArea.containsMouse;
            }
        }
    }

    Loader {
        id: connectionItemSettings;

        anchors {
            left: parent.left;
            right: parent.right;
            top: connectionItemBasic.bottom;
            topMargin: padding.margins.top;
        }
    }

    Component {
        id: connectionConnectedComponent;

        Item {
            height: childrenRect.height;
            anchors {
                fill: parent;
            }
            // I had to move PlasmaNM.TrafficMonitor into a separated item, because it's not possible to adjust the height to 0 and the traffic monitor
            // still occupied the space.
            Item {
                id: trafficMonitor;

                height: visible ? 150 : 0;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                visible: (itemDevicePath && itemConnectionState == PlasmaNM.Enums.Activated && itemType != PlasmaNM.Enums.Vpn)

                PlasmaNM.TrafficMonitor {
                    anchors.fill: parent;
                    device: itemDevicePath;
                }
            }

            PlasmaComponents.Button {
                id: disconnectButton;

                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    top: trafficMonitor.bottom;
                }
                text: i18n("Disconnect");

                onClicked: {
                    itemExpanded(itemUni, false);

                    handler.deactivateConnection(itemConnectionPath, itemDevicePath);
                }
            }
        }
    }

    Component {
        id: connectionDisconnectedComponent;

        Item {
            height: childrenRect.height;
            anchors {
                fill: parent;
            }

            PlasmaComponents.TextField {
                id: passwordInput;

                width: 200;
                height: visible ? implicitHeight : 0;
                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    top: parent.top;
                }
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                visible: predictableWirelessPassword;
                placeholderText: i18n("Password...");
            }

            PlasmaComponents.CheckBox {
                id: showPasswordCheckbox;

                height: visible ? defaultCheckboxHeight : 0;
                anchors {
                    left: passwordInput.left;
                    right: parent.right;
                    top: passwordInput.bottom;
                }
                visible: predictableWirelessPassword;
                checked: false;
                text: i18n("Show password");
            }

            PlasmaComponents.CheckBox {
                id: automaticallyConnectCheckbox;

                height: visible ? defaultCheckboxHeight : 0;
                anchors {
                    left: passwordInput.left;
                    right: parent.right;
                    top: showPasswordCheckbox.bottom;
                }
                visible: predictableWirelessPassword;
                checked: true;
                text: i18n("Automatically connect");
            }

            PlasmaComponents.Button {
                id: connectButton;

                anchors {
                    horizontalCenter: parent.horizontalCenter;
                    top: automaticallyConnectCheckbox.bottom;
                }
                text: i18n("Connect");

                onClicked: {
                    itemExpanded(itemUni, false);

                    if (itemConnectionState != PlasmaNM.Enums.Activated && itemConnectionState != PlasmaNM.Enums.Activating) {
                        if (itemUuid) {
                            handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
                        } else {
                            handler.addAndActivateConnection(itemDevicePath, itemSpecificPath, passwordInput.text, automaticallyConnectCheckbox.checked);
                        }
                    }
                }
            }
        }
    }

    Component {
        id: connectionDetailsComponent;

        Item {
            height: childrenRect.height;
            anchors {
                fill: parent;
            }

            Item {
                height: visible ? childrenRect.height : 0;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                }
                visible: detailsView;

                TextEdit {
                    id: detailsText;

                    width: detailsView.width;
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
                }

                PlasmaComponents.Button {
                    id: editButton;

                    anchors {
                        horizontalCenter: parent.horizontalCenter;
                        top: detailsText.bottom;
                        topMargin: padding.margins.top;
                    }
                    text: i18n("Edit connection");
                    enabled: itemUuid;

                    onClicked: {
                        itemExpanded(itemUni, false);
                        handler.editConnection(itemUuid);
                    }
                }
            }
        }
    }

    states: [
        State {
            name: "Collapsed";
            when: !expanded && !sectionHidden;
            StateChangeScript { script: if (connectionItemSettings.status == Loader.Ready) {connectionItemSettings.sourceComponent = undefined} }
        },

        State {
            name: "CollapsedHidden";
            when: sectionHidden;
            StateChangeScript { script: if (connectionItemSettings.status == Loader.Ready) {connectionItemSettings.sourceComponent = undefined} }
            PropertyChanges { target: connectionItem; height: 0; }
            PropertyChanges { target: connectionItem; visible: false; }
        },

        State {
            name: "ConnectionExpanded";
            when: expanded && !sectionHidden;
            StateChangeScript { script: createContent(); }
            AnchorChanges { target: connectionItemBasic; anchors.bottom: undefined; }
        }
    ]

    transitions: Transition {
        NumberAnimation { duration: 300; properties: "height" }
    }

    function createContent() {
        if (itemConnectionState == PlasmaNM.Enums.Activated || itemConnectionState == PlasmaNM.Enums.Activating)
            connectionItemSettings.sourceComponent = connectionConnectedComponent;
        else
            connectionItemSettings.sourceComponent = connectionDisconnectedComponent;
    }

    Behavior on height {
        NumberAnimation { duration: 300 }
    }

    PlasmaCore.FrameSvgItem {
        id: buttonPadding;

        anchors.fill: parent;
        imagePath: "widgets/button";
        prefix: "normal";
        opacity: 0;
    }

    onExpandedChanged: {
        if (!expanded) {
            detailsView = false;
        }
    }

    onDetailsViewChanged: {
        if (detailsView) {
            if (connectionItemSettings.status == Loader.Ready)
                connectionItemSettings.sourceComponent = undefined;
            connectionItemSettings.sourceComponent = connectionDetailsComponent;
        } else {
            if (connectionItemSettings.status == Loader.Ready)
                connectionItemSettings.sourceComponent = undefined;
            createContent();
        }
    }
}
