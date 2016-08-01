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

import QtQuick 2.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Item {
    id: toolbar

    height: wifiSwitchButton.height

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
        anchors {
            bottom: parent.bottom
            left: parent.left
            top: parent.top
        }

        SwitchButton {
            id: wifiSwitchButton

            checked: enabled && enabledConnections.wirelessEnabled
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled
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
            icon: enabled ? "network-mobile-on" : "network-mobile-off"
            visible: availableDevices.modemDeviceAvailable

            onClicked: {
                handler.enableWwan(checked);
            }
        }

        SwitchButton {
            id: planeModeSwitchButton

            property bool airplaneModeEnabled: false

            checked: airplaneModeEnabled
            icon: airplaneModeEnabled ? "network-flightmode-on" : "network-flightmode-off"

            onClicked: {
                handler.enableAirplaneMode(checked);
                airplaneModeEnabled = !airplaneModeEnabled;
            }

            Binding {
                target: connectionIconProvider
                property: "airplaneMode"
                value: planeModeSwitchButton.airplaneModeEnabled
            }
        }
    }

    PlasmaComponents.ToolButton {
        id: openEditorButton

        anchors {
            right: parent.right
            rightMargin: Math.round(units.gridUnit / 2)
            verticalCenter: parent.verticalCenter
        }

        iconSource: "configure"
        tooltip: i18n("Configure network connections...")

        onClicked: {
            handler.openEditor();
        }
    }
}
