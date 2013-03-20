/*
    Copyright 2012-2013  Jan Grulich <jgrulich@redhat.com>

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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: detailInformationsWidget;

    property alias text: details.text;
    property bool editable;

    signal hideDetails();

    PlasmaExtras.Heading {
        id: title;

        anchors { left: parent.left; right: parent.right; top: parent.top }
        level: 3;
        text: i18n("Connection details");
    }

    Rectangle {
        id: separator;

        height: 2;
        anchors { top: title.bottom; left: parent.left; right: parent.right; topMargin: 5 }
        radius: 2;
        color: theme.highlightColor;
    }

    PlasmaComponents.Label {
        id: details;

        anchors { top: separator.bottom; horizontalCenter: parent.horizontalCenter; topMargin: 10 }
        lineHeight: 1.5;
        text: details;
    }

    PlasmaComponents.ButtonRow {
        id: buttons;

        spacing: 2;
        exclusive: false;
        anchors { horizontalCenter: parent.horizontalCenter; bottom: parent.bottom }

        PlasmaComponents.ToolButton {
            id: backButton;

            height: 25;
            flat: true;
            iconSource: QIcon("edit-undo");
            text: i18n("Back");

            onClicked: hideDetails();
        }

        PlasmaComponents.ToolButton {
            id: editButton;

            height: 25;
            iconSource: QIcon("configure");
            text: i18n("Edit");
            enabled: editable;
        }

        PlasmaComponents.ToolButton {
            id: removeButton;

            height: 25;
            iconSource: QIcon("edit-delete");
            text: i18n("Remove");
            enabled: editable;
        }
    }
}
