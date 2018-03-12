/*
 *   Copyright 2017 Martin Kacej <m.kacej@atlas.sk>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.6
import QtQuick.Controls 2.2 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 2.0 as Kirigami

Kirigami.SwipeListItem {
    id: listitem
    width: parent.width
    enabled: true
    backgroundColor: theme.backgroundColor
    property var map : []

    Item {
        height: connectionSvgIcon.height
        width: parent.width

        PlasmaCore.SvgItem {
            id: connectionSvgIcon
            anchors {
                left: parent.left
                rightMargin: 10
            }
            elementId: ConnectionIcon
            //height: units.iconSizes.big; width: height
            svg: PlasmaCore.Svg {
                multipleImages: true
                imagePath: "icons/network"
                colorGroup: PlasmaCore.ColorScope.colorGroup
            }
        }

        PlasmaComponents.BusyIndicator {
            id: connectingIndicator

            anchors {
                left: parent.left
                horizontalCenter: connectionSvgIcon.horizontalCenter
                verticalCenter: connectionSvgIcon.verticalCenter
            }
            height: units.iconSizes.medium; width: height
            running: ConnectionState == PlasmaNM.Enums.Activating
            visible: running
        }

        PlasmaComponents.Label {
            id: connectionNameLabel

            anchors {
                left: connectionSvgIcon.right
                leftMargin: units.gridUnit
                verticalCenter: connectionSvgIcon.verticalCenter
            }
            height: paintedHeight
            elide: Text.ElideRight
            font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
            font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
            text: ItemUniqueName
            textFormat: Text.PlainText
        }
    }

    actions: [
        Kirigami.Action {
            iconName: "configure"
            onTriggered: getDetails()
        },
        Kirigami.Action {
            iconName: "remove"
            visible: (Uuid != "")? true : false
            onTriggered: {
                forgetNetwork()
            }
        }
    ]

    function getDetails(){
        if (ConnectionDetails)
            networkDetailsViewContent.details = ConnectionDetails
        if (ConnectionDetails[1] !== "") {
            detailsDialog.titleText = ItemUniqueName
        } else {
            detailsDialog.titleText = i18n("Network details")
        }
        map = handler.getConnectionSettings(ConnectionPath,"ipv4")
        if (ConnectionState == PlasmaNM.Enums.Activated){
            handler.getActiveConnectionInfo(ConnectionPath)
        }
        networkDetailsViewContent.map = map
        console.info(map["method"])
        networkDetailsViewContent.fillDetails()
        detailsDialog.open()
    }

    function connect() {
        console.info(ConnectionDetails[1] + ' trying to connect')
    }
    function forgetNetwork() {
        console.info(ConnectionPath + ' trying to forget')
        deleteConfirmation.open()
        // ItemUniqueName
    }
}
