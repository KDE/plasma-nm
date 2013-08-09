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
import org.kde.qtextracomponents 0.1
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasmanm 0.1 as PlasmaNM

Item {
    id: optionsWidget;

     PlasmaNM.EnabledConnections {
        id: enabledConnections;

        onNetworkingEnabled: {
            networkingEnabled.checked = enabled;
        }

        onWirelessEnabled: {
            wirelessEnabled.checked = enabled;
        }

        onWirelessHwEnabled: {
            wirelessEnabled.enabled = enabled;
        }

        onWimaxEnabled: {
            wimaxEnabled.checked = enabled;
        }

        onWimaxHwEnabled: {
            wimaxEnabled.enabled = enabled;
        }

        onWwanEnabled: {
            wwanEnabled.checked = enabled;
        }

        onWwanHwEnabled: {
            wwanEnabled.enabled = enabled;
        }
    }

    signal openEditor();

    PlasmaNM.AvailableDevices {
        id: availableDevices;
    }

    Column {
        id: checkboxes;

        spacing: 5;
        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            topMargin: padding.margins.top;
            leftMargin: padding.margins.left;
            bottomMargin: padding.margins.bottom;
        }

        PlasmaComponents.CheckBox {
            id: networkingEnabled;

            anchors { left: parent.left; right: parent.right }
            text: i18n("Networking enabled");

            onClicked: {
                handler.enableNetworking(checked);
            }
        }

        PlasmaComponents.CheckBox {
            id: wirelessEnabled;

            height: availableDevices.wirelessAvailable ? networkingEnabled.height : 0;
            anchors { left: parent.left; right: parent.right }
            visible: availableDevices.wirelessAvailable;
            text: i18n("Wireless enabled");

            onClicked: {
                handler.enableWireless(checked);
            }
        }

        PlasmaComponents.CheckBox {
            id: wimaxEnabled;

            height: availableDevices.wimaxAvailable ? networkingEnabled.height : 0;
            anchors { left: parent.left; right: parent.right }
            visible: availableDevices.wimaxAvailable;
            text: i18n("Wimax enabled");

            onClicked: {
                handler.enableWimax(checked);
            }
        }

        PlasmaComponents.CheckBox {
            id: wwanEnabled;

            height: availableDevices.wwanAvailable ? networkingEnabled.height : 0;
            anchors { left: parent.left; right: parent.right }
            visible: availableDevices.wwanAvailable;
            text: i18n("Mobile broadband enabled");

            onClicked: {
                handler.enableWwan(checked);
            }
        }

        PlasmaComponents.Button {
            id: openEditorButton;

            anchors { horizontalCenter: parent.horizontalCenter }
            text: i18n("Edit connections...");

            onClicked: {
                console.log("clicked");
                handler.openEditor();
            }
        }
    }

    Component.onCompleted: {
        availableDevices.init();
        enabledConnections.init();
    }
}
