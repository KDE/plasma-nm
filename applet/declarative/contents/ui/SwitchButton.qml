/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>

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
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: switchButton;

    property alias icon: switchButtonIcon.elementId;
    property alias checked: switchButtonCheckbox.checked;
    property alias enabled: switchButtonCheckbox.enabled;

    signal clicked();

    height: switchButtonIcon.height + padding.margins.top + padding.margins.bottom;
    width: switchButtonCheckbox.width + switchButtonIcon.width + padding.margins.left * 2 + padding.margins.right;

    PlasmaCore.FrameSvgItem {
        id: switchButtonBackground;

        imagePath: "widgets/listitem"
        prefix: "normal"

        anchors {
            fill: parent;
        }

        PlasmaComponents.CheckBox {
            id: switchButtonCheckbox;

            anchors {
                left: parent.left;
                leftMargin: padding.margins.right;
                verticalCenter: parent.verticalCenter;
            }
        }

        PlasmaCore.SvgItem {
            id: switchButtonIcon;

            anchors {
                left: switchButtonCheckbox.right;
                leftMargin: padding.margins.left;
                verticalCenter: parent.verticalCenter;
            }
            svg: svgNetworkIcons;
        }

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;

            onEntered: {
                switchButtonBackground.prefix = "pressed";
            }

            onExited: {
                switchButtonBackground.prefix = "normal";
            }

            onClicked: {
                if (switchButton.enabled) {
                    switchButtonCheckbox.checked = !switchButtonCheckbox.checked;
                }
                switchButton.clicked();
            }
        }
    }
}
