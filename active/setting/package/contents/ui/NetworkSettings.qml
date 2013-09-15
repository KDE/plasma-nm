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
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.active.settings 0.1
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    id: networkModule;
    objectName: "networkModule"

    NetworkSettings {
        id: networkSettings;
    }

    MobileComponents.Package {
        id: networkPackage;
        name: "org.kde.active.settings.network";
    }

    Column {
        id: titleCol;

        anchors {
            top: parent.top;
            left: parent.left;
            right: planeMode.left
        }

        PlasmaExtras.Title {
            id: titleLabel;
            text: networkSettings.settingName;
            opacity: 1;
        }

        PlasmaComponents.Label {
            id: descriptionLabel;
            text: networkSettings.status;
            opacity: .3;
        }
    }

    PlasmaNm.Handler {
        id: handler;
    }

    PlasmaNm.EnabledConnections {
        id: enabledConnections;

        onNetworkingEnabled: {
            planeModeSwitch.checked = !enabled;
        }
    }

    Item {
        id: planeMode;

        anchors {
            top: parent.top;
            right: parent.right;
            verticalCenter: titleCol.verticalCenter;
        }

        PlasmaComponents.Label {
            id: planeModeLabel;

            anchors {
                right: planeModeSwitch.left;
                rightMargin: 8;
                verticalCenter: parent.verticalCenter;
            }
            text: i18n("Airplane Mode");
        }

        PlasmaComponents.Switch {
            id: planeModeSwitch;

            anchors {
                right: parent.right;
                rightMargin: 10;
                verticalCenter: parent.verticalCenter;
            }
            height: 30;

            onClicked: {
                handler.enableNetworking(!checked);
            }
        }
    }

    PlasmaComponents.TabBar {
        id: tabBar;

        anchors {
            horizontalCenter: parent.horizontalCenter;
            top: titleCol.bottom;
            topMargin: 10;
        }

        PlasmaNm.AvailableDevices {
            id: availableDevices;
        }

        PlasmaComponents.TabButton {
            id: wirelessTabButton;

            property int type: PlasmaNm.Enums.Wireless;

            tab: connectionList;
            text: i18n("Wireless");
            visible: availableDevices.wirelessAvailable;
        }

        PlasmaComponents.TabButton {
            id: modemTabButton;

            property int type: PlasmaNm.Enums.Gsm;

            tab: connectionList;
            text: i18n("Modem");
            visible: availableDevices.wwanAvailable;
        }

        PlasmaComponents.TabButton {
            id: wiredTabButton;

            property int type: PlasmaNm.Enums.Wired;

            tab: connectionList;
            text: i18n("Ethernet");
            visible: availableDevices.wiredAvailable;
        }

        PlasmaComponents.TabButton {
            id: vpnTabButton;

            property int type: PlasmaNm.Enums.Vpn;

            tab: connectionList;
            text: i18n("VPN");
        }

        onCurrentTabChanged: {
            setNetworkSetting();
            connectionList.resetIndex();
        }

        Component.onCompleted: {
            availableDevices.init();
            setNetworkSetting();
        }

        function setNetworkSetting() {
            if (currentTab.type == PlasmaNm.Enums.Wireless) {
                networkSettings.setNetworkSetting(PlasmaNm.Enums.Wireless, availableDevices.wirelessDevicePath);
            } else if (currentTab.type == PlasmaNm.Enums.Gsm) {
                networkSettings.setNetworkSetting(PlasmaNm.Enums.Gsm, availableDevices.wwanDevicePath);

            } else if (currentTab.type == PlasmaNm.Enums.Wired) {
                networkSettings.setNetworkSetting(PlasmaNm.Enums.Wired, availableDevices.wiredDevicePath);

            } else if (currentTab.type == PlasmaNm.Enums.Vpn) {
                networkSettings.setNetworkSetting(PlasmaNm.Enums.Vpn);
            }
        }
    }

    PlasmaComponents.TabGroup {
        id: tabContent;

        anchors {
            left: parent.left;
            right: parent.right;
            top: tabBar.bottom;
            bottom: parent.bottom;
            topMargin: 10;
        }

        ConnectionList {
            id: connectionList;

            anchors.fill: parent;
        }
    }

    Component.onCompleted: {
        enabledConnections.init();
    }
}
