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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Dialog {
    id: dialog
    title: i18n("Choose a connection type")

    signal requestCreateConnection(int type, string vpnType, string specificType, bool shared)

    contentItem: Item {
        implicitHeight: 600
        implicitWidth: 500

        PlasmaNM.CreatableConnectionsModel {
            id: connectionModel
        }

        SystemPalette {
            id: palette
        }

        Rectangle {
            id: background
            anchors.fill: parent
            focus: true
            color: palette.base
        }

        PlasmaExtras.ScrollArea {
            id: scrollView
            anchors {
                bottom: buttonRow.top
                bottomMargin: Math.round(units.gridUnit / 2)
                left: parent.left
                right: parent.right
                top: parent.top
            }

            ListView {
                id: view

                property int currentlySelectedIndex: -1
                property bool connectionShared
                property string connectionSpecificType
                property int connectionType
                property string connectionVpnType

                anchors.fill: parent
                clip: true
                model: connectionModel
                currentIndex: -1
                boundsBehavior: Flickable.StopAtBounds
                section.property: "ConnectionTypeSection"
                section.delegate: Header { text: section }
                delegate: PlasmaComponents.ListItem {
                    checked: mouseArea.containsMouse || view.currentlySelectedIndex == index
                    height: connectionTypeBase.height

                    Item {
                        id: connectionTypeBase

                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            // Reset top margin from PlasmaComponents.ListItem
                            topMargin: -Math.round(units.gridUnit / 3)
                        }
                        height: Math.max(units.iconSizes.medium, connectionNameLabel.height + connectionDescriptionLabel.height) + Math.round(units.gridUnit / 2)

                        PlasmaCore.IconItem {
                            id: connectionIcon

                            anchors {
                                left: parent.left
                                verticalCenter: parent.verticalCenter
                            }
                            height: units.iconSizes.medium; width: height
                            source: ConnectionIcon
                        }

                        PlasmaComponents.Label {
                            id: connectionNameLabel

                            anchors {
                                bottom: ConnectionType == 11 ? connectionIcon.verticalCenter : undefined
                                left: connectionIcon.right
                                leftMargin: Math.round(units.gridUnit / 2)
                                right: parent.right
                                verticalCenter: ConnectionType == 11 ? undefined : parent.verticalCenter
                            }
                            height: paintedHeight
                            elide: Text.ElideRight
                            text: ConnectionTypeName
                            textFormat: Text.PlainText
                        }

                        PlasmaComponents.Label {
                            id: connectionDescriptionLabel

                            anchors {
                                left: connectionIcon.right
                                leftMargin: Math.round(units.gridUnit / 2)
                                right: parent.right
                                top: connectionNameLabel.bottom
                            }
                            height: visible ? paintedHeight : 0
                            elide: Text.ElideRight
                            font.pointSize: theme.smallestFont.pointSize
                            opacity: 0.6
                            text: ConnectionDescription
                            visible: ConnectionType == 11 // VPN
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            createButton.enabled = true
                            view.currentlySelectedIndex = index
                            view.connectionSpecificType = ConnectionSpecificType
                            view.connectionShared = ConnectionShared
                            view.connectionType = ConnectionType
                            view.connectionVpnType = ConnectionVpnType
                        }

                        onDoubleClicked: {
                            dialog.close()
                            dialog.requestCreateConnection(view.connectionType, view.connectionVpnType, view.ConnectionSpecificType, view.connectionShared)
                        }
                    }
                }
            }
        }

        Row {
            id: buttonRow
            anchors {
                bottom: parent.bottom
                right: parent.right
                margins: Math.round(units.gridUnit / 2)
            }
            spacing: Math.round(units.gridUnit / 2)

            PlasmaComponents.Button {
                id: createButton
                enabled: false
                text: i18n("Create")

                onClicked: {
                    dialog.close()
                    dialog.requestCreateConnection(view.connectionType, view.connectionVpnType, view.connectionSpecificType, view.connectionShared)
                }
            }

            PlasmaComponents.Button {
                id: closeButton
                text: i18n("Close")

                onClicked: {
                    dialog.close()
                }
            }
        }
    }
}
