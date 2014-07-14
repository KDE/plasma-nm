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
        spacing: 3;

        SwitchButton {
            id: wifiSwitchButton;

            checked: enabled && enabledConnections.wirelessEnabled;
            enabled: enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable && !globalConfig.airplaneModeEnabled;
            icon: checked ? "network-wireless-on" : "network-wireless-off";
            tooltipText: checked ? i18n("Wireless enabled") : i18n("Wireless disabled");
            visible: availableDevices.wirelessDeviceAvailable;

            onClicked: {
                handler.enableWireless(!checked);
            }
        }

        SwitchButton {
            id: wwanSwitchButton;

            checked: enabled && enabledConnections.wwanEnabled;
            enabled: enabledConnections.wwanHwEnabled && availableDevices.modemDeviceAvailable && !globalConfig.airplaneModeEnabled;
            icon: checked ? "network-mobile-on" : "network-mobile-off";
            tooltipText: checked ? i18n("Mobile broadband enabled") : i18n("Mobile broadband disabled");
            visible: availableDevices.modemDeviceAvailable;

            onClicked: {
                handler.enableWwan(!checked);
            }
        }

        SwitchButton {
            id: planeModeSwitchButton;

            checked: globalConfig.airplaneModeEnabled;
            icon: checked ? "flightmode-on" : "flightmode-off";
            tooltipText: checked ? i18n("Airplane mode enabled") : i18n("Airplane mode disabled");

            onClicked: {
                handler.enableAirplaneMode(!checked);
                globalConfig.setAirplaneModeEnabled(!checked);
            }
        }
    }

    PlasmaComponents.ToolButton {
        id: openEditorButton;

        anchors {
            right: parent.right;
            rightMargin: Math.round(units.gridUnit / 2);
            verticalCenter: parent.verticalCenter;
        }
        iconSource: "configure";

        onClicked: {
            handler.openEditor();
        }
    }
}
