/*
    Copyright 2016-2018 Jan Grulich <jgrulich@redhat.com>

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
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.2 as QtControls

import org.kde.kirigami 2.3 as Kirigami

import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

Dialog {
    id: addConnectionDialog
    title: i18nc("@title:window", "Choose a Connection Type")

    signal requestCreateConnection(int type, string vpnType, string specificType, bool shared)

    contentItem: Item {
        implicitHeight: 600
        implicitWidth: 500

        PlasmaNM.CreatableConnectionsModel {
            id: connectionModel
        }

        Kirigami.Theme.colorSet: Kirigami.Theme.View

        Rectangle {
            anchors.fill: parent
            focus: true
            color: Kirigami.Theme.backgroundColor
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            Kirigami.ScrollablePage {
                Layout.fillHeight: true

                ListView {
                    id: view

                    property int currentlySelectedIndex: -1
                    property bool connectionShared
                    property string connectionSpecificType
                    property int connectionType
                    property string connectionVpnType

                    clip: true
                    model: connectionModel
                    currentIndex: -1
                    boundsBehavior: Flickable.StopAtBounds

                    section {
                        property: "ConnectionTypeSection"
                        delegate: Kirigami.AbstractListItem {
                            supportsMouseEvents: false
                            background: Rectangle {
                                color: palette.window
                            }
                            QtControls.Label {
                                id: headerLabel
                                anchors.centerIn: parent
                                font.weight: Font.DemiBold
                                text: section
                            }
                        }
                    }

                    delegate: Kirigami.AbstractListItem {
                        checked: view.currentlySelectedIndex == index
                        highlighted: hovered


                        RowLayout {
                            anchors {
                                left: parent.left
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                                leftMargin: Kirigami.Units.largeSpacing
                            }
                            spacing: Kirigami.Units.largeSpacing

                            Kirigami.Icon {
                                Layout.minimumHeight: Kirigami.Units.iconSizes.smallMedium
                                Layout.maximumHeight: Layout.minimumHeight
                                Layout.minimumWidth: height
                                source: ConnectionIcon
                            }

                            ColumnLayout {
                                spacing: 0

                                QtControls.Label {
                                    id: nameLabel
                                    Layout.fillWidth: true
                                    height: paintedHeight
                                    elide: Text.ElideRight
                                    text: ConnectionTypeName
                                    textFormat: Text.PlainText
                                }

                                QtControls.Label {
                                    id: statusLabel
                                    Layout.fillWidth: true
                                    height: paintedHeight
                                    elide: Text.ElideRight
                                    font.pointSize: theme.smallestFont.pointSize
                                    text: ConnectionDescription
                                    textFormat: Text.PlainText
                                    opacity: 0.6
                                    visible: ConnectionType == 11 // VPN
                                }
                            }
                        }

                        onClicked: {
                            createConnectionButton.enabled = true
                            view.currentlySelectedIndex = index
                            view.connectionSpecificType = ConnectionSpecificType
                            view.connectionShared = ConnectionShared
                            view.connectionType = ConnectionType
                            view.connectionVpnType = ConnectionVpnType
                        }

                        onDoubleClicked: {
                            addConnectionDialog.close()
                            addConnectionDialog.requestCreateConnection(view.connectionType, view.connectionVpnType, view.connectionSpecificType, view.connectionShared)
                        }
                    }
                }
            }

            RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignRight
                Layout.bottomMargin: Kirigami.Units.smallSpacing
                Layout.rightMargin: Kirigami.Units.smallSpacing

                QtControls.Button {
                    id: createConnectionButton
                    enabled: false
                    text: i18n("Create")

                    onClicked: {
                        addConnectionDialog.close()
                        addConnectionDialog.requestCreateConnection(view.connectionType, view.connectionVpnType, view.connectionSpecificType, view.connectionShared)
                    }
                }

                QtControls.Button {
                    text: i18n("Cancel")

                    onClicked: {
                        addConnectionDialog.close()
                    }
                }
            }
        }
    }
}
