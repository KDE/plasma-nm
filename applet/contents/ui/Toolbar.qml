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
import org.kde.plasma.components 3.0 as PlasmaComponents3
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: toolbar
    spacing: units.smallSpacing

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
            wifiSwitchButton.enabled = enabled && availableDevices.wirelessDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
        }

        onWwanEnabledChanged: {
            wwanSwitchButton.checked = wwanSwitchButton.enabled && enabled
        }

        onWwanHwEnabledChanged: {
            wwanSwitchButton.enabled = enabled && availableDevices.modemDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
        }
    }

    PlasmaNM.Configuration {
        id: configuration
    }

    RowLayout {
        spacing: units.smallSpacing * 3

        PlasmaComponents3.CheckBox {
            id: wifiSwitchButton

            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled

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
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled

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
            property bool airplaneModeEnabled: false

            checked: airplaneModeEnabled

            icon.name: airplaneModeEnabled ? "network-flightmode-on" : "network-flightmode-off"

            visible: availableDevices.modemDeviceAvailable || availableDevices.wirelessDeviceAvailable

            onToggled: {
                handler.enableAirplaneMode(checked);
                airplaneModeEnabled = !airplaneModeEnabled;
            }

            Binding {
                target: configuration
                property: "airplaneModeEnabled"
                value: planeModeSwitchButton.airplaneModeEnabled
                when: planeModeSwitchButton.initialized
            }

            Component.onCompleted: {
                airplaneModeEnabled = configuration.airplaneModeEnabled
                initialized = true
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
                if (configuration.hotspotConnectionPath) {
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
                checked = configuration.hotspotConnectionPath
                tooltip.text = configuration.hotspotConnectionPath ? i18n("Disable Hotspot") : i18n("Create Hotspot")
            }
        }

        PlasmaComponents3.TextField {
            id: searchTextField

            Layout.fillWidth: true

            focus: true
            clearButtonShown: true
            placeholderText: i18nc("text field placeholder text", "Search...")

            onTextChanged: {
                appletProxyModel.setFilterRegExp(text)
            }
        }

        PlasmaComponents3.ToolButton {
            id: openEditorButton

            visible: mainWindow.kcmAuthorized && !(plasmoid.containmentDisplayHints & PlasmaCore.Types.ContainmentDrawsPlasmoidHeading)

            icon.name: "configure"

            PlasmaComponents3.ToolTip {
                text: i18n("Configure network connections...")
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
