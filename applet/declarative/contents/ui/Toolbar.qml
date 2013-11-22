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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.networkmanagement 0.1 as PlasmaNM

PlasmaComponents.ListItem {

    PlasmaNM.NetworkStatus {
        id: networkStatus;
    }

    height: Math.max(statusIcon.height, statusLabel.height) + padding.margins.top + padding.margins.bottom;
    enabled: true;

    PlasmaCore.IconItem {
        id: statusIcon;

        height: sizes.iconSize;
        width: height;
        anchors {
            left: parent.left;
            verticalCenter: parent.verticalCenter
        }
        source: (networkStatus.networkStatus == i18n("Connected") || networkStatus.networkStatus == "Connected") ? "user-online" : "user-offline";
    }

    PlasmaComponents.Label {
        id: statusLabel

        height: paintedHeight;
        anchors {
            left: statusIcon.right;
            leftMargin: padding.margins.left;
            verticalCenter: parent.verticalCenter;
        }
        text: networkStatus.networkStatus;
    }

    PlasmaCore.IconItem {
        id: configureButton;

        height: sizes.iconSize;
        width: height;
        anchors {
            right: parent.right;
            verticalCenter: parent.verticalCenter;
        }
        source: "configure";
    }
}
