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

Item {
    property alias checked: connectionItem.checked;

    signal itemSelected();

    height: 48;
    anchors {
        left: parent.left;
        right: parent.right;
    }

    Rectangle {
        id: connectedItemBackground;

        height: 48;
        anchors {
            left: parent.left;
            right: parent.right;
        }
        color: "green";
        opacity: itemConnected ? 0.2 : 0.1;
        visible: itemConnected || itemConnecting;
    }

    PlasmaComponents.ListItem {
        id: connectionItem;

        height: 48;
        anchors {
            left: parent.left;
            right: parent.right;
        }
        enabled: true

        QIconItem {
            id: connectionIcon;

            anchors {
                left: parent.left;
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 48;
            height: 48;

            icon: QIcon(itemConnectionIcon);
        }

        PlasmaComponents.Label {
            id: networkLabel;

            anchors {
                left: connectionIcon.right;
                right: securityLabel.left;
                verticalCenter: parent.verticalCenter;
            }
            font.weight: Font.Bold;
            elide: Text.ElideRight;
            text: itemName;
        }

        QIconItem {
            id: securedIcon;

            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
            }
            width: 48;
            height: 48;
            icon: QIcon("object-locked");
            visible: itemSecure;
        }

        PlasmaComponents.Label {
            id: securityLabel;

            anchors {
                right: securedIcon.left;
                top: parent.top;
                bottom: parent.bottom;
            }
            text: itemSecurityTypeString;
            visible: securedIcon.visible;
        }

        onClicked: itemSelected();
    }

    onCheckedChanged: {
        if (checked)
            itemSelected();
    }
}
