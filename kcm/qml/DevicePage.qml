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
import org.kde.kcoreaddons 1.0 as KCoreAddons
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kquickcontrolsaddons 2.0 as KQuickControlsAddons
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Item {
    id: devicePage

    property alias deviceLabelText: deviceLabel.text
    property alias deviceSubLabelText: deviceSubLabel.text
    property alias deviceIconSource: deviceIcon.source

    property var deviceDetailsModel

    PlasmaCore.IconItem {
        id: deviceIcon

        anchors {
            left: parent.left
            leftMargin: Math.round(units.gridUnit / 2)
            top: parent.top
            topMargin: Math.round(units.gridUnit / 2)
        }
        height: units.iconSizes.medium; width: height
    }

    PlasmaComponents.Label {
        id: deviceLabel

        anchors {
            bottom: deviceIcon.verticalCenter
            left: deviceIcon.right
            leftMargin: Math.round(units.gridUnit / 2)
        }
        font.pointSize: theme.defaultFont.pointSize * 1.2
        font.weight: Font.DemiBold
        height: paintedHeight
    }

    PlasmaComponents.Label {
        id: deviceSubLabel

        anchors {
            left: deviceIcon.right
            leftMargin: Math.round(units.gridUnit / 2)
            top: deviceLabel.bottom
        }
        height: paintedHeight
        opacity: 0.6
        textFormat: Text.PlainText
    }

//     Item {
    PlasmaCore.FrameSvgItem {
        id: deviceDetailsFrame

        anchors {
            left: deviceIcon.left
            right: parent.right
            rightMargin: Math.round(units.gridUnit / 2)
            top: deviceSubLabel.bottom
            topMargin: Math.round(units.gridUnit / 2)
        }

        height: childrenRect.height
        imagePath: "widgets/frame"
        prefix: "sunken"

        Column {
            id: details

            anchors {
                left: parent.left
                leftMargin: units.iconSizes.medium
                right: parent.right
                top: parent.top
                topMargin: Math.round(units.gridUnit / 3)
            }
            height: childrenRect.height + Math.round(units.gridUnit / 2)

            Repeater {
                id: repeater

                property int longestString: 0

                model: deviceDetailsModel.length / 2

                Item {
                    anchors {
                        left: parent.left
                        right: parent.right
                        topMargin: Math.round(units.gridUnit / 3)
                    }
                    height: Math.max(detailNameLabel.height, detailValueLabel.height)

                    PlasmaComponents.Label {
                        id: detailNameLabel

                        anchors {
                            left: parent.left
                            leftMargin: repeater.longestString - paintedWidth + Math.round(units.gridUnit / 2)
                            verticalCenter: parent.verticalCenter
                        }
                        height: paintedHeight
                        font.pointSize: theme.smallestFont.pointSize
                        horizontalAlignment: Text.AlignRight
                        opacity: 0.6
                        text: "<b>" + deviceDetailsModel[index*2] + "</b>: &nbsp"

                        Component.onCompleted: {
                            if (paintedWidth > repeater.longestString) {
                                repeater.longestString = paintedWidth
                            }
                        }
                    }

                    PlasmaComponents.Label {
                        id: detailValueLabel

                        anchors {
                            left: detailNameLabel.right
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                        }
                        height: paintedHeight
                        elide: Text.ElideRight
                        font.pointSize: theme.smallestFont.pointSize
                        opacity: 0.6
                        text: deviceDetailsModel[(index*2)+1]
                        textFormat: Text.PlainText
                    }
                }
            }
        }
    }

//     Item {
    PlasmaCore.FrameSvgItem {
        id: deviceConnectionsFrame

        anchors {
            bottom: parent.bottom
            left: deviceIcon.left
            right: parent.right
            rightMargin: Math.round(units.gridUnit / 2)
            top: deviceDetailsFrame.bottom
            topMargin: Math.round(units.gridUnit / 2)
        }

        imagePath: "widgets/frame"
        prefix: "sunken"

        PlasmaExtras.ScrollArea {
            id: connectionScrollView

            anchors.fill: parent

            ListView {
                id: connectionView

                anchors.fill: parent
                clip: true
                model: kcmProxyModel
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                delegate: ConnectionItem { }
            }
        }
    }
}