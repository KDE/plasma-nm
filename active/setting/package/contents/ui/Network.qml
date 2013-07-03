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
import org.kde.plasmanm 0.1 as PlasmaNm

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
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        PlasmaExtras.Title {
            id: titleLabel;
            text: networkSettings.settingName;
            opacity: 1;
        }
        PlasmaComponents.Label {
            id: descriptionLabel;
            text: networkSettings.status;
            opacity: .3;
        }
    }

    Image {
        id: frame;

        anchors { horizontalCenter: parent.horizontalCenter; top: titleCol.bottom; topMargin: 10 }
        width: networkSettings.networkModel.count * 150;
        height: 48;
        source: "image://appbackgrounds/contextarea";
        fillMode: Image.Tile;

        Border {
            anchors.fill: parent;
        }

        ListView {
            id: networksView;

            property string selectedItem;

            anchors.fill: parent;
            orientation: ListView.Horizontal;
            clip: true;
            interactive: false;
            model: networkSettings.networkModel;
            // TODO
            currentIndex: -1;
            highlight: PlasmaComponents.Highlight {}
            delegate: NetworkItem {
                anchors { top: parent.top; bottom: parent.bottom }
                checked: networksView.currentIndex == index;
                onClicked: {
                    networksView.currentIndex = index;
                    networkSettings.setNetwork(itemType, itemPath);
                }
            }
        }
    }

    PlasmaComponents.ButtonRow {
        id: listViewButtons;

        anchors { left: parent.left; bottom: parent.bottom }
        height: 40; width: 250;
        exclusive: false;

        PlasmaComponents.Button {
            id: addButton;

            width: 120;
            text: i18n("Add");
            iconSource: "list-add";
            // TODO: implement
            enabled: false;
        }

        PlasmaComponents.Button {
            id: removeButton;

            width: 120;
            text: i18n("Remove");
            iconSource: "list-remove";
            // TODO: implement
            enabled: false;
        }
    }

    Item {
        id: itemContent;

        anchors { left: parent.left; right: parent.right; top: frame.bottom; bottom: parent.bottom; leftMargin: 10; topMargin: 10 }

        PlasmaComponents.Label {
            id: detailsText;

            anchors { top: parent.top; horizontalCenter: parent.horizontalCenter; topMargin: 10 }
            textFormat: Text.RichText;
            text: networkSettings.details;
        }
    }


    PlasmaComponents.ButtonRow {
        id: configureButton;

        anchors { right: parent.right; bottom: parent.bottom; rightMargin: 10 }
        height: 40;
        exclusive: false;

        PlasmaComponents.Button {
            id: configureDetails;

            width: 200;
            text: i18n("Configure details to show");
            iconSource: "configure";
            // TODO: implement
            enabled: false;

            onClicked: configureDetailsDialog.open();
        }
    }

    PlasmaComponents.CommonDialog {
        id: configureDetailsDialog

        titleText: i18n("Configure details to show")
        buttonTexts: [i18n("Save"), i18n("Close")]
        onButtonClicked: {
            if (index == 0) {
                console.log("Save");
            } else {
                console.log("Close");
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

    // TEST
//     DetailsWidget {
//         id: details;
//
//         anchors { verticalCenter: parent.verticalCenter; horizontalCenter: parent.horizontalCenter }
//         width: 500; height: 500;
//     }
}
