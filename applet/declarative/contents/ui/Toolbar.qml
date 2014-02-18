/*
    Copyright 2013 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.networkmanagement 0.1 as PlasmaNM

Item {
    id: toolbar;

    height: wifiSwitchButton.height;

    PlasmaNM.EnabledConnections {
        id: enabledConnections;
    }

    PlasmaNM.AvailableDevices {
        id: availableDevices;
    }

    Row {
        anchors {
            bottom: parent.bottom;
            left: parent.left;
            top: parent.top;
        }
        spacing: 3;

        SwitchButton {
            id: wifiSwitchButton;

            checked: enabled && enabledConnections.wirelessEnabled;
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !globalConfig.airplaneModeEnabled;
            icon: checked ? "network-wireless-on" : "network-wireless-off";

            onClicked: {
                handler.enableWireless(!checked);
            }
        }

        SwitchButton {
            id: wwanSwitchButton;

            checked: enabled && enabledConnections.wwanEnabled;
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !globalConfig.airplaneModeEnabled;
            icon: checked ? "network-mobile-on" : "network-mobile-off";

            onClicked: {
                handler.enableWwan(!checked);
            }
        }

        SwitchButton {
            id: planeModeSwitchButton;

            checked: globalConfig.airplaneModeEnabled;
            icon: checked ? "flightmode-on" : "flightmode-off";

            onClicked: {
                handler.enableAirplaneMode(!checked);
                globalConfig.setAirplaneModeEnabled(!checked);
            }
        }
    }

    PlasmaComponents.Button {
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

    PlasmaComponents.Button {
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
