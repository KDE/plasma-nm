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
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kquickcontrolsaddons 2.0

ColumnLayout {
    id: toolbar
    spacing: units.smallSpacing

    readonly property var displayWifiMessage: !wifiSwitchButton.checked && wifiSwitchButton.visible
    readonly property var displayWwanMessage: !wwanSwitchButton.checked && wwanSwitchButton.visible
    readonly property var displayplaneModeMessage: planeModeSwitchButton.checked && planeModeSwitchButton.visible

    function closeSearch() {
        searchToggleButton.checked = false
    }

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
        spacing: units.smallSpacing

        PlasmaExtras.SwitchButton {
            id: wifiSwitchButton

            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
            tooltipText: i18n("Enable Wi-Fi")
            icon: enabled ? "network-wireless-on" : "network-wireless-off"
            visible: availableDevices.wirelessDeviceAvailable

            onToggled: {
                handler.enableWireless(checked);
            }
        }

        PlasmaExtras.SwitchButton {
            id: wwanSwitchButton

            checked: enabled && enabledConnections.wwanEnabled
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
            tooltipText: i18n("Enable mobile network")
            icon: enabled ? "network-mobile-on" : "network-mobile-off"
            visible: availableDevices.modemDeviceAvailable

            onToggled: {
                handler.enableWwan(checked);
            }
        }

        // Add some extra spacing between the wifi and airplane mode toggles
        Item {
            Layout.preferredWidth: units.smallSpacing * 2
        }

        PlasmaExtras.SwitchButton {
            id: planeModeSwitchButton

            property bool initialized: false
            property bool airplaneModeEnabled: false

            checked: airplaneModeEnabled
            tooltipText: airplaneModeEnabled ?
                         xi18nc("@info", "Disable airplane mode<nl/><nl/>This will enable Wi-Fi and Bluetooth") :
                         xi18nc("@info", "Enable airplane mode<nl/><nl/>This will disable Wi-Fi and Bluetooth")
            icon: airplaneModeEnabled ? "network-flightmode-on" : "network-flightmode-off"
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
        }

        Item {
            Layout.fillWidth: true
        }

        PlasmaComponents3.ToolButton {
            id: hotspotButton

            icon {
                height: units.iconSizes.small
                width: units.iconSizes.small
                name: "network-wireless-on"
            }
            checkable: true
            text: i18n("Hotspot")
            visible: handler.hotspotSupported

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
                onHotspotCreated: {
                    hotspotButton.checked = true
                    tooltip.text = i18n("Disable Hotspot")
                }

                onHotspotDisabled: {
                    hotspotButton.checked = false
                    tooltip.text = i18n("Create Hotspot")
                }
            }

            Component.onCompleted: {
                checked = configuration.hotspotConnectionPath
                tooltip.text = configuration.hotspotConnectionPath ? i18n("Disable Hotspot") : i18n("Create Hotspot")
            }
        }

        PlasmaComponents3.ToolButton {
            id: searchToggleButton
            icon {
                height: units.iconSizes.small
                width: units.iconSizes.small
                name: "search"
            }
            checkable: true

            PlasmaComponents3.ToolTip {
                text: i18ndc("plasma-nm", "button tooltip", "Search the connections")
            }
        }

        PlasmaComponents3.ToolButton {
            id: openEditorButton
            icon {
                height: units.iconSizes.small
                width: units.iconSizes.small
                name: "configure"
            }
            visible: mainWindow.kcmAuthorized

            PlasmaComponents3.ToolTip {
                text: i18n("Configure network connections...")
            }

            onClicked: {
                KCMShell.openSystemSettings(mainWindow.kcm)
            }
        }
    }

   PlasmaComponents3.TextField {
        id: searchTextField

        Layout.fillWidth: true
        Layout.leftMargin: units.smallSpacing
        Layout.rightMargin: units.smallSpacing
        Layout.topMargin: units.smallSpacing
        Layout.bottomMargin: units.smallSpacing

        focus: true
        clearButtonShown: true
        placeholderText: i18ndc("plasma-nm", "text field placeholder text", "Search...")

        visible: searchToggleButton.checked
        onVisibleChanged: {
            if (visible) {
                searchTextField.forceActiveFocus()
            } else {
                text = ""
            }
        }
        Keys.onEscapePressed: {
            //Check if the searchbar is actually visible before accepting the escape key. Otherwise, the escape key cannot dismiss the applet until one interacts with some other element.
            if (searchToggleButton.checked) {
                searchToggleButton.checked = false;
            } else {
                event.accepted = false;
            }
        }

        onTextChanged: {
            // Show search field when starting to type directly
            if (text.length && !searchToggleButton.checked) {
                searchToggleButton.checked = true
            }
            appletProxyModel.setFilterRegExp(text)
        }
    }
}
