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
import org.kde.plasmanm 0.1 as PlasmaNm
import org.kde.active.settings 0.1

Item {
    id: connectionList;

    ConnectionSettingsHandler {
        id: connectionSettingsDialogHandler;
    }

    PlasmaComponents.Button {
        id: addButton;

        anchors {
            left: parent.left;
            bottom: parent.bottom;
            bottomMargin: 2;
            leftMargin: 2;
        }
        text: i18n("Add");
        iconSource: "list-add";
        enabled: networkSettings.connectionType == PlasmaNm.Enums.Wireless ||
                 networkSettings.connectionType == PlasmaNm.Enums.Wired ||
                 networkSettings.connectionType == PlasmaNm.Enums.Gsm

        onClicked: {
            addConnectionDialog.open();
        }
    }

//     PlasmaComponents.Button {
//         id: configureDetails;
//
//         anchors { right: parent.right; bottom: parent.bottom; bottomMargin: 2; rightMargin: 2 }
//         text: i18n("Configure details to show");
//         iconSource: "configure";
//
//         onClicked: configureDetailsDialog.open();
//     }

//     PlasmaComponents.Label {
//         id: detailsText;
//
//         anchors { top: availableConnectionsLabel.bottom; right: parent.right; topMargin: 10 }
//         width: parent.width/2;
//         textFormat: Text.RichText;
//         text: networkSettings.details;
//         wrapMode: Text.WordWrap;
//     }


//     PlasmaComponents.CommonDialog {
//         id: configureDetailsDialog
//
//         titleText: i18n("Configure details to show")
//         buttonTexts: [i18n("Save"), i18n("Close")]
//         onButtonClicked: {
//             if (index == 0) {
//                 configureDetailsLoader.item.save();
//                 networkSettings.detailKeys = configureDetailsLoader.item.detailKeys;
//                 configureDetailsLoader.source = "";
//             } else {
//                 configureDetailsLoader.source = "";
//                 close();
//             }
//         }
//         content: Loader {
//             id: configureDetailsLoader;
//             height: 400; width: 450;
//         }
//         onStatusChanged: {
//             if (status == PlasmaComponents.DialogStatus.Open) {
//                 configureDetailsLoader.source = "DetailsWidget.qml"
//             }
//         }
//     }


    PlasmaNm.Model {
        id: connectionModel;
    }

    PlasmaNm.SortModel {
        id: connectionSortModel;

        sourceModel: connectionModel;
        filterType: networkSettings.connectionType;
    }



    Image {
        id: connectionsFrame;

        anchors {
            left: parent.left;
            top: parent.top;
            bottom: parent.bottom;
            topMargin: 10;
            bottomMargin: 50;
        }
        width: parent.width/2;
        source: "image://appbackgrounds/contextarea";
        fillMode: Image.Tile;

        Border {
            anchors.fill: parent;
        }

        ListView {
            id: connectionsView;

            property string previouslySelectedItem: "";

            anchors.fill: parent;
            clip: true;
            model: connectionSortModel;
            delegate: ConnectionModelItem {
                        property variant myData: model;

                        checked: (connectionsView.previouslySelectedItem == itemConnectionPath && itemConnectionPath != "") ||
                                 (itemConnectionPath == "" && connectionsView.previouslySelectedItem == itemName);
                        onItemSelected: {
                            connectionsView.previouslySelectedItem = itemUuid ? itemConnectionPath : itemName;
                            if (connectionSettings.status == Loader.Null) {
                                connectionSettings.source = "ConnectionSettings.qml";
                            }
                            connectionSettings.item.selectedItemModel = myData;
                        }
            }

            Component.onCompleted: {
                if (count) {
                    selectFirstItem();
                }
            }

            onCountChanged: {
                if (!count) {
                    if (connectionSettings.status != Loader.Null) {
                        connectionSettings.source = "";
                    }
                    previouslySelectedItem = "";
                }
            }

            function selectFirstItem() {
                currentIndex = 0;
                if (connectionSettings.status == Loader.Null) {
                    connectionSettings.source = "ConnectionSettings.qml";
                }
                connectionSettings.item.selectedItemModel = currentItem.myData;
                previouslySelectedItem = currentItem.myData.itemUuid ? currentItem.myData.itemConnectionPath : currentItem.myData.itemName;
            }
        }
    }

    Loader {
        id: connectionSettings;

        anchors {
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;
            topMargin: 10;
            bottomMargin: 50;
        }
        width: parent.width/2;
    }

    PlasmaComponents.CommonDialog {
        id: addConnectionDialog;

        titleText: i18n("Add new connection")
        buttonTexts: [i18n("Add"), i18n("Close")]
        onButtonClicked: {
            if (index == 0) {
                var map = {};
                map = addConnectionLoader.item.getSettings();
                connectionSettingsDialogHandler.addConnection(map);
                addConnectionLoader.source = "";
            } else {
                addConnectionLoader.source = "";
                close();
            }
        }
        content: Loader {
            id: addConnectionLoader;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                verticalCenter: parent.verticalCenter;
            }
            height: 500; width: 450;
        }
        onStatusChanged: {
            if (status == PlasmaComponents.DialogStatus.Open) {
                if (networkSettings.connectionType == PlasmaNm.Enums.Wireless) {
                    addConnectionLoader.source = "WirelessSettings.qml"
                } else if (networkSettings.connectionType == PlasmaNm.Enums.Wired) {
                    addConnectionLoader.source = "WiredSettings.qml"
                }
            }
        }
    }

    function resetIndex() {
        if (connectionsView.count) {
            connectionsView.selectFirstItem();
        } else {
            connectionSettings.source = "";
        }
    }
}
