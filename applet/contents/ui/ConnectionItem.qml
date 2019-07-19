/*
    Copyright 2013-2017 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.4 as Controls
import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.kquickcontrolsaddons 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    id: connectionItem

    property bool activating: ConnectionState == PlasmaNM.Enums.Activating
    property int  baseHeight: Math.max(units.iconSizes.medium, connectionNameLabel.height + connectionStatusLabel.height) + Math.round(units.gridUnit / 2)
    property bool expanded: visibleDetails || visiblePasswordDialog
    property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless &&
                                               (SecurityType == PlasmaNM.Enums.StaticWep || SecurityType == PlasmaNM.Enums.WpaPsk ||
                                                SecurityType == PlasmaNM.Enums.Wpa2Psk)
    property bool showSpeed: plasmoid.expanded &&
                             ConnectionState == PlasmaNM.Enums.Activated &&
                             (Type == PlasmaNM.Enums.Wired ||
                              Type == PlasmaNM.Enums.Wireless ||
                              Type == PlasmaNM.Enums.Gsm ||
                              Type == PlasmaNM.Enums.Cdma)
    property bool visibleDetails: false
    property bool visiblePasswordDialog: false

    property real rxBytes: 0
    property real txBytes: 0

    checked: connectionItem.containsMouse
    enabled: true
    height: expanded ? baseHeight + separator.height + expandableComponentLoader.height + (2 * Math.round(units.gridUnit / 3)) : baseHeight

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            spacing: Math.round(units.gridUnit / 2)

            PlasmaCore.SvgItem {
                id: connectionSvgIcon
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                Layout.preferredHeight: units.iconSizes.medium
                Layout.preferredWidth: units.iconSizes.medium
                elementId: ConnectionIcon
                svg: PlasmaCore.Svg {
                    multipleImages: true
                    imagePath: "icons/network"
                    colorGroup: PlasmaCore.ColorScope.colorGroup
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                spacing: 0

                PlasmaComponents.Label {
                    id: connectionNameLabel
                    Layout.fillWidth: true
                    height: paintedHeight
                    elide: Text.ElideRight
                    font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
                    font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
                    text: ItemUniqueName
                    textFormat: Text.PlainText
                }

                PlasmaComponents.Label {
                    id: connectionStatusLabel
                    Layout.fillWidth: true
                    height: paintedHeight
                    elide: Text.ElideRight
                    font.pointSize: theme.smallestFont.pointSize
                    opacity: 0.6
                    text: itemText()
                }
            }

            PlasmaComponents.BusyIndicator {
                id: connectingIndicator
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                Layout.preferredHeight: units.iconSizes.medium
                Layout.preferredWidth: units.iconSizes.medium
                running: plasmoid.expanded && !stateChangeButton.visible && ConnectionState == PlasmaNM.Enums.Activating
                visible: running
                opacity: visible
            }

            PlasmaComponents.Button {
                id: stateChangeButton
                Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                opacity: connectionView.currentVisibleButtonIndex == index ? 1 : 0
                visible: opacity != 0
                text: (ConnectionState == PlasmaNM.Enums.Deactivated) ? i18n("Connect") : i18n("Disconnect")

                Behavior on opacity { NumberAnimation { duration: units.shortDuration } }

                onClicked: changeState()
            }

            MouseArea {
                acceptedButtons: Qt.RightButton
                Layout.alignment: Qt.AlignTop | Qt.AlignLeft
                width: parent.width
                height: parent.height
                onPressed: {
                    contextMenu.visualParent = parent
                    contextMenu.prepare();
                    contextMenu.open(mouse.x, mouse.y)
                }
            }
        }

        ColumnLayout {
            Layout.fillHeight: true
            PlasmaCore.SvgItem {
                id: separator
                height: lineSvg.elementSize("horizontal-line").height
                Layout.fillWidth: true
                Layout.maximumHeight: height
                elementId: "horizontal-line"
                svg: PlasmaCore.Svg { id: lineSvg; imagePath: "widgets/line" }
                visible: connectionItem.expanded
                opacity: visible
            }

            Loader {
                id: expandableComponentLoader
                Layout.fillHeight: true
                Layout.fillWidth: true
                height: childrenRect.height
            }
        }
    }


    PlasmaComponents.Menu {
        id: contextMenu

        property Component showQRComponent: null

        function prepare() {
            showQRMenuItem.visible = false;

            if (Uuid && Type === PlasmaNM.Enums.Wireless &&
                    (SecurityType === PlasmaNM.Enums.StaticWep || SecurityType === PlasmaNM.Enums.WpaPsk || SecurityType === PlasmaNM.Enums.Wpa2Psk)) {
                if (!showQRComponent) {
                    showQRComponent = Qt.createComponent("ShowQR.qml", this);
                    if (showQRComponent.status === Component.Error) {
                        console.warn("Cannot create QR code component:", showQRComponent.errorString());
                    }
                }

                showQRMenuItem.visible = (showQRComponent.status === Component.Ready);
            }
        }

        PlasmaComponents.MenuItem {
            text: ItemUniqueName
            enabled: false
        }
        PlasmaComponents.MenuItem {
            text: stateChangeButton.text
            icon: (ConnectionState == PlasmaNM.Enums.Deactivated) ? "network-connect" : "network-disconnect"
            onClicked: changeState()
        }
        PlasmaComponents.MenuItem {
            id: showQRMenuItem
            text: i18n("Show network's QR code")
            icon: "view-barcode"
            // Updated in prepare()
            visible: false
            onClicked: {
                const data = handler.wifiCode(ConnectionPath, Ssid, SecurityType)
                var obj = contextMenu.showQRComponent.createObject(connectionItem, { content: data });
                obj.showMaximized()
            }
        }
        PlasmaComponents.MenuItem {
            text: i18n("Configure...")
            icon: "settings-configure"
            onClicked: KCMShell.open([mainWindow.kcm, "--args", "Uuid=" + Uuid])
        }
    }

    Component {
        id: detailsComponent

        Item {
            height: childrenRect.height

            PlasmaComponents.TabBar {
                id: detailsTabBar

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                height: visible ? implicitHeight : 0
                visible: showSpeed

                PlasmaComponents.TabButton {
                    id: speedTabButton
                    text: i18n("Speed")
                }

                PlasmaComponents.TabButton {
                    id: detailsTabButton
                    text: i18n("Details")
                }

                Component.onCompleted: {
                    if (!speedTabButton.visible) {
                        currentTab = detailsTabButton
                    }
                }
            }

            DetailsText {
                anchors {
                    left: parent.left
                    leftMargin: units.iconSizes.medium
                    right: parent.right
                    top: detailsTabBar.visible ? detailsTabBar.bottom : parent.top
                    topMargin: Math.round(units.gridUnit / 3)
                }
                details: ConnectionDetails
                visible: detailsTabBar.currentTab == detailsTabButton
            }

            TrafficMonitor {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: detailsTabBar.visible ? detailsTabBar.bottom : parent.top
                    topMargin: Math.round(units.gridUnit / 3)
                }
                rxBytes: RxBytes
                txBytes: TxBytes
                interval: 2000
                visible: detailsTabBar.currentTab == speedTabButton
            }
        }
    }

    Component {
        id: passwordDialogComponent

        Item {
            property alias password: passwordField.text
            property alias passwordInput: passwordField

            height: childrenRect.height

            PasswordField {
                id: passwordField
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                securityType: SecurityType

                onAccepted: {
                    stateChangeButton.clicked()
                }

                onAcceptableInputChanged: {
                    stateChangeButton.enabled = acceptableInput
                }

                Component.onCompleted: {
                    stateChangeButton.enabled = false
                }

                Component.onDestruction: {
                    stateChangeButton.enabled = true
                }
            }
        }
    }

    Timer {
        id: timer
        repeat: true
        interval: 2000
        running: showSpeed
        property real prevRxBytes
        property real prevTxBytes
        Component.onCompleted: {
            prevRxBytes = RxBytes
            prevTxBytes = TxBytes
        }
        onTriggered: {
            rxBytes = (RxBytes - prevRxBytes) * 1000 / interval
            txBytes = (TxBytes - prevTxBytes) * 1000 / interval
            prevRxBytes = RxBytes
            prevTxBytes = TxBytes
        }
    }

    states: [
        State {
            name: "collapsed"
            when: !(visibleDetails || visiblePasswordDialog)
            StateChangeScript { script: if (expandableComponentLoader.status == Loader.Ready) {expandableComponentLoader.sourceComponent = undefined} }
        },

        State {
            name: "expandedDetails"
            when: visibleDetails
            StateChangeScript { script: createContent() }
        },

        State {
            name: "expandedPasswordDialog"
            when: visiblePasswordDialog
            StateChangeScript { script: createContent() }
            PropertyChanges { target: stateChangeButton; opacity: 1 }
        }
    ]

    function createContent() {
        if (visibleDetails) {
            expandableComponentLoader.sourceComponent = detailsComponent
        } else if (visiblePasswordDialog) {
            expandableComponentLoader.sourceComponent = passwordDialogComponent
            expandableComponentLoader.item.passwordInput.forceActiveFocus()
        }
    }

    function changeState() {
        visibleDetails = false
        if (Uuid || !predictableWirelessPassword || visiblePasswordDialog) {
            if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                if (!predictableWirelessPassword && !Uuid) {
                    handler.addAndActivateConnection(DevicePath, SpecificPath)
                } else if (visiblePasswordDialog) {
                    if (expandableComponentLoader.item.password != "") {
                        handler.addAndActivateConnection(DevicePath, SpecificPath, expandableComponentLoader.item.password)
                        visiblePasswordDialog = false
                    } else {
                        connectionItem.clicked()
                    }
                } else {
                    handler.activateConnection(ConnectionPath, DevicePath, SpecificPath)
                }
            } else {
                handler.deactivateConnection(ConnectionPath, DevicePath)
            }
        } else if (predictableWirelessPassword) {
            appletProxyModel.dynamicSortFilter = false
            visiblePasswordDialog = true
        }
    }

    /* This generates the formatted text under the connection name
       in the popup where the connections can be "Connect"ed and
       "Disconnect"ed. */
    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            if (Type == PlasmaNM.Enums.Vpn)
                return VpnState
            else
                return DeviceState
        } else if (ConnectionState == PlasmaNM.Enums.Deactivating) {
            if (Type == PlasmaNM.Enums.Vpn)
                return VpnState
            else
                return DeviceState
        } else if (ConnectionState == PlasmaNM.Enums.Deactivated) {
            var result = LastUsed
            if (SecurityType > PlasmaNM.Enums.NoneSecurity)
                result += ", " + SecurityTypeString
            return result
        } else if (ConnectionState == PlasmaNM.Enums.Activated) {
            if (showSpeed) {
                var downloadColor = theme.highlightColor
                // cycle upload color by 180 degrees
                var uploadColor = Qt.hsva((downloadColor.hsvHue + 0.5) % 1, downloadColor.hsvSaturation, downloadColor.hsvValue, downloadColor.a)

                return i18n("Connected, <font color='%1'>⬇</font> %2/s, <font color='%3'>⬆</font> %4/s",
                            downloadColor,
                            KCoreAddons.Format.formatByteSize(rxBytes),
                            uploadColor,
                            KCoreAddons.Format.formatByteSize(txBytes))
            } else {
                return i18n("Connected")
            }
        }
    }

    onShowSpeedChanged: {
        connectionModel.setDeviceStatisticsRefreshRateMs(DevicePath, showSpeed ? 2000 : 0)
    }

    onActivatingChanged: {
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            ListView.view.positionViewAtBeginning()
        }
    }

    onClicked: {
        if (visiblePasswordDialog) {
            appletProxyModel.dynamicSortFilter = true
            visiblePasswordDialog = false
        } else {
            visibleDetails = !visibleDetails
        }

        if (visibleDetails || visiblePasswordDialog) {
            ListView.view.currentIndex = index
        } else {
            ListView.view.currentIndex = -1
        }
    }

    onContainsMouseChanged: {
        if (connectionItem.containsMouse) {
            connectionView.currentVisibleButtonIndex = index
        }
    }
}
