/*
    SPDX-FileCopyrightText: 2013-2017 Jan Grulich <jgrulich@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

import QtQuick 2.2
import QtQuick.Layouts 1.2
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: toolbar
    spacing: PlasmaCore.Units.smallSpacing

    readonly property var displayWifiMessage: !wifiSwitchButton.checked && wifiSwitchButton.visible
    readonly property var displayWwanMessage: !wwanSwitchButton.checked && wwanSwitchButton.visible
    readonly property var displayplaneModeMessage: planeModeSwitchButton.checked && planeModeSwitchButton.visible

    PlasmaCore.Svg {
        id: lineSvg
        imagePath: "widgets/line"
    }

    PlasmaNM.EnabledConnections {
        id: enabledConnections

        onWirelessEnabledChanged: {
            wifiSwitchButton.checked = wifiSwitchButton.enabled && enabled
        }

        onWirelessHwEnabledChanged: {
            wifiSwitchButton.enabled = enabled && availableDevices.wirelessDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled
        }

        onWwanEnabledChanged: {
            wwanSwitchButton.checked = wwanSwitchButton.enabled && enabled
        }

        onWwanHwEnabledChanged: {
            wwanSwitchButton.enabled = enabled && availableDevices.modemDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled
        }
    }

    RowLayout {
        spacing: PlasmaCore.Units.smallSpacing * 3

        PlasmaComponents3.CheckBox {
            id: wifiSwitchButton

            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled

            icon.name: enabled ? "network-wireless-on" : "network-wireless-off"
            visible: availableDevices.wirelessDeviceAvailable

            onToggled: handler.enableWireless(checked);

            PlasmaComponents3.ToolTip {
                text: i18n("Enable Wi-Fi")
            }
        }

        PlasmaComponents3.CheckBox {
            id: wwanSwitchButton

            checked: enabled && enabledConnections.wwanEnabled
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !PlasmaNM.Configuration.airplaneModeEnabled

            icon.name: enabled ? "network-mobile-on" : "network-mobile-off"
            visible: availableDevices.modemDeviceAvailable

            onToggled: handler.enableWwan(checked);

            PlasmaComponents3.ToolTip {
                text: i18n("Enable mobile network")
            }
        }

        PlasmaComponents3.CheckBox {
            id: planeModeSwitchButton

            property bool initialized: false

            checked: PlasmaNM.Configuration.airplaneModeEnabled

            icon.name: PlasmaNM.Configuration.airplaneModeEnabled ? "network-flightmode-on" : "network-flightmode-off"

            visible: availableDevices.modemDeviceAvailable || availableDevices.wirelessDeviceAvailable

            onToggled: {
                handler.enableAirplaneMode(checked);
                PlasmaNM.Configuration.airplaneModeEnabled = checked;
            }

            PlasmaComponents3.ToolTip {
                text: planeModeSwitchButton.checked ?
                      xi18nc("@info", "Disable airplane mode<nl/><nl/>This will enable Wi-Fi and Bluetooth") :
                      xi18nc("@info", "Enable airplane mode<nl/><nl/>This will disable Wi-Fi and Bluetooth")
            }
        }

        PlasmaComponents3.ToolButton {
            id: hotspotButton

            visible: handler.hotspotSupported
            checkable: true

            text: i18n("Hotspot")
            icon.name: "network-wireless-on"

            onClicked: {
                if (PlasmaNM.Configuration.hotspotConnectionPath) {
                    checked = false
                    handler.stopHotspot()
                } else {
                    checked = true
                    handler.createHotspot()
                }
            }

            PlasmaComponents3.ToolTip {
                id: tooltip
            }

            Connections {
                target: handler
                function onHotspotCreated() {
                    hotspotButton.checked = true
                    tooltip.text = i18n("Disable Hotspot")
                }

                function onHotspotDisabled() {
                    hotspotButton.checked = false
                    tooltip.text = i18n("Create Hotspot")
                }
            }

            Component.onCompleted: {
                checked = PlasmaNM.Configuration.hotspotConnectionPath
                tooltip.text = PlasmaNM.Configuration.hotspotConnectionPath ? i18n("Disable Hotspot") : i18n("Create Hotspot")
            }
        }

        PlasmaComponents3.TextField {
            id: searchTextField

            Layout.fillWidth: true

            focus: true
            clearButtonShown: true
            placeholderText: i18nc("text field placeholder text", "Search…")

            onTextChanged: {
                appletProxyModel.setFilterRegExp(text)
            }
        }

        PlasmaComponents3.ToolButton {
            id: openEditorButton

            visible: mainWindow.kcmAuthorized && !(plasmoid.containmentDisplayHints & PlasmaCore.Types.ContainmentDrawsPlasmoidHeading)

            icon.name: "configure"

            PlasmaComponents3.ToolTip {
                text: i18n("Configure network connections…")
            }

            onClicked: {
                KCMShell.openSystemSettings(mainWindow.kcm)
            }
        }
    }

    Component.onCompleted: {
        searchTextField.forceActiveFocus()
    }
}
