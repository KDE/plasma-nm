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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasmanm 0.1 as PlasmaNm

Item {
    id: mainWindow;

    property int minimumWidth: 300;
    property int minimumHeight: 300;
    property bool autoHideOptions: true;
    property Component compactRepresentation: CompactRepresantation{
        Component.onCompleted: {
            plasmoid.addEventListener('configChanged', mainWindow.configChanged)
        }
    }

    signal sectionChanged();

    function hideOptions() {
        if (autoHideOptions) {
//             connectionView.itemExpandable = true;
            toolbar.toolbarExpandable = false;
        }
    }

    PlasmaNm.GlobalConfig {
        id: globalConfig;

        onDetailKeysChanged: {
            connectionModel.updateItems();
        }

        onNetworkSpeedUnitChanged: {
            connectionModel.updateItems();
        }
    }

    PlasmaNm.Handler {
            id: handler;
    }

    PlasmaNm.Model {
        id: connectionModel;
    }

    PlasmaNm.SortModel {
        id: connectionSortModel;

        sourceModel: connectionModel;
    }

    ListView {
        id: connectionView;

//         property bool itemExpandable: true;
        property bool expandedItem: false;
        property string previouslyExpandedItem: "";

        property bool activeExpanded: true;
        property bool previousExpanded: true;
        property bool unknownExpanded: true;

        anchors { left: parent.left; right: parent.right; top: parent.top; bottom: toolbarSeparator.top; topMargin: 5; bottomMargin: 10 }
        clip: true
        model: connectionSortModel;
        highlight: PlasmaComponents.Highlight{}
        highlightMoveSpeed: 1000;
        currentIndex: -1;
        interactive: true;
        section.property: "itemSection";
        section.delegate: SectionHeader {
            onHideSection: {
                if (section == i18n("Active connections")) {
                    connectionView.activeExpanded = false;
                } else if (section == i18n("Previous connections")) {
                    connectionView.previousExpanded = false;
                } else {
                    connectionView.unknownExpanded = false;
                }
            }

            onShowSection: {
                if (section == i18n("Active connections")) {
                    connectionView.activeExpanded = true;
                } else if (section == i18n("Previous connections")) {
                    connectionView.previousExpanded = true;
                } else {
                    connectionView.unknownExpanded = true;
                }
            }
        }
        delegate: ConnectionItem {
            expanded: (connectionView.expandedItem && ((connectionView.previouslyExpandedItem == itemConnectionPath && itemConnectionPath != "") || (itemConnectionPath == "" && connectionView.previouslyExpandedItem == itemName)))
            onItemExpanded: {
                if (itemExpanded) {
                    connectionView.expandedItem = true;
                    connectionView.previouslyExpandedItem = connectionPath;
                } else {
                    connectionView.expandedItem = false;
                    connectionView.previouslyExpandedItem = "";
                }
//                 connectionView.currentIndex = -1;
//                 connectionView.itemExpandable = true;
//                 if (autoHideOptions) {
//                     plasmoid.writeConfig("optionsExpanded", "hidden");
//                     toolbar.toolbarExpandable = false;
//                 }
            }
//             onRemoveConnectionItem: dialog.openDialog(connectionName, connectionPath);
        }
    }

    Rectangle {
        id: toolbarSeparator;

        height: 1;
        anchors { left: parent.left; right: parent.right; bottom: toolbar.top; bottomMargin: 2; leftMargin: 5; rightMargin: 5 }
        radius: 2;
        color: theme.highlightColor;
    }

    Toolbar {
        id: toolbar;

        property bool toolbarExpandable: true;

        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }

        onToolbarExpanded: {
            toolbarExpandable = true;
//             connectionView.itemExpandable = false;
        }
    }

//     PlasmaComponents.Dialog {
//         id: dialog;
//
//         property string path;
//
//         function openDialog(connectionName, connectionPath) {
//             dialogText.name = connectionName;
//             path = connectionPath;
//
//             open();
//         }
//
//         title: [
//             PlasmaComponents.Label {
//                 id: dialogText;
//
//                 property string name;
//
//                 anchors { left: parent.left; right: parent.right; leftMargin: 10; rightMargin: 10 }
//                 textFormat: Text.RichText;
//                 wrapMode: Text.WordWrap;
//                 font.weight: Font.DemiBold;
//                 horizontalAlignment: Text.AlignHCenter;
//                 text: i18n("Do you really want to remove connection %1?", name);
//             }
//         ]
//
//         buttons: [
//             Row {
//                 PlasmaComponents.Button {
//                     id: confirmRemoveButton;
//
//                     height: 20; width: 150;
//                     text: i18n("Remove")
//
//                     onClicked: dialog.accept();
//                 }
//                 PlasmaComponents.Button {
//                     id: cancelRemoveButton;
//
//                     height: 20; width: 150;
//                     text: i18n("Cancel")
//
//                     onClicked: dialog.reject();
//                 }
//             }
//         ]
//
//         onAccepted: {
//             handler.removeConnection(path);
//         }
//     }

    Component.onCompleted: {
        configChanged();
        plasmoid.popupEvent.connect(mainWindow.hideOptions);
        plasmoid.addEventListener('configChanged', mainWindow.configChanged)
    }

    function configChanged() {
        var keys;
        keys = plasmoid.readConfig("detailKeys");
        globalConfig.setDetailKeys(keys);
        var speedUnit;
        speedUnit = plasmoid.readConfig("networkSpeedUnit");
        globalConfig.setNetworkSpeedUnit(speedUnit);
        autoHideOptions = plasmoid.readConfig("autoHideOptions");
    }
}
