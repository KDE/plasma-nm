/*
 *
 *   Copyright 2017 Martin Kacej <>
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

import QtQuick 2.0
import QtQuick.Controls 1.4 as Controls
import QtQuick.Layouts 1.2
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM
import org.kde.kirigami 1.0 as Kirigami

Kirigami.SwipeListItem{
    width: parent.width
    enabled: true

    Item{
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
            iconName: "configure-small"
            onTriggered: {
                networkDetailsViewContent.details = ConnectionDetails
                if (ConnectionDetails[1] !== "") {
                    detailsDialog.titleText = ConnectionDetails[1]
                } else {
                    detailsDialog.titleText = i18n("Network details")
                }
                networkDetailsViewContent.fillDetails()
                detailsDialog.open()
            }
        }
    ]
}
