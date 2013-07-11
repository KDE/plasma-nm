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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore

PlasmaComponents.ListItem {
    id: connectionItem;

    enabled: true
    height: 48;

    QIconItem {
        id: connectionIcon;

        anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
        width: 48; height: 48;

        icon: QIcon(itemConnectionIcon);
    }

    PlasmaComponents.Label {
        id: networkLabel;

        anchors { left: connectionIcon.right; right: editButton.left; verticalCenter: parent.verticalCenter }
        font.weight: Font.Bold;
        elide: Text.ElideRight;
        text: itemName;
    }

    PlasmaComponents.ToolButton {
        id: editButton;

        anchors { right: securedIcon.left; top: parent.top; bottom: parent.bottom }

        height: 48;
        flat: false;
        iconSource: "configure";
        visible: itemUuid.length != 0;
        // TODO
        enabled: false;
    }

    QIconItem {
        id: securedIcon;

        anchors { right: parent.right; top: parent.top; bottom: parent.bottom }
        width: 48; height: 48;
        icon: QIcon("object-locked");
        visible: itemSecure;
    }
}
