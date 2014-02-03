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
    property bool checked: false;
    property bool enabled: true;

    signal clicked();

    height: slider.height + padding.margins.top;
    width: slider.width + switchButtonIcon.width + padding.margins.left * 2 + padding.margins.right;

    PlasmaCore.FrameSvgItem {
        id: switchButtonBackground;

        imagePath: "widgets/listitem"
        prefix: "normal"
        opacity: switchButton.enabled ? 1 : 0.4;

        anchors {
            fill: parent;
        }

        PlasmaCore.FrameSvgItem {
            id: slider;
            imagePath: "widgets/slider";
            prefix: "groove";
            height: width * 2;
            width: theme.defaultFont.mSize.height;
            anchors {
                left: parent.left;
                leftMargin: padding.margins.right;
                verticalCenter: parent.verticalCenter;
            }

            PlasmaCore.FrameSvgItem {
                id: highlight;
                imagePath: "widgets/slider";
                prefix: "groove-highlight";
                anchors.fill: parent;

                opacity: switchButton.checked ? 1 : 0
                Behavior on opacity {
                    PropertyAnimation { duration: 100 }
                }
            }

            PlasmaCore.FrameSvgItem {
                imagePath: "widgets/button";
                prefix: "shadow";
                anchors {
                    fill: button;
                    leftMargin: -margins.left;
                    topMargin: -margins.top;
                    rightMargin: -margins.right;
                    bottomMargin: -margins.bottom;
                }
            }

            PlasmaCore.FrameSvgItem {
                id: button;
                imagePath: "widgets/button";
                prefix: "normal";
                anchors {
                    left: parent.left;
                    right: parent.right;
                }
                height: width;
                y: switchButton.checked ? 0 : height;
                Behavior on y {
                    PropertyAnimation { duration: 100 }
                }
            }
        }

        PlasmaCore.SvgItem {
            id: switchButtonIcon;

            anchors {
                left: slider.right;
                leftMargin: padding.margins.left;
                verticalCenter: parent.verticalCenter;
            }
            svg: svgNetworkIcons;
        }

        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            enabled: switchButton.enabled;

            onEntered: {
                switchButtonBackground.prefix = "pressed";
            }

            onExited: {
                switchButtonBackground.prefix = "normal";
            }

            onClicked: {
                if (switchButton.enabled) {
                    switchButton.checked = !switchButton.checked;
                }
                switchButton.clicked();
            }
        }
    }
}
