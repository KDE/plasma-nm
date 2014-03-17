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

import QtQuick 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as Components
import org.kde.networkmanagement 0.1 as PlasmaNM

FocusScope {

    PlasmaNM.Handler {
            id: handler;
    }

    PlasmaNM.NetworkModel {
        id: connectionModel;
    }

    PlasmaNM.AppletProxyModel {
        id: appletProxyModel;

        sourceModel: connectionModel;
    }

    PlasmaCore.FrameSvgItem {
        id: padding
        imagePath: "widgets/viewitem"
        prefix: "hover"
        opacity: 0
        anchors.fill: parent
    }

    Toolbar {
        id: toolbar;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
        }
    }

    ListView {
        id: connectionView;

        anchors {
            bottom: parent.bottom;
            left: parent.left;
            right: parent.right;
            top: toolbar.bottom;
        }
        clip: true
        model: appletProxyModel;
        currentIndex: -1;
        interactive: true;
        boundsBehavior: Flickable.StopAtBounds;
        section.delegate: Header { text: section }
        delegate: ConnectionItem {
            onStateChanged: {
                if (state == "expanded") {
                    connectionView.currentIndex = index;
                }
            }
        }
    }
}
