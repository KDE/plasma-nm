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
import org.kde.kirigami 2.2 as Kirigami

Kirigami.SwipeListItem {
    id: listitem

    height: units.gridUnit * 3
    enabled: true
    backgroundColor: theme.backgroundColor

    property var map : []
    property bool predictableWirelessPassword: !Uuid && Type == PlasmaNM.Enums.Wireless &&
                                                   (SecurityType == PlasmaNM.Enums.StaticWep ||
                                                    SecurityType == PlasmaNM.Enums.WpaPsk ||
                                                    SecurityType == PlasmaNM.Enums.Wpa2Psk)

    onClicked: {
        changeState()
    }

    Item {

        height: parent.height
        width: parent.width
        anchors.fill: parent

        PlasmaCore.SvgItem {
            id: connectionSvgIcon

            anchors {
                left: parent.left
                leftMargin: units.gridUnit
                verticalCenter: parent.verticalCenter
            }
            height: units.iconSizes.big;
            width: height
            elementId: ConnectionIcon

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

        Item{
            id: connectionPasswordField

            anchors.left: connectionNameLabel.left
            anchors.verticalCenter: parent.verticalCenter
            height: units.gridUnit * 1.5
            visible: false

            onVisibleChanged: {
                connectionPasswordFieldField.text = ""
            }

            PasswordField{
                id: connectionPasswordFieldField
                securityType: SecurityType
                height: parent.height
            }

            PlasmaComponents.Button{
                id: connectionPasswordFieldButton

                anchors.left: connectionPasswordFieldField.right
                width: units.gridUnit * 5
                height: parent.height
                text: "ok" // TODO icon, size

                onClicked: {
                    handler.addAndActivateConnection(DevicePath, SpecificPath, connectionPasswordFieldField.text);
                }
            }
        }
    }

    actions: [
        Kirigami.Action {
            iconName: "go-previous" // TODO better icon
            visible: ConnectionState != PlasmaNM.Enums.Activated
            onTriggered: changeState()
        },
        Kirigami.Action {
            iconName: "go-next" // TODO better icon
            visible: ConnectionState == PlasmaNM.Enums.Activated
            onTriggered: handler.deactivateConnection(ConnectionPath, DevicePath)
        },
        Kirigami.Action {
            iconName: "configure"
            onTriggered: getDetails()
        },
        Kirigami.Action {
            iconName: "remove"
            visible: (Uuid != "")? true : false
            onTriggered: forgetNetwork()
        }
    ]

    function getDetails() {
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
        networkDetailsViewContent.fillDetails()
        detailsDialog.open()
    }

    function changeState() {
        if (Uuid || !predictableWirelessPassword || connectionPasswordField.visible) {
            if (ConnectionState == PlasmaNM.Enums.Deactivated) {
                if (!predictableWirelessPassword && !Uuid) {
                    handler.addAndActivateConnection(DevicePath, SpecificPath);
                } else if (connectionPasswordField.visible) {
                            if (connectionPasswordFieldField.text != "") {
                                handler.addAndActivateConnection(DevicePath, SpecificPath, connectionPasswordFieldField.text);
                                connectionPasswordField.visible = false;
                            } else {
                                connectionPasswordField.visible = false;
                            }
                        } else {
                            handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
                        }
            }
        } else if (predictableWirelessPassword) {
           connectionPasswordField.visible = true;
        }
    }

    function forgetNetwork() {
        // TODO confirmation dialog
        handler.removeConnection(ConnectionPath)
    }
}
