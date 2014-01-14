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

    height: sizes.iconSize + padding.margins.top + padding.margins.bottom;

    PlasmaCore.Svg {
        id: svgNetworkIcons;

        multipleImages: true;
        imagePath: "icons/plasma-networkmanagement";
    }

    PlasmaComponents.ListItem {
        id: plane;

        height: sizes.iconSize + padding.margins.top + padding.margins.bottom;
        width: parent.width/4;
        anchors {
            left: parent.left;
            top: parent.top;
        }
        enabled: true;

        PlasmaCore.SvgItem {
            anchors {
                horizontalCenter: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
            }
            svg: svgNetworkIcons;
            elementId: "plane-mode";
        }
    }

    PlasmaComponents.ListItem {
        id: wifi;

        property bool switchEnabled: true;

        height: sizes.iconSize + padding.margins.top + padding.margins.bottom;
        width: parent.width/4;
        anchors {
            left: plane.right;
            leftMargin: 2;
            top: parent.top;
        }
        enabled: true;

        PlasmaCore.SvgItem {
            anchors {
                horizontalCenter: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
            }
            svg: svgNetworkIcons;
            elementId: wifi.switchEnabled ? "network-wireless-100" : "network-wireless-0";
        }

        onClicked: {
            switchEnabled = !switchEnabled;
        }
    }

    PlasmaComponents.ListItem {
        id: modem;

        property bool switchEnabled: true;

        height: sizes.iconSize + padding.margins.top + padding.margins.bottom;
        width: parent.width/4;
        anchors {
            left: wifi.right;
            leftMargin: 2;
            top: parent.top;
        }
        enabled: true;

        PlasmaCore.SvgItem {
            anchors {
                horizontalCenter: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
            }
            svg: svgNetworkIcons;
            elementId: modem.switchEnabled ? "network-mobile-100" : "network-mobile";
        }

        onClicked: {
            switchEnabled = !switchEnabled;
        }
    }

    PlasmaComponents.ListItem {
        id: configuration;

        height: sizes.iconSize + padding.margins.top + padding.margins.bottom;
        width: parent.width/4;
        anchors {
            left: modem.right;
            leftMargin: 2;
            right: parent.right;
            top: parent.top;
        }
        enabled: true;

        PlasmaCore.IconItem {
            anchors {
                horizontalCenter: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
            }
            source: "configure";
        }

        onClicked: {
            handler.openEditor();
        }
    }
}
