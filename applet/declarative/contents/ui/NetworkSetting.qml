/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasmanm 0.1 as PlasmaNM

Item {
    id: networkSetting;

    height: 20;

    PlasmaComponents.Button {
        id: popupButton;

        anchors.fill: parent;
        text: i18n("Display more options");
    }

//     Column {
//         id: checkboxes;
//
//         anchors.fill: parent;
//         visible: false;
//
//         PlasmaComponents.CheckBox {
//             id: networkEnabled;
//
//             height: 15;
//             text: i18n("Disable networking");
//         }
//
//         PlasmaComponents.CheckBox {
//             id: wirelessEnabled;
//
//             height: 15;
//             text: i18n("Disable wireless");
//         }
//
//         PlasmaComponents.CheckBox {
//             id: wwanEnabled;
//
//             height: 15;
//             text: i18n("Disable mobile broadband");
//         }
//     }

    PlasmaNM.AppletInfo {
        id: networkInfo;

//         onNetworkingEnabled: {
//             if (enabled) {
//                 plasmoid.setAction("disableNetworking", i18n("Disable networking"), "network-wired", true);
//                 plasmoid.removeAction("enableNetworking");
//             } else {
//                 plasmoid.setAction("enableNetworking", i18n("Enable networking"), "network-wired", true);
//                 plasmoid.removeAction("disableNetworking");
//             }
//         }
//
//         onWirelessEnabled: {
//             if (enabled) {
//                 plasmoid.setAction("disableWireless", i18n("Disable wireless"), "network-wireless");
//                 plasmoid.removeAction("enableWireless");
//             } else {
//                 plasmoid.setAction("enableWireless", i18n("Enable wireless"), "network-wireless");
//                 plasmoid.removeAction("disableWireless");
//             }
//         }
//
//         onWwanEnabled: {
//             if (enabled) {
//                 plasmoid.setAction("disableWwan", i18n("Disable mobile broadband"), "phone");
//                 plasmoid.removeAction("enableWwan");
//             } else {
//                 plasmoid.setAction("enableWwan", i18n("Enable mobile broadband"), "phone");
//                 plasmoid.removeAction("disableWwan");
//             }
//         }
    }

//     function action_disableNetworking() {
//         enableNetworking(false);
//     }
//
//     function action_enableNetworking() {
//         enableNetworking(true);
//     }
//
//     function action_disableWireless() {
//         enableWireless(false);
//     }
//
//     function action_enableWireless() {
//         enableWireless(true);
//     }
//
//     function action_disableWwan() {
//         enableWwan(false);
//     }
//
//     function action_enableWwan() {
//         enableWwan(true);
//     }

    Component.onCompleted: {
        networkingInfo.initNetworkInfo();
    }
}
