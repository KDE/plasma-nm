/*
    Copyright 2013-2014 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.0
import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.1 as PlasmaNM

PlasmaComponents.ListItem {
    id: connectionItem;

    property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless &&
                                               (SecurityType == PlasmaNM.Enums.StaticWep || SecurityType == PlasmaNM.Enums.WpaPsk ||
                                                SecurityType == PlasmaNM.Enums.Wpa2Psk);

    property bool visibleDetails: false;
    property bool visiblePasswordDialog: false;

    property int baseHeight: connectionItemBase.height + padding.margins.top + padding.margins.bottom;

    height: (visibleDetails || visiblePasswordDialog) ? baseHeight + expandableComponentLoader.height : baseHeight;
    checked: ListView.isCurrentItem;
    enabled: true;

    PlasmaCore.Svg {
        id: svgNetworkIcons;

        multipleImages: true;
        imagePath: "icons/network";
    }

    Item {
        id: connectionItemBase;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }

        height: Math.max(units.iconSizes.medium, connectionNameLabel.height + connectionStatusLabel.height);

        PlasmaCore.SvgItem {
            id: connectionSvgIcon;

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: units.iconSizes.medium;
            width: height;
            svg: svgNetworkIcons;
            elementId: ConnectionIcon;
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            anchors {
                left: connectionSvgIcon.right;
                leftMargin: padding.margins.left;
                right: stateChangeButton.visible ? stateChangeButton.left : parent.right;
                bottom: connectionSvgIcon.verticalCenter
            }

            height: paintedHeight;
            elide: Text.ElideRight;
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal;
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false;
            text: ItemUniqueName;
        }

        PlasmaComponents.Label {
            id: connectionStatusLabel;

            anchors {
                left: connectionSvgIcon.right;
                leftMargin: padding.margins.left;
                right: stateChangeButton.visible ? stateChangeButton.left : parent.right;
                top: connectionNameLabel.bottom;
            }

            height: paintedHeight;
            elide: Text.ElideRight;
            font.pointSize: theme.smallestFont.pointSize;
            opacity: 0.6;
            text: itemText();
        }

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator;

            anchors {
                right: stateChangeButton.visible ? stateChangeButton.left : parent.right;
                rightMargin: padding.margins.right;
                verticalCenter: connectionSvgIcon.verticalCenter;
            }

            height: units.iconSizes.medium;
            width: height;
            running: ConnectionState == PlasmaNM.Enums.Activating;
            visible: running && !stateChangeButton.visible;
        }

        PlasmaComponents.Button {
            id: stateChangeButton;

            anchors {
                right: parent.right;
                rightMargin: padding.margins.right;
                verticalCenter: connectionSvgIcon.verticalCenter;
            }

            opacity: connectionItem.containsMouse ? 1 : 0
            visible: opacity != 0
            text: (ConnectionState == PlasmaNM.Enums.Deactivated) ? i18n("Connect") : i18n("Disconnect");

            Behavior on opacity { NumberAnimation { duration: units.shortDuration } }

            onClicked: changeState();
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
            height: childrenRect.height + padding.margins.bottom;

            PlasmaCore.SvgItem {
                id: detailsSeparator;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top;
                }

                height: lineSvg.elementSize("horizontal-line").height;
                width: parent.width;
                elementId: "horizontal-line";
                svg: PlasmaCore.Svg {
                    id: lineSvg;
                    imagePath: "widgets/line";
                }
            }

            Column {
                id: details;

                anchors {
                    left: parent.left;
                    leftMargin: units.iconSizes.medium;
                    right: parent.right;
                    top: detailsSeparator.bottom;
                    topMargin: padding.margins.top;
                }

                Repeater {
                    id: repeater;

                    property int longestString: 0;

                    model: ConnectionDetails.length/2;

                    Item {
                        anchors {
                            left: parent.left;
                            right: parent.right;
                            topMargin: padding.margins.top;
                        }

                        height: Math.max(detailNameLabel.height, detailValueLabel.height);

                        PlasmaComponents.Label {
                            id: detailNameLabel;

                            anchors {
                                left: parent.left;
                                leftMargin: repeater.longestString - paintedWidth + padding.margins.left;
                                verticalCenter: parent.verticalCenter;
                            }

                            height: paintedHeight;
                            font.pointSize: theme.smallestFont.pointSize;
                            horizontalAlignment: Text.AlignRight;
                            opacity: 0.6;
                            text: "<b>" + ConnectionDetails[index*2] + "</b>: &nbsp";

                            Component.onCompleted: {
                                if (paintedWidth > repeater.longestString) {
                                    repeater.longestString = paintedWidth;
                                }
                            }
                        }

                        PlasmaComponents.Label {
                            id: detailValueLabel;

                            anchors {
                                left: detailNameLabel.right;
                                right: parent.right;
                                verticalCenter: parent.verticalCenter;
                            }

                            height: paintedHeight;
                            elide: Text.ElideRight;
                            font.pointSize: theme.smallestFont.pointSize;
                            opacity: 0.6;
                            text: ConnectionDetails[(index*2)+1];
                            textFormat: Text.StyledText;
                        }
                    }
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
            name: "expandedDetails";
            when: visibleDetails;
            StateChangeScript { script: createContent(); }
        },

        State {
            name: "expandedPasswordDialog";
            when: visiblePasswordDialog;
            StateChangeScript { script: createContent(); }
            PropertyChanges { target: stateChangeButton; opacity: 1; }
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

    function changeState() {
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

    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            if (Type == PlasmaNM.Enums.Vpn)
                return VpnState;
            else
                return DeviceState;
        } else if (ConnectionState == PlasmaNM.Enums.Deactivating) {
            if (Type == PlasmaNM.Enums.Vpn)
                return VpnState;
            else
                return DeviceState;
        } else if (ConnectionState == PlasmaNM.Enums.Deactivated) {
            var result = LastUsed;
            if (SecurityType > PlasmaNM.Enums.None)
                result += ", " + SecurityTypeString;
            return result;
        } else if (ConnectionState == PlasmaNM.Enums.Activated) {
            if (Type == PlasmaNM.Enums.Wired) {
                return i18n("Connected") + ", ⬇ " + Download + ", ⬆ " + Upload;
            } else if (Type == PlasmaNM.Enums.Wireless) {
                return i18n("Connected") + ", ⬇ " + Download + ", ⬆ " + Upload;
            } else if (Type == PlasmaNM.Enums.Gsm || Type == PlasmaNM.Enums.Cdma) {
                return i18n("Connected") + ", ⬇ " + Download + ", ⬆ " + Upload;
            } else {
                return i18n("Connected");
            }
        }
    }

    onStateChanged: {
        if (state == "expandedPasswordDialog" || state == "expandedDetails") {
            ListView.view.currentIndex = index;
        }
    }

    onClicked: {
        if (visiblePasswordDialog) {
            visiblePasswordDialog = false;
        } else {
            visibleDetails = !visibleDetails;
        }

        if (!visibleDetails) {
            ListView.view.currentIndex = -1;
        }
    }
}
