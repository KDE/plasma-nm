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
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kquickcontrolsaddons 2.0

GridLayout {
    id: toolbar

    function closeSearch() {
        searchToggleButton.checked = false
    }

    rows: 2
    columns: 2

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

    Row {
        Layout.fillWidth: true

        SwitchButton {
            id: wifiSwitchButton

            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
            tooltip: i18n("Enable wireless")
            icon: enabled ? "network-wireless-on" : "network-wireless-off"
            visible: availableDevices.wirelessDeviceAvailable

            onClicked: {
                handler.enableWireless(checked);
            }
        }

        SwitchButton {
            id: wwanSwitchButton

            checked: enabled && enabledConnections.wwanEnabled
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
            tooltip: i18n("Enable mobile network")
            icon: enabled ? "network-mobile-on" : "network-mobile-off"
            visible: availableDevices.modemDeviceAvailable

            onClicked: {
                handler.enableWwan(checked);
            }
        }

        SwitchButton {
            id: planeModeSwitchButton

            property bool initialized: false
            property bool airplaneModeEnabled: false

            checked: airplaneModeEnabled
            tooltip: i18n("Enable airplane mode")
            icon: airplaneModeEnabled ? "network-flightmode-on" : "network-flightmode-off"
            visible: availableDevices.modemDeviceAvailable || availableDevices.wirelessDeviceAvailable

            onClicked: {
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
    }

    Row {
        Layout.column: 1

        PlasmaComponents.ToolButton {
            id: searchToggleButton
            iconSource: "search"
            tooltip: i18ndc("plasma-nm", "button tooltip", "Search the connections")
            checkable: true
        }

        PlasmaComponents.ToolButton {
            id: openEditorButton
            iconSource: "configure"
            tooltip: i18n("Configure network connections...")
            visible: mainWindow.kcmAuthorized

            onClicked: {
                KCMShell.open(mainWindow.kcm)
            }
        }
    }

    PlasmaComponents.TextField {
        id: searchTextField

        Layout.row: 1
        Layout.columnSpan: 2
        Layout.fillWidth: true
        Layout.leftMargin: units.smallSpacing
        Layout.rightMargin: units.smallSpacing
        Layout.bottomMargin: units.smallSpacing

        focus: true
        clearButtonShown: true
        placeholderText: i18ndc("plasma-nm", "text field placeholder text", "Search...")

        visible: searchToggleButton.checked
        onVisibleChanged: if (!visible) text = ""
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
