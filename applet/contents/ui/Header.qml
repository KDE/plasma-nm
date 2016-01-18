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

import QtQuick 2.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    id: header

    property alias text: headerLabel.text

    height: headerLabel.height + units.gridUnit; width: parent.width
    sectionDelegate: true

    PlasmaNM.EnabledConnections {
        id: enabledConnections
    }

    PlasmaComponents.Label {
        id: headerLabel

        anchors.centerIn: parent
        height: paintedHeight
        font.weight: Font.DemiBold
    }

    PlasmaComponents.ToolButton {
        anchors {
            top: parent.top
            bottom: parent.bottom
            margins: -Math.round(units.gridUnit / 3)
            right: parent.right
            rightMargin: units.gridUnit / 2
        }
        width: height
        flat: true
        tooltip: i18n("Rescan wireless networks")
        visible: (header.text === i18n("Available connections") || !connectionView.availableConnectionsVisible) &&
                 enabledConnections.wirelessEnabled && enabledConnections.wirelessHwEnabled && availableDevices.wirelessDeviceAvailable

        onClicked: {
            handler.requestScan();
            refreshAnimation.restart();
        }

        PlasmaCore.SvgItem {
            anchors {
                fill: parent
                margins: Math.round(units.gridUnit / 3)
            }
            elementId: "view-refresh"
            svg: PlasmaCore.FrameSvg { imagePath: "icons/view" }

            RotationAnimator on rotation {
                id: refreshAnimation

                duration: 1000
                running: false
                from: 0
                to: 720
            }
        }
    }

    Component.onCompleted: {
        if (header.text === i18n("Available connections")) {
            connectionView.availableConnectionsVisible = true
        }
    }

    Component.onDestruction: {
        connectionView.availableConnectionsVisible = false
    }

    onVisibleChanged: {
        connectionView.availableConnectionsVisible = visible
    }
}
