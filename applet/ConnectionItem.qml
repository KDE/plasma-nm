/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>
    SPDX-FileCopyrightText: 2020 Nate Graham <nate@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Templates as T

import org.kde.coreaddons 1.0 as KCoreAddons
import org.kde.kcmutils as KCMUtils

import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.plasma.plasmoid 2.0

PlasmaExtras.ExpandableListItem {
    id: connectionItem

    required property var model
    required property var listView
    required property int index

    property bool activating: model.ConnectionState === PlasmaNM.Enums.Activating
    property bool deactivated: model.ConnectionState === PlasmaNM.Enums.Deactivated
    property bool passwordIsStatic: (model.SecurityType === PlasmaNM.Enums.StaticWep || model.SecurityType == PlasmaNM.Enums.WpaPsk ||
                                     model.SecurityType === PlasmaNM.Enums.Wpa2Psk || model.SecurityType == PlasmaNM.Enums.SAE)
    property bool predictableWirelessPassword: !model.Uuid && model.Type === PlasmaNM.Enums.Wireless && passwordIsStatic
    property bool showSpeed: mainWindow.expanded && model.ConnectionState === PlasmaNM.Enums.Activated

    property real rxSpeed: 0
    property real txSpeed: 0

    property alias passwordDialogComponent: passwordDialogComponent

    icon: model.ConnectionIcon
    title: model.ItemUniqueName
    subtitle: itemText()
    isBusy: mainWindow.expanded && model.ConnectionState === PlasmaNM.Enums.Activating
    defaultActionButtonAction: Action {
        id: stateChangeButton

        readonly property bool isDeactivated: connectionItem.model.ConnectionState === PlasmaNM.Enums.Deactivated

        enabled: {
            if (!connectionItem.expanded) {
                return true;
            }
            if (connectionItem.customExpandedViewContent === passwordDialogComponent) {
                return connectionItem.customExpandedViewContentItem?.passwordField.acceptableInput ?? false;
            }
            return true;
        }

        icon.name: isDeactivated ? "network-connect" : "network-disconnect"
        text: isDeactivated ? i18n("Connect") : i18n("Disconnect")
        onTriggered: {
            connectionItem.changeState()
            listView.newIndexSelected(index)
        }
    }
    showDefaultActionButtonWhenBusy: true

    Keys.onPressed: event => {
        if (!connectionItem.expanded) {
            event.accepted = false;
            return;
        }

        if ((customExpandedViewContent === detailsComponent) && showSpeed) {
            if (event.key === Qt.Key_Right) {
                customExpandedViewContentItem.detailsTabBar.currentIndex = 1;
                event.accepted = true;
            } else if (event.key === Qt.Key_Left) {
                customExpandedViewContentItem.detailsTabBar.currentIndex = 0;
                event.accepted = true;
            }
        }
    }

    contextualActions: [
        Action {
            enabled: connectionItem.model.Uuid && connectionItem.model.Type === PlasmaNM.Enums.Wireless && connectionItem.passwordIsStatic
            text: i18nc("Share Wi-Fi network's password or QR code", "Share")
            icon.name: "view-barcode-qr"
            onTriggered: handler.requestWifiCode(connectionItem.model.ConnectionPath, connectionItem.model.Ssid, connectionItem.model.SecurityType);
        },
        Action {
            enabled: connectionItem.model.Uuid !== ""
            text: i18n("Configure…")
            icon.name: "configure"
            onTriggered: KCMUtils.KCMLauncher.openSystemSettings(mainWindow.kcm, ["--args", "Uuid=" + connectionItem.model.Uuid])
        }
    ]

    customExpandedViewContent: detailsComponent

    Accessible.description: `${model.AccessibleDescription} ${subtitle}`

    Component {
        id: detailsComponent

        Column {
            readonly property T.TabBar detailsTabBar: detailsTabBar

            spacing: Kirigami.Units.smallSpacing

            PlasmaComponents3.TabBar {
                id: detailsTabBar

                anchors {
                    left: parent.left
                    right: parent.right
                }
                // Be very careful when changing this as Qt (6.8) liked to bug out the tab highlight when the height is initially 0
                // https://bugs.kde.org/show_bug.cgi?id=495948
                height: implicitHeight
                implicitHeight: contentHeight
                position: T.TabBar.Header
                visible: connectionItem.showSpeed
                onCurrentIndexChanged: {
                    // Only if there are the two tabs.
                    if (connectionItem.showSpeed) {
                        Plasmoid.configuration.currentDetailsTab = ["speed", "details"][currentIndex];
                    }
                }

                PlasmaComponents3.TabButton {
                    id: speedTabButton
                    text: i18n("Speed")
                }

                PlasmaComponents3.TabButton {
                    id: detailsTabButton
                    text: i18n("Details")
                }

                Component.onCompleted: {
                    if (!connectionItem.showSpeed || Plasmoid.configuration.currentDetailsTab === "details") {
                        currentIndex = 1;
                    }

                    // Workaround for Qt bugging out the highlight when doing this as an expression on the property directly
                    // https://bugs.kde.org/show_bug.cgi?id=495948
                    height = Qt.binding(function() { return visible ? implicitHeight : 0 })
                }
            }

            DetailsText {
                id: detailsTextColumn

                width: parent.width
                visible: detailsTabBar.currentIndex === 1

                activeFocusOnTab: detailsModel && detailsModel.rowCount() > 0
                detailsModel: model.ConnectionDetailsModel

                Accessible.description: detailsModel ? detailsModel.accessibilityDescription() : ""

                Loader {
                    anchors.fill: parent
                    active: parent.activeFocus
                    asynchronous: true
                    z: -1

                    sourceComponent: PlasmaExtras.Highlight {
                        hovered: true
                    }
                }
            }

            FocusScope {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: trafficMonitorGraph.implicitHeight
                visible: detailsTabBar.currentIndex === 0

                activeFocusOnTab: true

                Accessible.description: i18nc("@info:tooltip", "Current download speed is %1 kibibytes per second; current upload speed is %2 kibibytes per second", Math.round(connectionItem.rxSpeed / 1024), Math.round(connectionItem.txSpeed / 1024))

                Loader {
                    anchors.fill: parent
                    active: parent.activeFocus
                    asynchronous: true
                    z: -1

                    sourceComponent: PlasmaExtras.Highlight {
                        hovered: true
                    }
                }

                PlasmaNM.TrafficMonitor {
                    id: trafficMonitorGraph
                    width: parent.width
                    downloadSpeed: connectionItem.rxSpeed
                    uploadSpeed: connectionItem.txSpeed
                }
            }

        }
    }

    Component {
        id: passwordDialogComponent

        ColumnLayout {
            property alias password: passwordField.text
            property alias passwordField: passwordField

            PasswordField {
                id: passwordField

                Layout.fillWidth: true
                Layout.leftMargin: Kirigami.Units.gridUnit
                Layout.rightMargin: Kirigami.Units.gridUnit

                securityType: connectionItem.model.SecurityType

                onAccepted: {
                    stateChangeButton.trigger()
                    connectionItem.customExpandedViewContent = detailsComponent
                }

                Component.onCompleted: {
                    passwordField.forceActiveFocus()
                    setDelayModelUpdates(true)
                }
            }
        }
    }

    Timer {
        id: timer
        repeat: true
        interval: 2000
        running: connectionItem.showSpeed
        triggeredOnStart: true
        // property int can overflow with the amount of bytes.
        property double prevRxBytes: 0
        property double prevTxBytes: 0
        onTriggered: {
            connectionItem.rxSpeed = prevRxBytes === 0 ? 0 : (connectionItem.model.RxBytes - prevRxBytes) * 1000 / interval
            connectionItem.txSpeed = prevTxBytes === 0 ? 0 : (connectionItem.model.TxBytes - prevTxBytes) * 1000 / interval
            prevRxBytes = connectionItem.model.RxBytes
            prevTxBytes = connectionItem.model.TxBytes
        }
    }

    function changeState() {
        if (model.Uuid || !predictableWirelessPassword || connectionItem.customExpandedViewContent == passwordDialogComponent) {
            if (model.ConnectionState == PlasmaNM.Enums.Deactivated) {
                if (!predictableWirelessPassword && !model.Uuid) {
                    handler.addAndActivateConnection(model.DevicePath, model.SpecificPath)
                } else if (connectionItem.customExpandedViewContent == passwordDialogComponent) {
                    const item = connectionItem.customExpandedViewContentItem;
                    if (item && item.password !== "") {
                        handler.addAndActivateConnection(model.DevicePath, model.SpecificPath, item.password)
                        connectionItem.customExpandedViewContent = detailsComponent
                        connectionItem.collapse()
                    } else {
                        connectionItem.expand()
                    }
                } else {
                    handler.activateConnection(model.ConnectionPath, model.DevicePath, model.SpecificPath)
                }
            } else {
                handler.deactivateConnection(model.ConnectionPath, model.DevicePath)
            }
        } else if (predictableWirelessPassword) {
            setDelayModelUpdates(true)
            connectionItem.customExpandedViewContent = passwordDialogComponent
            connectionItem.expand()
        }
    }

    /* This generates the formatted text under the connection name
       in the popup where the connections can be "Connect"ed and
       "Disconnect"ed. */
    function itemText() {
        if (model.ConnectionState === PlasmaNM.Enums.Activating || model.ConnectionState === PlasmaNM.Enums.Deactivating) {
            if (model.Type === PlasmaNM.Enums.Vpn) {
                return model.VpnState
            } else {
                return model.DeviceState
            }
        } else if (model.Uuid && model.ConnectionState === PlasmaNM.Enums.Deactivated) {
            return model.LastUsed
        } else if (model.ConnectionState === PlasmaNM.Enums.Activated && showSpeed) {
            return i18nc("Download and upload rates in some unit per second", "↓ %1/s, ↑ %2/s",
                KCoreAddons.Format.formatByteSize(rxSpeed),
                KCoreAddons.Format.formatByteSize(txSpeed))
        }
        return ""
    }

    function setDelayModelUpdates(delay: bool) {
        appletProxyModel.setData(appletProxyModel.index(index, 0), delay, PlasmaNM.NetworkModel.DelayModelUpdatesRole);
    }

    onShowSpeedChanged: {
        connectionModel.setDeviceStatisticsRefreshRateMs(model.DevicePath, showSpeed ? 2000 : 0)
    }

    onActivatingChanged: {
        if (model.ConnectionState === PlasmaNM.Enums.Activating) {
            ListView.view.positionViewAtBeginning()
        }
    }

    onItemCollapsed: {
        connectionItem.customExpandedViewContent = detailsComponent;
        setDelayModelUpdates(false);
    }

    Component.onDestruction: {
        setDelayModelUpdates(false);
    }
}
