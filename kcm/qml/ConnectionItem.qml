/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

PlasmaComponents.ListItem {
    id: connectionItem

    checked: containsMouse || ConnectionPath === connectionView.currentConnection
    enabled: true
    height: connectionItemBase.height

    Item {
        id: connectionItemBase

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            // Reset top margin from PlasmaComponents.ListItem
            topMargin: -Math.round(units.gridUnit / 3)
        }
        height: Math.max(units.iconSizes.medium, connectionNameLabel.height + connectionStatusLabel.height) + Math.round(units.gridUnit / 2)

        PlasmaCore.IconItem {
            id: connectionIcon

            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            height: units.iconSizes.medium; width: height
            source: KcmConnectionIcon
        }

        PlasmaComponents.Label {
            id: connectionNameLabel

            anchors {
                bottom: connectionIcon.verticalCenter
                left: connectionIcon.right
                leftMargin: Math.round(units.gridUnit / 2)
                right: parent.right
            }
            height: paintedHeight
            elide: Text.ElideRight
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
            text: Name
            textFormat: Text.PlainText
        }

        PlasmaComponents.Label {
            id: connectionStatusLabel

            anchors {
                left: connectionIcon.right
                leftMargin: Math.round(units.gridUnit / 2)
                right: parent.right
                top: connectionNameLabel.bottom
            }
            height: paintedHeight
            elide: Text.ElideRight
            font.pointSize: theme.smallestFont.pointSize
            opacity: 0.6
            text: itemText()
        }
    }

    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else {
            return LastUsed
        }
    }

    onClicked: {
        connectionView.currentConnection = ConnectionPath
    }
}
