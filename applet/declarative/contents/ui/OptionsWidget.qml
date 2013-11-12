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
import org.kde.networkmanagement 0.1 as PlasmaNM

Item {
    id: optionsWidget;

    signal openEditor();

    PlasmaNM.EnabledConnections {
        id: enabledConnections;
    }

    PlasmaNM.AvailableDevices {
        id: availableDevices;
    }

    Column {
        id: checkboxes;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            leftMargin: padding.margins.left;
        }

        PlasmaComponents.CheckBox {
            id: networkingEnabled;

            anchors {
                left: parent.left;
                right: parent.right;
            }
            text: i18n("Networking enabled");
            checked: enabledConnections.networkingEnabled;

            onClicked: {
                handler.enableNetworking(checked);
            }
        }

        PlasmaComponents.CheckBox {
            id: wirelessEnabled;

            height: availableDevices.wirelessDeviceAvailable ? networkingEnabled.height : 0;
            anchors {
                left: parent.left;
                right: parent.right;
            }
            text: i18n("Wireless enabled");
            visible: availableDevices.wirelessDeviceAvailable;
            checked: enabledConnections.wirelessEnabled;
            enabled: enabledConnections.wirelessHwEnabled;

            onClicked: {
                handler.enableWireless(checked);
            }
        }

        PlasmaComponents.CheckBox {
            id: wimaxEnabled;

            height: availableDevices.wimaxDeviceAvailable ? networkingEnabled.height : 0;
            anchors {
                left: parent.left;
                right: parent.right;
            }
            text: i18n("Wimax enabled");
            visible: availableDevices.wimaxDeviceAvailable;
            checked: enabledConnections.wimaxEnabled;
            enabled: enabledConnections.wimaxHwEnabled;

            onClicked: {
                handler.enableWimax(checked);
            }
        }

        PlasmaComponents.CheckBox {
            id: wwanEnabled;

            height: availableDevices.modemDeviceAvailable ? networkingEnabled.height : 0;
            anchors {
                left: parent.left;
                right: parent.right;
            }
            text: i18n("Mobile broadband enabled");
            visible: availableDevices.modemDeviceAvailable;
            checked: enabledConnections.wwanEnabled;
            enabled: enabledConnections.wwanHwEnabled;

            onClicked: {
                handler.enableWwan(checked);
            }
        }

        PlasmaComponents.Button {
            id: openEditorButton;

            anchors.horizontalCenter: parent.horizontalCenter;
            text: i18n("Edit connections...");

            onClicked: {
                console.log("clicked");
                handler.openEditor();
            }
        }
    }
}
