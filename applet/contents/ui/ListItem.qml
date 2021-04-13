/*
    Copyright 2010 Marco Martin <notmart@gmail.com>
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>
    Copyright 2020 George Vogiatzis <gvgeo@protonmail.com>

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

/**
 * Ignores the theme's listItem margins, and uses custom highlight(pressed) area.
 * Could break some themes but the majority look fine.
 * Also includes a separator to be used in sections.
 */
Item {
    id: listItem

    signal clicked

    property alias containsMouse: itemMouse.containsMouse
    property bool checked: false
    property bool separator: false
    property rect highlightRect: Qt.rect(0, 0, width, height)

    width: parent.width

    // Sections have spacing above but not below. Will use 2 of them below.
    height: separator ? separatorLine.height + PlasmaCore.Units.smallSpacing * 3 : parent.height

    PlasmaCore.Svg {
        id: lineSvg
        imagePath: "widgets/line"
    }
    PlasmaCore.SvgItem {
        id: separatorLine
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: PlasmaCore.Units.smallSpacing
        }
        elementId: "horizontal-line"
        width: parent.width - PlasmaCore.Units.gridUnit * 2
        svg: lineSvg
        visible: separator
    }

    PlasmaCore.FrameSvgItem {
        id : background
        imagePath: "widgets/listitem"
        prefix: "normal"
        anchors.fill: parent
        visible: separator ? false : true
    }

    PlasmaCore.FrameSvgItem {
        id : pressed
        imagePath: "widgets/listitem"
        prefix: "pressed"
        opacity: checked ? 1 : 0
        Behavior on opacity { NumberAnimation { duration: PlasmaCore.Units.shortDuration } }

        x: highlightRect.x
        y: highlightRect.y
        height: highlightRect.height
        width: highlightRect.width
    }

    MouseArea {
        id: itemMouse
        anchors.fill: parent
        hoverEnabled: true
        onClicked: listItem.clicked()
    }
}
