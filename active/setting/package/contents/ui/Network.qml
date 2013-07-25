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
        id: titleCol;

        anchors {top: parent.top; left: parent.left; right: planeMode.left }

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

    PlasmaNm.Handler {
        id: handler;
    }

    PlasmaNm.EnabledConnections {
        id: enabledConnections;

        onNetworkingEnabled: {
            planeModeSwitch.checked = !enabled;
        }
    }

    Item {
        id: planeMode;

        anchors { top: parent.top; right: parent.right }

        PlasmaExtras.Title {
            id: planeModeLabel;

            anchors { right: planeModeSwitch.left; rightMargin: 8 }
            text: i18n("Airplane Mode");
        }

        PlasmaComponents.Switch {
            id: planeModeSwitch;

            anchors { right: parent.right; rightMargin: 10 }
            height: 30;

            onClicked: {
                handler.enableNetworking(!checked);
            }
        }
    }

    Image {
        id: networkSettingsBackground;

        anchors { horizontalCenter: parent.horizontalCenter; top: titleCol.bottom; topMargin: 10 }
        width: networkSettings.networkSettingsModel.count * 150;
        height: 48;
        source: "image://appbackgrounds/contextarea";
        fillMode: Image.Tile;

        Border {
            anchors.fill: parent;
        }

        ListView {
            id: networksSettingsView;

            property string selectedItem;

            anchors.fill: parent;
            orientation: ListView.Horizontal;
            clip: true;
            interactive: false;
            model: networkSettings.networkSettingsModel;
            currentIndex: 0;
            highlight: PlasmaComponents.Highlight {}
            delegate: PlasmaComponents.ListItem {
                id: networkSettingItem;

                property variant myData: model;

                anchors { top: parent.top; bottom: parent.bottom }
                width: 150;
                checked: networksSettingsView.currentIndex == index;
                enabled: true

                QIconItem {
                    id: networkIcon;

                    anchors { left: parent.left; top: parent.top; bottom: parent.bottom }
                    width: 48; height: 48;
                    icon: QIcon(itemIcon);
                }

                PlasmaComponents.Label {
                    id: networkLabel;

                    anchors { left: networkIcon.right; verticalCenter: parent.verticalCenter }
                    font.weight: Font.Bold;
                    elide: Text.ElideRight;
                    text: itemName;
                }

                onClicked: {
                    networksSettingsView.currentIndex = index;
                    networkSettings.setNetworkSetting(itemType, itemPath);
                    networkSetting.resetIndex();
                }
            }
        }
    }

    NetworkSetting {
        id: networkSetting;

        anchors { left: parent.left; right: parent.right; top: networkSettingsBackground.bottom; bottom: parent.bottom; topMargin: 10 }
    }

    Component.onCompleted: {
        networkSettings.setNetworkSetting(networksSettingsView.currentItem.myData.itemType, networksSettingsView.currentItem.myData.itemPath);
        enabledConnections.init();
    }
}
