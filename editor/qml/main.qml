/*
    Copyright 2014 Jan Grulich <jgrulich@redhat.com>

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

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
// import QtQuick.Dialogs 1.0
// import org.kde.plasma.plasmoid 2.0
// import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.plasma.core 2.0 as PlasmaCore
// import org.kde.plasma.extras 2.0 as PlasmaExtras
import org.kde.plasma.networkmanagement 0.1 as PlasmaNM

ApplicationWindow {
    id: mainWindow;

    width: 500
    height: 600
    minimumWidth: 400
    minimumHeight: 400

    title: i18n("Connection editor");
    visible: true;

    Action {
        id: addAction;

        iconName: "list-add";
        shortcut: StandardKey.Insert;
        text: i18n("Add");

        onTriggered: {
            addConnectionDialog.show();
        }
    }

    Action {
        id: editAction;

        iconName: "configure"
        shortcut: StandardKey.Edit;
        enabled: connectionView.currentRow != -1;
        text: i18n("Edit");

        onTriggered: {
            handler.editConnection(connectionView.selectedConnectionUuid);
        }
    }

    Action {
        id: removeAction;

        iconName: "edit-delete"
        shortcut: StandardKey.Delete;
        enabled: connectionView.currentRow != -1;
        text: i18n("Remove");

        onTriggered: {
            handler.removeConnection(connectionView.selectedConnectionPath);
        }
    }

    menuBar: MenuBar {
        Menu {
            title: i18n("File");

            // TODO import/export VPN

            MenuItem {
                text: i18n("Close");
                shortcut: StandardKey.Quit;
                onTriggered: Qt.quit();
            }
        }

        Menu {
            title: i18n("Connection");

            MenuItem { action: addAction }
            MenuItem { action: editAction }
            MenuItem { action: removeAction }
        }

        // TODO about dialog
    }

    toolBar: ToolBar {
        id: toolbar;

        RowLayout {
            id: toolbarLayout;

            spacing: 0;

            ToolButton { action: addAction }
            ToolButton { action: editAction }
            ToolButton { action: removeAction }
        }
    }

    PlasmaNM.Handler {
        id: handler;
    }

    PlasmaNM.NetworkModel {
        id: connectionModel;
    }

    PlasmaNM.EditorProxyModel {
        id: editorProxyModel;

        sourceModel: connectionModel;
        sortRole: connectionView.getColumn(connectionView.sortIndicatorColumn).role;
        sortOrder: connectionView.sortIndicatorOrder;
        filterString: searchBar.text;
    }

    Dialog {
        id: addConnectionDialog;
    }

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    GroupBox {

        anchors.fill: parent;

        TextField {
            id: searchBar;

            anchors {
                left: parent.left;
                right: parent.right;
            }

            placeholderText: i18n("Search for a connection");
        }

        TableView {
            id: connectionView;

            property string selectedConnectionDevicePath;
            property string selectedConnectionPath;
            property string selectedConnectionSpecificPath;
            property string selectedConnectionUuid;

            anchors {
                bottom: parent.bottom;
                left: parent.left;
                right: parent.right;
                top: searchBar.bottom;
            }

            model: editorProxyModel;

            TableViewColumn {
                role: "Name";
                title: i18n("Connection name");
                width: 300

//                 delegate: Item {
//                     PlasmaCore.IconItem {
//                         id: stateIcon;
//
//                         anchors {
//                             left: parent.left;
//                             leftMargin: 5;
//                             verticalCenter: parent.verticalCenter;
//                         }
//                         width: units.iconSizes.small;
//                         height: units.iconSizes.small;
//
//                         source: model.get(styleData.row, "ConnectionState") == PlasmaNM.Enums.Activated ? "user-online" : "user-offline";
//                         active: true;
//                         smooth: true;
//                     }
//
//                     Text {
//                         anchors {
//                             left: stateIcon.right;
//                             leftMargin: 8;
//                             verticalCenter: parent.verticalCenter;
//                         }
//
//
//                         color: styleData.textColor
//                         elide: styleData.elideMode
//                         text: styleData.value
//                     }
//                 }
            }

            TableViewColumn { role: "LastUsedDateOnly"; title: i18n("Last used") }

            section.property: "TypeString";
            section.delegate: Rectangle {
                    height: connectionIcon.height;
                    width: connectionView.width;

                    color: myPalette.highlight;

                    PlasmaCore.SvgItem {
                        id: connectionIcon;

                        anchors {
                            left: parent.left;
                            leftMargin: 5;
                            verticalCenter: parent.verticalCenter;
                        }

                        height: units.iconSizes.medium;
                        width: height;

                        // FIXME:
                        // unfortunately I haven't found a way how to access the model from section.delegate
                        elementId: {
                            if (section == i18n("ADSL") ||
                                section == i18n("DSL") ||
                                section == i18n("Mobile broadband")) {
                                "network-mobile-100";
                            } else if (section == i18n("Bluetooth")) {
                                "network-bluetooth";
                            } else if (section == i18n("WiMAX") ||
                                    section == i18n("Wireless")) {
                                "network-wireless-100";
                            } else if (section == i18n("VPN")) {
                                "network-vpn";
                            } else {
                                "network-wired";
                            }
                        }

                        svg: PlasmaCore.Svg {
                            multipleImages: true;
                            imagePath: "icons/network";
                        }
                    }

                    Text {
                        anchors {
                            left: connectionIcon.right;
                            leftMargin: 8;
                            verticalCenter: parent.verticalCenter
                        }
                        text: section;
                    }
            }

            rowDelegate: Rectangle {
                height: units.iconSizes.medium;
                color: styleData.selected ? myPalette.highlight : (styleData.alternate ? myPalette.midlight : myPalette.light);
            }

            sortIndicatorVisible: true;

            onClicked: {
                selectedConnectionDevicePath = model.get(row, "DevicePath");
                selectedConnectionPath = model.get(row, "ConnectionPath");
                selectedConnectionSpecificPath = model.get(row, "SpecificPath");
                selectedConnectionUuid = model.get(row, "Uuid");
                console.log("Connection " + model.get(row, "Name") + " selected");
            }

            onDoubleClicked: {
                handler.editConnection(model.get(currentRow, "Uuid"));
            }
        }
    }
}
