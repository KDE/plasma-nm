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

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.locale 0.1 as KLocale
import org.kde.networkmanagement 0.1 as PlasmaNM

ListItem {
    id: connectionItem;

    property int baseHeight: connectionItemBase.height + Math.round(units.gridUnit / 3);
    property bool expanded: visibleDetails || visiblePasswordDialog;
    property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless &&
                                               (SecurityType == PlasmaNM.Enums.StaticWep || SecurityType == PlasmaNM.Enums.WpaPsk ||
                                                SecurityType == PlasmaNM.Enums.Wpa2Psk);
    property bool showSpeed: ConnectionState == PlasmaNM.Enums.Activated &&
                             (Type == PlasmaNM.Enums.Wimax ||
                              Type == PlasmaNM.Enums.Wired ||
                              Type == PlasmaNM.Enums.Wireless ||
                              Type == PlasmaNM.Enums.Gsm ||
                              Type == PlasmaNM.Enums.Cdma);
    property bool visibleDetails: false;
    property bool visiblePasswordDialog: false;

    height: expanded ? baseHeight + expandableComponentLoader.height : baseHeight;
    enabled: true;

    KLocale.Locale { id: locale }

    PlasmaCore.Svg {
        id: svgNetworkIcons;

        multipleImages: true;
        imagePath: "icons/plasma-networkmanagement2";
    }

    PlasmaCore.DataSource {
        id: dataSource;

        property string downloadSource: "network/interfaces/" + DeviceName + "/receiver/data";
        property string uploadSource: "network/interfaces/" + DeviceName + "/transmitter/data";

        engine: "systemmonitor";
        connectedSources: showSpeed && mainWindow.isExpanded ? [downloadSource, uploadSource] : [];
        interval: 2000;
    }

    Item {
        id: connectionItemBase;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            // Reset top margin from PlasmaComponents.ListItem
            topMargin: -Math.round(units.gridUnit / 2);
        }

        height: Math.max(theme.iconSizes.toolbar, connectionNameLabel.height + connectionStatusLabel.height) + Math.round(units.gridUnit / 2);

        PlasmaCore.SvgItem {
            id: connectionSvgIcon;

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }

            height: theme.iconSizes.toolbar;
            width: height;
            svg: svgNetworkIcons;
            elementId: ConnectionIcon;
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            anchors {
                left: connectionSvgIcon.right;
                leftMargin: Math.round(units.gridUnit / 2);
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
                leftMargin: Math.round(units.gridUnit / 2);
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
                rightMargin: Math.round(units.gridUnit / 2);
                verticalCenter: connectionSvgIcon.verticalCenter;
            }

            height: theme.iconSizes.toolbar;
            width: height;
            running: ConnectionState == PlasmaNM.Enums.Activating;
            visible: running && !stateChangeButton.visible;
        }

        PlasmaComponents.Button {
            id: stateChangeButton;

            anchors {
                right: parent.right;
                rightMargin: Math.round(units.gridUnit / 2);
                verticalCenter: connectionSvgIcon.verticalCenter;
            }

            opacity: connectionItem.containsMouse ? 1 : 0
            visible: opacity != 0
            text: (ConnectionState == PlasmaNM.Enums.Deactivated) ? i18n("Connect") : i18n("Disconnect");

            Behavior on opacity { NumberAnimation { duration: 50 } }

            onClicked: changeState();
        }
    }

    Loader {
        id: expandableComponentLoader;

        anchors {
            left: parent.left;
            right: parent.right;
            top: connectionItemBase.bottom;
        }
    }

    Component {
        id: detailsComponent;

        Item {
            height: childrenRect.height;

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
                    topMargin: Math.round(units.gridUnit / 3);
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
                    topMargin: Math.round(units.gridUnit / 3);
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
                    opacity: 0.6;
                    readOnly: true;
                    selectByMouse: true;
                    textFormat: Text.RichText;
                    text: ConnectionDetails;
                    visible: detailsTabBar.currentTab == detailsTabButton;
                    wrapMode: TextEdit.WordWrap;
                }
            }
        }
    }

    Component {
        id: passwordDialogComponent;

        Item {
            height: childrenRect.height;

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
                    topMargin: Math.round(units.gridUnit / 3);
                }
                echoMode: showPasswordCheckbox.checked ? TextInput.Normal : TextInput.Password
                placeholderText: i18n("Password...");
                validator: RegExpValidator {
                                regExp: if (SecurityType == PlasmaNM.Enums.StaticWep) {
                                            /^(?:[\x20-\x7F]{5}|[0-9a-fA-F]{10}|[\x20-\x7F]{13}|[0-9a-fA-F]{26}){1}$/
                                        } else {
                                            /^(?:[\x20-\x7F]{8,64}){1}$/
                                        }
                                }

                onAccepted: {
                    stateChangeButton.clicked();
                }

                onAcceptableInputChanged: {
                    stateChangeButton.enabled = acceptableInput;
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

            Component.onCompleted: {
                stateChangeButton.enabled = false;
            }

            Component.onDestruction: {
                stateChangeButton.enabled = true;
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
                    if (expandableComponentLoader.item.password != "") {
                        handler.addAndActivateConnection(DevicePath, SpecificPath, expandableComponentLoader.item.password);
                    }
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
            if (showSpeed && dataSource.data && dataSource.data[dataSource.downloadSource] && dataSource.data[dataSource.uploadSource]) {
                return i18n("Connected, ⬇ %1/s, ⬆ %2/s",
                            locale.formatByteSize(dataSource.data[dataSource.downloadSource].value * 1024 || 0),
                            locale.formatByteSize(dataSource.data[dataSource.uploadSource].value * 1024 || 0))
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
