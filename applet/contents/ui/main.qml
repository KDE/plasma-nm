/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.2
import org.kde.plasma.plasmoid 2.0
import org.kde.kirigami 2.20 as Kirigami
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement as PlasmaNM
import org.kde.networkmanager as NMQt
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
    readonly property bool inPanel: (Plasmoid.location === PlasmaCore.Types.TopEdge
        || Plasmoid.location === PlasmaCore.Types.RightEdge
        || Plasmoid.location === PlasmaCore.Types.BottomEdge
        || Plasmoid.location === PlasmaCore.Types.LeftEdge)
    property alias planeModeSwitchAction: planeAction

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
    Plasmoid.icon: inPanel ? connectionIconProvider.connectionIcon + "-symbolic" : connectionIconProvider.connectionTooltipIcon
    switchWidth: Kirigami.Units.gridUnit * 10
    switchHeight: Kirigami.Units.gridUnit * 10

    // Only exists because the default CompactRepresentation doesn't expose
    // a middle-click action.
    // TODO remove once it gains that feature.
    compactRepresentation: CompactRepresentation {
        airplaneModeAvailable: mainWindow.airplaneModeAvailable
        iconName: Plasmoid.icon
    }
    fullRepresentation: PopupDialog {
        id: dialogItem
        nmHandler: handler
        nmStatus: networkStatus
        Layout.minimumWidth: Kirigami.Units.iconSizes.medium * 10
        Layout.minimumHeight: Kirigami.Units.gridUnit * 20
        anchors.fill: parent
        focus: true
    }

    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: i18n("Enable Wi-Fi")
            icon.name: "network-wireless-on"
            priority: PlasmaCore.Action.LowPriority
            checkable: true
            checked: enabledConnections.wirelessEnabled
            visible: enabledConnections.wirelessHwEnabled
                        && availableDevices.wirelessDeviceAvailable
                        && !PlasmaNM.Configuration.airplaneModeEnabled
            onTriggered: checked => {handler.enableWireless(checked)}
        },
        PlasmaCore.Action {
            text: i18n("Enable Mobile Network")
            icon.name: "network-mobile-on"
            priority: PlasmaCore.Action.LowPriority
            checkable: true
            checked: enabledConnections.wwanEnabled
            visible: enabledConnections.wwanHwEnabled
                        && availableDevices.modemDeviceAvailable
                        && !PlasmaNM.Configuration.airplaneModeEnabled
            onTriggered: checked => {handler.enableWwan(checked)}
        },
        PlasmaCore.Action {
            id: planeAction
            text: i18n("Enable Airplane Mode")
            icon.name: "network-flightmode-on"
            priority: PlasmaCore.Action.LowPriority
            checkable: true
            checked: PlasmaNM.Configuration.airplaneModeEnabled
            visible: mainWindow.airplaneModeAvailable
            onTriggered: checked => {
                handler.enableAirplaneMode(checked)
                PlasmaNM.Configuration.airplaneModeEnabled = checked
            }
        },
        PlasmaCore.Action {
            text: i18n("Open Network Login Page…")
            icon.name: "network-flightmode-on"
            priority: PlasmaCore.Action.LowPriority
            visible: networkStatus.connectivity === NMQt.NetworkManager.Portal
            onTriggered: Qt.openUrlExternally("http://networkcheck.kde.org")
        }
    ]

    PlasmaCore.Action {
        id: configureAction
        text: i18n("&Configure Network Connections…")
        icon.name: "configure"
        visible: kcmAuthorized
        shortcut: "alt+d, s"
        onTriggered: KCMUtils.KCMLauncher.openSystemSettings(kcm)
    }

    Component.onCompleted: {
        plasmoid.setInternalAction("configure", configureAction);
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
        connectivity: networkStatus.connectivity
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
