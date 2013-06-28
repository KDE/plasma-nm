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
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.active.settings 0.1 as ActiveSettings
import org.kde.plasma.extras 0.1 as PlasmaExtras
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.mobilecomponents 0.1 as MobileComponents
import org.kde.active.settings 0.1

Item {
    id: networkModule;
    objectName: "networkModule"

    NetworkSettings {
        id: networkSettings;
    }

    MobileComponents.Package {
        id: networkPackage;
        name: "org.kde.active.settings.network";
    }

    Column {
        id: titleCol
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        PlasmaExtras.Title {
            text: settingsComponent.name
            opacity: 1
        }
        PlasmaComponents.Label {
            id: descriptionLabel
            text: settingsComponent.description
            opacity: .4
        }
    }

    Image {
        id: frame;

        anchors { left: parent.left; top: titleCol.bottom; bottom: listViewButtons.top; leftMargin: 1; topMargin: 10; bottomMargin: 10 }
        width: 250;
        source: "image://appbackgrounds/contextarea";
        fillMode: Image.Tile;

        Image {
            source: "image://appbackgrounds/shadow-left"
            fillMode: Image.Tile;
            anchors { right: parent.right; top: parent.top; bottom: parent.bottom; rightMargin: -1 }
        }

        Image {
            source: "image://appbackgrounds/shadow-right"
            fillMode: Image.Tile;
            anchors { left: parent.left; top: parent.top; bottom: parent.bottom; leftMargin: -1 }
        }

        Image {
            source: "image://appbackgrounds/shadow-bottom"
            fillMode: Image.Tile;
            anchors { right: parent.right; left: parent.left; top: parent.top; topMargin: -1 }
        }

        Image {
            source: "image://appbackgrounds/shadow-top"
            fillMode: Image.Tile
            anchors { right: parent.right; left: parent.left; bottom: parent.bottom; bottomMargin: -1 }
        }

        ListView {
            id: networksView;

            property string selectedItem;

            anchors { left: parent.left; right: parent.right; top: parent.top; bottom: parent.bottom }
            clip: true;
            interactive: false;
            model: networkSettings.networkModel;
            delegate: NetworkItem {
                anchors { left: parent.left; right: parent.right }

                onClicked: {
                    networksView.selectedItem = itemPath;
                    removeButton.enabled = removableItem;
                    networkSettings.setNetwork(itemType, itemPath);
                }
            }
        }
    }

    PlasmaComponents.ButtonRow {
        id: listViewButtons;

        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
        height: 40;
        exclusive: false;

        PlasmaComponents.Button {
            id: addButton;

            width: 120;
            text: i18n("Add");
            iconSource: "list-add";
        }

        PlasmaComponents.Button {
            id: removeButton;

            width: 120;
            text: i18n("Remove");
            iconSource: "list-remove";
            enabled: false;
        }
    }

    Item {
        id: itemContent;

        anchors { left: frame.right; right: parent.right; top: titleCol.bottom; bottom: parent.bottom; leftMargin: 10; topMargin: 10 }

        QIconItem {
            id: mainNetworkIcon;

            height: 48; width: 48;
            anchors { left: parent.left; top: parent.top }
            icon: networkSettings.icon;
        }

        PlasmaExtras.Title {
            id: networkTitle;

            anchors { left: mainNetworkIcon.right; right: parent.right; top: parent.top; leftMargin: 15 }
            text: networkSettings.name;
        }

        PlasmaComponents.Label {
            id: statusLabel;

            anchors { left: mainNetworkIcon.right; right: parent.right; top: networkTitle.bottom; leftMargin: 15 }
            text: networkSettings.status;
        }
    }
}
