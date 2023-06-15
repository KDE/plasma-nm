/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.2
import org.kde.plasma.plasmoid 2.0
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import QtQuick.Layouts 1.1
import org.kde.kcmutils as KCMUtils
import org.kde.config as KConfig

PlasmoidItem {
    id: mainWindow

    readonly property string kcm: "kcm_networkmanagement"
    readonly property bool kcmAuthorized: KConfig.KAuthorized.authorizeControlModule("kcm_networkmanagement")
    readonly property bool delayModelUpdates: fullRepresentationItem !== null
        && fullRepresentationItem.connectionModel !== null
        && fullRepresentationItem.connectionModel.delayModelUpdates
    readonly property bool airplaneModeAvailable: availableDevices.modemDeviceAvailable || availableDevices.wirelessDeviceAvailable

    toolTipMainText: i18n("Networks")
    toolTipSubText: {
        const activeConnections = networkStatus.activeConnections;

        if (!airplaneModeAvailable) {
            return activeConnections;
        }

        if (PlasmaNM.Configuration.airplaneModeEnabled) {
            return i18nc("@info:tooltip", "Middle-click to turn off Airplane Mode");
        } else {
            const hint = i18nc("@info:tooltip", "Middle-click to turn on Airplane Mode");
            return activeConnections ? `${activeConnections}\n${hint}` : hint;
        }
    }

    Plasmoid.busy: connectionIconProvider.connecting
    Plasmoid.icon: connectionIconProvider.connectionTooltipIcon
    switchWidth: Kirigami.Units.gridUnit * 10
    switchHeight: Kirigami.Units.gridUnit * 10
    compactRepresentation: CompactRepresentation {
        airplaneModeAvailable: mainWindow.airplaneModeAvailable
        iconName: connectionIconProvider.connectionIcon
    }
    fullRepresentation: PopupDialog {
        id: dialogItem
        nmHandler: handler
        Layout.minimumWidth: Kirigami.Units.iconSizes.medium * 10
        Layout.minimumHeight: Kirigami.Units.gridUnit * 20
        anchors.fill: parent
        focus: true
    }

    function action_wifiSwitch() {
        handler.enableWireless(plasmoid.action("wifiSwitch").checked)
    }

    function action_wwanSwitch() {
        handler.enableWwan(plasmoid.action("wifiSwitch").checked)
    }

    function action_planeModeSwitch() {
        let checked = plasmoid.action("wifiSwitch").checked
        handler.enableAirplaneMode(checked)
        PlasmaNM.Configuration.airplaneModeEnabled = checked
    }

    function action_configure() {
        KCMUtils.KCMLauncher.openSystemSettings(kcm)
    }

    function action_showPortal() {
        Qt.openUrlExternally("http://networkcheck.kde.org")
    }

    Component.onCompleted: {
        Plasmoid.setAction("wifiSwitch", i18n("Enable Wi-Fi"), "network-wireless-on")
        plasmoid.action("wifiSwitch").priority = 0
        plasmoid.action("wifiSwitch").checkable = true
        plasmoid.action("wifiSwitch").checked = Qt.binding(() => enabledConnections.wirelessEnabled)
        plasmoid.action("wifiSwitch").visible = Qt.binding(() => enabledConnections.wirelessHwEnabled
                                                                 && availableDevices.wirelessDeviceAvailable
                                                                 && !PlasmaNM.Configuration.airplaneModeEnabled)

        Plasmoid.setAction("wwanSwitch", i18n("Enable Mobile Network"), "network-mobile-on")
        plasmoid.action("wwanSwitch").priority = 0
        plasmoid.action("wwanSwitch").checkable = true
        plasmoid.action("wwanSwitch").checked = Qt.binding(() => enabledConnections.wwanEnabled)
        plasmoid.action("wwanSwitch").visible = Qt.binding(() => enabledConnections.wwanHwEnabled
                                                                 && availableDevices.modemDeviceAvailable
                                                                 && !PlasmaNM.Configuration.airplaneModeEnabled)

        Plasmoid.setAction("planeModeSwitch", i18n("Enable Airplane Mode"), "network-flightmode-on")
        plasmoid.action("planeModeSwitch").priority = 0
        plasmoid.action("planeModeSwitch").checkable = true
        plasmoid.action("planeModeSwitch").checked = Qt.binding(() => PlasmaNM.Configuration.airplaneModeEnabled)
        plasmoid.action("planeModeSwitch").visible = Qt.binding(() => mainWindow.airplaneModeAvailable)

        plasmoid.setAction("showPortal", i18n("Open Network Login Page…"), "internet-services")
        plasmoid.action("showPortal").visible = Qt.binding(() => connectionIconProvider.needsPortal)

        plasmoid.removeAction("configure")
        if (kcmAuthorized) {
            plasmoid.setAction("configure", i18n("&Configure Network Connections…"), "configure", "alt+d, s")
        }
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections
    }

    PlasmaNM.AvailableDevices {
        id: availableDevices
    }

    PlasmaNM.NetworkStatus {
        id: networkStatus
    }

    PlasmaNM.ConnectionIcon {
        id: connectionIconProvider
    }

    PlasmaNM.Handler {
        id: handler
    }

    Timer {
        id: scanTimer
        interval: 10200
        repeat: true
        running: mainWindow.expanded && !PlasmaNM.Configuration.airplaneModeEnabled && !mainWindow.delayModelUpdates

        onTriggered: handler.requestScan()
    }
}
