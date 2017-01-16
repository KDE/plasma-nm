/*
    Copyright 2010 Marco Martin <notmart@gmail.com>
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
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: listItem

    default property alias content: paddingItem.data
    property alias containsMouse: itemMouse.containsMouse
    property alias enabled: itemMouse.enabled
    property bool checked: false
    property bool sectionDelegate: false

    signal clicked

    width: parent ? parent.width : childrenRect.width
    height: paddingItem.childrenRect.height + background.margins.top + background.margins.bottom

    implicitHeight: paddingItem.childrenRect.height + Math.round(units.gridUnit / 2)

    Connections {
        target: listItem
        onCheckedChanged: background.color = (listItem.checked ? highlightColor : baseColor)
        onSectionDelegateChanged: background.color = (listItem.sectionDelegate ? alternateBaseColor : baseColor)
    }

    Rectangle {
        id : background

        anchors.fill: parent
        visible: listItem.ListView.view ? listItem.ListView.view.highlight === null : true
        opacity: itemMouse.containsMouse && !itemMouse.pressed ? 0.5 : 1
        Component.onCompleted: {
            color = (listItem.sectionDelegate ? alternateBaseColor : (listItem.checked ? highlightColor : baseColor))
        }
        Behavior on opacity { NumberAnimation { duration: units.longDuration } }
    }

    PlasmaCore.SvgItem {
        svg: PlasmaCore.Svg {imagePath: "widgets/listitem"}
        elementId: "separator"
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: naturalSize.height
        visible: listItem.sectionDelegate || (typeof(index) != "undefined" && index > 0 && !listItem.checked && !itemMouse.pressed)
    }

    MouseArea {
        id: itemMouse
        property bool changeBackgroundOnPress: !listItem.checked && !listItem.sectionDelegate
        anchors.fill: background
        enabled: false

        onClicked: listItem.clicked()
        onPressAndHold: listItem.pressAndHold()
        onPressed: if (changeBackgroundOnPress) background.prefix = "pressed"
        onReleased: if (changeBackgroundOnPress) background.prefix = "normal"
        onCanceled: if (changeBackgroundOnPress) background.prefix = "normal"

        Item {
            id: paddingItem
            anchors {
                fill: parent
                margins: Math.round(units.gridUnit / 3)
            }
        }
    }

    Accessible.role: Accessible.ListItem
}
