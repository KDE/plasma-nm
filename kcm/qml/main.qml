/*
    Copyright 2016 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Item {
    id: root
    focus: true
    anchors.fill: parent

    SystemPalette {
        id: palette
    }

    Rectangle {
        id: background
        anchors.fill: parent
        focus: true
        color: palette.base
    }

//     PlasmaNM.Handler {
//         id: handler
//     }

    PlasmaNM.NetworkModel {
        id: connectionModel
    }

    PlasmaNM.KCMProxyModel {
        id: kcmProxyModel

        sourceModel: connectionModel
    }

    PlasmaNM.DeviceModel {
        id: deviceModel
    }

    PlasmaNM.DeviceSortModel {
        id: deviceSortModel

        sourceModel: deviceModel
    }

    PlasmaCore.FrameSvgItem {
        id: deviceListFrame

        anchors {
            bottom: parent.bottom
            left: parent.left
            top: parent.top
        }
        // TODO get proper width?
        width: parent.width > 900 ? 300 : parent.width / 3

        imagePath: "widgets/frame"
        prefix: "sunken"

        PlasmaExtras.ScrollArea {
            id: deviceScrollView

            anchors.fill: parent

            ListView {
                id: deviceView

                anchors.fill: parent
                clip: true
                model: deviceSortModel
                boundsBehavior: Flickable.StopAtBounds
                delegate: DeviceItem { }
            }
        }

    }

    DevicePage {
        id: devicePage

        anchors {
            bottom: parent.bottom
            left: deviceListFrame.right
            right: parent.right
            top: parent.top
        }
    }

//         PlasmaComponents.Button {
//             id: addDeviceButton
//
//             anchors {
//                 bottom: parent.bottom
//                 bottomMargin: Math.round(units.gridUnit / 3)
//                 left: parent.left
//                 leftMargin: Math.round(units.gridUnit / 3)
//             }
//
//             iconSource: "list-add"
//             // TODO
//             tooltip: i18n("Add some tooltip")
//
//             onClicked: {
//                 // TODO
//             }
//         }
//
//         PlasmaComponents.Button {
//             id: removeDeviceButton
//
//             anchors {
//                 bottom: parent.bottom
//                 bottomMargin: Math.round(units.gridUnit / 3)
//                 left: addDeviceButton.right
//                 leftMargin: Math.round(units.gridUnit / 3)
//             }
//
//             // TODO check enabled
//             enabled: false
//             iconSource: "list-remove"
//             // TODO
//             tooltip: i18n("Add some tooltip")
//
//             onClicked: {
//                 // TODO
//             }
//         }
}