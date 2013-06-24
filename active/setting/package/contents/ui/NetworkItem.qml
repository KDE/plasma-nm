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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.plasma.core 0.1 as PlasmaCore

PlasmaComponents.ListItem {
    id: networkItem;

    enabled: true
    height: 64;

    PlasmaCore.Svg {
        id: svgIcons;

        multipleImages: true;
        imagePath: "icons/plasma-nm";
    }

    PlasmaCore.SvgItem {
        id: smallNetworkIcon;

        width: 35; height: 35;
        anchors { left: parent.left; verticalCenter: parent.verticalCenter; leftMargin: 5 }
        svg: svgIcons;
        elementId: itemSvgIcon;
    }

    PlasmaComponents.Label {
        id: networkLabel;

        anchors { left: smallNetworkIcon.right; right: parent.right; leftMargin: 5; verticalCenter: parent.verticalCenter }
        font.weight: Font.Bold;
        text: itemName;
    }

    states: [
        State {
            name: "Selected";
            when: networksView.selectedItem == itemPath;
            PropertyChanges { target: networkItem; checked: true }
        }
    ]
}
