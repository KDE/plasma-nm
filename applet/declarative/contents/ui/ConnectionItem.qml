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

PlasmaComponents.ListItem {
    id: connectionItem;

    property bool sectionHidden: (!connectionView.activeExpanded && itemSection == i18n("Active connections")) ||
                                 (!connectionView.previousExpanded && itemSection == i18n("Previous connections")) ||
                                 (!connectionView.unknownExpanded && itemSection == i18n("Unknown connections"))
    property bool displayCategories: true;
    property bool displayStatusLabel: displayCategories ||
                                      (itemConnectionState != PlasmaNM.Enums.Activated && itemType == PlasmaNM.Enums.Wireless && itemSecurityType != PlasmaNM.Enums.None);

    enabled: true
    height: connectionLabels.height + padding.margins.top + padding.margins.bottom;

    Item {
        id: connectionLabels;

        height: if (displayStatusLabel) Math.max(connectionIcon.height, connectionNameLabel.height + connectionStatusLabel.height);
                else Math.max(connectionIcon.height, connectionNameLabel.height);

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        PlasmaCore.Svg {
            id: svgIcons;

            multipleImages: true;
            imagePath: "icons/plasma-networkmanagement";
        }

        PlasmaCore.SvgItem {
            id: connectionIcon;

            width: sizes.iconSize;
            height: width;
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            svg: svgIcons;
            elementId: itemConnectionIcon;
        }

        PlasmaComponents.Label {
            id: connectionNameLabel;

            height: paintedHeight;
            anchors {
                left: connectionIcon.right;
                leftMargin: padding.margins.left;
                right: connectingIndicator.left;
                top: displayStatusLabel ? parent.top : undefined;
                verticalCenter: displayStatusLabel ? undefined : parent.verticalCenter;
            }
            text: itemName;
            elide: Text.ElideRight;
            font.weight: itemConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal;
            font.italic: itemConnectionState == PlasmaNM.Enums.Activating ? true : false;
        }

        PlasmaComponents.Label {
            id: connectionStatusLabel;

            height: paintedHeight
            anchors {
                bottom: parent.bottom;
                left: connectionIcon.right;
                leftMargin: padding.margins.left;
                right: connectingIndicator.left;
            }
            visible: displayStatusLabel;

            font.pointSize: theme.smallestFont.pointSize;
            color: "#99"+(theme.textColor.toString().substr(1))
            text:   if (displayCategories) {
                        if (itemConnectionState == PlasmaNM.Enums.Activated) i18n("Connected");
                        else if (itemConnectionState == PlasmaNM.Enums.Activating) i18n("Connecting");
                        else if (itemUuid && itemSecurityType != PlasmaNM.Enums.None) i18n("Saved") + ", " + i18n("Security") + ": " + itemSecurityString;
                        else if (itemUuid) i18n("Saved");
                        else if (itemSecurityType != PlasmaNM.Enums.None) i18n("Unknown") + ", " + i18n("Security") + ": " + itemSecurityString;
                        else i18n("Uknown");
                    } else {
                        if (itemSecurityType != PlasmaNM.Enums.None) i18n("Security") + ": " + itemSecurityString;
                        else i18n("None");
                    }
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
            visible: running;
        }
    }

    // TODO: just for now
    onClicked: {
        console.log(itemConnectionState);
        if (itemConnectionState == PlasmaNM.Enums.Deactivated) {
            handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
        } else {
            handler.deactivateConnection(itemConnectionPath, itemDevicePath);
        }
    }
    states: [
        State {
            name: "CollapsedHidden";
            when: sectionHidden;
            PropertyChanges { target: connectionItem; height: 0; }
            PropertyChanges { target: connectionItem; visible: false; }
        }
    ]
}
