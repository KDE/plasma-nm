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
import org.kde.plasma.components 0.1 as PlasmaComponents

PlasmaComponents.ListItem {
    id: sectionHeader;

    signal hideSection(string name);
    signal showSection(string name);

    sectionDelegate: true;
    property bool expanded: true;

    height: Math.max(expandButton.height, sectionLabel.height) + padding.margins.top + padding.margins.bottom;
    anchors {
        left: parent.left;
        right: parent.right;
    }

    PlasmaComponents.Label {
        id: sectionLabel

        height: paintedHeight;
        anchors {
            left: expandButton.right;
            verticalCenter: parent.verticalCenter;
            leftMargin: padding.margins.left
        }
        text: section;

        MouseArea {
            id: sectionLabelMouseArea

            anchors.fill: parent;

            onClicked: sectionClicked();
        }
    }

    PlasmaComponents.ToolButton {
        id: expandButton;

        height: sizes.iconSize;
        width: height;
        anchors {
            left: parent.left;
            verticalCenter: parent.verticalCenter
        }
        iconSource: "list-remove";

        onClicked: sectionClicked();
    }

    function sectionClicked() {
         if (expanded) {
            hideSection(section);
            expandButton.iconSource = "list-add";
        } else {
            showSection(section);
            expandButton.iconSource = "list-remove";
        }
        expanded = !expanded;
    }
}
