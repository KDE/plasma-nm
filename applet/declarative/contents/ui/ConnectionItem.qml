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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.networkmanagement 0.1 as PlasmaNM

ListItem {
    id: connectionItem;

    property bool displayStatusLabel: mainWindow.showSectionLabels ||
                                      (itemConnectionState != PlasmaNM.Enums.Activated && itemType == PlasmaNM.Enums.Wireless && itemSecurityType != PlasmaNM.Enums.None);

    enabled: true

    height: if (displayStatusLabel) Math.max(connectionIcon.height, connectionNameLabel.height + connectionStatusLabel.height) + padding.margins.top + padding.margins.bottom;
            else Math.max(connectionIcon.height, connectionNameLabel.height) + padding.margins.top + padding.margins.bottom;

    PlasmaCore.SvgItem {
        id: connectionIcon;

        width: sizes.iconSize;
        height: width;
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
        }
        svg: svgNetworkIcons;
        elementId: itemConnectionIcon;

        PlasmaCore.Svg {
            id: svgNetworkIcons;

            multipleImages: true;
            imagePath: "icons/plasma-networkmanagement";
        }
    }

    PlasmaComponents.Label {
        id: connectionNameLabel;

        height: paintedHeight;
        anchors {
            left: connectionIcon.right;
            leftMargin: padding.margins.left;
            right: !connectionItem.containsMouse ? connectingIndicator.left : configButton.left;
            top: displayStatusLabel ? parent.top : undefined;
            topMargin: displayStatusLabel ? 0 : padding.margins.top;
        }
        text: itemName;
        elide: Text.ElideRight;
        font.weight: itemConnectionState == PlasmaNM.Enums.Activated || itemUuid ? Font.DemiBold : Font.Normal;
        font.italic: itemConnectionState == PlasmaNM.Enums.Activating ? true : false;
    }

    PlasmaComponents.Label {
        id: connectionStatusLabel;

        height: paintedHeight;
        anchors {
            bottom: parent.bottom;
            left: connectionIcon.right;
            leftMargin: padding.margins.left;
            right: !connectionItem.containsMouse ? connectingIndicator.left : configButton.left;
        }
        visible: displayStatusLabel;

        font.pointSize: theme.smallestFont.pointSize;
        color: "#99"+(theme.textColor.toString().substr(1))
        text:   if (mainWindow.showSectionLabels) {
                    if (itemConnectionState == PlasmaNM.Enums.Activated)
                        i18n("Connected") + ", " + i18n("Speed: %1", itemSpeed);
                    else if (itemConnectionState == PlasmaNM.Enums.Activating)
                        i18n("Connecting");
                    else if (itemUuid && itemSecurityType != PlasmaNM.Enums.None)
                        i18n("Last used: %1", itemLastUsed) + ", " + i18n("Security: %1", itemSecurityString);
                    else if (itemUuid)
                         i18n("Last used: %1", itemLastUsed);
                    else if (itemSecurityType != PlasmaNM.Enums.None)
                        i18n("Never used") + ", " + i18n("Security: %1", itemSecurityString);
                    else
                        i18n("Never used");
                } else {
                    if (itemSecurityType != PlasmaNM.Enums.None)
                        i18n("Security: %1", itemSecurityString);
                    else
                        i18n("None");
                }
        elide: Text.ElideRight;
    }

    PlasmaComponents.BusyIndicator {
        id: connectingIndicator;

        width: sizes.iconSize;
        height: width;
        anchors {
            right: parent.right;
            rightMargin: padding.margins.right;
            verticalCenter: parent.verticalCenter;
        }
        running: itemConnectionState == PlasmaNM.Enums.Activating;
        visible: running && !connectionItem.containsMouse;
    }

    PlasmaComponents.Button {
        id: stateChangeButton;

        implicitWidth: minimumWidth + padding.margins.left + padding.margins.right;
        anchors {
            verticalCenter: parent.verticalCenter;
            right: parent.right;
        }
        text: if (itemConnectionState == PlasmaNM.Enums.Deactivated)
                  i18n("Connect");
              else
                  i18n("Disconnect");
        visible: connectionItem.containsMouse;

        onClicked: {
            if (itemConnectionState == PlasmaNM.Enums.Deactivated)
                handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
            else
                handler.deactivateConnection(itemConnectionPath, itemDevicePath);
        }
    }

    PlasmaCore.SvgItem {
        id: configButton;

        height: sizes.iconSize;
        width: height;
        anchors {
            verticalCenter: parent.verticalCenter;
            right: stateChangeButton.left;
            rightMargin: padding.margins.right;
        }
        svg: svgNotificationIcons;
        elementId: configButtonMouse.containsMouse ? "notification-inactive" : "notification-active";
        visible: connectionItem.containsMouse;

        PlasmaCore.Svg {
            id: svgNotificationIcons;

            multipleImages: true;
            imagePath: "icons/notification";
        }

        MouseArea {
            id: configButtonMouse;
            anchors.fill: parent;
            hoverEnabled: true;

            onClicked: {
                // TODO
            }
        }
    }

    Component.onCompleted: {
        console.log(itemSection);
    }
}
