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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.networkmanagement 0.1 as PlasmaNM

Item {
    id: panelIconWidget;

    PlasmaNM.NetworkStatus {
        id: networkStatus;
    }

    PlasmaNM.ConnectionIcon {
        id: connectionIconProvider;
    }

    PlasmaCore.Svg {
        id: svgIcons;

        multipleImages: true;
        imagePath: "icons/plasma-networkmanagement2";
    }

    PlasmaCore.SvgItem {
        id: connectionIcon;

        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height)
        height: width
        svg: svgIcons;
        elementId: connectionIconProvider.connectionIcon

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator;

            anchors.fill: parent;
            running: connectionIconProvider.connecting;
            visible: running;
        }

        MouseArea {
            id: mouseAreaPopup;

            anchors.fill: parent;
            hoverEnabled: true;
            onClicked: plasmoid.togglePopup();

            PlasmaCore.ToolTip {
                id: tooltip;
                target: mouseAreaPopup;
                image: connectionIconProvider.connectionTooltipIcon;
                subText: networkStatus.activeConnections;
            }
        }
    }
}
