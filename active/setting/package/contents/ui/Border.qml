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
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    Image {
        source: "image://appbackgrounds/shadow-left"
        fillMode: Image.Tile;
        anchors { right: parent.right; top: parent.top; bottom: parent.bottom; rightMargin: -1 }
    }

    Image {
        source: "image://appbackgrounds/shadow-right"
        fillMode: Image.Tile;
        anchors { left: parent.left; top: parent.top; bottom: parent.bottom; leftMargin: -1 }
    }

    Image {
        source: "image://appbackgrounds/shadow-bottom"
        fillMode: Image.Tile;
        anchors { right: parent.right; left: parent.left; top: parent.top; topMargin: -1 }
    }

    Image {
        source: "image://appbackgrounds/shadow-top"
        fillMode: Image.Tile
        anchors { right: parent.right; left: parent.left; bottom: parent.bottom; bottomMargin: -1 }
    }
}
