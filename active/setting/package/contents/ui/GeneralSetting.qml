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
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    id: generalSetting;

    PlasmaNm.AvailableDevices {
        id: availableDevices;
    }

    PlasmaNm.Handler {
        id: generalSettingHandler;
    }

    PlasmaNm.EnabledConnections {
        id: enabledConnections;

        onNetworkingEnabled: {
            networkingEnabled.checked = enabled;
        }

        onWirelessEnabled: {
            wirelessEnabled.checked = enabled;
        }

        onWirelessHwEnabled: {
            wirelessHwEnabled.checked = enabled;
        }

        onWwanEnabled: {
            wwanEnabled.checked = enabled;
        }

        onWwanHwEnabled: {
            wwanHwEnabled.checked = enabled;
        }
    }

    Grid {
        id: networkSwitches;

        columns: 2
        spacing: theme.defaultFont.mSize.height
        anchors { top: parent.top; bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; topMargin: 10 }

        PlasmaComponents.Label {
            id: networkingEnabledLabel;

            height: 30;
            text: i18n("Networking enabled");
        }

        PlasmaComponents.Switch {
            id: networkingEnabled;

            height: 30;

            onClicked: {
                generalSettingHandler.enableNetworking(checked);
            }
        }

        PlasmaComponents.Label {
            id: wirelessEnabledLabel;

            height: availableDevices.wirelessAvailable ? 30 : 0;
            visible: availableDevices.wirelessAvailable;
            text: i18n("Wireless enabled");
        }

        PlasmaComponents.Switch {
            id: wirelessEnabled;

            height: availableDevices.wirelessAvailable ? 30 : 0;
            visible: availableDevices.wirelessAvailable;

            onClicked: {
                generalSettingHandler.enableWireless(checked);
            }
        }

        PlasmaComponents.Label {
            id: wwanEnabledLabel;

            text: i18n("Mobile broadband enabled");
            height: availableDevices.wwanAvailable ? 30 : 0;
            visible: availableDevices.wwanAvailable;
        }

        PlasmaComponents.Switch {
            id: wwanEnabled;

            height: availableDevices.wwanAvailable ? 30 : 0;
            visible: availableDevices.wwanAvailable;

            onClicked: {
                generalSettingHandler.wwanEnabled(checked);
            }
        }
    }

    Component.onCompleted: {
        availableDevices.init();
        enabledConnections.init();
    }
}
