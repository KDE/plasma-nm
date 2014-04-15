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

import QtQuick 2.0
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.1 as PlasmaNM

Item {
    id: toolbar;

    height: wifiSwitchButton.height;

    PlasmaNM.EnabledConnections {
        id: enabledConnections;
    }

    PlasmaNM.AvailableDevices {
        id: availableDevices;
    }

    PlasmaCore.Svg {
        id: lineSvg;
        imagePath: "widgets/line";
    }

    Row {
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            top: parent.top;
        }

        SwitchButton {
            id: wifiSwitchButton;

            checked: enabled && enabledConnections.wirelessEnabled;
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled;
            icon: enabled ? "network-wireless-on" : "network-wireless-off";
            visible: availableDevices.wirelessDeviceAvailable;

            onClicked: {
                handler.enableWireless(checked);
            }
        }

        SwitchButton {
            id: wwanSwitchButton;

            checked: enabled && enabledConnections.wwanEnabled;
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !planeModeSwitchButton.airplaneModeEnabled;
            icon: enabled ? "network-mobile-on" : "network-mobile-off";
            visible: availableDevices.modemDeviceAvailable;

            onClicked: {
                handler.enableWwan(checked);
            }
        }

        SwitchButton {
            id: planeModeSwitchButton;

            property bool airplaneModeEnabled: false;

            checked: airplaneModeEnabled;
            icon: airplaneModeEnabled ? "flightmode-on" : "flightmode-off";

            onClicked: {
                handler.enableAirplaneMode(checked);
                airplaneModeEnabled = !airplaneModeEnabled;
                console.log(airplaneModeEnabled);
            }
        }
    }

    PlasmaComponents.ToolButton {
        id: openEditorButton;

        anchors {
            bottom: parent.bottom;
            bottomMargin: padding.margins.bottom/2;
            right: parent.right;
            rightMargin: padding.margins.right;
            top: parent.top;
            topMargin: padding.margins.top/2;
        }

        iconSource: "configure";

        onClicked: {
            handler.openEditor();
        }
    }

    PlasmaComponents.ToolButton {
        id: refreshButton;

        anchors {
            bottom: parent.bottom;
            bottomMargin: padding.margins.bottom/2;
            right: openEditorButton.left;
            rightMargin: padding.margins.right;
            top: parent.top;
            topMargin: padding.margins.top/2;
        }

        iconSource: "view-refresh";

        onClicked: {
            handler.requestScan();
        }
    }
}
