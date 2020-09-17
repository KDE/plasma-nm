/*
    Copyright 2013-2017 Jan Grulich <jgrulich@redhat.com>
    Copyright 2020 Nate Graham <nate@kde.org>

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

import QtQuick 2.12
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.12

import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents // for ContextMenu+MenuItem/TabBar+TabButton
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaExtras.ExpandableListItem {
    id: connectionItem

    property bool activating: ConnectionState == PlasmaNM.Enums.Activating
    property bool deactivated: ConnectionState === PlasmaNM.Enums.Deactivated
    property bool passwordIsStatic: (SecurityType == PlasmaNM.Enums.StaticWep || SecurityType == PlasmaNM.Enums.WpaPsk ||
                                     SecurityType == PlasmaNM.Enums.Wpa2Psk || SecurityType == PlasmaNM.Enums.SAE)
    property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless && passwordIsStatic
    property bool showSpeed: plasmoid.expanded &&
                             ConnectionState == PlasmaNM.Enums.Activated &&
                             (Type == PlasmaNM.Enums.Wired ||
                              Type == PlasmaNM.Enums.Wireless ||
                              Type == PlasmaNM.Enums.Gsm ||
                              Type == PlasmaNM.Enums.Cdma)

    property real rxBytes: 0
    property real txBytes: 0

    icon: model.ConnectionIcon
    title: model.ItemUniqueName
    subtitle: itemText()
    iconUsesPlasmaSVG: true // We want the nice detailed network SVGs from the Plasma theme
    isBusy: plasmoid.expanded && model.ConnectionState == PlasmaNM.Enums.Activating
    isDefault: ConnectionState == PlasmaNM.Enums.Activated
    defaultActionButtonAction: Action {
        id: stateChangeButton
        icon.name: model.ConnectionState == PlasmaNM.Enums.Deactivated ? "network-connect" : "network-disconnect"
        text: model.ConnectionState == PlasmaNM.Enums.Deactivated ? i18n("Connect") : i18n("Disconnect")
        onTriggered: changeState()
    }
    showDefaultActionButtonWhenBusy: true
    customExpandedViewContent: detailsComponent
    contextMenu: PlasmaComponents.Menu {
        id: contextMenu

        property Component showQRComponent: null

        function prepare() {
            showQRMenuItem.visible = false;

            if (Uuid && Type === PlasmaNM.Enums.Wireless && passwordIsStatic) {
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
            icon: "view-barcode-qr"
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

        Column {
            spacing: units.smallSpacing

            PlasmaComponents.TabBar {
                id: detailsTabBar

                anchors {
                    left: parent.left
                    right: parent.right
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
                    if (!showSpeed) {
                        currentTab = detailsTabButton
                    }
                }
            }

            DetailsText {
                anchors {
                    left: parent.left
                    leftMargin: units.iconSizes.smallMedium
                    right: parent.right
                }
                details: ConnectionDetails
                visible: detailsTabBar.currentTab == detailsTabButton
            }

            TrafficMonitor {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                downloadSpeed: rxBytes
                uploadSpeed: txBytes
                visible: detailsTabBar.currentTab == speedTabButton
            }
        }
    }

    Component {
        id: passwordDialogComponent

        ColumnLayout {
            property alias password: passwordField.text
            property alias passwordInput: passwordField

            PasswordField {
                id: passwordField
                Layout.fillWidth: true
                Layout.leftMargin: units.iconSizes.smallMedium + units.smallSpacing * 4
                Layout.rightMargin: units.iconSizes.smallMedium + units.smallSpacing * 4

                securityType: SecurityType

                onAccepted: {
                    stateChangeButton.trigger()
                    connectionItem.customExpandedViewContent = detailsComponent
                }

                onAcceptableInputChanged: {
                    stateChangeButton.enabled = acceptableInput
                }

                Component.onCompleted: {
                    stateChangeButton.enabled = false
                    passwordField.forceActiveFocus()
                    appletProxyModel.dynamicSortFilter = true
                }

                Component.onDestruction: {
                    stateChangeButton.enabled = true
                    connectionItem.customExpandedViewContent = detailsComponent
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

    function changeState() {
        if (Uuid || !predictableWirelessPassword || connectionItem.customExpandedViewContent == passwordDialogComponent) {
            if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                if (!predictableWirelessPassword && !Uuid) {
                    handler.addAndActivateConnection(DevicePath, SpecificPath)
                } else if (connectionItem.customExpandedViewContent == passwordDialogComponent) {
                    if (connectionItem.customExpandedViewContentItem.password != "") {
                        handler.addAndActivateConnection(DevicePath, SpecificPath, connectionItem.customExpandedViewContentItem.password)
                        connectionItem.customExpandedViewContent = detailsComponent
                        connectionItem.collapse()
                    } else {
                        connectionItem.expand()
                    }
                } else {
                    handler.activateConnection(ConnectionPath, DevicePath, SpecificPath)
                }
            } else {
                handler.deactivateConnection(ConnectionPath, DevicePath)
            }
        } else if (predictableWirelessPassword) {
            appletProxyModel.dynamicSortFilter = false
            connectionItem.customExpandedViewContent = passwordDialogComponent
            connectionItem.expand()
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
        } else if (Uuid && ConnectionState == PlasmaNM.Enums.Deactivated) {
            return LastUsed
        } else if (ConnectionState == PlasmaNM.Enums.Activated) {
            if (showSpeed) {
                return i18n("Connected, ⬇ %1/s, ⬆ %2/s",
                    KCoreAddons.Format.formatByteSize(rxBytes),
                    KCoreAddons.Format.formatByteSize(txBytes))
            } else {
                return i18n("Connected")
            }
        }
        return ""
    }

    onShowSpeedChanged: {
        connectionModel.setDeviceStatisticsRefreshRateMs(DevicePath, showSpeed ? 2000 : 0)
    }

    onActivatingChanged: {
        if (ConnectionState == PlasmaNM.Enums.Activating) {
            ListView.view.positionViewAtBeginning()
        }
    }

    onDeactivatedChanged: {
        /* Separator is part of section, which is visible only when available connections exist. Need to determine
           if there is a connection in use, to show Separator. Otherwise need to hide it from the top of the list.
           Connections in use are always on top, only need to check the first one. */
        if (appletProxyModel.data(appletProxyModel.index(0, 0), PlasmaNM.NetworkModel.SectionRole) !== "Available connections") {
            if (connectionView.showSeparator != true) {
                connectionView.showSeparator = true
            }
            return
        }
        connectionView.showSeparator = false
        return
    }

    // Re-activate the default button if the password field is hidden without
    // sending a password
    onItemCollapsed: {
        stateChangeButton.enabled = true;
    }
}
