/*
    Copyright 2013-2017 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore

Item {
    id: switchButton

    property alias icon: switchButtonIcon.source
    property alias checked: switchButtonCheckbox.checked
    property alias enabled: switchButtonCheckbox.enabled
    property alias tooltip: switchButtonCheckbox.tooltip

    signal clicked

    height: switchButtonIcon.height + units.gridUnit / 2
    width: switchButtonCheckbox.width + switchButtonIcon.width + units.gridUnit

    PlasmaComponents.CheckBox {
        id: switchButtonCheckbox
        anchors {
            bottomMargin: Math.round(units.gridUnit / 3)
            left: parent.left
            leftMargin: units.gridUnit / 2
            topMargin: Math.round(units.gridUnit / 3)
            verticalCenter: parent.verticalCenter
        }

        onClicked: {
            switchButton.clicked()
        }
    }

    PlasmaCore.IconItem {
        id: switchButtonIcon
        anchors {
            left: switchButtonCheckbox.right
            leftMargin: units.gridUnit / 2
            top: switchButtonCheckbox.top
            bottom: switchButtonCheckbox.bottom
        }
        width: height
    }
}
