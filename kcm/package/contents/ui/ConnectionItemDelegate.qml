/*
    Copyright 2016-2018 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4 as QQC
import QtQuick.Controls 2.5 as QQC2

import org.kde.kirigami 2.9 as Kirigami

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Kirigami.AbstractListItem {
    id: connectionItem

    Accessible.role: Accessible.ListItem
    Accessible.name: model.Name

    checked: ConnectionPath === root.currentConnectionPath
    highlighted: focus

    signal aboutToChangeConnection(bool exportable, string name, string path)
    signal aboutToExportConnection(string path)
    signal aboutToRemoveConnection(string name, string path)

    RowLayout {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            leftMargin: Kirigami.Units.largeSpacing
        }
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Icon {
            id: connectionIcon
            Layout.minimumHeight: Kirigami.Units.iconSizes.smallMedium
            Layout.maximumHeight: Layout.minimumHeight
            Layout.minimumWidth: height
            source: KcmConnectionIcon
        }

        ColumnLayout {
            spacing: 0

            QQC2.Label {
                id: nameLabel
                Layout.fillWidth: true
                height: paintedHeight
                elide: Text.ElideRight
                font.weight: ConnectionState == PlasmaNM.Enums.Activated ? Font.DemiBold : Font.Normal
                font.italic: ConnectionState == PlasmaNM.Enums.Activating ? true : false
                text: Name
                textFormat: Text.PlainText
            }

            QQC2.Label {
                id: statusLabel
                Layout.fillWidth: true
                height: paintedHeight
                elide: Text.ElideRight
                font.pointSize: theme.smallestFont.pointSize
                text: itemText()
                textFormat: Text.PlainText
                opacity: 0.6
            }
        }
    }

//     QQC.Menu {
//         id: connectionItemMenu
//
//         QQC.MenuItem {
//             text: ConnectionState == PlasmaNM.Enums.Deactivated ? i18n("Connect") : i18n("Disconnect")
//             visible: ItemType == 1
//             onTriggered: {
//                 if (ConnectionState == PlasmaNM.Enums.Deactivated) {
//                     handler.activateConnection(ConnectionPath, DevicePath, SpecificPath);
//                 } else {
//                     handler.deactivateConnection(ConnectionPath, DevicePath);
//                 }
//             }
//         }
//
//         QQC.MenuItem {
//             iconName: "list-remove"
//             text: i18n("Delete");
//
//             onTriggered: {
//                 aboutToRemoveConnection(Name, ConnectionPath)
//             }
//         }
//
//         QQC.MenuItem {
//             iconName: "document-export"
//             visible: KcmVpnConnectionExportable
//             text: i18n("Export");
//
//             onTriggered: aboutToExportConnection(ConnectionPath)
//         }
//     }

    onClicked: {
        if (mouse.button === Qt.LeftButton) {
            aboutToChangeConnection(KcmVpnConnectionExportable, Name, ConnectionPath)
        } else if (mouse.button == Qt.RightButton) {
            connectionItemMenu.popup()
        }
    }

    /* This generates the status description under each connection
       in the list at the left side of the applet. */
    function itemText() {
        if (ConnectionState == PlasmaNM.Enums.Activated) {
            return i18n("Connected")
        } else if (ConnectionState == PlasmaNM.Enums.Activating) {
            return i18n("Connecting")
        } else {
            return LastUsed
        }
    }
}
