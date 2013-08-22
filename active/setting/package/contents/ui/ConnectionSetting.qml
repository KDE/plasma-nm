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
    property variant selectedItemModel;

    anchors.fill: parent;

    PlasmaExtras.ScrollArea {
        id: settingsScrollArea;

        anchors {
            left: parent.left;
            right: parent.right;
            top: parent.top;
            bottom: addButton.top;
            bottomMargin: 10;
        }

        // TODO: load setting
        Flickable {
            ConnectionSettingWidget {
                id: connectionWidget;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: parent.top
                }
            }

            WirelessSettingWidget {
                id: wirelessWidget;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: connectionWidget.bottom;
                    topMargin: 24;
                }
            }
            Ipv4SettingWidget {
                id: ipv4Widget;

                anchors {
                    left: parent.left;
                    right: parent.right;
                    top: wirelessWidget.bottom;
                    topMargin: 24;
                }
            }
        }
    }

    PlasmaComponents.Button {
        anchors {
            right: addButton.left;
            bottom: parent.bottom;
            rightMargin: 10;
        }
        text: "Print setting";

        onClicked: {
            //TODO do a real action
            var resultingMap = [];
            resultingMap.connection = connectionWidget.getSetting();
            resultingMap.ipv4 = ipv4Widget.getSetting();
            resultingMap.wireless = wirelessWidget.getSetting();
            for (var key in resultingMap["connection"]) {
                console.log(key + ":" + resultingMap["connection"][key]);
            }
            for (var key in resultingMap["ipv4"]) {
                console.log(key + ":" + resultingMap["ipv4"][key]);
            }
            for (var key in resultingMap["wireless"]) {
                console.log(key + ":" + resultingMap["wireless"][key]);
            }
        }
    }

    PlasmaComponents.Button {
        id: addButton;

        anchors {
            right: parent.right;
            bottom: parent.bottom;
            rightMargin: 5;
        }
        text: {
            if (selectedItemModel) {
                if (selectedItemModel.itemConnected || selectedItemModel.itemConnecting) {
                    i18n("Disconnect");
                } else {
                    i18n("Connect");
                }
            } else {
                "";
            }
        }

        onClicked: {
            if (selectedItemModel) {
                if (!selectedItemModel.itemConnected && !selectedItemModel.itemConnecting) {
                    if (selectedItemModel.itemUuid) {
                        handler.activateConnection(selectedItemModel.itemConnectionPath, selectedItemModel.itemDevicePath, selectedItemModel.itemSpecificPath);
                    } else {
                        handler.addAndActivateConnection(selectedItemModel.itemDevicePath, selectedItemModel.itemSpecificPath);
                    }
                } else {
                    handler.deactivateConnection(selectedItemModel.itemConnectionPath);
                }
            }
        }
    }

    onSelectedItemModelChanged: {
        // TODO: reload setting
        console.log(selectedItemModel.itemName);
    }
}
