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
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: detailsView;

    property alias model: view.model;
    property alias index: view.currentIndex;

    signal itemClicked();

    Image {
        id: viewBackground;

        anchors.fill: parent;
        source: "image://appbackgrounds/contextarea";
        fillMode: Image.Tile;

        Border {
            anchors.fill: parent;
        }

        ListView {
            id: view;

            anchors.fill: parent;
            clip: true;
            currentIndex: -1;
            delegate: PlasmaComponents.ListItem {
                enabled: true
                height: 48;
                checked: view.currentIndex == index;

                PlasmaComponents.Label {
                    text: name;

                    anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                    wrapMode: Text.Wrap;
                    width: parent.width;
                    font.weight: Font.Bold;
                }

                onClicked: {
                    view.currentIndex = index;
                    itemClicked();
                }
            }
        }
    }
}
