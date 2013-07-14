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
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    id: networkSetting;

    PlasmaComponents.Button {
        id: addButton;

        anchors { left: parent.left; bottom: parent.bottom; bottomMargin: 2; leftMargin: 2 }
        text: i18n("Add");
        iconSource: "list-add";
        // TODO: implement
        enabled: false;
    }

    PlasmaComponents.Button {
        id: configureDetails;

        anchors { right: parent.right; bottom: parent.bottom; bottomMargin: 2; rightMargin: 2 }
        text: i18n("Configure details to show");
        iconSource: "configure";

        onClicked: configureDetailsDialog.open();
    }

    PlasmaNm.Model {
        id: connectionModel;
    }

    PlasmaNm.SortModel {
        id: connectionSortModel;

        sourceModel: connectionModel;
        filterType: networkSettings.connectionType;
    }

    PlasmaExtras.Heading {
        id: availableConnectionsLabel;

        anchors { left: parent.left; right: parent.right; top: networkSettingsBackground.bottom; topMargin: 10 }
        text: i18n("Available connections");
        level: 2;
    }

    PlasmaComponents.Label {
        id: detailsText;

        anchors { top: availableConnectionsLabel.bottom; right: parent.right; topMargin: 10 }
        width: parent.width/2;
        textFormat: Text.RichText;
        text: networkSettings.details;
        wrapMode: Text.WordWrap;
    }

    PlasmaNm.Handler {
        id: handler;
    }

    Image {
        id: connectionsFrame;

        anchors { left: parent.left; top: availableConnectionsLabel.bottom; bottom: parent.bottom; topMargin: 10; bottomMargin: 50 }
        width: parent.width/2;
        source: "image://appbackgrounds/contextarea";
        fillMode: Image.Tile;

        Border {
            anchors.fill: parent;
        }

        ListView {
            id: connectionsView;

            anchors.fill: parent;
            clip: true;
            currentIndex: -1;
            model: connectionSortModel;
            delegate: ConnectionModelItem {
                checked: itemConnecting;
                onClicked: {
                    if (!itemConnected && !itemConnecting) {
                        if (itemUuid) {
                            handler.activateConnection(itemConnectionPath, itemDevicePath, itemSpecificPath);
                        } else {
                            handler.addAndActivateConnection(itemDevicePath, itemSpecificPath);
                        }
                    } else {
                        handler.deactivateConnection(itemConnectionPath);
                    }
                }

                onPressAndHold: {
                    if (itemUuid != "") {
                        editConnectionDialog.openDialog(itemName, itemUuid, itemConnectionPath);
                    }
                }
            }
        }
    }

    PlasmaComponents.Dialog {
        id: editConnectionDialog;

        property string path;
        property string uuid;
        property string name;

        function openDialog(connectionName, connectionUuid, connectionPath) {

            path = connectionPath;
            uuid = connectionUuid;
            name = connectionName;

            open();
        }

        title: [
            PlasmaComponents.Label {
                id: dialogText;

                property string name;

                anchors { left: parent.left; right: parent.right; leftMargin: 10; rightMargin: 10 }
                font.weight: Font.DemiBold;
                horizontalAlignment: Text.AlignHCenter;
                elide: Text.ElideRight;
                text: i18n("%1", editConnectionDialog.name);
            }
        ]

        buttons: [
            Row {
                PlasmaComponents.Button {
                    id: dialogEditButton;

                    height: 30;
                    text: i18n("Remove")

                    onClicked: {
                        handler.removeConnection(editConnectionDialog.path);
                        editConnectionDialog.accept();
                    }
                }

                PlasmaComponents.Button {
                    id: dialogRemoveButton;

                    height: 30;
                    text: i18n("Edit")

                    onClicked: {
                        handler.editConnection(editConnectionDialog.uuid);
                        editConnectionDialog.accept();
                    }
                }
            }
        ]
    }

    PlasmaComponents.CommonDialog {
        id: configureDetailsDialog

        titleText: i18n("Configure details to show")
        buttonTexts: [i18n("Save"), i18n("Close")]
        onButtonClicked: {
            if (index == 0) {
                configureDetailsLoader.item.save();
                networkSettings.detailKeys = configureDetailsLoader.item.detailKeys;
                configureDetailsLoader.source = "";
            } else {
                configureDetailsLoader.source = "";
                close();
            }
        }
        content: Loader {
            id: configureDetailsLoader;
            height: 400; width: 450;
        }
        onStatusChanged: {
            if (status == PlasmaComponents.DialogStatus.Open) {
                configureDetailsLoader.source = "DetailsWidget.qml"
            }
        }
    }
}
